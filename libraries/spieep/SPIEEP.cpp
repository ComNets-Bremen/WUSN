/* Copyright (C) 2011 by Eric Brundick <spirilis@linux.com>
 * Distributed under the terms of the GNU Lesser General Public License
 * A copy of this license may be obtained at:
 * http://www.gnu.org/copyleft/lesser.html
 */


#include <SPIEEP.h>
#include <SPI.h>

// Microchip 25LC512 SPI EEPROM Instruction Set
#define SPIEEP_READ 0x03
#define SPIEEP_WRITE 0x02
#define SPIEEP_WREN 0x06
#define SPIEEP_WRDI 0x04
#define SPIEEP_RDSR 0x05
#define SPIEEP_WRSR 0x01
#define SPIEEP_PE 0x42
#define SPIEEP_SE 0xD8
#define SPIEEP_CE 0xC7
#define SPIEEP_RDID 0xAB
#define SPIEEP_DPD 0xB9

// Microchip 25LC512 SPI EEPROM Status Register Bits
#define SPIEEP_STATUS_WPEN 7
#define SPIEEP_STATUS_BP1 3
#define SPIEEP_STATUS_BP0 2
#define SPIEEP_STATUS_WEL 1
#define SPIEEP_STATUS_WIP 0

boolean SPIEEP::verify_enabled() {
  if (_cspin < 0)
    return false;
  return true;
}

int SPIEEP::pagesize() {
  if (!verify_enabled()) return 0;
  return _pagesize;
}

SPIEEP::SPIEEP(int addrwidth, int pagesize, int totalsize) {
  _cspin = -1;
  _addrwidth = addrwidth / 8;
  _pagesize = pagesize;
  _numpages = totalsize / pagesize;
  _highestaddr = totalsize - 1;
  if (_addrwidth == 1 && totalsize == 512)  // Is it a 25(AA|LC)040 512-byte chip?
    // these require special support where they use 8-bit addresses but the 9th bit is included
    // inside the instruction.
    _is_25LC040 = 1;
  else
    _is_25LC040 = 0;
}

void SPIEEP::begin(int cspin) {
  // SPI needs to be enabled prior to initialization of this object
  if ((SPCR >> SPE) & 0x1) {
    _cspin = cspin;
    pinMode(_cspin, OUTPUT);
    digitalWrite(_cspin, HIGH);  // CS pin is high when idle.
  } else
    _cspin = -1;  // If SPI was not enabled, disable this library!
}

void SPIEEP::begin_spi(int cspin) {
  // Enable SPI using some defaults
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);

  begin(cspin);
}

void SPIEEP::end() {
  _cspin = -1;
}

byte SPIEEP::readstatus() {
  byte ret;

  if (!verify_enabled()) return (byte)0;
  digitalWrite(_cspin, LOW);
    SPI.transfer(SPIEEP_RDSR);
    ret = SPI.transfer(0x0);
  digitalWrite(_cspin, HIGH);

  return ret;
}

void SPIEEP::writestatus(byte b) {
  if (!verify_enabled()) return;
  digitalWrite(_cspin, LOW);
    SPI.transfer(SPIEEP_WRSR);
    SPI.transfer(b);
  digitalWrite(_cspin, HIGH);
}

byte SPIEEP::rdid() {
  byte ret;

  if (!verify_enabled()) return (byte)0;

  digitalWrite(_cspin, LOW);
    SPI.transfer(SPIEEP_RDID);
    _write_address(0);  // Need to send a dummy address for this command.  Odd
    ret = SPI.transfer(0x0);
  digitalWrite(_cspin, HIGH);
  delayMicroseconds(100);  // Per 25LC512 datasheet

  return ret;
}

void SPIEEP::dpd() {
  if (!verify_enabled()) return;
  digitalWrite(_cspin, LOW);
    SPI.transfer(SPIEEP_DPD);
  digitalWrite(_cspin, HIGH);
}

void SPIEEP::sleepmode() {
  dpd();
}

byte SPIEEP::wakeup() {
  return rdid();
}

// This is a destructive function -- it overwrites the byte at the very last address on the chip.
boolean SPIEEP::test_chip() {
  /* Run the chip through a few tests to validate that we're not just pissing in the wind when
   * we do I/O to this chip.
   */

  if (!verify_enabled()) return false;

  /* The "return-from-powerdown" command should always return a unique manufacturer ID, not 0x00
   * This works even if the chip is not in "deep-sleep" mode.
   */
  if (_highestaddr >= 65535) { // Deep Sleep only supported on 25LC512 and higher
    if (rdid() == 0x00)  
      return false;
  }

  /* Since we can guarantee we're no longer in "deep-sleep" mode, let's try writing. */
  if (!write(_highestaddr, (byte)0x89))
    return false;

  /* Now read it back */
  if (read(_highestaddr) != (byte)0x89)
    return false;

  /* Go into deep-sleep */
  if (_highestaddr >= 65535) { // Deep Sleep only supported on 25LC512 and higher
    dpd();
    delayMicroseconds(100);  // Per 25LC512 datasheet

    /* Try reading our previously-written byte; we should receive garbage, not 0x89
     * Can't guarantee we'll see 0x00 or 0xFF necessarily though, as the SerOUT pin is
     * tri-stated during deep-sleep.  Doubt we'll see something as intricate as 0x89 though.
     */
    if (read(_highestaddr) == (byte)0x89)
      return false;

    /* Come back out */
    if (rdid() == 0x00)
      return false;
  }

  /* Try writing something else to _highestaddr */
  if (!write(_highestaddr, (byte)0x76))
    return false;

  /* Now read it back */
  if (read(_highestaddr) != (byte)0x76)
    return false;

  /* OK, guess we're all good! */
  return true;
}

void SPIEEP::wren() {
  if (!verify_enabled()) return;

  digitalWrite(_cspin, LOW);
    SPI.transfer(SPIEEP_WREN);
  digitalWrite(_cspin, HIGH);
}

void SPIEEP::wrdi() {
  if (!verify_enabled()) return;

  digitalWrite(_cspin, LOW);
    SPI.transfer(SPIEEP_WRDI);
  digitalWrite(_cspin, HIGH);
}

boolean SPIEEP::is_wren() {
  byte ret;

  if (!verify_enabled()) return false;

  ret = readstatus();
  if (((ret >> SPIEEP_STATUS_WEL) & 0x1) == 0x1)
    return true;
  return false;
}

boolean SPIEEP::_write_validation() {
  long m = millis();
  byte ret;

  if (!verify_enabled()) return false;

  // Wait until the Write-In-Progress status register has cleared
  // Or 20ms (timeout) has passed, whichever happens first.  No write operations
  // should take more than 10ms or so.
  do {
    delayMicroseconds(200);
    ret = readstatus();
  } while (((ret >> SPIEEP_STATUS_WIP) & 0x01) == 0x01 && (millis()-m) < 20);

  // Check if Write-Enable has cleared to validate whether this command succeeded.
  return !is_wren();
}

void SPIEEP::_write_address(uint32_t p) {
  int8_t i;

  for (i=_addrwidth-1; i>=0; i--)
    SPI.transfer( (byte)(p >> (8*i)) & 0xFF );
}

/* This is here to support the 25[AA|LC]040 512-byte chips.
 * They have 9 bits of addressing but only use an 8-bit wide address, so the 9th bit
 * just happens to go into bit #3 of the instruction.
 * It's only used for simple read & write, as the 25LC040 doesn't support much more than that.
 */
inline byte SPIEEP::_compose_instruction_addrbit(uint32_t addr, byte instr) {
  byte addrbit;

  addrbit = (addr >> 5) & 0x08;
  return (instr | addrbit);
}

boolean SPIEEP::chip_erase() {
  byte ret;

  if (!verify_enabled()) return false;

  wren();
  if (!is_wren())
    return false;  // Couldn't enable WREN for some reason?

  digitalWrite(_cspin, LOW);
    SPI.transfer(SPIEEP_CE);
  digitalWrite(_cspin, HIGH);

  return _write_validation();
}

boolean SPIEEP::page_erase(uint32_t p) {
  byte ret;

  if (!verify_enabled()) return false;

  wren();
  if (!is_wren())
    return false;  // Couldn't enable WREN for some reason?

  digitalWrite(_cspin, LOW);
    SPI.transfer(SPIEEP_PE);
    _write_address(p);
  digitalWrite(_cspin, HIGH);

  return _write_validation();
}

byte SPIEEP::read(uint32_t p) {
  byte ret;

  if (!verify_enabled()) return false;

  digitalWrite(_cspin, LOW);
    if (_is_25LC040)
      SPI.transfer(_compose_instruction_addrbit(p, SPIEEP_READ));
    else
      SPI.transfer(SPIEEP_READ);
    _write_address(p);
    ret = SPI.transfer(0x0);
  digitalWrite(_cspin, HIGH);

  return ret;
}

boolean SPIEEP::read_page(uint32_t p, char *buf) {
  return read_page(p, (byte *)buf);
}

boolean SPIEEP::read_page(uint32_t p, byte *buf) {
  byte ret;
  uint16_t i;

  if (!verify_enabled()) return false;

  if (p % _pagesize != 0)
    // We're reading a whole page here, so the address MUST lie at the start of a page boundary.
    return false;

  digitalWrite(_cspin, LOW);
    if (_is_25LC040)
      SPI.transfer(_compose_instruction_addrbit(p, SPIEEP_READ));
    else
      SPI.transfer(SPIEEP_READ);
    _write_address(p);
    for (i=0; i<_pagesize; i++)
      buf[i] = SPI.transfer(0x0);
  digitalWrite(_cspin, HIGH);

  return true;
}

boolean SPIEEP::write(uint32_t p, char b) {
  return write(p, (byte)b);
}

boolean SPIEEP::write(uint32_t p, byte b) {
  if (!verify_enabled()) return false;

  wren();
  if (!is_wren())
    return false;  // Couldn't enable WREN for some reason?
  digitalWrite(_cspin, LOW);
    if (_is_25LC040)
      SPI.transfer(_compose_instruction_addrbit(p, SPIEEP_WRITE));
    else
      SPI.transfer(SPIEEP_WRITE);
    _write_address(p);
    SPI.transfer(b);
  digitalWrite(_cspin, HIGH);

  return _write_validation();
}

boolean SPIEEP::write_page(uint32_t p, char *buf) {
  return write_page(p, (byte *)buf);
}

boolean SPIEEP::write_page(uint32_t p, byte *buf) {
  byte ret;
  uint16_t i;

  if (!verify_enabled()) return false;

  if (p % _pagesize != 0)
    // We're writing a whole page here, so the address MUST lie at the start of a page boundary.
    return false;

  wren();
  if (!is_wren())
    return false;  // Couldn't enable WREN for some reason?
  digitalWrite(_cspin, LOW);
    if (_is_25LC040)
      SPI.transfer(_compose_instruction_addrbit(p, SPIEEP_WRITE));
    else
      SPI.transfer(SPIEEP_WRITE);
    _write_address(p);
    for (i=0; i<_pagesize; i++)
      SPI.transfer(buf[i]);
  digitalWrite(_cspin, HIGH);

  return _write_validation();
}

boolean SPIEEP::readn(uint32_t p, char *buf, uint16_t len) {
  return readn(p, (byte *)buf, len);
}

boolean SPIEEP::readn(uint32_t p, byte *buf, uint16_t len) {
  uint32_t idx, e, pg_i_e, rdlen, i;  /* idx = start of current read block, e = last read address + 1,
                                       * pg_i_e = address of last byte in idx's current page + 1,
                                       * rdlen = # bytes to read in current block
                                       */

  if (!verify_enabled()) return false;

  if ((p+len-1) > _highestaddr) return false;

  idx = p;
  e = p+len;
  do {
    pg_i_e = (idx/_pagesize)*_pagesize + _pagesize;
    if (e > pg_i_e)
      rdlen = pg_i_e - idx;
    else
      rdlen = e - idx;

      // Perform read for this block
      digitalWrite(_cspin, LOW);
      if (_is_25LC040)
        SPI.transfer(_compose_instruction_addrbit(idx, SPIEEP_READ));
      else
        SPI.transfer(SPIEEP_READ);
        _write_address(idx);
        for (i=0; i<rdlen; i++)
          buf[idx-p+i] = SPI.transfer(0x0);
      digitalWrite(_cspin, HIGH);

      idx += rdlen;
  } while (idx < e);

  return true;
}

boolean SPIEEP::writen(uint32_t p, char *buf, uint16_t len) {
  return writen(p, (byte *)buf, len);
}

boolean SPIEEP::writen(uint32_t p, byte *buf, uint16_t len) {
  uint32_t idx, e, pg_i_e, wrlen, i;  /* idx = start of current read block, e = last read address + 1,
                                       * pg_i_e = address of last byte in idx's current page + 1,
                                       * wrlen = # bytes to write in current block
                                       */

  if (!verify_enabled()) return false;

  if ((p+len-1) > _highestaddr) return false;

  idx = p;
  e = p+len;
  do {
    pg_i_e = (idx/_pagesize)*_pagesize + _pagesize;
    if (e > pg_i_e)
      wrlen = pg_i_e - idx;
    else
      wrlen = e - idx;

      // Perform write for this block
      wren();
      if (!is_wren())
        return false;  // Couldn't enable WREN for some reason?
      digitalWrite(_cspin, LOW);
      if (_is_25LC040)
        SPI.transfer(_compose_instruction_addrbit(idx, SPIEEP_WRITE));
      else
        SPI.transfer(SPIEEP_WRITE);
        _write_address(idx);
        for (i=0; i<wrlen; i++)
          SPI.transfer(buf[idx-p+i]);
      digitalWrite(_cspin, HIGH);

      if (!_write_validation())
        return false;  // A failed write, abort before performing any more.

      idx += wrlen;
  } while (idx < e);

  return true;
}

/* Integer address-translating variants */
int SPIEEP::read_int(uint32_t p) {
  int ret;

  if (readn(p * sizeof(int), (byte *)&ret, sizeof(int)))
    return ret;
  else
    return 0;
}

boolean SPIEEP::readn_int(uint32_t p, int *buf, uint16_t len) {
  return readn(p * sizeof(int), (byte *)buf, len * sizeof(int));
}

boolean SPIEEP::write_int(uint32_t p, int n) {
  return writen(p * sizeof(int), (byte *)&n, sizeof(int));
}

boolean SPIEEP::writen_int(uint32_t p, int *buf, uint16_t len) {
  return writen(p * sizeof(int), (byte *)buf, len * sizeof(int));
}

int SPIEEP::read_int_offset(uint32_t offset, uint32_t p) {
  int ret;

  if (readn(p * sizeof(int) + offset, (byte *)&ret, sizeof(int)))
    return ret;
  else
    return 0;
}

boolean SPIEEP::readn_int_offset(uint32_t offset, uint32_t p, int *buf, uint16_t len) {
  return readn(p * sizeof(int) + offset, (byte *)buf, len * sizeof(int));
}

boolean SPIEEP::write_int_offset(uint32_t offset, uint32_t p, int n) {
  return writen(p * sizeof(int) + offset, (byte *)&n, sizeof(int));
}

boolean SPIEEP::writen_int_offset(uint32_t offset, uint32_t p, int *buf, uint16_t len) {
  return writen(p * sizeof(int) + offset, (byte *)buf, len * sizeof(int));
}

/* Long integer address-translating variants */
long SPIEEP::read_long(uint32_t p) {
  long ret;

  if (readn(p * sizeof(long), (byte *)&ret, sizeof(long)))
    return ret;
  else
    return 0;
}

boolean SPIEEP::readn_long(uint32_t p, long *buf, uint16_t len) {
  return readn(p * sizeof(long), (byte *)buf, len * sizeof(long));
}

boolean SPIEEP::write_long(uint32_t p, long n) {
  return writen(p * sizeof(long), (byte *)&n, sizeof(long));
}

boolean SPIEEP::writen_long(uint32_t p, long *buf, uint16_t len) {
  return writen(p * sizeof(long), (byte *)buf, len * sizeof(long));
}

long SPIEEP::read_long_offset(uint32_t offset, uint32_t p) {
  long ret;

  if (readn(p * sizeof(long) + offset, (byte *)&ret, sizeof(long)))
    return ret;
  else
    return 0;
}

boolean SPIEEP::readn_long_offset(uint32_t offset, uint32_t p, long *buf, uint16_t len) {
  return readn(p * sizeof(long) + offset, (byte *)buf, len * sizeof(long));
}

boolean SPIEEP::write_long_offset(uint32_t offset, uint32_t p, long n) {
  return writen(p * sizeof(long) + offset, (byte *)&n, sizeof(long));
}

boolean SPIEEP::writen_long_offset(uint32_t offset, uint32_t p, long *buf, uint16_t len) {
  return writen(p * sizeof(long) + offset, (byte *)buf, len * sizeof(long));
}

/* Float address-translating variants */
float SPIEEP::read_float(uint32_t p) {
  float ret;

  if (readn(p * sizeof(float), (byte *)&ret, sizeof(float)))
    return ret;
  else
    return 0;
}

boolean SPIEEP::readn_float(uint32_t p, float *buf, uint16_t len) {
  return readn(p * sizeof(float), (byte *)buf, len * sizeof(float));
}

boolean SPIEEP::write_float(uint32_t p, float n) {
  return writen(p * sizeof(float), (byte *)&n, sizeof(float));
}

boolean SPIEEP::writen_float(uint32_t p, float *buf, uint16_t len) {
  return writen(p * sizeof(float), (byte *)buf, len * sizeof(float));
}

float SPIEEP::read_float_offset(uint32_t offset, uint32_t p) {
  float ret;

  if (readn(p * sizeof(float) + offset, (byte *)&ret, sizeof(float)))
    return ret;
  else
    return 0;
}

boolean SPIEEP::readn_float_offset(uint32_t offset, uint32_t p, float *buf, uint16_t len) {
  return readn(p * sizeof(float) + offset, (byte *)buf, len * sizeof(float));
}

boolean SPIEEP::write_float_offset(uint32_t offset, uint32_t p, float n) {
  return writen(p * sizeof(float) + offset, (byte *)&n, sizeof(float));
}

boolean SPIEEP::writen_float_offset(uint32_t offset, uint32_t p, float *buf, uint16_t len) {
  return writen(p * sizeof(float) + offset, (byte *)buf, len * sizeof(float));
}

/* Double-wide float address-translating variants */
double SPIEEP::read_double(uint32_t p) {
  double ret;

  if (readn(p * sizeof(double), (byte *)&ret, sizeof(double)))
    return ret;
  else
    return 0;
}

boolean SPIEEP::readn_double(uint32_t p, double *buf, uint16_t len) {
  return readn(p * sizeof(double), (byte *)buf, len * sizeof(double));
}

boolean SPIEEP::write_double(uint32_t p, double n) {
  return writen(p * sizeof(double), (byte *)&n, sizeof(double));
}

boolean SPIEEP::writen_double(uint32_t p, double *buf, uint16_t len) {
  return writen(p * sizeof(double), (byte *)buf, len * sizeof(double));
}

double SPIEEP::read_double_offset(uint32_t offset, uint32_t p) {
  double ret;

  if (readn(p * sizeof(double) + offset, (byte *)&ret, sizeof(double)))
    return ret;
  else
    return 0;
}

boolean SPIEEP::readn_double_offset(uint32_t offset, uint32_t p, double *buf, uint16_t len) {
  return readn(p * sizeof(double) + offset, (byte *)buf, len * sizeof(double));
}

boolean SPIEEP::write_double_offset(uint32_t offset, uint32_t p, double n) {
  return writen(p * sizeof(double) + offset, (byte *)&n, sizeof(double));
}

boolean SPIEEP::writen_double_offset(uint32_t offset, uint32_t p, double *buf, uint16_t len) {
  return writen(p * sizeof(double) + offset, (byte *)buf, len * sizeof(double));
}


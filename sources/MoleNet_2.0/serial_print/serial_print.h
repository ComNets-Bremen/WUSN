/******************************************************************************
 * MoleNet - A Wireless Underground Sensor Network
 *
 * Copyright (C) 2016, Communication Networks, University of Bremen, Germany
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 *
 * This file is part of MoleNet
 *
 ******************************************************************************
 * ============================================================================
 *
 *       Filename:  serial_print.h
 *
 *    Description:  A RAM optimized Serial print for Arduino
 *                  http://www.utopiamechanicus.com/article/low-memory-serial-print/  
 *                  Header File.     
 *
 *        Version:  1.0
 *        Created:  14/08/2016 10:00:00 
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Muhammad Haseeb (MSc. Student-IAE University of Bremen), 
 *                  mhaseeb@uni-bremen.de, 
 *                  hasee.b@hotmail.com
 *   Organization:  COMNETS, Communication Networks Department
 *                  University of Bremen, Bremen, Germany
 *
 * ============================================================================
 */

#ifndef MOLENET_SERIAL_PRINT_HPP_INCLUDED
#define MOLENET_SERIAL_PRINT_HPP_INCLUDED

#if defined (__AVR__) || (__avr__)
#include <Arduino.h>   
#endif
// http://stackoverflow.com/questions/26618443/how-to-use-int16-t-or-int32-t-with-functions-like-scanf

#ifdef DEBUG

    struct SerialPrint {
 #if defined (__AVR__) || (__avr__)
        static void specialPrint(Print &out,PGM_P format,...) {
            // program memory version of printf - copy of format string and result share a buffer
            // so as to avoid too much memory use
            char formatString[128], *ptr;
            strncpy_P( formatString, format, sizeof(formatString) ); // copy in from program mem
            // null terminate - leave last char since we might need it in worst case for result's \0
            formatString[ sizeof(formatString)-2 ]='\0'; 
            ptr=&formatString[ strlen(formatString)+1 ]; // our result buffer...
            va_list args;
            va_start (args,format);
            vsnprintf(ptr, sizeof(formatString)-1-strlen(formatString), formatString, args );
            va_end (args);
            formatString[ sizeof(formatString)-1 ]='\0'; 
            out.print(ptr);
        }
#endif
    };

 #if defined (__AVR__) || (__avr__)
    #define Serialprint(format, ...) SerialPrint::specialPrint(Serial,PSTR(format),##__VA_ARGS__)
    #define Streamprint(stream,format, ...) SerialPrint::specialPrint(stream,PSTR(format),##__VA_ARGS__)
#else
    #define Serialprint(format, ...)
    #define Streamprint(stream,format, ...)
#endif
    
    #define PRINT_UNREACHABLE       Serialprint("Unreachable");
    #define PRINT_UNABLE_TO_SEND    Serialprint("Unable to send\n");
    #define PRINT_NUMBER_OF_PACKET_SENT(x)    Serialprint("[%d Packet Sent]",x);
    #define PRINT_PACKET_RECEIVED(senderID, receiverID) \
            Serialprint("\n[PACKET_RECEIVED %d<=%d] ",receiverID, senderID );
    #define PRINT_PACKET_SENT(senderID, receiverID) \
            Serialprint("\n[PACKET_SENT %d=>%d] ",senderID ,receiverID );
    #define PRINT_SLEEP_MODE_ACTIVATED Serialprint("[NODE_SLEEP_MODE_ACTIVATED]"); 
    #define PRINT_SLEEP_MODE_DEACTIVATED Serialprint("[NODE_SLEEP_MODE_DEACTIVATED]\n"); 

    #define PRINT_ERROR_SENSOR_NOT_FOUND Serialprint( "[Sensor Not Found]" );
    #define PRINT_ERROR_SENSOR_CHECKSUM_INCORRECT Serialprint( "[Sensor Checksum Inccoret]");
    
    #define PRINT_EEPROM_EMPTY Serialprint( "[EEPROM EMPTY]");
    
#else
    #define Serialprint(format, ...)
    #define Streamprint(stream,format, ...)
    #define PRINT_UNREACHABLE 
    #define PRINT_UNABLE_TO_SEND
    #define PRINT_NUMBER_OF_PACKET_SENT(x)
    #define PRINT_PACKET_RECEIVED(senderID, receiverID)
    #define PRINT_PACKET_SENT(senderID, receiverID)
    #define PRINT_SLEEP_MODE_ACTIVATED
    #define PRINT_SLEEP_MODE_DEACTIVATED 
    
    #define PRINT_ERROR_SENSOR_NOT_FOUND    
    #define PRINT_ERROR_SENSOR_CHECKSUM_INCORRECT    
    #define PRINT_EEPROM_EMPTY    
#endif

#endif // MOLENET_SERIAL_PRINT_HPP_INCLUDED

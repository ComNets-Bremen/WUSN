from django.core.management.base import BaseCommand, CommandError
from SimpleLogger.models import SimpleWusnData

import serial, traceback, re, datetime, pytz

RE_STRING = r"^#\[(?P<p_number>\d+)\]\[(?P<node_id>\d+)\]\s+D(?P<y>\d+)\/(?P<m>\d+)\/(?P<d>\d+)\s+t(?P<hour>\d+)\:(?P<minute>\d+)\s+W(?P<water>\d+)\s+T(?P<temp>\d+)\s+(?P<chksum>\d+)\s+(?P<success_ind>\d+)\s+\[RX_RSSI:(?P<rssi>[\d+-]+)\]"

class Command(BaseCommand):
    help = 'Listen on a given USB port for incoming data and insert this into model'

    def add_arguments(self, parser):
        parser.add_argument('port', type=str, default="/dev/ttyUSB0")
        parser.add_argument('baudrate', type=int, default=115200)

    def handle(self, *args, **options):
        self.stdout.write(self.style.SUCCESS('Starting listener on Port %s...' %
            options['port']))
        self.stdout.write(self.style.SUCCESS('Baudrate: %s...' %
            options['baudrate']))

        re_c = re.compile(RE_STRING)

        ser = serial.Serial(options['port'], options['baudrate'], timeout=10)
        while True:
            data = ser.readline()[:-1]
            if len(data) == 0:
                continue
            print "Received:", data
            values = re_c.search(data)
            if values != None:
                dt = datetime.datetime(
                    int(values.group('y'))+2000,
                    int(values.group('m')),
                    int(values.group('d')),
                    int(values.group('hour')),
                    int(values.group('minute')),
                    tzinfo=pytz.utc
                    )
                water_raw = int(values.group('water'))
                # The following value is used for the dielectric value epsilon
                # Adapt it according to the soil
                water = float(water_raw)/50.0

                temperature_raw = int(values.group('temp'))
                temperature_decompressed = temperature_raw
                if temperature_raw > 900:
                    temperature_decompressed = \
                            5.0*(float(temperature_raw)-900.0)+900.0

                temperature = (float(temperature_decompressed) - 400.0)/10.0

                try:
                    dataset = SimpleWusnData()
                    dataset.date_time_sensor = dt
                    dataset.packet_number = values.group('p_number')
                    dataset.node_id = values.group('node_id')
                    dataset.water_raw = water_raw
                    dataset.water = water
                    dataset.temperature_raw = temperature_raw
                    dataset.temperature = temperature
                    dataset.rssi = int(values.group('rssi'))
                    dataset.success_ind = int(values.group('success_ind'))
                    dataset.checksum = int(values.group('chksum'))
                    dataset.save()
                except:
                    self.stdout.write(self.style.ERROR('Cannot insert data'))
                    print traceback.format_exc()
                else:
                    self.stdout.write(self.style.SUCCESS('Data inserted'))


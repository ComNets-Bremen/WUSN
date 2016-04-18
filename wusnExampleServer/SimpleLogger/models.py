from __future__ import unicode_literals

from django.db import models

# Create your models here.

class SimpleWusnData(models.Model):
    received_timestamp = models.DateTimeField("ReceivedTime", auto_now_add=True)
    date_time_sensor = models.DateTimeField("DateTimeSensor")
    packet_number = models.IntegerField("PacketNumber")
    node_id = models.IntegerField("NodeId")
    water_raw = models.IntegerField("WaterRaw")
    water = models.FloatField("Water")
    temperature_raw = models.IntegerField("TemperatureRaw")
    temperature = models.FloatField("Temperature")
    rssi = models.IntegerField("RSSI")
    success_ind = models.IntegerField("SuccessIndicator")
    checksum = models.IntegerField("Checksum")

    def __unicode__(self):
        return "Message from " + str(self.node_id) + " received at " +\
                str(self.received_timestamp)

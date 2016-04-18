from django.shortcuts import render
from django.http import HttpResponse
from .models import SimpleWusnData

import csv
import datetime

# Create your views here.

def index(request):
    latest_data = SimpleWusnData.objects.order_by('-date_time_sensor')[:50]
    context = {
        'latest_data' : latest_data,
        }
    return render(request, 'SimpleLogger/index.html', context)


def dataset(request, dataset_id):
    response = "You're looking for the dataset %s."
    return HttpResponse(response % dataset_id)

def csvExport(request):
    fname = "measurements_" + \
	datetime.datetime.strftime(datetime.datetime.now(),
	     "%Y-%m-%d_%H-%M-%S") +\
        ".csv"
    print fname
    response = HttpResponse(content_type='text/csv')
    response['Content-Disposition'] = 'attachment; filename='+fname
    writer = csv.writer(response)

    data = SimpleWusnData.objects.order_by('-date_time_sensor')
    field_names = [f.name for f in SimpleWusnData._meta.fields]
    writer.writerow(field_names)
    for instance in data.all():
      writer.writerow([unicode(getattr(instance, f)).encode('utf-8') for f in field_names])
    return response

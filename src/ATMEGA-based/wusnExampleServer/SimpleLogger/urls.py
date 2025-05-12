from django.conf.urls import patterns, url

from . import views

urlpatterns = patterns('',
    url(r'^csvexport', views.csvExport, name='export'),
#    url(r'^(?P<dataset_id>[0-9]+)/$', views.dataset, name='detail'),
    url(r'^', views.index, name='index'),
)

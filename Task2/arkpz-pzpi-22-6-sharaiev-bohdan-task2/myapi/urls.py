from django.urls import path
from .views import *

urlpatterns = [
    path('auth/login/', LoginView.as_view(), name='login'),
    path('users/', UserList.as_view(), name='user-list'),
    path('users/<int:id>/', UserDetail.as_view(), name='user-detail'),
    path('users/<int:id>/sensors/', UserSensorsList.as_view(), name='user-sensors'),
    path('locations/', LocationList.as_view(), name='location-list'),
    path('locations/<int:id>/', LocationDetail.as_view(), name='location-detail'),
    path('sensors/', SensorList.as_view(), name='sensor-list'),
    path('sensors/<int:id>/', SensorDetail.as_view(), name='sensor-detail'),
    path('sensors/<int:id>/radiation-data/', SensorDataList.as_view(), name='user-sensors'),
    path('radiation-data/', RadiationDataList.as_view(), name='radiation-data-list'),
    path('radiation-data/<int:id>/', RadiationDataDetail.as_view(), name='radiation-data-detail'),
    path('alerts/', AlertList.as_view(), name='alert-list'),
    path('alerts/<int:id>/', AlertDetail.as_view(), name='alert-detail'),
    path('reports/', ReportList.as_view(), name='report-list'),
    path('reports/<int:id>/', ReportDetail.as_view(), name='report-detail'),
]

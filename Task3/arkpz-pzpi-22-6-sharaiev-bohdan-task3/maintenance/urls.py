from django.urls import path

from .views import DBManagementView

urlpatterns = [
    path('db-management/<str:action>/', DBManagementView.as_view(), name='db-management'),
]

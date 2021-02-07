from django.db import models

# Create your models here.

class User(models.Model):
    name = models.CharField(max_length=40)
    psw = models.CharField(max_length=30)
    reportTm = models.DateTimeField(auto_now_add=True)
    val = models.FloatField(default=0.0)
    tempertuer = models.FloatField(default=0.0)

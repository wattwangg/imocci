from django.shortcuts import render
from django.http import HttpResponse
from PiwebModel.models import User
from django.middleware.csrf import get_token
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_http_methods
from django.conf import settings

@csrf_exempt
def testDB(request):
    test = User(name='zhangsan')
    test.save()
    return HttpResponse("<p>成功</p>")

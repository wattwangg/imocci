from django.http import HttpResponse,JsonResponse
import os
import glob
import time
import json  

import threading
import time

from django.middleware.csrf import get_token
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_http_methods
from django.conf import settings

class PrintThread(threading.Thread):
    passed = 0
    def run(self):
        while True:
            print("start....",self.passed)
            for i in range(30):
                time.sleep(1)
            self.passed += 1
            print("end....",self.passed)

prints = None
def startThread():
    global prints
    if prints is None:
        prints = PrintThread()
        prints.start()

@require_http_methods(["POST"])
@csrf_exempt
def uploadfile(request):
    req = request.FILES.get('file')
    name = req.name
    print("upload::: ",name)
    with open('./static/'+name,mode='wb+') as f:
        for chunk in req.chunks():
            f.write(chunk)
        f.close()
    return JsonResponse({'name':name})

@csrf_exempt
def getData(request):
    print("getData----")
    from PiwebModel.models import User
    dis = request.GET.get("dis")
    if dis is None:
        users = User.objects.all()
    else: 
        import datetime
        minute_ago=datetime.datetime.now()-datetime.timedelta(minutes=int(dis)) 
        users = User.objects.filter(reportTm__gte=minute_ago)
    print(users)
    vals = []
    timeline = []
    tempertuers = []
    import datetime
    for user in users:
        vals.append(user.val)
        tempertuers.append(user.tempertuer)
        user.reportTm = user.reportTm + datetime.timedelta(hours=8)
        timeline.append(user.reportTm)
    return JsonResponse({"res":vals, "timeline":timeline, "tempertuer":tempertuers})

@csrf_exempt
def delData(request):
    from PiwebModel.models import User
    import datetime
    hour_ago=datetime.datetime.now()-datetime.timedelta(hours=4)
    users = User.objects.filter(reportTm__lte=hour_ago)
    for user in users:
        print(user.id)
        print(user.reportTm.minute)
        if user.reportTm.minute % 5 != 0:
            User.objects.filter(id=user.id).delete()
    return JsonResponse({"code":"0"})

@csrf_exempt
def testget(request):
    #import os,json
    #file_names = os.listdir('./static/')
    #return JsonResponse({'files': file_names})
    arg = request.GET.get("name")
    val = request.GET.get("val")
    tempertuer = request.GET.get("tempertuer")
    if tempertuer is None:
        tempertuer = 0.0
    if arg is None:
        return JsonResponse({"code":100})
    if val is None:
        val = 0.0
    from PiwebModel.models import User
    user = User(name=arg, val=val, tempertuer=tempertuer)
    user.save()
    import time
    if arg is not None and arg == 'pi':
        print("name",arg,' ', time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
    return JsonResponse({"code":0})

def gettemperature(request):
    startThread()
    import requests
    r = requests.get("http://192.168.193.231:8000/gettemperature")
    print(r.text)
    return JsonResponse(json.loads(r.text))

@require_http_methods(["POST"])
@csrf_exempt
def getBaterryVoltage(request):
    #startThread()
    #import requests
    #r = requests.get("http://192.168.193.231:8000/getBaterryVoltage")
    #print(r.text)
    #return JsonResponse(json.loads(r.text))
    return JsonResponse({})


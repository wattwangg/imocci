from django.contrib import admin
from .models import User

class UserAdmin(admin.ModelAdmin):
    fields = ['name', 'psw', 'val', 'tempertuer']
    list_display = ('name', 'psw', 'reportTm', 'val', 'tempertuer')
    list_filter = ['name']
    search_fields = ['name']

#admin.site.register(User)
admin.site.register(User, UserAdmin)

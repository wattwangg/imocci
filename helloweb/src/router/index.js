import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/',
      name: 'HelloWorld',
      component: resolve => require(['@/components/HelloWorld'], resolve),
      meta: { requiresAuth: true }
    },
    {
      path: '/sun',
      name: 'SunWatch',
      component: resolve => require(['@/components/SunWatch'], resolve),
      meta: { requiresAuth: true }
    },
    {
      path: '/SetSwitch',
      name: 'SetSwitch',
      component: resolve => require(['@/components/SetSwitch'], resolve),
      meta: { requiresAuth: true }
    },
    {
      path: '/Login',
      name: 'Login',
      component: resolve => require(['@/components/Login'], resolve)
    }
  ]
})

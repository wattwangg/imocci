<template>
  <div>
    <img src="../assets/logo.png">
    <br><br>
    <p>设置页</p>
    <br><br>
  </div>
</template>
<script>
import Vue from 'vue'
export default {
  name: 'SetSwitch',
  data () {
    return {
      resData: [],
      postData: [],
      msg: '--'
    }
  },
  created () {
  },
  // vue高版本中，推荐使用axios进行网络请求，而不再使用vue-resource
  methods: {
    doGet () {
      // 发起get请求
      Vue.axios.get('/api/get', {
        // get传递的query参数（传递的参数应与后台人员协商，本次模拟不做限制，不做判断）
        params: {
          name: '嬴政',
          age: 45
        }
      }).then((response) => {
        // then 指成功之后的回调 (注意：使用箭头函数，可以不考虑this指向)
        console.log(response)
        console.log(response.data)
        this.resData = response.data
      }).catch((error) => {
        // catch 指请求出错的处理
        console.log(error)
      })
    },
    doPost () {
      // 提示：该方式传递的参数是json格式，如上传不成功，需检查后台接收的方式是不是application/x-www-form-urlencoded默认格式，jquery中ajax请求的就是application/x-www-form-urlencoded，后台需要body-parser解码
      Vue.axios.post('/api/post', {
        // 此参数就是写到请求体中的参数
        stuName: '盖聂',
        height: 180
      }).then((response) => {
        console.log(response)
        console.log(response.data)
        this.postData = response.data
      }).catch((error) => {
        console.log(error)
      })
    }
  }
}
</script>

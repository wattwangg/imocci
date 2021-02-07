<template>
  <div>
    <img src="../assets/logo.png">
    <br><br>
    <router-link to='/sun'>
      <Button type="text">查看电压</button>
    </router-link>
    <br><br>
    <Upload action="/uploadfile">
      <Button icon="ios-cloud-upload-outline">Upload files</Button>
    </Upload>
    <br><br>
  </div>
</template>
<script>
import Vue from 'vue'
import mqtt from 'mqtt'
var client
const options = {
  connectTimeout: 100,
  clientId: 'mqtitId-Home',
  username: 'mosquitto',
  password: 'mosquitto',
  clean: true
}

export default {
  name: 'HelloWorld',
  data () {
    return {
      resData: [],
      postData: [],
      msg: '--'
    }
  },
  created () {
    client = mqtt.connect('ws://47.104.220.154:9001', options)
    this.mqttMsg()
  },
  // vue高版本中，推荐使用axios进行网络请求，而不再使用vue-resource
  methods: {
    mqttMsg () {
      console.log('connect 2 quittomqtt server')
      client.once('connect', (e) => {
        console.log('连接成功！！！')
        client.subscribe('/wjw1014', { qos: 2 }, (error) => {
          if (!error) {
            console.log('订阅成功')
          } else {
            console.log('订阅失败')
          }
        })
      })
      // 接收消息处理
      client.on('message', (topic, message) => {
        console.log('收到来自', topic, '的消息', message.toString())
        this.msg = message.toString()
      })
    },
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

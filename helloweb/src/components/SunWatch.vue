<template>
  <!--为echarts准备一个具备大小的容器dom-->
  <div id='main'>
    <a-radio-group style="margin-top: 15px" :value="timedis" @change="handleDisChange">
      <a-radio-button value="all" >全部</a-radio-button>
      <a-radio-button value="day">24小时</a-radio-button>
       <a-radio-button value="halfday">12小时</a-radio-button>
        <a-radio-button value="sixhour">6小时</a-radio-button>
        <a-radio-button value="threehour">3小时</a-radio-button>
      <a-radio-button value="hour">1小时</a-radio-button>
    </a-radio-group>
    <div id='recoard' :style="{ height: conheight + 'px' }"></div>
  </div>
</template>
<script>
import Vue from 'vue'
import echarts from 'echarts'

export default {
  name: 'SunWatch',
  data () {
    return {
      resData: [],
      postData: [],
      msg: '--',
      charts: '',
      opinionData: [],
      tempertuer: [],
      timeline: [],
      conheight: 0,
      timedis: 'all'
    }
  },
  mounted () {
    this.doGetData()
    console.log('=============mounted==============')
  },
  created () {
    this.getHeight()
    console.log('=============created==============')
  },
  methods: {
    handleDisChange (e) {
      this.timedis = e.target.value
      this.doGetData()
    },
    getHeight () {
      this.conheight = window.innerHeight - 100
      console.log('----------------------->>>>' + this.conheight)
    },
    drawLine (id) {
      console.log('------drawLine-------------')
      this.charts = echarts.init(document.getElementById(id))
      this.charts.setOption({
        tooltip: {
          trigger: 'axis'
        },
        legend: {
          data: []
        },
        grid: {
          left: '3%',
          right: '4%',
          bottom: '3%',
          containLabel: true
        },

        toolbox: {
          feature: {
          }
        },
        xAxis: {
          type: 'category',
          boundaryGap: false,
          data: this.timeline // ['1', '2', '3', '4', '5']
        },
        yAxis: {
          type: 'value'
        },

        series: [{
          name: '电压',
          type: 'line',
          stack: '',
          data: this.opinionData
        },
        {
          name: '温度',
          type: 'line',
          stack: '',
          data: this.tempertuer // [15,10,9,8,6,5.9,0,-1,5]
        }]
      })
      console.log('>------drawLine-------------<')
    },
    getMinutes () {
      if (this.timedis === 'all') {
        return null
      } else if (this.timedis === 'day') {
        return 24 * 60
      } else if (this.timedis === 'halfday') {
        return 12 * 60
      } else if (this.timedis === 'sixhour') {
        return 6 * 60
      } else if (this.timedis === 'threehour') {
        return 3 * 60
      } else {
        return 60
      }
    },
    doGetData () {
      Vue.axios.get('https://www.imooci.com/api/getData', {
        params: {
          name: 'pi',
          age: 45,
          dis: this.getMinutes()
        }
      }).then((response) => {
        console.log('-------------------------------')
        console.log(response.data)
        console.log(response.data.res)
        this.opinionData = response.data.res
        this.timeline = response.data.timeline
        this.tempertuer = response.data.tempertuer

        this.drawLine('recoard')
        console.log('-------------------------------')
      }).catch((error) => {
        console.log(error)
      })
    },
    doPost () {
      Vue.axios.post('/api/post', {
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
<style scoped>
</style>

import axios from 'axios'
axios.defaults.baseURL = '/' // 动态配置网络请求不同环境下的地址
export function get (url, params = {}, context) { // context vue 实例
}

export function post (url, params = {}, context) {
}

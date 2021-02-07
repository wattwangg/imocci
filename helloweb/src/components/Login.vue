<template>
   <div>
    <img src="../assets/logo.png">
    <a-form layout="horizontal" :form="form" @submit="handleSubmit" class="login-form">
        <a-form-item :validate-status="userNameError() ? 'error' : ''" :help="userNameError() || ''">
        <a-input
            v-decorator="[
            'userName',
            { rules: [{ required: true, message: 'Please input your username!' }] },
            ]"
            placeholder="Username"
        >
            <a-icon slot="prefix" type="user" style="color:rgba(0,0,0,.25)" />
        </a-input>
        </a-form-item>
        <a-form-item :validate-status="passwordError() ? 'error' : ''" :help="passwordError() || ''">
        <a-input
            v-decorator="[
            'password',
            { rules: [{ required: true, message: 'Please input your Password!' }] },
            ]"
            type="password"
            placeholder="Password"
        >
            <a-icon slot="prefix" type="lock" style="color:rgba(0,0,0,.25)" />
        </a-input>
        </a-form-item>
        <a-form-item>
        <a-button type="primary" html-type="submit" :disabled="hasErrors(form.getFieldsError())">
            Log in
        </a-button>
        </a-form-item>
    </a-form>
   </div>
</template>
<script>
import Vue from 'vue'
function hasErrors (fieldsError) {
  return Object.keys(fieldsError).some(field => fieldsError[field])
}
export default {
  name: 'Login',
  data () {
    return {
      resData: [],
      postData: [],
      hasErrors,
      form: this.$form.createForm(this, { name: 'horizontal_login' })
    }
  },
  created () {
  },
  mounted () {
    this.$nextTick(() => {
      // To disabled submit button at the beginning.
      this.form.validateFields()
    })
  },
  // vue高版本中，推荐使用axios进行网络请求，而不再使用vue-resource
  methods: {
    // Only show error after a field is touched.
    userNameError () {
      const { getFieldError, isFieldTouched } = this.form
      return isFieldTouched('userName') && getFieldError('userName')
    },
    // Only show error after a field is touched.
    passwordError () {
      const { getFieldError, isFieldTouched } = this.form
      return isFieldTouched('password') && getFieldError('password')
    },
    handleSubmit (e) {
      e.preventDefault()
      this.form.validateFields((err, values) => {
        if (!err) {
          console.log('Received values of form: ', values)
        }
      })
      localStorage.setItem('token', 'pi')
      this.$router.push('/')
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
<style>
.login-form {
  align-content: center;
  margin-left: 40%;
  margin-right: 40%;
}
</style>

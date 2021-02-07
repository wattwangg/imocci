<template>
  <div id="app">
    <app-header v-if="false"></app-header>
    <!--img src="./assets/logo.png"-->
    <router-view/>
    <app-footer></app-footer>
  </div>
</template>

<script>
import Header from './components/header'
import Footer from './components/footer'
export default {
  name: 'App',
  components: {
    'app-header': Header,
    'app-footer': Footer
  }
}
</script>

<style>
#app {
  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  text-align: center;
  position: absolute;
  top: 0;
  right: 0;
  bottom: 0;
  left: 0;
}
#router-view {
  width: 100%;
  position: absolute;
  bottom: 72px;
}
#app-footer {
  width: 100%;
  background-color:lightgray;
  font-family: Verdana, Arial;
  color: lightgray;
  text-align: center;
  height: 72px;
  position: absolute;
  bottom: 0;
}
</style>

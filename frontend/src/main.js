import './assets/main.css'

import { createApp } from 'vue'
import {createPinia} from "pinia";
import App from './App.vue'
import router from "@/router/index.js";
import ApexCharts from "vue3-apexcharts";


const app = createApp(App)

app.component("apexchart", ApexCharts);
app.use(router)
app.mount('#app')
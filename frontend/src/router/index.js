import { createRouter, createWebHistory } from 'vue-router'
import Dashboard from '../views/Dashboard.vue'
import Configs from '../views/Configs.vue'
import CreateConfig from '../views/CreateConfig.vue'

const routes = [
    { path: '/', name: 'Dashboard', component: Dashboard },
    { path: '/configs', name: 'Configs', component: Configs },
    { path: '/create', name: 'CreateConfig', component: CreateConfig },
]

const router = createRouter({
    history: createWebHistory(),
    routes,
})

export default router

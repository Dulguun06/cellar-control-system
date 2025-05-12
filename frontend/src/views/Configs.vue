<template>
  <div class="container py-4">
    <h2 class="mb-4 fw-bold text-primary">🛠️ Configuration Management</h2>

    <!-- Active Config -->
    <div v-if="activeConfig" class="alert alert-primary d-flex align-items-center gap-2 shadow-sm">
      <i class="bi bi-check-circle-fill"></i>
      <div><strong>Active:</strong> {{ activeConfig.name }}</div>
    </div>

    <!-- Config Table -->
    <div class="table-responsive shadow-sm rounded bg-white p-3 mb-4">
      <table class="table table-hover align-middle">
        <thead class="table-light">
        <tr>
          <th>Name</th>
          <th>Temp Min</th>
          <th>Temp Max</th>
          <th>Humidity Min</th>
          <th>Humidity Max</th>
          <th>Actions</th>
        </tr>
        </thead>
        <tbody>
        <tr v-for="config in configs" :key="config.id">
          <td>{{ config.name }}</td>
          <td>{{ config.minTemp }}</td>
          <td>{{ config.maxTemp }}</td>
          <td>{{ config.minHum }}</td>
          <td>{{ config.maxHum }}</td>
          <td class="d-flex gap-1">
            <button class="btn btn-outline-success btn-sm" @click="apply(config.id)" title="Apply this config">
              <i class="bi bi-check2-circle"></i>
            </button>
            <button class="btn btn-outline-warning btn-sm" @click="edit(config)" title="Edit config">
              <i class="bi bi-pencil-square"></i>
            </button>
            <button class="btn btn-outline-danger btn-sm" @click="remove(config.id)" title="Delete config">
              <i class="bi bi-trash3"></i>
            </button>
          </td>
        </tr>
        </tbody>
      </table>
    </div>

    <!-- Config Form -->
    <div class="card shadow-sm">
      <div class="card-body">
        <h4 class="card-title mb-3 text-secondary">
          {{ form.id ? '✏️ Edit Configuration' : '➕ Create New Configuration' }}
        </h4>
        <form @submit.prevent="save">
          <div class="row mb-3">
            <div class="col-md-6">
              <label class="form-label fw-semibold">Config Name</label>
              <input v-model="form.name" class="form-control" required />
            </div>
          </div>
          <div class="row">
            <div class="col-md-3">
              <label class="form-label fw-semibold">Temp Min (°C)</label>
              <input type="number" v-model.number="form.minTemp" class="form-control" required />
            </div>
            <div class="col-md-3">
              <label class="form-label fw-semibold">Temp Max (°C)</label>
              <input type="number" v-model.number="form.maxTemp" class="form-control" required />
            </div>
            <div class="col-md-3">
              <label class="form-label fw-semibold">Humidity Min (%)</label>
              <input type="number" v-model.number="form.minHum" class="form-control" required />
            </div>
            <div class="col-md-3">
              <label class="form-label fw-semibold">Humidity Max (%)</label>
              <input type="number" v-model.number="form.maxHum" class="form-control" required />
            </div>
          </div>
          <div class="mt-4 d-flex gap-2">
            <button type="submit" class="btn btn-primary">
              <i class="bi" :class="form.id ? 'bi-save2' : 'bi-plus-circle'"></i>
              {{ form.id ? 'Update' : 'Add' }}
            </button>
            <button type="button" class="btn btn-secondary" @click="reset">
              <i class="bi bi-x-circle"></i> Cancel
            </button>
          </div>
        </form>
      </div>
    </div>
  </div>
</template>

<script setup>
import { onMounted, ref } from 'vue'
import {
  addConfig, applyConfig, deleteConfig,
  fetchActiveConfig, getAllConfigs, updateConfig
} from '@/service/api'

const configs = ref([])
const activeConfig = ref(null)

const form = ref({
  id: null,
  name: '',
  minTemp: 0,
  maxTemp: 0,
  minHum: 0,
  maxHum: 0
})

const fetchConfigs = async () => {
  try {
    const res = await getAllConfigs()
    const active = await fetchActiveConfig()
    activeConfig.value = active.data
    configs.value = res.data
  } catch (error) {
    console.error('Error fetching configs:', error)
  }
}

const apply = async (id) => {
  try {
    await applyConfig(id)
    activeConfig.value = configs.value.find(c => c.id === id)
  } catch (error) {
    console.error('Error applying config:', error)
  }
}

const save = async () => {
  try {
    if (form.value.id) {
      await updateConfig(form.value.id, form.value)
    } else {
      await addConfig(form.value)
    }
    await fetchConfigs()
    reset()
  } catch (error) {
    console.error('Error saving config:', error)
  }
}

const remove = async (id) => {
  try {
    await deleteConfig(id)
    await fetchConfigs()
  } catch (error) {
    console.error('Error removing config:', error)
  }
}

const edit = (config) => {
  form.value = { ...config }
}

const reset = () => {
  form.value = {
    id: null,
    name: '',
    minTemp: 0,
    maxTemp: 0,
    minHum: 0,
    maxHum: 0
  }
}

onMounted(fetchConfigs)
</script>

<style scoped>
.table td,
.table th {
  vertical-align: middle;
}
</style>

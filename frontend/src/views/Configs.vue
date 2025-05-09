<template>
  <div class="container">
    <h2 class="my-3">Configuration Management</h2>

    <!-- Active Config Display -->
    <div v-if="activeConfig" class="alert alert-success">
      <strong>Active Config:</strong> {{ activeConfig.name }}
    </div>

    <!-- Config Table -->
    <table class="table table-bordered mt-3">
      <thead>
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
        <td>
          <button class="btn btn-success btn-sm" @click="apply(config.id)">Apply</button>
          <button class="btn btn-warning btn-sm" @click="edit(config)">Edit</button>
          <button class="btn btn-danger btn-sm" @click="remove(config.id)">Delete</button>
        </td>
      </tr>
      </tbody>
    </table>

    <!-- Add/Edit Form -->
    <form @submit.prevent="save" class="mt-4">
      <h4>{{ form.id ? 'Edit Config' : 'Create New Config' }}</h4>
      <div class="row">
        <div class="col">
          <p class="fw-semibold">Config Name</p>
          <input v-model="form.name" placeholder="Name" class="form-control mb-2" required/>
        </div>
      </div>
      <div class="row">
        <div class="col">
          <p class="fw-semibold">Temp min</p>
          <input v-model.number="form.minTemp" placeholder="Temp Min" class="form-control mb-2" required/>
          <p class="fw-semibold">Temp max</p>
          <input v-model.number="form.maxTemp" placeholder="Temp Max" class="form-control mb-2" required/>
        </div>
        <div class="col">
          <p class="fw-semibold">Humidity min</p>
          <input v-model.number="form.minHum" placeholder="Humidity Min" class="form-control mb-2" required/>
          <p class="fw-semibold">Humidity max</p>
          <input v-model.number="form.maxHum" placeholder="Humidity Max" class="form-control mb-2" required/>
        </div>
      </div>
      <button type="submit" class="btn btn-primary">{{ form.id ? 'Update' : 'Add' }}</button>
      <button type="button" class="btn btn-secondary ms-2" @click="reset">Cancel</button>
    </form>
  </div>
</template>

<script setup>
import {onMounted, ref} from "vue"
import {addConfig, applyConfig, deleteConfig, fetchActiveConfig, getAllConfigs, updateConfig,} from "@/service/api"

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
  const res = await getAllConfigs()
  const active = await fetchActiveConfig()
  activeConfig.value = active.data
  configs.value = res.data
}

const apply = async (id) => {
  await applyConfig(id)
  activeConfig.value = configs.value.find(c => c.id === id)
}

const save = async () => {
  if (form.value.id) {
    await updateConfig(form.value.id, form.value)
  } else {
    await addConfig(form.value)
  }
  await fetchConfigs()
  reset()
}

const remove = async (id) => {
  await deleteConfig(id)
  await fetchConfigs()
}

const edit = (config) => {
  form.value = {...config}
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
.container {
  padding-top: 20px;
}
</style>

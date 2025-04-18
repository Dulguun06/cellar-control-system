<template>
  <div class="p-6">
    <h1 class="text-2xl font-bold mb-4">Cellar Dashboard</h1>

    <div class="grid grid-cols-2 gap-6">
      <!-- Sensor 1 -->
      <div class="bg-white rounded-2xl shadow p-4">
        <h2 class="text-xl font-semibold mb-2">Sensor 1</h2>
        <p>🌡 Temp: {{ sensorData.temperature }} °C</p>
        <p>💧 Humidity: {{ sensorData.humidity }} %</p>
        <p>🔥 Heater:
          <span :class="relayStatus.heater ? 'text-green-600' : 'text-red-600'">
            {{ relayStatus.heater ? 'ON' : 'OFF' }}
          </span>
        </p>
        <p>❄️ Cooler:
          <span :class="relayStatus.cooler ? 'text-green-600' : 'text-red-600'">
            {{ relayStatus.cooler ? 'ON' : 'OFF' }}
          </span>
        </p>
        <p>💦 Humidifier:
          <span :class="relayStatus.humidifier ? 'text-green-600' : 'text-red-600'">
            {{ relayStatus.humidifier ? 'ON' : 'OFF' }}
          </span>
        </p>
        <p>💨 Ventilation:
          <span :class="relayStatus.ventilation ? 'text-green-600' : 'text-red-600'">
            {{ relayStatus.ventilation ? 'ON' : 'OFF' }}
          </span>
        </p>
      </div>
    </div>
  </div>
</template>

<script setup>
import {ref, onMounted, onBeforeUnmount} from 'vue'
import { getLatestSensorData, getRelayStatus } from "@/service/api.js"

const sensorData = ref({ temperature: 0, humidity: 0 })

const relayStatus = ref({
  cooler: false,
  heater: false,
  humidifier: false,
  ventilation: false
})

const fetchSensorData = async () => {
  try {
    const res = await getLatestSensorData(1)
    sensorData.value = res.data
  } catch (err) {
    console.error('Sensor data fetch failed', err)
  }
}

const fetchRelayStatus = async () => {
  try {
    const res = await getRelayStatus()
    relayStatus.value = res.data
  } catch (err) {
    console.error('Relay status fetch failed', err)
  }
}
let intervalId = null

onMounted(() => {
  fetchSensorData()
  fetchRelayStatus()

  intervalId = setInterval(() => {
    fetchSensorData()
    fetchRelayStatus()
  }, 1000) // every second
})

onBeforeUnmount(() => {
  clearInterval(intervalId)
})
</script>

<style scoped>
/* optional additional styling */
</style>

<template>
  <div class="p-6">
    <h1 class="text-2xl font-bold mb-4">Cellar Dashboard</h1>

    <div class="grid grid-cols-2 gap-6">
      <!-- Sensor 1 -->
      <div class="bg-white rounded-2xl shadow p-4">
        <h2 class="text-xl font-semibold mb-2">Sensor 1</h2>
        <p>🌡 Temp: {{ sensor1Data.temperature }} °C</p>
        <p>💧 Humidity: {{ sensor1Data.humidity }} %</p>
        <p>🔥 Heater: <span :class="relayStatus.relay1 ? 'text-green-600' : 'text-red-600'">{{ relayStatus.relay1 ? 'ON' : 'OFF' }}</span></p>
        <p>🌀 Ventilation: <span :class="relayStatus.relay3 ? 'text-green-600' : 'text-red-600'">{{ relayStatus.relay3 ? 'ON' : 'OFF' }}</span></p>
      </div>

<!--      &lt;!&ndash; Sensor 2 &ndash;&gt;-->
<!--      <div class="bg-white rounded-2xl shadow p-4">-->
<!--        <h2 class="text-xl font-semibold mb-2">Sensor 2</h2>-->
<!--        <p>🌡 Temp: {{ sensorData.temperature2 }} °C</p>-->
<!--        <p>💧 Humidity: {{ sensorData.humidity2 }} %</p>-->
<!--        <p>🔥 Heater: <span :class="relayStatus.relay2 ? 'text-green-600' : 'text-red-600'">{{ relayStatus.relay2 ? 'ON' : 'OFF' }}</span></p>-->
<!--        <p>🌀 Ventilation: <span :class="relayStatus.relay4 ? 'text-green-600' : 'text-red-600'">{{ relayStatus.relay4 ? 'ON' : 'OFF' }}</span></p>-->
<!--      </div>-->
    </div>
  </div>
</template>

<!------------SCRIPT-------------->

<script setup>
import { ref, onMounted } from 'vue'
import { getLatestSensorData, getRelayStatus } from "@/service/api.js"

const sensor1Data = ref({ temperature: 0, humidity: 0 })
const sensor2Data = ref({ temperature: 0, humidity: 0 })

const relayStatus = ref({
  relay1: false,
  relay2: false,
  relay3: false,
  relay4: false
})

const fetchSensorData = async () => {
  try {
    const res1 = await getLatestSensorData(1)
    const res2 = await getLatestSensorData(2)
    sensor1Data.value = res1.data
    sensor2Data.value = res2.data
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

onMounted(() => {
  fetchSensorData()
  fetchRelayStatus()
})

</script>

<style scoped>
/* optional additional styling */
</style>

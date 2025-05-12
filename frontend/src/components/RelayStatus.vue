<template>
  <div class="container text-center py-4 rounded shadow-sm bg-light">
    <!-- Mode Switch Buttons -->
    <div class="mb-4">
      <label class="fw-semibold fs-5 mb-2">Mode</label>
      <div>
        <button
            :class="['btn', isAuto ? 'btn-primary' : 'btn-outline-primary', 'rounded-pill', 'px-4', 'me-2']"
            @click="switchToAuto"
        >
          Auto
        </button>
        <button
            :class="['btn', !isAuto ? 'btn-primary' : 'btn-outline-primary', 'rounded-pill', 'px-4']"
            @click="switchToManual"
        >
          Manual
        </button>
      </div>
    </div>

    <!-- Manual Switch Controls -->
    <div v-if="!isAuto" class="card p-3 shadow-sm mb-4 bg-white">
      <div class="d-flex flex-wrap justify-content-center">
        <div
            class="form-check form-switch d-inline-block mx-3 my-2"
            v-for="(name, key) in relayNames"
            :key="key"
        >
          <input class="form-check-input" type="checkbox" v-model="relayStatus[key]" @change="sendManualControl">
          <label class="form-check-label ms-1">{{ name }}</label>
        </div>
      </div>
    </div>

    <!-- LED Status Indicators -->
    <div class="row">
      <div
          v-for="(status, key) in relayStatus"
          :key="key"
          class="col-6 col-sm-6 col-md-3 mb-4 d-flex flex-column align-items-center"
      >
        <p class="fw-semibold fs-5 mb-2">{{ relayNames[key] }}</p>
        <div
            :class="status ? 'bg-green-led' : 'bg-red-led'"
            class="rounded-circle shadow led-circle"
        ></div>
        <p class="mt-2 fw-medium">{{ status ? 'ON' : 'OFF' }}</p>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount } from "vue";
import { getRelayStatus, sendManualCommand, setMode } from "@/service/api";

const isAuto = ref(true)

const switchToAuto = async () => {
  isAuto.value = true
  await setMode(true)
}

const switchToManual = async () => {
  isAuto.value = false
  await setMode(false)
}

const sendManualControl = async () => {
  await sendManualCommand({ ...relayStatus.value })
}

const relayStatus = ref({
  heater: false,
  cooler: false,
  humidifier: false,
  ventilation: false,
})

const relayNames = {
  heater: "🔥 Heater",
  cooler: "❄️ Cooler",
  humidifier: "💧 Humidifier",
  ventilation: "🌬️ Ventilation",
}

const fetchRelayStatus = async () => {
  try {
    const res = await getRelayStatus()
    const data = res.data
    relayStatus.value = {
      heater: data.heater,
      cooler: data.cooler,
      humidifier: data.humidifier,
      ventilation: data.ventilation,
    }
  } catch (error) {
    console.error("Failed to fetch relay status:", error)
  }
}

let intervalId
onMounted(() => {
  fetchRelayStatus()
  intervalId = setInterval(fetchRelayStatus, 1000)
})
onBeforeUnmount(() => {
  clearInterval(intervalId)
})
</script>

<style scoped>
.container {
  max-width: 900px;
  background: linear-gradient(to right, #f9f9f9, #eef1f5);
}

.led-circle {
  width: 5rem;
  height: 5rem;
  transition: background-color 0.3s ease, box-shadow 0.3s ease;
}

.bg-green-led {
  background-color: limegreen;
  box-shadow: 0 0 20px 5px rgba(0, 255, 0, 0.6);
  animation: pulse 1s infinite;
}

.bg-red-led {
  background-color: crimson;
  box-shadow: 0 0 20px 5px rgba(255, 0, 0, 0.6);
}

@keyframes pulse {
  0% {
    transform: scale(1);
    opacity: 1;
  }
  50% {
    transform: scale(1.1);
    opacity: 0.75;
  }
  100% {
    transform: scale(1);
    opacity: 1;
  }
}
</style>

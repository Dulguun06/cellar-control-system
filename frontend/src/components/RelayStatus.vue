<template>
  <div class="container text-center py-4">
    <div class="row">
      <div
          v-for="(status, key) in relayStatus"
          :key="key"
          class="col-md-3 col-6 mb-4 d-flex flex-column align-items-center"
      >
        <p class="fw-semibold fs-5 mb-2">{{ relayNames[key] }}</p>
        <div
            :class="status ? 'bg-green-led' : 'bg-red-led'"
            class="rounded-circle shadow"
            style="width: 5rem; height: 5rem;"
        ></div>
        <p class="mt-2 fw-medium">{{ status ? 'ON' : 'OFF' }}</p>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount } from "vue";
import { getRelayStatus } from "@/service/api";

const relayStatus = ref({
  heater: false,
  cooler: false,
  humidifier: false,
  ventilation: false,
});

const relayNames = {
  heater: "Heater",
  cooler: "Cooler",
  humidifier: "Humidifier",
  ventilation: "Ventilation",
};

const fetchRelayStatus = async () => {
  try {
    const res = await getRelayStatus();
    const data = res.data;
    relayStatus.value = {
      heater: data.heater,
      cooler: data.cooler,
      humidifier: data.humidifier,
      ventilation: data.ventilation,
    };
  } catch (error) {
    console.error("Failed to fetch relay status:", error);
  }
};

let intervalId;
onMounted(() => {
  fetchRelayStatus();
  intervalId = setInterval(fetchRelayStatus, 1000);
});
onBeforeUnmount(() => {
  clearInterval(intervalId);
});
</script>

<style scoped>
.bg-green-led {
  background-color: limegreen;
  box-shadow: 0 0 20px 5px rgba(0, 255, 0, 0.6);
}

.bg-red-led {
  background-color: crimson;
  box-shadow: 0 0 20px 5px rgba(255, 0, 0, 0.6);
}
</style>

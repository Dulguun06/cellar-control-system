<template>
  <div class="flex flex-col items-center gap-12 py-8">
    <!-- Temperature Donut Chart -->
    <div class="flex flex-col items-center">
      <h2 class="text-2xl font-bold text-gray-700 mb-4">🌡 Temperature</h2>
      <div class="relative w-48 h-48">
        <canvas ref="tempChartRef"></canvas>
        <div class="absolute inset-0 flex items-center justify-center text-xl font-semibold text-gray-800">
          {{ sensorData.temperature }}°C
        </div>
      </div>
    </div>

    <!-- Humidity Donut Chart -->
    <div class="flex flex-col items-center">
      <h2 class="text-2xl font-bold text-gray-700 mb-4">💧 Humidity</h2>
      <div class="relative w-48 h-48">
        <canvas ref="humidityChartRef"></canvas>
        <div class="absolute inset-0 flex items-center justify-center text-xl font-semibold text-gray-800">
          {{ sensorData.humidity }}%
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount } from "vue";
import { Chart } from "chart.js/auto";
import { getLatestSensorData } from "@/service/api";

const tempChartRef = ref(null);
const humidityChartRef = ref(null);
const sensorData = ref({
  temperature: 0,
  humidity: 0
});

let chartInstanceTemp = null;
let chartInstanceHumidity = null;

// Helper: pick color based on value ranges
const getColor = (value, type) => {
  if (type === 'temperature') {
    if (value < 15) return '#00bcd4'; // Cold - Blue
    if (value < 25) return '#4caf50'; // Normal - Green
    return '#f44336'; // Hot - Red
  }
  if (type === 'humidity') {
    if (value < 30) return '#ff9800'; // Dry - Orange
    if (value < 60) return '#4caf50'; // Normal - Green
    return '#2196f3'; // Wet - Blue
  }
  return '#9e9e9e'; // Default gray
};

const createDonutChart = (refElement, data, type) => {
  return new Chart(refElement, {
    type: 'doughnut',
    data: {
      datasets: [{
        data: [data, 100 - data],
        backgroundColor: [getColor(data, type), '#e0e0e0'],
        borderWidth: 0,
        cutout: '75%',
      }]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      plugins: {
        tooltip: {
          enabled: true
        },
        legend: {
          display: true
        },
      }
    }
  });
};

const updateCharts = () => {
  if (chartInstanceTemp) {
    chartInstanceTemp.data.datasets[0].data = [sensorData.value.temperature, 100 - sensorData.value.temperature];
    chartInstanceTemp.data.datasets[0].backgroundColor[0] = getColor(sensorData.value.temperature, 'temperature');
    chartInstanceTemp.update();
  }
  if (chartInstanceHumidity) {
    chartInstanceHumidity.data.datasets[0].data = [sensorData.value.humidity, 100 - sensorData.value.humidity];
    chartInstanceHumidity.data.datasets[0].backgroundColor[0] = getColor(sensorData.value.humidity, 'humidity');
    chartInstanceHumidity.update();
  }
};

const fetchSensorData = async () => {
  try {
    const res = await getLatestSensorData();
    sensorData.value = res.data;
    updateCharts();
  } catch (error) {
    console.error("Failed to fetch sensor data:", error);
  }
};

let intervalId = null;

onMounted(() => {
  chartInstanceTemp = createDonutChart(tempChartRef.value, sensorData.value.temperature, 'temperature');
  chartInstanceHumidity = createDonutChart(humidityChartRef.value, sensorData.value.humidity, 'humidity');
  intervalId = setInterval(fetchSensorData, 1000);
});

onBeforeUnmount(() => {
  clearInterval(intervalId);
  if (chartInstanceTemp) chartInstanceTemp.destroy();
  if (chartInstanceHumidity) chartInstanceHumidity.destroy();
});
</script>

<style scoped>
canvas {
  width: 94% !important;
  height: 94% !important;
}
</style>

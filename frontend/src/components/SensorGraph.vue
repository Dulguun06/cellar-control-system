<template>
  <div class="grid grid-cols-1 md:grid-cols-2 gap-6 p-4">
    <!-- Temperature Chart Card -->
    <div class="bg-white rounded-2xl shadow-lg p-4 flex flex-col items-center">
      <h2 class="text-lg font-semibold text-gray-700 mb-4">📈 Temperature History</h2>
      <canvas ref="tempGraphRef" class="w-full h-64"></canvas>
    </div>

    <!-- Humidity Chart Card -->
    <div class="bg-white rounded-2xl shadow-lg p-4 flex flex-col items-center">
      <h2 class="text-lg font-semibold text-gray-700 mb-4">💧 Humidity History</h2>
      <canvas ref="humidityGraphRef" class="w-full h-64"></canvas>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount } from "vue";
import { Chart } from "chart.js/auto";
import { getAllSensorData } from "@/service/api";

const tempGraphRef = ref(null);
const humidityGraphRef = ref(null);

const labels = ref([]);
const tempData = ref([]);
const humidityData = ref([]);

let tempChart = null;
let humidityChart = null;

const createLineGraph = (refElement, labels, data, label, color, chartInstance) => {
  if (chartInstance) chartInstance.destroy();

  return new Chart(refElement, {
    type: 'line',
    data: {
      labels,
      datasets: [{
        label,
        data,
        borderColor: color,
        backgroundColor: `${color}33`,
        tension: 0.3,
        fill: false,
        pointRadius: 1,
        pointHoverRadius: 4
      }]
    },
    options: {
      responsive: true,
      animation: false,
      plugins: {
        legend: {
          display: true,
          labels: {
            color: '#555',
            font: {
              size: 12,
              weight: '500',
            }
          }
        }
      },
      scales: {
        x: {
          ticks: {
            color: '#666',
            font: { size: 11 }
          },
          grid: {
            color: '#f0f0f0'
          }
        },
        y: {
          beginAtZero: true,
          ticks: {
            color: '#666',
            font: { size: 11 }
          },
          grid: {
            color: '#f0f0f0'
          }
        }
      }
    }
  });
};

const fetchSensorData = async () => {
  try {
    const res = await getAllSensorData();
    const data = res.data;

    labels.value = data.map((entry) => new Date(entry.timestamp).toLocaleTimeString());
    tempData.value = data.map((entry) => entry.temperature);
    humidityData.value = data.map((entry) => entry.humidity);

    if (tempGraphRef.value && humidityGraphRef.value) {
      tempChart = createLineGraph(tempGraphRef.value, labels.value, tempData.value, 'Temperature (°C)', 'rgba(255, 99, 132, 1)', tempChart);
      humidityChart = createLineGraph(humidityGraphRef.value, labels.value, humidityData.value, 'Humidity (%)', 'rgba(54, 162, 235, 1)', humidityChart);
    }
  } catch (error) {
    console.error("Failed to fetch sensor data:", error);
  }
};

let intervalId = null;

onMounted(() => {
  fetchSensorData();
  intervalId = setInterval(fetchSensorData, 1000);
});

onBeforeUnmount(() => {
  clearInterval(intervalId);
  if (tempChart) tempChart.destroy();
  if (humidityChart) humidityChart.destroy();
});
</script>

<style scoped>
canvas {
  max-width: 100%;
}
</style>

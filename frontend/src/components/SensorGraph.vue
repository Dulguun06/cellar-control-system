<template>
  <div class="grid grid-cols-2 gap-6">
    <div class="w-full">
      <canvas ref="tempGraphRef"></canvas>
    </div>
    <div class="w-full">
      <canvas ref="humidityGraphRef"></canvas>
    </div>
  </div>
</template>

<script setup>
import {ref, onMounted, onBeforeUnmount} from "vue";
import {Chart} from "chart.js/auto";
import {getAllSensorData} from "@/service/api"; // Ensure this is the correct path

const tempGraphRef = ref(null);
const humidityGraphRef = ref(null);

const labels = ref([]);
const tempData = ref([]);
const humidityData = ref([]);
let tempChart = null; // Store the temperature chart instance
let humidityChart = null; // Store the humidity chart instance

// Function to create line graph
const createLineGraph = (refElement, labels, data, label, color, chartInstance) => {

  refElement.height = 150;
  // If a chart instance exists, destroy it before creating a new one
  if (chartInstance) {
    chartInstance.destroy();
  }

  return new Chart(refElement, {
    type: 'line',
    data: {
      labels,
      datasets: [{
        label,
        data,
        borderColor: color,
        backgroundColor: `${color}50`,
        tension: 0.3,
        pointStyle: 'none', // Remove the circle markers
        radius: 0,
      }]
    },
    options: {
      responsive: true,
      animation: false,
      scales: {
        y: {
          beginAtZero: true,
        }
      }
    }
  });
};

// Fetch all sensor data
const fetchSensorData = async () => {
  try {
    const res = await getAllSensorData();
    const data = res.data;

    // Update labels and data for temperature and humidity
    labels.value = data.map((entry) => new Date(entry.timestamp).toLocaleTimeString());
    tempData.value = data.map((entry) => entry.temperature);
    humidityData.value = data.map((entry) => entry.humidity);

    // Update graphs with new data
    if (tempGraphRef.value && humidityGraphRef.value) {
      tempChart = createLineGraph(tempGraphRef.value, labels.value, tempData.value, 'Temperature (°C)', 'rgba(255, 99, 132, 1)', tempChart);
      humidityChart = createLineGraph(humidityGraphRef.value, labels.value, humidityData.value, 'Humidity (%)', 'rgba(54, 162, 235, 1)', humidityChart);
    }
  } catch (error) {
    console.error("Failed to fetch sensor data:", error);
  }
};

// Set interval to fetch sensor data every 5 seconds (or adjust as needed)
let intervalId = null;

onMounted(() => {
  fetchSensorData(); // Fetch data initially

  // Fetch new sensor data every 1 second (or adjust the timing as needed)
  intervalId = setInterval(fetchSensorData, 1000);
});

onBeforeUnmount(() => {
  clearInterval(intervalId); // Clear the interval when the component is unmounted

  // Destroy charts if they exist
  if (tempChart) tempChart.destroy();
  if (humidityChart) humidityChart.destroy();
});
</script>

<style scoped>
canvas {
  max-width: 100%;
}
</style>

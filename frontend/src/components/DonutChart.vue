<template>
  <div class="flex flex-col items-center row-gap-5">
    <!-- Temperature Donut Chart -->
    <div class="flex flex-col items-center">
      <h2 class="text-xl font-semibold mb-2">Temperature</h2>
      <div class="w-48 h-48">
        <canvas ref="tempChartRef"></canvas>
      </div>
      <p class="text-lg font-semibold mt-2">{{ sensorData.temperature }} °C</p>
    </div>

    <!-- Humidity Donut Chart -->
    <div class="flex flex-col items-center" id="hum">
      <h2 class="text-xl font-semibold mb-2">Humidity</h2>
      <div class="w-48 h-48">
        <canvas ref="humidityChartRef"></canvas>
      </div>
      <p class="text-lg font-semibold mt-2">{{ sensorData.humidity }} %</p>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount } from "vue";
import { Chart } from "chart.js/auto";
import { getLatestSensorData } from "@/service/api"; // Make sure to update this path according to your project structure

const tempChartRef = ref(null);
const humidityChartRef = ref(null);
const sensorData = ref({
  temperature: 0, // Default value before data is fetched
  humidity: 0     // Default value before data is fetched
});
let chartInstanceTemp = null;
let chartInstanceHumidity = null;

// Function to create the donut chart
const createDonutChart = (refElement, data, label, color) => {
  return new Chart(refElement, {
    type: 'doughnut',
    data: {
      labels: [label],
      datasets: [{
        data: [data, 100 - data],
        backgroundColor: [color, '#e0e0e0'],
        borderWidth: 0,
      }]
    },
    options: {
      responsive: true,
      plugins: {
        tooltip: {
          enabled: false
        },
        legend: {
          display: false
        },
        datalabels: {
          display: true,
          color: '#333',
          font: {
            weight: 'bold',
            size: 18,
          },
          formatter: (value) => value + '%', // Display percentage in the center
        }
      }
    }
  });
};

// Fetch latest sensor data
const fetchSensorData = async () => {
  try {
    const res = await getLatestSensorData(1); // 1 is the sensorId (use the appropriate ID)
    sensorData.value = res.data; // Assuming the response data contains temperature and humidity
    // Update the charts with new data
    if (chartInstanceTemp) {
      chartInstanceTemp.data.datasets[0].data = [sensorData.value.temperature, 100 - sensorData.value.temperature];
      chartInstanceTemp.update();
    }
    if (chartInstanceHumidity) {
      chartInstanceHumidity.data.datasets[0].data = [sensorData.value.humidity, 100 - sensorData.value.humidity];
      chartInstanceHumidity.update();
    }
  } catch (error) {
    console.error("Failed to fetch sensor data:", error);
  }
};

// Set interval to fetch sensor data every second
let intervalId = null;

onMounted(() => {
  // Create initial charts
  chartInstanceTemp = createDonutChart(tempChartRef.value, sensorData.value.temperature, 'Temp', 'rgba(255, 99, 132, 1)');
  chartInstanceHumidity = createDonutChart(humidityChartRef.value, sensorData.value.humidity, 'Humidity', 'rgba(54, 162, 235, 1)');

  // Start fetching sensor data every second
  intervalId = setInterval(fetchSensorData, 1000);
});

onBeforeUnmount(() => {
  // Clear interval when component is unmounted
  clearInterval(intervalId);

  // Destroy charts when unmounted
  if (chartInstanceTemp) chartInstanceTemp.destroy();
  if (chartInstanceHumidity) chartInstanceHumidity.destroy();
});
</script>

<style scoped>
canvas {
  max-width: 100%;
}
#hum{
  margin-top: 50%;
}
</style>
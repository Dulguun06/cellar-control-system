<template>
  <div class="container py-4">
    <h1 class="text-center mb-5 display-5 fw-bold">Cellar Dashboard</h1>

    <!-- Top section: Donut chart (1/3) + Sensor graphs (2/3) -->
    <div class="row mb-3">
      <div class="col-md-3 mb-3 mb-md-0">
        <DonutChart />
      </div>
      <div class="col-md-9">
        <SensorGraph />
      </div>
    </div>

    <!-- Bottom section: Relay status -->
    <div class="row">
      <div class="col">
        <RelayStatus />
      </div>
    </div>
  </div>
</template>

<script setup>
import DonutChart from "@/components/DonutChart.vue";
import SensorGraph from "@/components/SensorGraph.vue";
import RelayStatus from "@/components/RelayStatus.vue";
</script>

<style scoped>

</style>

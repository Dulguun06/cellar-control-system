<template>
  <div>
    <!-- Proper Bootstrap Navbar -->
    <nav class="navbar navbar-expand-lg navbar-dark bg-dark">
      <div class="container-fluid">
        <a class="navbar-brand" href="#">Cellar Control</a>
        <button
            class="navbar-toggler"
            type="button"
            data-bs-toggle="collapse"
            data-bs-target="#navbarNav"
            aria-controls="navbarNav"
            aria-expanded="false"
            aria-label="Toggle navigation"
        >
          <span class="navbar-toggler-icon"></span>
        </button>
        <div class="collapse navbar-collapse" id="navbarNav">
          <ul class="navbar-nav ms-3">
            <li class="nav-item">
              <a class="nav-link" :class="{ active: activeTab === 'realtime' }" href="#" @click="activeTab = 'realtime'">Dashboard</a>
            </li>
            <li class="nav-item">
              <a class="nav-link" :class="{ active: activeTab === 'config' }" href="#" @click="activeTab = 'config'">Configurations</a>
            </li>
            <li class="nav-item">
              <a class="nav-link" :class="{ active: activeTab === 'profile' }" href="#" @click="activeTab = 'profile'">User Profile</a>
            </li>
          </ul>
        </div>
      </div>
    </nav>

    <!-- Main Content Section -->
    <div class="container mt-4">
      <Dashboard v-if="activeTab === 'realtime'" />
      <Configs v-if="activeTab === 'config'" />
      <Profile v-if="activeTab === 'profile'" />
    </div>
  </div>
</template>

<script>

import Dashboard from './Dashboard.vue';
import Configs from './Configs.vue';
import Profile from './Profile.vue';

export default {
  components: {
    Dashboard,
    Configs,
    Profile,
  },
  data() {
    return {
      activeTab: 'realtime',
    };
  },
};
</script>

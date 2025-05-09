<template>
  <div>
    <h2 class="text-xl font-semibold mb-4">Notification Settings</h2>

    <form @submit.prevent="handleSubmit">
      <label for="email" class="block font-medium mb-1">Notification Email:</label>
      <input
          id="email"
          v-model="email"
          type="email"
          class="border rounded px-3 py-2 w-full mb-3"
          placeholder="Enter new email address"
          required
      />

      <button type="submit" class="bg-blue-500 text-black px-4 py-2 rounded hover:bg-blue-600">
        Update Email
      </button>
    </form>

    <p v-if="successMessage" class="text-green-600 mt-3">{{ successMessage }}</p>
    <p v-if="errorMessage" class="text-red-600 mt-3">{{ errorMessage }}</p>
  </div>
</template>

<script>


import {getNotificationEmail, updateNotificationEmail} from "@/service/api.js";

export default {
  data() {
    return {
      email: '',
      successMessage: '',
      errorMessage: ''
    };
  },
  mounted() {
    this.fetchCurrentEmail();
  },
  methods: {
    async fetchCurrentEmail() {
      try {
        const response = await getNotificationEmail();
        this.email = response.data;
      } catch (error) {
        console.error("Failed to fetch current email:", error);
      }
    },
    async handleSubmit() {
      try {
        await updateNotificationEmail(this.email);
        this.successMessage = 'Notification email updated successfully!';
        this.errorMessage = '';
      } catch (error) {
        this.successMessage = '';
        this.errorMessage = error.response?.data || 'Failed to update email.';
      }
    }
  }
};
</script>

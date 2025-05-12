import axios from 'axios'

const API_URL = 'http://localhost:8080/api'

export const getLatestSensorData = () => axios.get(`${API_URL}/sensor/latest`)
export const getAllSensorData = () => axios.get(`${API_URL}/sensor/all`)
export const getRelayStatus = () => axios.get(`${API_URL}/relay/latest`)

// CONFIG ENDPOINTS
export const getAllConfigs = () => axios.get(`${API_URL}/config`)
export const addConfig = (config) => axios.post(`${API_URL}/config`, config)
export const applyConfig = (id) => axios.post(`${API_URL}/config/apply/${id}`)
export const updateConfig = (id, updatedConfig) => axios.put(`${API_URL}/config/${id}`, updatedConfig)
export const deleteConfig = (id) => axios.delete(`${API_URL}/config/${id}`)
export const fetchActiveConfig = () => axios.get(`${API_URL}/config/active`);

export const updateNotificationEmail = (email) => axios.post(`${API_URL}/update-notification-email`, {email});
export const getNotificationEmail = () => axios.get(`${API_URL}/notification-email`);
export const setMode = (mode) => axios.post(`${API_URL}/relay/switch-mode`, {mode}) // Post mode change to backend
// export const fetchCurrentMode = () => axios.get('/api/mode')
export const sendManualCommand = (relayStates) => axios.post(`${API_URL}/relay/manual-control`, relayStates)


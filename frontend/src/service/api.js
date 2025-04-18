import axios from 'axios'

const API_URL = 'http://localhost:8080/api'

export const getLatestSensorData = (sensorId) => {
    return axios.get(`${API_URL}/sensor/latest/${sensorId}`)
}

export const getRelayStatus = () => {
    return axios.get(`${API_URL}/relay/latest`)
}
import SockJS from "sockjs-client";
import Stomp from "stompjs";

export default class WebSocketService {
    constructor() {
        this.client = null;
    }

    connect() {
        const socket = new SockJS("http://localhost:8080/ws");
        this.client = Stomp.over(socket);
        this.client.connect({}, this.onConnect, this.onError);
        this.client.send("/app/test", {}, "Ping from Vue!");
    }

    onConnect = () => {
        console.log("Connected to WebSocket");

        // Subscribe to topic for sensor data updates
        this.client.subscribe("/topic/sensorUpdates", (message) => {
            const data = JSON.parse(message.body);
            console.log("Received sensor data:", data);
            // Handle updates (e.g., update UI state)
        });

        // Subscribe to topic for relay status updates
        this.client.subscribe("/topic/relayUpdates", (message) => {
            const data = JSON.parse(message.body);
            console.log("Received relay status:", data);
            // Handle updates (e.g., update UI state)
        });
    };

    onError = (error) => {
        console.error("WebSocket error:", error);
    };

    disconnect() {
        if (this.client) {
            this.client.disconnect();
            console.log("Disconnected from WebSocket");
        }
    }
}

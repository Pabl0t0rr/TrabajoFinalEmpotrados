// Configuración
const ESP32s3_IP = 'http://192.168.1.120'; // Reemplaza con la IP de tu ESP32
const UPDATE_INTERVAL = 3000; // Actualizar cada 3 segundos

let ledState = true; // Estado inicial del LED
let isConnected = true;

// Elementos del DOM
const statusElement = document.getElementById('status');
const tempElement = document.getElementById('temperature');
const humidityElement = document.getElementById('humidity');
const ledButton = document.getElementById('ledButton');
const errorElement = document.getElementById('errorMessage');

// Función para obtener datos del sensor
async function fetchSensorData() {
    try {
        const response = await fetch(`${ESP32s3_IP}/api/sensor`, {
            method: 'GET',
            headers: { 'Content-Type': 'application/json' }
        });

        if (!response.ok) throw new Error('Error en la respuesta');

        const data = await response.json();
        
        // Actualizar valores
        tempElement.textContent = data.temperature.toFixed(1);
        humidityElement.textContent = data.humidity.toFixed(0);
        
        // Actualizar estado de conexión
        if (!isConnected) setConnected(true);

    } catch (error) {
        console.error('Error al obtener datos:', error);
        setConnected(false);
    }
}

// Función para controlar el LED
async function toggleLED() {
    try {
        const response = await fetch(`${ESP32s3_IP}/toggle/1`, { method: 'GET' });

        if (!response.ok) throw new Error('Error al controlar LED');

        updateLEDButton();
        ledState = !ledState;
        setConnected(true);
        
    } catch (error) {
        console.error('Error al controlar LED:', error);
        setConnected(false);
    }
}

// Actualizar apariencia del botón
function updateLEDButton() {
    if (ledState) {
        ledButton.textContent = 'Apagar LED';
        ledButton.className = 'led-button off';
    } else {
        ledButton.textContent = 'Encender LED';
        ledButton.className = 'led-button on';
    }
}

// Actualizar estado de conexión
// Aparece como desconectado aunque esté conectado (si se pulsa el botón se cambia el led)
// (la red me va rara creo xd)
function setConnected(connected) {
    if (connected) {
        statusElement.textContent = '✅ Conectado';
        statusElement.className = 'status connected';
        errorElement.innerHTML = '';
    } else {
        statusElement.innerHTML = '<span class="loading"></span> Desconectado';
        statusElement.className = 'status disconnected';
        errorElement.innerHTML = '<div class="error-message">⚠️ No se puede conectar con el ESP32. Verifica la dirección IP y la conexión de red.</div>';
    }
}

// Event listeners
ledButton.addEventListener('click', toggleLED);

// Inicializar
fetchSensorData();
setInterval(fetchSensorData, UPDATE_INTERVAL);

console.log('Panel IoT iniciado. Conectando a:', ESP32s3_IP);

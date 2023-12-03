# IoTren
## Proyecto de Monitoreo y Localización

Este proyecto utiliza un conjunto de sensores y módulos para monitorear condiciones ambientales, como temperatura y humedad, así como para rastrear la ubicación del tenecito del Campus Tec Qro en tiempo real mediante un módulo GPS. Además, se integra con la plataforma Ubidots para enviar y visualizar los datos recopilados en la nube.

## Componentes Principales: Librerias

1. **UbidotsESPMQTT Library:** Se utiliza para la comunicación con la plataforma Ubidots a través del protocolo MQTT, permitiendo el envío de datos a la nube.

2. **DHT Library:** Implementa la funcionalidad para la lectura del sensor de humedad y temperatura DHT11, proporcionando datos ambientales.

3. **LiquidCrystal_I2C Library:** Se encarga de controlar y manejar la pantalla LCD I2C, utilizada para mostrar información local y de ubicación.

4. **TinyGPS++ Library y SoftwareSerial:** Permiten la adquisición y decodificación de datos de localización GPS a través de un módulo GPS conectado al puerto serie software.

## Configuración y Conexión

- **WiFi y Token:** Configurar las constantes `WIFINAME` y `WIFIPASS` con los detalles de la red WiFi, y `TOKEN` con el token de Ubidots para establecer la conexión.

- **Botones:** Conectar los botones en los pines definidos como `buttonOnTrack` y `buttonMenu`.

- **GPS:** Conectar el módulo GPS al puerto serie software (pines 13 y 15) para la obtención de coordenadas geográficas.

## Funcionalidades Principales

### Sensores Ambientales (DHT11)
- Mide la humedad y temperatura ambiente.
- Publica estos datos en Ubidots bajo los nombres "Temperatura" y "Humedad".

### Localización GPS
- Lee las coordenadas GPS y calcula la distancia a ubicaciones predefinidas.
- Si la distancia es menor que un umbral (`RADIUS_THRESHOLD`), envía el nombre de la ubicación a Ubidots bajo el nombre "Ultima parada".

### Circulación y Menú
- Monitoriza el estado de un botón (`buttonOnTrack`) para determinar si el dispositivo está en movimiento.
- Utiliza otro botón (`buttonMenu`) para cambiar entre diferentes menús y visualizaciones en la pantalla LCD.

## Pantalla LCD
- Muestra información como latitud y longitud en uno de los menús.
- Informa sobre la última parada si se encuentra dentro del radio establecido alrededor de ubicaciones predefinidas.


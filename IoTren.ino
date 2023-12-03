// Librerias
#include <UbidotsESPMQTT.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Constantes Wifi y DB
#define TOKEN "BBUS-WOaH9Y0Y85G0qjt45djta3vmpKKSIo" // Your Ubidots TOKEN
#define WIFINAME "Galaxy A34 5G" // Your SSID
#define WIFIPASS "dakna2023" // Your Wifi Pass

Ubidots client(TOKEN);

// Declaraciones para DHT11 y botones
#define DHTPIN D3 // Pin donde está conectado el sensor DHT11 
#define buttonOnTrack 14 //Pin donde esta conectado hacia el botton cuadrado
#define buttonMenu 12 //Pin donde esta conectado hacia el boton circular
int buttonOnTrackState = 0;
int buttonMenuState = 0;
int menu = 0;

#define DHTTYPE DHT11 // Tipo de sensor (DHT11, DHT22, o DHT21)
DHT dht(DHTPIN, DHTTYPE);

// Display
LiquidCrystal_I2C lcd(0X27, 16, 2);

// Radio de proximidad en metros
#define RADIUS_THRESHOLD 15 
const double targetLatitudes[] = {20.614929, 20.613983, 20.613616, 20.614063, 20.613201, 20.612295};
const double targetLongitudes[] = {-100.403320, -100.403839, -100.404875, -100.405609, -100.406692, -100.407394};
const char *locations[] = {"Edificio 2", "OXXO", "Borrrego Cosmico", "Arqui", "Prepa", "CIMA"};


TinyGPSPlus gps;
SoftwareSerial ss(13, 15);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  //client.ubidotsSetBroker("industrial.api.ubidots.com"); // Sets the broker properly for the industrial account
  client.setDebug(true); // Pass a true or false bool value to activate debug messages
  Serial.begin(115200);
  client.wifiConnection(WIFINAME, WIFIPASS);
  client.begin(callback);

  //Genral
  ss.begin(9600);

  //DHT11
  dht.begin();                      //Initailize DHT sensor

  //LCD I2C
  lcd.init();                       // Initialize the LCD
  lcd.backlight();                  // Turn on the backlight
  lcd.clear();                      // Clear the LCD screen

  //Buttons
  pinMode(buttonOnTrack, INPUT);
  pinMode(buttonMenu, INPUT);
}

void loop() {
  if(!client.connected()) {
    client.reconnect();
  }

  delay(200); // Esperar entre lecturas
  bool enEstacion = false; 

  // DHT11
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  Serial.print("Humedad: ");
  Serial.print(humidity);
  Serial.print("%\tTemperatura: ");
  Serial.print(temperature);
  Serial.println("°C");

  // Buttons
  buttonOnTrackState = digitalRead(buttonOnTrack);
  buttonMenuState = digitalRead(buttonMenu);

  // GPS
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;
  static bool withinRadius = false;

while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      if (gps.location.isValid() && currentMillis - previousMillis >= 10000) {
        previousMillis = currentMillis;

        double currentLat = gps.location.lat();
        double currentLng = gps.location.lng();

        Serial.print("Latitude: ");
        Serial.println(currentLat, 6);
        Serial.print("Longitude: ");
        Serial.println(currentLng, 6);

        if (menu == 3) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Lat: "); lcd.print(currentLat, 6);
          lcd.setCursor(0, 1);
          lcd.print("Lng: "); lcd.print(currentLng, 6);
        } 

        for (int i = 0; i < sizeof(targetLatitudes) / sizeof(targetLatitudes[0]); ++i) {
          double distance = TinyGPSPlus::distanceBetween(currentLat, currentLng, targetLatitudes[i], targetLongitudes[i]);
          Serial.print("Distance to ");
          Serial.print(locations[i]);
          Serial.print(": ");
          Serial.println(distance);

          if (distance <= RADIUS_THRESHOLD) {
            withinRadius = true;
            Serial.print("Te encuentras en: ");
            Serial.println(locations[i]);

            client.add("Ultima parada", *locations[i]);
            client.ubidotsPublish("IoTren");
            delay(200);

            if (menu == 4) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Te encuentras en: ");
              lcd.setCursor(0, 1);
              lcd.print(locations[i]);
              enEstacion = true;
            }
            
          } else {
            withinRadius = false;
          }
        }

        client.add("latitude", currentLat);
        client.ubidotsPublish("IoTren");
        delay(100);
        client.add("longitude", currentLng);
        client.ubidotsPublish("IoTren");
        delay(1000);

      }
    }
 }

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Error al leer el sensor DHT!");
  } else {
    Serial.print("Humedad: ");
    Serial.print(humidity);
    Serial.print("%\tTemperatura: ");
    Serial.print(temperature);
    Serial.println("°C");

    client.add("Temperatura", temperature);
    client.add("Humedad", humidity);
    client.ubidotsPublish("IoTren");
    delay(1000);
  }

  if (buttonMenuState == LOW) {
    menu = menu + 1;
  }

  if (menu == 5) {
    menu = 0;
  }

  if (buttonOnTrackState == HIGH) {
    client.add("circulation_state", 1);
    client.ubidotsPublish("IoTren");
    delay(1000);
  } else {
    client.add("circulation_state", 0);
    client.ubidotsPublish("IoTren");
    delay(1000);
  }
}

/*
Controller programming code: Written on CPP modified to work on Arduino IDE
Written for and by Electronics & Robotics Club, IIT Bombay with assistance from internet resources.

For: ESP01-MPU circuit meant to act as motion based controller
Uploading procedure will be done by convenors, special circuit required.

©ERC // MIT License 2024 
*/


#include <ESP8266WiFi.h> //Library that has functions to help use WiFi to communicate
#include <Wire.h> //Library that helps communication between ESP01 and MPU6050
#include <floatToString.h> //What do you think this library does?

const char* ssid = "PicoW_AP";  //SSID is your Raspberry Pi Pico W's WiFi's name!
const char* password = "12345678"; // keep it secret, or people might try to interfere during your final run XD
const char* host = "192.168.42.1"; // IP Address of the Pico
const int MPU = 0x68; // I2C address of the MPU-6050 sensor

float gyrox, gyroy, gyroz;
char gx[6], gy[6], gz[6];

void readIMUData() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Register address to start reading from
  Wire.endTransmission(false);

  // Request 14 bytes of data from the MPU-6050
  Wire.requestFrom((uint8_t)MPU, (size_t)14, true);

  int16_t rawValues[7];
  for (int i = 0; i < 7; i++) {
    rawValues[i] = Wire.read() << 8 | Wire.read(); // Read two bytes and combine them into an integer
  }

  // Convert raw sensor values to meaningful gyro readings
  gyrox = rawValues[0] / 16384.0 * 10;
  gyroy = rawValues[1] / 16384.0 * 10;
  gyroz = rawValues[2] / 16384.0 * 10;
  floatToString(gyrox, gx, 5, 2 );
  floatToString(gyroy, gy, 5, 2 );
  floatToString(gyroz, gz, 5, 2 ); 
}
void setup() {
  Serial.begin(115200);
  Wire.begin(0, 2); // Initialize I2C communication
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); // Power management register address
  Wire.write(0);    // Wake up the MPU-6050``
  Wire.endTransmission(true);
  delay(10);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  readIMUData();
  WiFiClient client;
  const int httpPort = 80;
  Serial.println(gx);
  Serial.println(gy);
  Serial.println(gz);
  
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }
  client.print(String(gx));
  client.print(String(", "));
  client.print(String(gy));
  client.print(String(", "));
  client.println(String(gz));
  delay(100);
}


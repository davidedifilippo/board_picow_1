#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <string> 
#include <SparkFunTMP102.h> // Used to send and recieve specific information from our sensor
#include "My_LiquidCrystal_I2C.h"

#include "secret.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "broker.hivemq.com";
int        port     = 1883;
const char topic[]  = "picow_1_board/tmp102";

const long interval = 10000;
unsigned long previousMillis = 0;

int count = 0;
char countString[3];

TMP102 tmpSensor;

// Set the I2C address to 0x3F to send data to PCF8574_Expanders
LiquidCrystal_I2C lcd(0x3F, 20, 2);

void setup() {

  Wire1.setSDA(14);
  Wire1.setSCL(15);
  Wire1.begin();

  Wire.setSDA(20);
  Wire.setSCL(21);
  Wire.begin();

  lcd.begin();
  lcd.backlight();
  lcd.print("IIS AOSTA!");
  lcd.setCursor(0,1);
  lcd.print("Elettronica 2026");
  delay(4000);
  lcd.clear();
  lcd.setCursor(0,0);


   /*It will return trueon success or false on failure to communicate. */
  if(!tmpSensor.begin(0x48,Wire1))
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("TMP102 Ko!" );
    lcd.setCursor(0,1);
    lcd.print("STOP!" );
    while(1);
  }

 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("TMP102 Ok!" );
 delay(2000);

 lcd.clear();
 lcd.setCursor(0,0);

  // attempt to connect to WiFi network:
  lcd.print("Access Point: ");
  lcd.setCursor(0,1);
  lcd.print(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    lcd.print(".");
    delay(5000);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi connesso!");
  
  delay(4000);
  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("MQTT broker: ");
  lcd.setCursor(0,1);
  lcd.print(broker);
  delay(2000);

  if (!mqttClient.connect(broker, port)) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("MQTT KO!");
    lcd.setCursor(0,1);

    lcd.print("Error: " + mqttClient.connectError());

    while (1);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("MQTT broker: ");
  lcd.setCursor(0,1);
  lcd.print("connesso...");
  delay(4000);

    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("topic: ");
    lcd.setCursor(0,1);
    lcd.print(topic);
    delay(2000);
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();

  float temp = tmpSensor.readTempC();
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("temp= ");
    lcd.print(temp);
    lcd.setCursor(2,1);
    lcd.print("tx: ");
    lcd.print(count);
    //sprintf(countString, "%d", count);
    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    //mqttClient.print("weather,location=us-midwest temperature=82 1465839830100400200"); //dataformat influx in telegraf
    mqttClient.print("device=picow_1_board,location=casa/stanza_1 temperature="); //dataformat senza timestamp
    mqttClient.print(temp);
    mqttClient.endMessage();

    count++;
  }
}
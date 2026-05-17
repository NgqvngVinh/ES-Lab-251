#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "secrets.h"

#define AIO_SERVER    "io.adafruit.com"
#define AIO_SERVERPORT 1883

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// set publish
Adafruit_MQTT_Publish light_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/led");
Adafruit_MQTT_Publish temp_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");

// setup subcribe
Adafruit_MQTT_Subscribe light_sub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led", MQTT_QOS_1);

int led_counter = 0;
int led_status = HIGH;

void lightcallback(char* value, uint16_t len) {
  if (value[0] == '0') Serial.print('a');
  if (value[0] == '1') Serial.print('A');
}

void setup() {
  // put your setup code here, to run once:

  pinMode(2, OUTPUT); // set pin 2,5 as OUTPUT
  pinMode(5, OUTPUT);
  // (turn led while connecting wifi), turn on led
  digitalWrite(5, HIGH);

  Serial.begin(115200);

  //connect Wifi
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // subscribe light feed
  light_sub.setCallback(lightcallback);
  mqtt.subscribe(&light_sub);

  // connect MQTT
  while (mqtt.connect() != 0) {
    mqtt.disconnect();
    delay(500);
  }

  //finish setup (connected wifi successfully), turn off led
  digitalWrite(5, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  // receive packet
  mqtt.processPackets(1);

  //read serial
  if (Serial.available()) {

    // Kiểm tra xem có phải là gói tin nhiệt độ không (bắt đầu bằng '!')
    if (Serial.peek() == '!') { 
       String data = Serial.readStringUntil('#');
       if (data.startsWith("!TEMP:")) {
          String temp_str = data.substring(6);
          float temp_val = temp_str.toFloat();
          
          // Gửi lên Adafruit IO
          // Serial.print("Publishing Temp: ");
          // Serial.println(temp_val);
          temp_pub.publish(temp_val);
       }
    } 
    else {
       // Xử lý các ký tự đơn lẻ ('o', 'a', 'A')
       int msg = Serial.read();
       if (msg == 'o') Serial.print('O');
       else if (msg == 'a') light_pub.publish(0);
       else if (msg == 'A') light_pub.publish(1);
    }
  }

  led_counter++;
  if (led_counter == 100) {
    led_counter = 0;
    //toggle LED
    led_status = !led_status; // Swap led ON/OFF
    digitalWrite(2, led_status);
  }
  delay(10);
}
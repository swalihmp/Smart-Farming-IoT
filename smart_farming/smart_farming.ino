#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID       "Tenda1"
#define WLAN_PASS       "00000000swali"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish moisture = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/moisture_level");
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/manual");

char switch_sts='F';
int moisture_val;
void MQTT_connect();
int relay = 0;
int LED =16;

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(relay,OUTPUT);
  digitalWrite(relay,LOW);
    pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);

  Serial.println(F("Moisture Sensing Device"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton);
}

uint32_t x=0;

void loop() {
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) 
    {

     switch_sts=onoffbutton.lastread[1];
     Serial.println(switch_sts);
     
    }
    
  }
  moisture_val=(820-analogRead(A0))/2.4;
  if((moisture_val<40&&switch_sts=='F')||switch_sts=='N')
  {
  digitalWrite(relay,HIGH);
    digitalWrite(LED,LOW);
     Serial.println("Motor ON");
  }
  else
  {
  digitalWrite(LED,HIGH);
  digitalWrite(relay,LOW);
   Serial.println("Motor off");
  }

  

  // Now we can publish stuff!
  Serial.print(F("\nSending moisture val "));

  if (! moisture.publish(moisture_val)) {
  
  } else {
    Serial.println(F("OK!"));
       Serial.print(moisture_val);
        Serial.print("%");
  }

}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

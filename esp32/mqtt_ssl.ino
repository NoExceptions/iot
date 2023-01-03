#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// This code is just to help someone to do a server validation from the ESP32 using a CA cert :)


const char* ssid = "your_wifi_SSID";
const char* password = "your_wifi_passsword";
const char* clientId = "an_mqtt_identifier+for_your_esp32";
const char* mqtt_user = "mqtt_user";
const char* mqtt_pwd = "mqtt_pass";
const char* host = "broker_hostname"; //remembering the 

/* Important! The certificate myust be written just like bellow! This cert is not valid, 
it is just an syntax example */

const char* cert =
"-----BEGIN CERTIFICATE-----\n" \
"MIIGEzCCA/ugAwIBAgIQfVtRJrR2uhHbdBYLvFMNpzANBgkqhkiG9w0BAQwFADCB\n" \
"iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n" \
"cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n" \
"BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTgx\n" \
"MTAyMDAwMDAwWhcNMzAxMjMxMjM1OTU5WjCBjzELMAkGA1UEBhMCR0IxGzAZBgNV\n" \
"BAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEYMBYGA1UE\n" \
"ChMPU2VjdGlnbyBMaW1pdGVkMTcwNQYDVQQDEy5TZWN0aWdvIFJTQSBEb21haW4g\n" \
"VmFsaWRhdGlvbiBTZWN1cmUgU2VydmVyIENBMIIBIjANBgkqhkiG9w0BAQEFAAOC\n" \
"Haswha6vsC6eep3BwEIc4gLw6uBK0u+QDrTBQBbwb4VCSmT3pDCg/r8uoydajotY\n" \
"uK3DGReEY+1vVv2Dy2A0xHS+5p3b4eTlygxfFQIDAQABo4IBbjCCAWowHwYDVR0j\n" \
"LcmsJWTyXnW0OMGuf1pGg+pRyrbxmRE1a6Vqe8YAsOf4vmSyrcjC8azjUeqkk+B5\n" \
"yOGBQMkKW+ESPMFgKuOXwIlCypTPRpgSabuY0MLTDXJLR27lk8QyKGOHQ+SwMj4K\n" \
"00u/I5sUKUErmgQfky3xxzlIPK1aEn8=\n" \
"-----END CERTIFICATE-----";




void callback(char* topic, byte* payload, unsigned int length) {


  if (strcmp(topic, "g/manual") == 0) {
    if (!strncmp((char*)payload, "on", length)) {
      digitalWrite(25, HIGH);
      delay(200);
    } else if (!strncmp((char*)payload, "off", length)) {
      digitalWrite(25, LOW);
      delay(200);
    }
  }
}

WiFiClientSecure ethClient;
PubSubClient client(ethClient);

void wifiConnect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
}

void mqttConnect() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnect();
  } else {
    if (!client.connected()) {
      if (client.connect(clientId, mqtt_user, mqtt_pwd)) {
        Serial.println("Connecting to Broker..");
        client.publish("g/alive", "hello");
        client.subscribe("g/manual");
      }
    }
  }
}

void setup() {
  pinMode(25, OUTPUT);
  Serial.begin(115200);
  client.setServer(host, 8883);
  client.setCallback(callback);
  ethClient.setCACert(cert); // Do not forget that the responsible to check the CACert is the WiFiClientSecure class and not the PubSubClient




}

void loop() {
  if (!client.connected()) {
    Serial.println("MQTT reconection needed!");
    mqttConnect();
  } else {
    client.loop();
    delay(1200);
  }
}

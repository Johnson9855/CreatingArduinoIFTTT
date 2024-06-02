#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

// Replace these with your network credentials
const char* ssid = "WiFi_SSID";
const char* password = "WiFi_PASSWORD";

// IFTTT settings
const char* host = "maker.ifttt.com";
String eventName = "Light_sensor_triggered"; // Replace with your IFTTT event name
String iftttKey = "bMU9J_AaonEwnGBAYIrFkB"; // Replace with your IFTTT key

BH1750 lightMeter;
WiFiSSLClient wifi; // Use SSL client for HTTPS
HttpClient client = HttpClient(wifi, host, 443); // Use port 443 for HTTPS

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 started");
  } else {
    Serial.println("Error initializing BH1750");
  }

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  if (lux > 100) { // Threshold for bright light
    sendIFTTTNotification("Sunlight detected: " + String(lux) + " lx");
  } else {
    sendIFTTTNotification("Sunlight stopped: " + String(lux) + " lx");
  }
  delay(60000); // Check every minute
}

void sendIFTTTNotification(String value) {
  String url = "/trigger/" + eventName + "/with/key/" + iftttKey + "?value1=" + urlencode(value);
  client.get(url);

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
}

String urlencode(String str) {
  String encodedString = "";
  char buf[4];
  for (char c : str) {
    if (isalnum(c)) {
      encodedString += c;
    } else if (c == ' ') {
      encodedString += '+';
    } else {
      sprintf(buf, "%%%02X", c);
      encodedString += buf;
    }
  }
  return encodedString;
}

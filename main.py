#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* serverAddress = "hooks.zapier.com";  // Zapier Webhooks server
const String zapPath = "/hooks/catch/your_zapier_webhook_id";

int sensorPin = A0;
int threshold = 500;  // Adjust based on calibration
boolean isSunny = false;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, 80);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);

  if (sensorValue > threshold && !isSunny) {
    isSunny = true;
    sendZapierNotification("Sunlight detected");
  } else if (sensorValue <= threshold && isSunny) {
    isSunny = false;
    sendZapierNotification("Sunlight stopped");
  }
  delay(1000);
}

void sendZapierNotification(String value) {
  String url = zapPath + "?value=" + value;
  client.get(url);

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
}

	#include <esp_now.h>
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// === CONFIGURAÇÕES ===
#define LED_PIN 2

// === WIFI ===
#define WIFI_SSID     ""
#define WIFI_PASSWORD ""

// === ADAFRUIT IO ===
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  
#define AIO_USERNAME    ""
#define AIO_KEY         ""

WiFiClient client;

Adafruit_MQTT_Client mqtt(
  &client,
  AIO_SERVER,
  AIO_SERVERPORT,
  AIO_USERNAME,
  AIO_KEY
);

// === BUFFER DE RECEPÇÃO (preenchido no callback, tratado no loop) ===
volatile bool novoDado = false;
char bufferJson[256];

// === FUNÇÕES AUXILIARES ===
void piscarLED(int tempo = 50) {
  digitalWrite(LED_PIN, HIGH);
  delay(tempo);
  digitalWrite(LED_PIN, LOW);
}

// === PARSE SIMPLES DE JSON (formato fixo enviado pela outra placa) ===
String extrairString(const String& json, const String& chave) {
  String alvo = "\"" + chave + "\":\"";
  int i = json.indexOf(alvo);
  if (i < 0) return "";
  i += alvo.length();
  int fim = json.indexOf("\"", i);
  if (fim < 0) return "";
  return json.substring(i, fim);
}

int extrairInt(const String& json, const String& chave) {
  String alvo = "\"" + chave + "\":";
  int i = json.indexOf(alvo);
  if (i < 0) return 0;
  i += alvo.length();
  while (i < (int)json.length() && json[i] == ' ') i++;
  int fim = i;
  while (fim < (int)json.length() &&
         (isDigit(json[fim]) || json[fim] == '-')) fim++;
  return json.substring(i, fim).toInt();
}

// === WIFI ===
void conectarWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Conectando ao WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi conectado");
  Serial.print("IP: ");      Serial.println(WiFi.localIP());
  Serial.print("Canal: ");   Serial.println(WiFi.channel());
}

// === MQTT ===
void conectarMQTT() {
  if (mqtt.connected()) return;

  Serial.print("Conectando ao Adafruit IO... ");
  int8_t ret;
  uint8_t tentativas = 0;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    mqtt.disconnect();
    delay(3000);
    if (++tentativas >= 5) {
      Serial.println("Desisti por agora, tento de novo no proximo dado");
      return;
    }
  }
  Serial.println("OK");
}

// === PUBLICAR UM FEED DINÂMICO: <id>-<grandeza> ===
void publicarFeed(const String& id, const String& grandeza, int valor) {
  String topico = String(AIO_USERNAME) + "/feeds/" + id + "-" + grandeza;
  String valorStr = String(valor);

  if (mqtt.publish(topico.c_str(), valorStr.c_str())) {
    Serial.print("Publicado ");
    Serial.print(topico);
    Serial.print(" = ");
    Serial.println(valor);
  } else {
    Serial.print("Falha ao publicar ");
    Serial.println(topico);
  }
}

// === TRATAR O JSON RECEBIDO ===
void processarJSON(const String& payload) {
  Serial.println("\n=== JSON ESP-NOW RECEBIDO ===");
  Serial.println(payload);
  Serial.println("=============================");

  String id = extrairString(payload, "id");
  if (id == "") {
    Serial.println("JSON sem 'id', ignorando");
    return;
  }

  int temperatura = extrairInt(payload, "temperatura");
  int umidade     = extrairInt(payload, "umidade");

  conectarWiFi();
  conectarMQTT();

  publicarFeed(id, "temperatura", temperatura);
  publicarFeed(id, "umidade", umidade);
}

// === CALLBACKS ESP-NOW (só copia os dados, não publica aqui) ===
#if ESP_ARDUINO_VERSION_MAJOR >= 3

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  int n = min(len, (int)sizeof(bufferJson) - 1);
  memcpy(bufferJson, incomingData, n);
  bufferJson[n] = '\0';
  novoDado = true;
  piscarLED(50);
}

#else

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  int n = min(len, (int)sizeof(bufferJson) - 1);
  memcpy(bufferJson, incomingData, n);
  bufferJson[n] = '\0';
  novoDado = true;
  piscarLED(50);
}

#endif

// === SETUP ===
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  conectarWiFi();   // conecta primeiro: fixa o rádio no canal do AP

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

  // Só recebe, então basta registrar o callback de recepção
  esp_now_register_recv_cb(OnDataRecv);

  conectarMQTT();
}

// === LOOP PRINCIPAL ===
void loop() {
  // mantém WiFi/MQTT vivos
  if (WiFi.status() != WL_CONNECTED) conectarWiFi();
  if (!mqtt.connected())             conectarMQTT();
  mqtt.ping();

  if (novoDado) {
    novoDado = false;
    processarJSON(String(bufferJson));
  }

  delay(100);
}

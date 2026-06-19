#include <esp_now.h>
#include <WiFi.h>
#include <DHT.h>

// === CONFIGURAÇÕES ===
#define ID "equipe02"
#define DHTPIN 4
#define DHTTYPE DHT11
#define LED_PIN 2
#define INTERVALO_ENVIO_MS 5000

// === VARIÁVEIS GLOBAIS ===
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

unsigned long ultimoEnvio = 0;
unsigned long contadorMensagens = 0;

String json;

DHT dht(DHTPIN, DHTTYPE);

// === FUNÇÕES AUXILIARES ===
void piscarLED(int tempo = 50) {
  digitalWrite(LED_PIN, HIGH);
  delay(tempo);
  digitalWrite(LED_PIN, LOW);
}

// === COLETAR E ENVIAR DADOS ===
void montarEEnviarInternos() {
  float temperatura = dht.readTemperature();
  float umidade     = dht.readHumidity();

  contadorMensagens++;

  int t = isnan(temperatura) ? 0 : (int)temperatura;
  int u = isnan(umidade)     ? 0 : (int)umidade;

  json =
    "{"
    "\"id\":\"" + String(ID) + "\","
    "\"contador\":" + String(contadorMensagens) + ","
    "\"temperatura\":" + String(t) + ","
    "\"umidade\":" + String(u) +
    "}";

  esp_now_send(broadcastAddress, (uint8_t*)json.c_str(), json.length() + 1);
  piscarLED(300);
}

// === CALLBACKS ESP-NOW ===
#if ESP_ARDUINO_VERSION_MAJOR >= 3

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  String jsonRecebido = (char*)incomingData;
  Serial.println("\n=== JSON ESP-NOW RECEBIDO ===");
  Serial.println(jsonRecebido);
  Serial.println("=============================");
}

void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.println("\n=== JSON ESP-NOW ENVIADO ===");
  Serial.println(json);
  Serial.println("============================");
}

#else

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  String jsonRecebido = (char*)incomingData;
  Serial.println("\n=== JSON ESP-NOW RECEBIDO ===");
  Serial.println(jsonRecebido);
  Serial.println("=============================");
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println("\n=== JSON ESP-NOW ENVIADO ===");
  Serial.println(json);
  Serial.println("============================");
}

#endif

// === SETUP ===
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  dht.begin();

  // ESP-NOW exige o rádio em modo STA, mas não precisa conectar a nenhum AP
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Erro ao adicionar peer");
  }

  ultimoEnvio = millis();
}

// === LOOP PRINCIPAL ===
void loop() {
  unsigned long agora = millis();

  if (agora - ultimoEnvio >= INTERVALO_ENVIO_MS) {
    montarEEnviarInternos();
    ultimoEnvio = agora;
  }

  delay(100);
}

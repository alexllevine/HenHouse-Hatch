// Necessary libraries
#include <WiFi.h>
#include <WebServer.h>
#include <esp32cam.h>

// WiFi Credentials
const char* WIFI_SSID = "iPhone";
const char* WIFI_PASS = "123Youowemehaha!!";

// Web server on port 80
WebServer server(80);

// Camera resolutions
static auto lowResolution = esp32cam::Resolution::find(320, 240);
static auto midResolution = esp32cam::Resolution::find(350, 530);
static auto highResolution = esp32cam::Resolution::find(800, 600);

void serveJpg() {
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "text/plain", "Capture failed");
    return;
  }

  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(), static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);  // Fixed function name
}

void handleJpgLowQuality() {
  if (!esp32cam::Camera.changeResolution(lowResolution)) {
    Serial.println("SET-LOW-RESOLUTION FAIL");
  }
  serveJpg();
}

void handleJpgMidQuality() {
  if (!esp32cam::Camera.changeResolution(midResolution)) {
    Serial.println("SET-MID-RESOLUTION FAIL");
  }
  serveJpg();
}

void handleJpgHighQuality() {
  if (!esp32cam::Camera.changeResolution(highResolution)) {
    Serial.println("SET-HIGH-RESOLUTION FAIL");
  }
  serveJpg();
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Configure the camera
  using namespace esp32cam;
  Config config;
  config.setPins(pins::AiThinker);
  config.setResolution(highResolution);
  config.setBufferCount(2);
  config.setJpeg(80);

  // Initialize the camera
  bool isCameraInitialized = Camera.begin(config);  // Fixed typo
  Serial.println(isCameraInitialized ? "CAMERA OK" : "CAMERA FAIL");

  // Connect to WiFi
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);  // Fixed typo
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("\nWiFi Connected!");
  Serial.print("Camera Stream: http://");
  Serial.println(WiFi.localIP());

  // Register the handlers
  server.on("/cam-lo.jpg", handleJpgLowQuality);
  server.on("/cam-mid.jpg", handleJpgMidQuality);
  server.on("/cam-hi.jpg", handleJpgHighQuality);

  // Start the server
  server.begin();
}

void loop() {
  server.handleClient();
}


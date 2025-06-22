#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Your_WiFi_UserName";
const char* password = "Your_WiFi_Password";

// Static IP configuration
IPAddress local_IP(192, 168, 1, 171);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(1, 1, 1, 1);

// Create web server on port 80
WebServer server(80);

// Sensor configuration
const int NUM_PORTS = 4;
String assignedSensor[NUM_PORTS] = {"temperature", "humidity", "vibration", "temperature"};

// Simulate sensor readings
float simulateTemperature() {
  return 25.0 + random(-200, 200) / 100.0;
}

float simulateHumidity() {
  return 55.0 + random(-1000, 1000) / 100.0;
}

float simulateVibration() {
  return 0.6 + random(-400, 400) / 1000.0;
}

// Enhanced error response function
void sendError(int code, const String& message) {
  DynamicJsonDocument doc(200);
  doc["error"] = message;
  doc["status_code"] = code;
  String response;
  serializeJson(doc, response);
  server.send(code, "application/json", response);
}

// Root endpoint - API documentation
void handleRoot() {
  DynamicJsonDocument doc(1024);
  doc["device"] = "ESP32 Sensor API";
  doc["version"] = "1.0";
  doc["mac_address"] = WiFi.macAddress();
  doc["ip_address"] = WiFi.localIP().toString();
  
  JsonArray endpoints = doc.createNestedArray("endpoints");
  endpoints.add("GET /{port}/{sensor_type} - Get sensor reading");
  endpoints.add("GET /status - Get device status");
  endpoints.add("GET /ports - Get port assignments");
  
  JsonObject portAssignments = doc.createNestedObject("port_assignments");
  for (int i = 0; i < NUM_PORTS; i++) {
    portAssignments[String(i)] = assignedSensor[i];
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Status endpoint
void handleStatus() {
  DynamicJsonDocument doc(512);
  doc["device"] = "ESP32";
  doc["status"] = "online";
  doc["uptime_ms"] = millis();
  doc["free_heap"] = ESP.getFreeHeap();
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["mac_address"] = WiFi.macAddress();
  doc["ip_address"] = WiFi.localIP().toString();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Port assignments endpoint
void handlePorts() {
  DynamicJsonDocument doc(512);
  JsonObject ports = doc.createNestedObject("port_assignments");
  for (int i = 0; i < NUM_PORTS; i++) {
    ports[String(i)] = assignedSensor[i];
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Main sensor request handler
void handleSensorRequest() {
  String uri = server.uri();
  
  // Remove leading slash
  if (uri.startsWith("/")) {
    uri = uri.substring(1);
  }
  
  // Handle empty URI (redirect to root)
  if (uri.length() == 0) {
    server.sendHeader("Location", "/");
    server.send(302);
    return;
  }
  
  // Split URI by '/'
  int firstSlash = uri.indexOf('/');
  
  // Validate URI format
  if (firstSlash == -1 || uri.indexOf('/', firstSlash + 1) != -1) {
    sendError(400, "Invalid URL format. Use: /{port}/{sensor_type}");
    return;
  }
  
  // Extract parameters
  String portStr = uri.substring(0, firstSlash);
  String sensorType = uri.substring(firstSlash + 1);
  
  // Validate port
  int port = portStr.toInt();
  if (port < 0 || port >= NUM_PORTS) {
    sendError(400, "Invalid port number (0-" + String(NUM_PORTS-1) + " allowed)");
    return;
  }
  
  // Validate sensor type
  if (sensorType != "temperature" && sensorType != "humidity" && sensorType != "vibration") {
    sendError(400, "Unsupported sensor type. Use: temperature, humidity, or vibration");
    return;
  }
  
  // Check port assignment
  bool isAssigned = (assignedSensor[port] == sensorType);
  
  float value = 0.0;
  String unit;
  String status;
  
  if (isAssigned) {
    status = "active";
    if (sensorType == "temperature") {
      value = simulateTemperature();
      unit = "°C";
    } else if (sensorType == "humidity") {
      value = simulateHumidity();
      unit = "%";
    } else if (sensorType == "vibration") {
      value = simulateVibration();
      unit = "mm/s";  // Corrected unit for vibration
    }
  } else {
    status = "not_assigned";
    value = 0.0;
    if (sensorType == "temperature") {
      unit = "°C";
    } else if (sensorType == "humidity") {
      unit = "%";
    } else if (sensorType == "vibration") {
      unit = "mm/s";
    }
  }
  
  // Create JSON response - FIXED: Proper DynamicJsonDocument declaration
  DynamicJsonDocument doc(512);
  doc["device"] = "ESP32";
  doc["mac_address"] = WiFi.macAddress();
  doc["timestamp"] = millis();
  doc["sensor"] = sensorType;
  doc["port"] = port;
  doc["assigned_sensor"] = assignedSensor[port];
  doc["status"] = status;
  doc["value"] = round(value * 100) / 100.0;  // Round to 2 decimal places
  doc["unit"] = unit;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Seed random number generator
  randomSeed(analogRead(0));
  
  Serial.println("\n=== ESP32 Sensor API Server ===");
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  
  // Configure static IP
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Failed to configure static IP");
  }
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n✓ Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Define routes
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/ports", handlePorts);
  server.onNotFound(handleSensorRequest);
  
  // Start server
  server.begin();
  Serial.println("✓ Web server started");
  Serial.println("\n=== API Endpoints ===");
  Serial.println("GET  /                    - API documentation");
  Serial.println("GET  /status              - Device status");
  Serial.println("GET  /ports               - Port assignments");
  Serial.println("GET  /{port}/{sensor}     - Sensor readings");
  Serial.println("\n=== Port Assignments ===");
  for (int i = 0; i < NUM_PORTS; i++) {
    Serial.println("Port " + String(i) + ": " + assignedSensor[i]);
  }
  Serial.println("\n=== Example URLs ===");
  Serial.println("http://" + WiFi.localIP().toString() + "/0/temperature");
  Serial.println("http://" + WiFi.localIP().toString() + "/1/humidity");
  Serial.println("http://" + WiFi.localIP().toString() + "/2/vibration");
}

void loop() {
  server.handleClient();
  
  // Optional: Add a small delay to prevent watchdog issues
  delay(2);
}


#include <SPI.h>
#include <Ethernet.h>

//Lights
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN        6
#define NUMPIXELS  461

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Sensor pins
const int trigPins[] = {22, 24, 26, 28, 30, 32, 34, 36};
const int echoPins[] = {46, 48, 50, 52, 38, 40, 42, 44};
const int numSensors = 8;

// Variables to manage the transition state
bool transitioning = false;
int transitionStep = 0;
int maxTransitionStep = 255;

int section1[] = {0, 116};
int section2[] = {117, 231};
int section3[] = {232, 345};
int section4[] = {346, 461};

// Ethernet settings
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
EthernetClient client;

// Sendable tube data
float TubeNumber = 404;
float ActivationFloat = 0;
bool AlreadyActivated;
float valueToSend[] = {TubeNumber, ActivationFloat};

// Client information
IPAddress ip(192, 168, 0, 2);  // Arduino IP address
int NextResponseTime = 5; // In seconds
String ServerResponse = "";

// Server information
IPAddress server(192, 168, 0, 1);  // Raspberry Pi IP address (server)
int ServerPort = 8080;

// More connection information
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

unsigned long responseTimeout = 5000; // Timeout for server response in ms
unsigned long startMillis;  // To store millis at the start of waiting

void setup() {
  // Initialize Ethernet and Serial communication
  Serial.begin(9600);
  Ethernet.begin(mac, ip);

  Serial.println("Ethernet initialized.");

  // Start sensor and DMX checks
  Serial.println("Start sensor check!");
  if (SensorCheck() == false) {
    Serial.println("Sensor failure!");
  }

  Serial.println("Start Light check!");
  if (LightCheck() == false) {
    Serial.println("Light failure!");
  }

  // Allow the Ethernet shield to initialize
  delay(10000);

  Serial.println("Start connection check!");
  if (client.connect(server, ServerPort)) {
    Serial.println("Connected to Raspberry Pi");
    Serial.println("Requesting TubeID!");
    RequestTubeID();
  } else {
    Serial.println("Connection failed");
  }
}

void loop() {
  CheckSensorValue();

  // Handle server response if available
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
    ServerResponse += c;  // Collect server response data
    startMillis = millis(); // Reset timeout for new data
  }

  // If the server disconnected or no response in a while, stop the client
  if (!client.connected() || (millis() - startMillis > responseTimeout)) {
    Serial.println();
    Serial.println("Disconnected or Timeout");
    handleServerResponse(); // Process collected server response

    // Wait before reconnecting
    delay(NextResponseTime * 1000);   
    
    sendValueToServer();
  }
}

// Function to send a value to the server using HTTP POST
void sendValueToServer() {
  // Update the valueToSend array before sending it to the server
  valueToSend[0] = TubeNumber;
  valueToSend[1] = ActivationFloat;

  int ArrayLength = sizeof(valueToSend) / sizeof(valueToSend[0]);

  String postData = "value=";
  for (int i = 0; i < ArrayLength; i++) {
    postData += String(valueToSend[i]);
    if (i < ArrayLength - 1) {
      postData += ","; 
    }
  }

  client.println(postData);

  Serial.print("Sent values: ");
  Serial.println(postData);
}

//Checks
bool SensorCheck() {
  for (int i = 0; i < numSensors; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    
    // Send a trigger pulse
    digitalWrite(trigPins[i], LOW);
    delayMicroseconds(2);
    digitalWrite(trigPins[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPins[i], LOW);
    
    // Measure the echo response
    long duration = pulseIn(echoPins[i], HIGH);
    
    // Check if duration is within a valid range (sensor is responding)
    if (duration > 0) {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.println(" is working.");
    } else {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.println(" is not responding.");
      return false;
    }
  }
  return true;
}

bool LightCheck() {
  AlreadyActivated = 0;

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  pixels.begin();
  pixels.clear();

  // Check if lights can turn on by setting them to a color (e.g., red)
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.show();
  
  delay(500); 
  
  // Optionally, check if the light actually changed (if possible)
  if (/* Add condition to verify if the light is working */) {
    Serial.println("Lights are working.");
  } else {
    Serial.println("Lights are not working.");
    return false; 
  }

  pixels.clear(); 
  pixels.show();

  return true;  // Lights are working
}

//Sensor working
void CheckSensorValue(){
  int ActivatedSensors = 0;

  for (int i = 0; i < numSensors; i++) {
    long duration, distance;
    
    digitalWrite(trigPins[i], LOW);
    delayMicroseconds(2);
    digitalWrite(trigPins[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPins[i], LOW);
    
    // Read the echo pin
    duration = pulseIn(echoPins[i], HIGH);

    // Calculate the distance (in cm)
    distance = duration * 0.034 / 2; // Speed of sound = 340 m/s = 0.034 cm/µs

    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(distance);
    Serial.println(" cm");
    
    if(distance < 175){
      ActivatedSensors++;
    }

    delay(100);
  }
  ActivationFloat = ActivatedSensors.toFloat();

  delay(200);
}

//Lights
void IdleMode(){
  Heartbeat();
}

void TransitionStep() {
  if (!transitioning) {
    transitioning = true;
    transitionStep = 0;
  }

  // Phase 1: Transition from current state to white (RGB: 255, 255, 255)
  if (transitionStep <= maxTransitionStep) {
    int red = map(transitionStep, 0, maxTransitionStep, currentRed, 255);
    int green = map(transitionStep, 0, maxTransitionStep, currentGreen, 255);
    int blue = map(transitionStep, 0, maxTransitionStep, currentBlue, 255);
    setTubeColor(pixels.Color(red, green, blue));  // Transition to white
    transitionStep++;
  }
  // Phase 2: Transition from white (RGB: 255, 255, 255) to red (RGB: 255, 0, 0)
  else if (transitionStep > maxTransitionStep && transitionStep <= maxTransitionStep * 2) {
    int step = transitionStep - maxTransitionStep;
    int green = map(step, 0, maxTransitionStep, 255, 0);  // Decrease green
    int blue = map(step, 0, maxTransitionStep, 255, 0);   // Decrease blue
    setTubeColor(pixels.Color(255, green, blue));  // Transition to red
    transitionStep++;
  }
  // End transition
  else {
    transitioning = false;
    AlreadyActivated = true;
  }
}

void TouchedMode() {
  if (ActivationFloat == 1) {
    setTubeColor(pixels.Color(255, 0, 0));  // Set color to red
  }
}

void Heartbeat() {
  for (int i = 0; i <= 255; i++) {
    setTubeColor(pixels.Color(i, i, 255));
  }
 
  for (int i = 255; i > 0; i--) {
    setTubeColor(pixels.Color(i, i, 255));
  }
}

//Server <-> Raspberry
void RequestTubeID() {
  if (client.connected()) {
    Serial.println("Connection successful, sending data...");
    String FirstPostData = "value=" + String(TubeNumber);

    client.println(FirstPostData);  // Body with the CSV data

    // Start response timeout
    startMillis = millis();
    ServerResponse = "";  // Clear previous response data
  } else {
    Serial.println("Connection failed, data not sent.");
  }
}

void handleServerResponse() {
  if (ServerResponse.length() > 0) {
    Serial.println("Server response:");
    Serial.println(ServerResponse);
    extractIDFromCustomResponse();
  } else {
    Serial.println("No response from server.");
  }
}

void extractIDFromCustomResponse() {
  int idIndex = ServerResponse.indexOf("ID=");
  if (idIndex != -1) {
    int idStart = idIndex + 3;  // Skip past "ID="
    int idEnd = ServerResponse.indexOf(",", idStart);  // Find the next comma (or end of the line)
    String idString = ServerResponse.substring(idStart, idEnd);
    float id = idString.toFloat();

    Serial.print("Extracted ID: ");
    Serial.println(id);
    TubeNumber = id;

    ServerResponse = "";  // Clear previous response data
  } else {
    Serial.println("ID not found.");
  }
}
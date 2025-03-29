#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Motor control pins
int enableRightMotor = D1;  // GPIO5
int rightMotorPin1 = D2;    // GPIO4
int rightMotorPin2 = D3;    // GPIO0 (be cautious with this pin during boot)
int enableLeftMotor = D4;   // GPIO2 (be cautious with this pin during boot)
int leftMotorPin1 = D5;     // GPIO14
int leftMotorPin2 = D6;     // GPIO12

// Web server running on port 80
ESP8266WebServer server(80);

// Motor control state (0 = stop, 1 = forward, 2 = backward, 3 = left, 4 = right)
int motorState = 0; // Initially stopped

// Motor control function
void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  // Right motor control
  if (rightMotorSpeed < 0)
  {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
  }
  else if (rightMotorSpeed > 0)
  {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);      
  }
  else
  {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, HIGH);      
  }

  // Left motor control
  if (leftMotorSpeed < 0)
  {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);    
  }
  else if (leftMotorSpeed > 0)
  {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);      
  }
  else
  {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);      
  }

  // PWM control
  analogWrite(enableRightMotor, abs(rightMotorSpeed));
  analogWrite(enableLeftMotor, abs(leftMotorSpeed));  
}

// Webpage HTML with 5 buttons (Forward, Backward, Left, Right, Stop)
String webpage = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <title>ESP8266 Motor Control</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        text-align: center;
      }
      .btn {
        width: 150px;
        padding: 10px;
        margin: 5px;
        font-size: 18px;
        background-color: #4CAF50;
        color: white;
        border: none;
        cursor: pointer;
      }
      .btn:active {
        background-color: #3e8e41;
      }
    </style>
  </head>
  <body>
    <h1>Control Motors</h1>
    <button class="btn" onclick="move('forward')">Forward</button><br>
    <button class="btn" onclick="move('backward')">Backward</button><br>
    <button class="btn" onclick="move('left')">Turn Left</button><br>
    <button class="btn" onclick="move('right')">Turn Right</button><br>
    <button class="btn" onclick="move('stop')">Stop</button><br>
    
    <script>
      function move(direction) {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/" + direction, true);
        xhttp.send();
      }
    </script>
  </body>
</html>
)rawliteral";

// Setup the motor control pins
void setupMotorPins() {
  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  
  // Initialize motors to stop
  rotateMotor(0, 0);
}

// Handle the requests from the webpage
void handleMotorControl() {
  String request = server.uri();
  
  // Process motor control based on URL path
  if (request == "/forward") {
    if (motorState != 1) {  // Only change direction if it's not already forward
      rotateMotor(255, 255);  // Forward
      motorState = 1;  // Set motor state to forward
    }
    server.send(200, "text/html", webpage);
  } else if (request == "/backward") {
    if (motorState != 2) {  // Only change direction if it's not already backward
      rotateMotor(-255, -255);  // Backward
      motorState = 2;  // Set motor state to backward
    }
    server.send(200, "text/html", webpage);
  } else if (request == "/left") {
    if (motorState != 3) {  // Only change direction if it's not already turning left
      rotateMotor(-255, 255);  // Turn Left
      motorState = 3;  // Set motor state to left turn
    }
    server.send(200, "text/html", webpage);
  } else if (request == "/right") {
    if (motorState != 4) {  // Only change direction if it's not already turning right
      rotateMotor(255, -255);  // Turn Right
      motorState = 4;  // Set motor state to right turn
    }
    server.send(200, "text/html", webpage);
  } else if (request == "/stop") {
    rotateMotor(0, 0);  // Stop motors
    motorState = 0;  // Set motor state to stopped
    server.send(200, "text/html", webpage);
  }
}

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);

  // Set up the ESP8266 in SoftAP mode
  WiFi.softAP("ESP8266_MotorControl", "123456789");  // Set up the WiFi Access Point
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());  // Display IP address of ESP8266
  
  // Initialize motor pins
  setupMotorPins();
  
  // Serve the HTML webpage
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", webpage);  // Send HTML webpage
  });

  // Handle motor control via URL
  server.on("/forward", HTTP_GET, handleMotorControl);
  server.on("/backward", HTTP_GET, handleMotorControl);
  server.on("/left", HTTP_GET, handleMotorControl);
  server.on("/right", HTTP_GET, handleMotorControl);
  server.on("/stop", HTTP_GET, handleMotorControl);

  // Start the server
  server.begin();
}

void loop() {
  // Handle client requests
  server.handleClient();
}

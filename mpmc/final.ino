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

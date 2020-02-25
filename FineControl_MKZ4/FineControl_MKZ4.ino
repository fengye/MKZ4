/*
 * Code modified by Ye Feng
 * 
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <Servo.h>
#include "webpage.h"

/* set I2C library*/
#define ADDR1  0x64

#define LED_PIN         12
#define SERVO_PWM_PIN   16

#define LED_H       (digitalWrite( 12, HIGH ))
#define LED_L       (digitalWrite( 12, LOW ))

#define CMD_STOP          0
#define CMD_FORWARD       1
#define CMD_REVERSE       2
#define CMD_BRAKE         3
#define CMD_BRAKE_FROM_FORWARD_TO_REVERSE  4
#define CMD_BRAKE_FROM_REVERSE_TO_FORWARD  5

#define forward       0x01
#define reverse       0x02

// NOTE: modifiy those 3 values after servo calibration
#define servo_left    90
#define servo_neutral 102
#define servo_right   115

#define MOTOR_MIN     0x06 // 0.48V
#define MOTOR_MAX     0x38 // 4.5V

const char *ssid = "MKZ4";
const char *password = "";

byte state = CMD_STOP;
byte vset = MOTOR_MIN;
byte dir = 0;
byte servo_dir = servo_neutral;
byte servo_dir_change = 2;
unsigned long last_drive_timestamp = 0;
const unsigned long DRIVE_TIMEOUT = 200;

ESP8266WebServer server(80);
ESP8266WebServer server_8080(8080);
Servo servo;


void motor_func(char addr , char duty){
  Wire.beginTransmission(addr);
  Wire.write(0x00);
  Wire.write(duty);
  Wire.endTransmission();
}

void stop_motor(){
  // actually coast motor, without applying brake force
  LED_L;
  motor_func(ADDR1 , (vset << 2) | 0x00); // coast
  delay(10);
  LED_H;

  state = CMD_STOP;
}

void brake_motor(){
  LED_L;
  motor_func(ADDR1 , (vset << 2) | 0x00); // coast
  delay(10);
  motor_func(ADDR1 , (vset << 2) | 0x03); // brake
  delay(10);
  LED_H;

  state = CMD_STOP;
}


void blink5()
{
  for(int i = 0; i < 5; ++i)
  {
    LED_H;
    delay(50);
    LED_L;
    delay(50);
  }
  LED_L;
}

void handle_staticpage() {
  server.send(200, "text/html", webpage);
}

void handle_stop() {
  Serial.print("stop\r\n");
  LED_L;
    //servo.write(servo_neutral);
    stop_motor();
    state = CMD_STOP;
  LED_H;
  server_8080.send(200, "text/html", "");
}

void handle_drive() {
  if (server_8080.hasArg("arg_x") && server_8080.hasArg("arg_y"))
  {
    LED_L;
      String str_arg_x = server_8080.arg("arg_x");
      String str_arg_y = server_8080.arg("arg_y");

      int arg_x = str_arg_x.toInt();
      int arg_y = str_arg_y.toInt();

      int angle = map(arg_x, -100, 100, servo_left, servo_right);
      servo.write(angle);

      if ((state == CMD_FORWARD && arg_y > 0))
      {
        // first has to stop
        brake_motor();
        state = CMD_BRAKE_FROM_FORWARD_TO_REVERSE;

        Serial.println("drive but get inverse brake!");
      }
      else if (state == CMD_BRAKE_FROM_FORWARD_TO_REVERSE)
      {
        Serial.println("Ignore drive request. To drive, first to stop");
      }
      else if (arg_y != 0)
      {
        vset = map(abs(arg_y), 0, 100, MOTOR_MIN, MOTOR_MAX);
        byte dir = arg_y < 0 ? forward : reverse;
        byte reg0 = (vset << 2) | dir;
        motor_func(ADDR1, reg0);

        state = arg_y < 0 ? CMD_FORWARD : CMD_REVERSE;
        last_drive_timestamp = millis();

        Serial.print("drive x:");
        Serial.print(arg_x);
        Serial.print(" y: ");
        Serial.print(arg_y);
        Serial.println();
      }
    LED_H;
    server_8080.send(200, "text/html", "");
  }
  else
  {
    blink5();
    server_8080.send(404, "text/plain", "Invalid argument.");
  }
}


void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(SERVO_PWM_PIN, OUTPUT);
  
  LED_L;
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Started serial at 115200 bps");

  Wire.begin(4, 14);
  delay(40);
  Serial.println("I2C configured");
  
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("WiFi AP configured to: ");
  Serial.println(myIP);

  server.on("/", handle_staticpage);
  server_8080.on("/stop", handle_stop);
  server_8080.on("/drive", handle_drive);

  server.begin();
  server_8080.begin();
  
  Serial.println("HTTP server on 80 and 8080 started");

  servo.attach(SERVO_PWM_PIN);
  delay(100);
  servo.write(servo_neutral);
  delay(500);
  Serial.print("Servo neutralised");
  Serial.println();
  
  blink5();
  LED_H;

  Serial.print("Boot completed");
  Serial.println();
  delay(100);
}


void loop() {
  server.handleClient();
  server_8080.handleClient();

  // stop driving if no subsequent drive command in a certain amount of time, in case of signal loss
  // or client's browser throttles the form submitting.
  if (millis() - last_drive_timestamp > DRIVE_TIMEOUT &&
      (state == CMD_FORWARD || state == CMD_REVERSE))
  {
    LED_L;
      stop_motor();
      state = CMD_STOP;
    LED_H;
  }
}

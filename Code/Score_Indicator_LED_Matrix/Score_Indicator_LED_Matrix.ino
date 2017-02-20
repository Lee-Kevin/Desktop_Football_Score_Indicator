/*    
 * Score_Indicator.c
 * Gardening Demo for Arduino 
 *   
 * Copyright (c) 2015 seeed technology inc.  
 * Author      : Jiankai.li  
 * Create Time:  Aug 2015
 * Change Log : 
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <SoftwareSerial.h>
#include "LEDMatrix.h"
 
#include "pitches.h"

#define RECEIVE_PIN      4
#define TRANSMIT_PIN     5
// #define CLK1 4//pins definitions for TM1637 and can be changed to other ports
// #define DIO1 5

#define DEBUG           1
#define IRDetection1    A0    // to detect if there is a ball in 
#define IRDetection2    A2
#define Button          2
#define Buzzer          6
#define TimeInterval    5000  // 1s
#define SensorLimen     800
#define OneSecond       1000 // 1s

SoftwareSerial matrix_serial(RECEIVE_PIN, TRANSMIT_PIN);

LEDMatrix led;

int8_t NumTab[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};//0~9,A,b,C,d,E,F
int8_t GoalFlag = 0;
int8_t score1 = 0;
int8_t score2 = 0;
int8_t ScoreHistory[10][2] = {0};
int8_t ScoreIndex = 0;
volatile int Ledstate = LOW;
long buttontime = 0;
long starttime = 0;

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

const uint8_t seeedLogo_32x20 [] = {
0x00, 0x08, 0x40, 0x00, 0x00, 0x18, 0x60, 0x00,
0x00, 0x38, 0x70, 0x00, 0x00, 0x30, 0x30, 0x00,
0x00, 0x70, 0x38, 0x00, 0x00, 0x70, 0x38, 0x00,
0x10, 0x70, 0x38, 0x20, 0x10, 0x70, 0x38, 0x20,
0x18, 0x70, 0x38, 0x60, 0x18, 0x70, 0x38, 0x60,
0x1C, 0x70, 0x38, 0x60, 0x0C, 0x70, 0x38, 0xE0,
0x0E, 0x30, 0x38, 0xC0, 0x0E, 0x30, 0x31, 0xC0,
0x07, 0x18, 0x33, 0x80, 0x03, 0x98, 0x67, 0x80,
0x01, 0xC8, 0x47, 0x00, 0x00, 0xE0, 0x1E, 0x00,
0x00, 0x30, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00
};


void setup() 
{
#if DEBUG
    Serial.begin(9600); 
    Serial.println("test!");
#endif
    delay(500);
    led.clear();
    led.setReverse(0xff);
    // Display1.init();
    // Display1.set(BRIGHT_TYPICAL);
    // Display1.point(true);
    led.setBrightness(50);
    led.drawImage(16, 0, 32, 20, seeedLogo_32x20, RED);       // draw image. This example provides an example image data. You can use your own image data. Note: Max:64*64!
    
    delay(1000);
    led.clear();
    
    displayNum1(ScoreHistory[0][0]);
    displayNum2(ScoreHistory[0][1]);
    // displayNum1(23);
    // displayNum2(56);

    /* Init Buzzer  */
    pinMode(Buzzer,OUTPUT);
    digitalWrite(Buzzer,LOW);
    /* Init Button  */
    pinMode(Button,INPUT);
    attachInterrupt(0,ButtonFun,RISING); 
    ScoreIndex = 50;
    starttime = millis() + TimeInterval;
}


void loop() 
{
    if ((millis() - starttime) > TimeInterval) {
        //Serial.println("Game begin");
        if(1 == IRDetection1Fun()) {
            ScoreIndex++;
            ScoreHistory[(ScoreIndex%10)][1] = ScoreHistory[(ScoreIndex - 1)%10][1];  // store the 
            ScoreHistory[(ScoreIndex%10)][0] = ScoreHistory[(ScoreIndex - 1)%10][0];
            ScoreHistory[(ScoreIndex%10)][0]++;
            displayNum1(ScoreHistory[(ScoreIndex)%10][0]);
            if( (ScoreHistory[(ScoreIndex%10)][0] == 5 ||  ScoreHistory[(ScoreIndex%10)][0] == 10 ||  ScoreHistory[(ScoreIndex%10)][0] == 20) && (ScoreHistory[(ScoreIndex%10)][0] > ScoreHistory[(ScoreIndex%10)][1])) {
               SuccessMusicPlay();
            }
            starttime = millis();
            BallIn();
        } 
        else if (1 == IRDetection2Fun()) {
            ScoreIndex++;
            ScoreHistory[(ScoreIndex%10)][1] = ScoreHistory[(ScoreIndex - 1)%10][1];  // store the 
            ScoreHistory[(ScoreIndex%10)][0] = ScoreHistory[(ScoreIndex - 1)%10][0];
            ScoreHistory[(ScoreIndex%10)][1]++;
            displayNum2(ScoreHistory[(ScoreIndex)%10][1]);
            starttime = millis();
            if( (ScoreHistory[(ScoreIndex%10)][1] == 5 ||  ScoreHistory[(ScoreIndex%10)][1] == 10 ||  ScoreHistory[(ScoreIndex%10)][1] == 20) &&(ScoreHistory[(ScoreIndex%10)][1] > ScoreHistory[(ScoreIndex%10)][0])) {
               SuccessMusicPlay();
            }
            BallIn();
        } 
        else {
            starttime = millis() + TimeInterval;
        }
    } 
    if (1 == digitalRead(Button)) {
        Serial.print("ScoreIndex:");
        Serial.println(ScoreIndex);
        switch(ButtonPressed()) {
        Serial.print("ButtonPressed:");
        case 1:
            if(ScoreIndex-- <= 50) {  
                ScoreIndex = 50;
            }
            Serial.println(1);
            displayNum1(ScoreHistory[(ScoreIndex)%10][0]);
            displayNum2(ScoreHistory[(ScoreIndex)%10][1]);
            break;
        case 2:
            Serial.println(2);
            ScoreIndex = 50;
            ScoreHistory[(ScoreIndex)%10][0] = 0;
            ScoreHistory[(ScoreIndex)%10][1] = 0;
            displayNum1(ScoreHistory[(ScoreIndex)%10][0]);
            displayNum2(ScoreHistory[(ScoreIndex)%10][1]);
            break;
        default:
            break;
        }
    }

}

int8_t IRDetection1Fun() 
{
    int sensorValue = analogRead(IRDetection1);
    Serial.print("IR1 Vaule");
    Serial.println(sensorValue);
    if (sensorValue < SensorLimen) {
        return 1;
    } else {
        return 0;
    }
}

int8_t IRDetection2Fun() 
{
    int sensorValue = analogRead(IRDetection2);
    Serial.print("IR2 Vaule");
    Serial.println(sensorValue);
    
    if (sensorValue < SensorLimen) {
        return 1;
    } else {
        return 0;
    }
}

void displayNum1(int num1) 
{
    char temp;
    temp = num1/10 + '0';
    // led.displayChar(0, 1, '2', FONT_12X16, RED);
    led.displayChar(0, 1, temp, FONT_12X16, RED);
    // led.displayChar(13,1,'4',FONT_12X16, RED);
    temp = num1%10 + '0';
    led.displayChar(13,1,temp,FONT_12X16, RED);
    // Display1.display(3,num1%10);
    // Display1.display(2,num1%100/10);
    led.displayChar(26,0,':',FONT_12X16, RED);
}
void displayNum2(int num1) 
{
    char temp;
    temp = num1/10 + '0';
    // led.displayChar(38, 1, '3', FONT_12X16, RED);
    led.displayChar(38, 1, temp, FONT_12X16, RED);
    // led.displayChar(51,1,'5',FONT_12X16, RED);
    temp = num1%10 + '0';
    led.displayChar(51,1,temp,FONT_12X16, RED);
    // Display1.display(1,num1%10);
    // Display1.display(0,num1/10);  
}
void ButtonFun() 
{
    Serial.print("button:");
    Serial.println(digitalRead(Button));
}
void BallIn() 
{
    digitalWrite(Buzzer,HIGH);
    delay(100);
    digitalWrite(Buzzer,LOW);
}
int8_t ButtonPressed()
{
    static long buttontime = 0;
    buttontime = millis();
    while (1 == digitalRead(Button));
    if ((millis() - buttontime) < OneSecond) {
        BallIn();
        return 1;
    }
    else {
        BallIn();
        return 2;
    }
    
}

void SuccessMusicPlay(void) 
{
  for(int i = 0;i<2; i++) {
    for (int thisNote = 0; thisNote < 8; thisNote++) {
    digitalWrite(Buzzer,Ledstate);
    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(8, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
    Ledstate = !Ledstate;
   }
  }
}

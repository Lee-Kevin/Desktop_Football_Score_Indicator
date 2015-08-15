/*    
 * Gardening.c
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
#include <Wire.h>
#include "TM1637.h"
#include "Seeed_QTouch.h"

#define CLK1 4//pins definitions for TM1637 and can be changed to other ports
#define DIO1 5
#define CLK2 6//pins definitions for TM1637 and can be changed to other ports
#define DIO2 7

#define IRDetection1    2
#define IRDetection2    3
#define TimeInterval    5000  // 1s


TM1637 Display1(CLK1,DIO1);
// TM1637 Display2(CLK2,DIO2);
int8_t NumTab[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};//0~9,A,b,C,d,E,F
int8_t GoalFlag = 0;
int8_t score1 = 0;
int8_t score2 = 0;
long time1;
void setup() 
{

    Serial.begin(9600); 
    Serial.println("test!");
    Display1.init();
    Display1.set(BRIGHT_TYPICAL);
    Display1.point(true);

    
    displayNum1(score1);
    displayNum2(score2);
    
    QTouch.setMaxDuration(62);
    
    /* Init IR detection */
    pinMode(IRDetection1,INPUT);
    attachInterrupt(0,IRDetection1Fun,RISING); 
    
    pinMode(IRDetection2,INPUT);
    attachInterrupt(1,IRDetection2Fun,RISING);
}


void loop() 
{
    int button =  QTouch.touchNum();

    if (button >= 0) {
        switch (button) {
        case 0:
            time1 = millis();                          // get origin time
            Serial.print("key 0 touched: ");
            while(QTouch.isTouch(0));                       // unless key released
            Serial.print(millis() - time1);                 
            Serial.println(" ms");
            
            score1 = 0;
            score2 = 0;
            
            displayNum1(score1);
            displayNum2(score2);
            
            break;
        case 1:
            time1 = millis();                          // get origin time
            Serial.print("key 1 touched: ");
            while(QTouch.isTouch(1));                       // unless key released
            Serial.print(millis() - time1);                 
            Serial.println(" ms");
            if (-- score1 <= 0) {
                score1 = 0;
            }
            displayNum1(score1);
            break;
        case 2:
            time1 = millis();                          // get origin time
            Serial.print("key 2 touched: ");
            while(QTouch.isTouch(2));                       // unless key released
            Serial.print(millis() - time1);                 
            Serial.println(" ms");
            
            if (-- score2 <= 0) {
                score2 = 0;
            }
            displayNum2(score2);
            break;
        default:
            break;
        }
    } else {
        switch (GoalFlag) {
        case 1:
            if (++score1 >= 99) {
                score1 = 0;
            }
            displayNum1(score1);
            delay(TimeInterval);
            GoalFlag = 0;
            break;
        case 2:
            if (++score2 >= 99) {
                score2 = 0;
            }
            displayNum2(score2);
            delay(TimeInterval);
            GoalFlag = 0;
            break;
        default:
            break;
        }
    }
}

void IRDetection1Fun() 
{
    if (digitalRead(IRDetection1) == 1) {
        delay(100);
        if(digitalRead(IRDetection1) == 1) {
            GoalFlag = 1;
            Serial.println("IRDetection1Fun");
        }
    }
}

void IRDetection2Fun() 
{
    if (digitalRead(IRDetection2) == 1) {
        delay(100);
        if(digitalRead(IRDetection2) == 1) {
            GoalFlag = 2;
            Serial.println("IRDetection2Fun");
        }
    }
}

void displayNum1(int num1) 
{
    Display1.display(3,num1%10);
    Display1.display(2,num1%100/10);
}
void displayNum2(int num1) 
{
    Display1.display(1,num1%10);
    Display1.display(0,num1/10);  
}















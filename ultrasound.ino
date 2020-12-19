//
//    usage.h
//    Purpose: Demonstration of how to use the HC-SR04 driver
//
//    Author: Richard Nash
//    Version: 1.0.1

#include "HC-SR04.h"

/*
 The HC-SR04 device is a 5V device. So, VIN (which is 5V when connected to a USB power supply)
 is used to power it. Also, the "Echo" pin will present a 5V pulse, which can be connected
 to any of the D* GPIO pins, as they are 5V tolerant. However, they cannot be connected to
 non-5V tolerant pins, like the A* pins, even if in digitial mode.

 This example expects the wiring to be as follows:
    Photon  HC-SR04
    GND     GND
    VIN     VCC
    A0      Trig
    D0      Echo
*/

// trigger / echo pins
const int triggerPin = D1;
const int echoPin = D0;
const int triggerPin2 = D4;
const int echoPin2 = D3;
HC_SR04 rangefinder = HC_SR04(triggerPin, echoPin);
HC_SR04 rangefinder2 = HC_SR04(triggerPin2, echoPin2);
//TODO: make device get the bin default depth on startup
double defaultDepthCm = 50.0;
float threshold000 = 0.00;
float threshold020 = 0.20;
float threshold040 = 0.40;
float threshold060 = 0.60;
float threshold080 = 0.80;
float threshold100 = 1.00;
//unsigned long publish_delay =       60000;
//unsigned long previousPublish =      0   ;

void setup()
{
    rangefinder.init();
    rangefinder2.init();
    
    Serial.begin(9600);
    /*
    while (!Serial.available()) {
        Serial.println("Press any key to start.");
		Particle.process();
    }
    */
    delay (1000);
}

void loop()
{
    //if (previousPublish == 0) {
	//		previousPublish = millis();
	//	}
    //if (millis()  - previousPublish >= publish_delay) {
    //unsigned long start = micros();
    float cm = rangefinder.distCM();
    //unsigned long calcTime = micros() - start;
    delay(1000);
    //unsigned long start2 = micros();
    float cm_2 = rangefinder2.distCM();
    //unsigned long calcTime2 = micros() - start2;
    
    //Fill percentage
    /*
    if(((cm+cm_2)/2)/defaultDepthCm < threshold020){
        char result[5] = "00";
        Particle.publish("AU665081_distance", result, PRIVATE);
    }
    else if(((cm+cm_2)/2)/defaultDepthCm >= threshold020 && ((cm+cm_2)/2)/defaultDepthCm < threshold040){
        char result[5] = "20";
        Particle.publish("AU665081_distance", result, PRIVATE);
    }
    else if(((cm+cm_2)/2)/defaultDepthCm >= threshold040 && ((cm+cm_2)/2)/defaultDepthCm < threshold060){
        char result[5] = "40";
        Particle.publish("AU665081_distance", result, PRIVATE);
    }
    else if(((cm+cm_2)/2)/defaultDepthCm >= threshold060 && ((cm+cm_2)/2)/defaultDepthCm < threshold080){
        char result[5] = "60";
        Particle.publish("AU665081_distance", result, PRIVATE);
    }
    else if(((cm+cm_2)/2)/defaultDepthCm >= threshold080 && ((cm+cm_2)/2)/defaultDepthCm < threshold100){
        char result[5] = "80";
        Particle.publish("AU665081_distance", result, PRIVATE);
    }
    else if(((cm+cm_2)/2)/defaultDepthCm >= threshold100){
        char result[5] = "100";
        Particle.publish("AU665081_distance", result, PRIVATE);
    }
    */
    //TODO: remove excess code
    
    //Empty char array to convert float into string type
    char cm1[20];
    sprintf(cm1, "%f", cm); // (array, write into string parameter, float)
    Serial.println("\n");
    Serial.println(cm1);
   
    char cm2[20];
    sprintf(cm2, "%f", cm_2);
    Serial.println("\n");
    Serial.println(cm2);
    
    char cm3[20];
    sprintf(cm3, "%f", cm+cm_2);
    Serial.println("\n");
    Serial.println(cm3);
    
    //const char * eventName = "AU665081_distance";
    //const char * myWriteAPIKey = "XBEUIB1C7NNVDO4S";
    ///char msg[256];
    //snprintf(msg, sizeof(msg), "{\"1\":\"%.1f\",\"2\":\"%.1f\",\"k\":\"%s\"}", cm1, cm2, myWriteAPIKey);
    //Particle.publish(eventName, msg, PRIVATE, NO_ACK);
    Particle.publish("AU665081_distance", cm3, PRIVATE);
    //previousPublish = millis();
    delay(60000);
    //}else{
    //    previousPublish = 0;
    //}
    //delay(1000);
}

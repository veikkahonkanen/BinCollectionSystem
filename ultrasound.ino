#include <HttpClient.h>

//Libraries included are from Particle IDE, made by Richard Nash

#include "HC-SR04.h"
#define HOOK_REPEAT 3

//TODO: make device get the bin default depth on startup
// Particle variable
static double defaultDepthCm = 28.0;

// Particles function (setter for the variable)
int setDepthInCM(String extra)
{
    defaultDepthCm = extra.toFloat();
    return 0;
}

void initializeParticleVariablesAndFunctions()
{
    Particle.variable("defaultDepthCm", &defaultDepthCm, DOUBLE);

    Particle.function("setDefaultDepthInCM", setDepthInCM);
}

// trigger / echo pins
const int triggerPin = D1;
const int echoPin = D0;

// Setup
HC_SR04 rangefinder = HC_SR04(triggerPin, echoPin);

float threshold000 = 0.00;
float threshold020 = 0.20;
float threshold040 = 0.40;
float threshold060 = 0.60;
float threshold080 = 0.80;
float threshold100 = 1.00;

int tiSuccessfulUpdates = 0;
static int n = 0;
static float currentDistance = 10.0;

void server_response_checker(const char *event, const char *data) {
  Serial.println(event);
  Serial.println(data);
}

//This is needed to see if publishing succeeded
static void updateFillPercentage()
{
    int tiInvalidCalls;
    bool tiValidResponse;

    tiValidResponse = false;
    tiInvalidCalls = 0;

    while (!tiValidResponse && tiInvalidCalls < HOOK_REPEAT)
    {
        float cm = rangefinder.distCM();
        delay(1000);
        
        //All HC-SR04 error codes are sub-zero
        if (cm < 0)
        {
            // error
            Serial.printf("Call #%d: fullness update failed due to internal error!\n", tiInvalidCalls);
            tiInvalidCalls++;
        }
        else
        {
            Serial.printf("We obtained distance of %f \n", cm);
            
            if (!Particle.connected())
            {
                //No internet connection, network failure
                Serial.printf("Call #%d: fullness update failed due to disconnection!\n", tiInvalidCalls);
                tiInvalidCalls++;
            }
            else
            {
                Serial.println("TI: Valid data.");
                currentDistance = cm;
                tiInvalidCalls = 0;
                tiValidResponse = true;
            }
        }
    }

    // Even after HOOK_REPEAT tries, module failed, we need to reuse the old data
    // and try again next time device will wake up
    if (!tiValidResponse)
    {
        Serial.println("Publishing failed!");
        if (tiSuccessfulUpdates > 0)
        {
            Serial.println("Using latest valid data...");
        }
        else
        {
            // We cannot show anything, because there is no valid data
            Serial.println("We cannot show anything, because there is no valid data!");
        }
    }
    else
    {
        tiSuccessfulUpdates++;
    }
}

void setup()
{
    // Put initialization like pinMode and begin functions here.
    initializeParticleVariablesAndFunctions();

    // Set timezone to Copenhagen
    Time.zone(1);
    
    rangefinder.init();
    
    Serial.begin(9600);
    delay (1000);
    Particle.subscribe("hook-response/distance", server_response_checker);
    Particle.subscribe("hook-response/status", server_response_checker);
}

void loop()
{

    updateFillPercentage();
    
    //Fill percentage
    //Two modules: uncomment the comments below and remove 'cm' from the formulas
    int fullness = 0;
    
    if((currentDistance / defaultDepthCm) >= threshold100){
        fullness = 0;
    }
    else if((currentDistance / defaultDepthCm) >= threshold080 && (currentDistance / defaultDepthCm) < threshold100){
        fullness = 20;
    }
    else if((currentDistance / defaultDepthCm) >= threshold060 && (currentDistance / defaultDepthCm) < threshold080){
        fullness = 40;
    }
    else if((currentDistance / defaultDepthCm) >= threshold040 && (currentDistance / defaultDepthCm) < threshold060){
        fullness = 60;
    }
     else if((currentDistance / defaultDepthCm) >= threshold020 && (currentDistance / defaultDepthCm) < threshold040){
        fullness = 80;
    }
    else if((currentDistance / defaultDepthCm) < threshold020){
        fullness = 100;
    }
    //In case if the distance was measured as valid, but still can't calculate to values above
    else {
        fullness = -1;
    }
    Serial.println("Publishing fullness " + String(fullness));
    Particle.publish("distance", "{ \"1\": " + String(fullness) + "}", 60, PRIVATE, WITH_ACK);
    delay(1000);
    
    int battery = 56;
    Serial.println("Publishing battery status " + String(battery));
    Particle.publish("status", "{ \"1\": " + String(battery) + "}", 60, PRIVATE, WITH_ACK);
    
    
    Serial.printlnf("Sleep cycle #%d: Going to sleep for %d minutes...", n, 480);
    SystemSleepConfiguration config;
    config.mode(SystemSleepMode::STOP)
        .duration(8h)
        .flag(SystemSleepFlag::WAIT_CLOUD);
    SystemSleepResult result = System.sleep(config);

    if (result.error() != 0)
    {
        Serial.printlnf("Something went wrong during #%d sleep cycle.", n);
    }
    else
    {
        Serial.printlnf("Sleep cycle #%d: Device successfully woke up from sleep.", n);
    }
    
    delay(1000);
    n++;
}

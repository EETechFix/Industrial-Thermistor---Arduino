#include "arduino_secrets.h"
/* 
  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  int Steinhart_Temp1;
  int Steinhart_Temp2;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"
#include <Wire.h>  // I2C bus OLED communication support
#include <Adafruit_GFX.h> // OLED graphics support
#include <Adafruit_SSD1306.h> // OLED display

#define SCREEN_WIDTH 128  // display width, in pixels
#define SCREEN_HEIGHT 64  // display height, in pixels
#define OLED_RESET -1 //Reset pin # (-1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D  // OLED

const int sensorPin1 = A0;
const int sensorPin2 = A2;

const float Rs1 = 9980.0;  // series resistor Ωohms (measured)
const float Rs2 = 9980.0;  // series resistor Ωohms (measured)
const float alpha = .95;   //data smoothing coefficient (0.0 - 1.0)

//Steinhart coefficients thermistor #1
const float A_1 = 8.756777012E-04, B_1 = 2.534289024E-04, C_1 = 1.845247963E-07; 

//Steinhart coefficients thermistor #2
const float A_2 = 8.756777012E-04, B_2 = 2.534289024E-04, C_2 = 1.845247963E-07;    

const float SSEC1 = 0.0; // steady-state error correction
const float SSEC2 = 0.0; // steady-state error correction

float Rt1;   // thermistor1 resistance Ωohms
float Rt2;  // thermistor2 resistance Ωohms
float filterVal_1 = 2048.0;  //set low-pass filter previous value
float filterVal_2 = 2048.0;  //set low-pass filter previous value

//float Steinhart_Temp1;  //calculated thermistor1 temperatureºC
//float Steinhart_Temp2; // calculated thermistor2 temperatureºC

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(2500); 
  
   //  SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);  // Don't proceed, wait forever
  }
  display.clearDisplay();
  display.display();

  analogReadResolution(12); Serial.println(" 12-bit read resolution set ");


  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  
  readThermistors(); // read resistance (Ω-ohms) from both termistors
  
  // -- Steinhart Equation -- get temperatures from thermistors
  Steinhart_Temp1 = (pow(A_1 + B_1 * log(Rt1) + C_1 * pow(log(Rt1), 3), -1) - 273.15) + SSEC1;
  Steinhart_Temp2 = (pow(A_2 + B_2 * log(Rt2) + C_2 * pow(log(Rt2), 3), -1) - 273.15) + SSEC2;

  updateMonitor(); // display temperature/resistance on serial monitor

  updateOLED(); //display temperatures on Oled display
  
  delay(200);
}

void readThermistors() {
  float adcVal_1 = (float)analogRead(sensorPin1);
  filterVal_1 = (alpha * filterVal_1) + ((1.0 - alpha) * adcVal_1); //Low-Pass Filter/data smoothing
  Rt1 = Rs1 * ((4095.0 / filterVal_1) - 1.0); //calculate thermistor resistance Ω-ohms

  float adcVal_2 = (float)analogRead(sensorPin2);
  filterVal_2 = (alpha * filterVal_2) + ((1.0 - alpha) * adcVal_2); //Low-Pass Filter/data smoothing
  Rt2 = Rs2 * ((4095.0 / filterVal_2) - 1.0); //calculate thermistor resistance Ω-ohms
}

void updateMonitor() {
  Serial.print("#T1  " + String(Steinhart_Temp1, 1) + "ºC");
  Serial.print("  " + String(Rt1, 0) + " Ω");
  Serial.print ("  filterVal_1: " + String(filterVal_1, 1));
  Serial.println();
  Serial.print("#T2  " + String(Steinhart_Temp2, 1) + "ºC  " );
  Serial.print ( String(Rt2, 0) + " Ω");
  Serial.println ("  filterVal_2: " + String(filterVal_2, 1));
  Serial.println("===========================================");
}

void updateOLED() {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);

  display.setCursor(15, 10);
  display.print(Steinhart_Temp1, 1);
  display.print("C");

  display.setCursor(15, 40);
  display.print(Steinhart_Temp2, 1);
  display.print("C");
  display.display();
}













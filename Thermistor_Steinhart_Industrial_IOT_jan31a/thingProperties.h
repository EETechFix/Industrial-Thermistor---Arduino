// Code generated by Arduino IoT Cloud, DO NOT EDIT.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char SSID[]     = SECRET_SSID;    // Network SSID (name)
const char PASS[]     = SECRET_OPTIONAL_PASS;    // Network password (use for WPA, or use as key for WEP)


int Steinhart_Temp1;
int Steinhart_Temp2;

void initProperties(){

  ArduinoCloud.addProperty(Steinhart_Temp1, READ, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(Steinhart_Temp2, READ, 60 * SECONDS, NULL);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

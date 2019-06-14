#include <Wire.h>
#include <MKRGSM.h>             // GSM library
#include "MMA7660.h"

MMA7660 accelemeter;
GSM gsmAccess;    // GSM access: include a 'true' parameter for debug enabled
GPRS gprsAccess;  // GPRS access
GSMClient client; // Client service for TCP connection
bool connected = false;

const char PINNUMBER[] = "";
char apn[20] = "soracom.io";
char login[8] = "sora";
char password[8] = "sora";
char url[] = "harvest.soracom.io";
char path[] = "/";

void setup()
{
	accelemeter.init();  
	Serial.begin(9600);
 while (!connected) {
   if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&
       (gprsAccess.attachGPRS(apn, login, password) == GPRS_READY)) {
     connected = true;
   } else {
     Serial.println("Not connected");
     delay(1000);
   }
 }
  Serial.println("Setup Finished");
}
void loop()
{
	int8_t x;
	int8_t y;
	int8_t z;
	float ax,ay,az;
  float xa=0;
  float ya=0;
  float za=0;
  
for (int i =0; i<10;i++)
{
  accelemeter.getAcceleration(&ax,&ay,&az);
  xa=max(abs(ax),xa);
  ya=max(abs(ay),ya);
  za=max(abs(az),za);
  delay(100);
}
  String PostData = "{\"xa\":"+ String(xa)+",\"ya\":"+ String(ya) +",\"za\":"+ String(za)+"}";
  if(Serial) Serial.print("Connecting and sending POST request to harvest.soracom.io...");
  int res_connect;
  res_connect = client.connect(url, 80);

        if (res_connect) {
        client.println("POST / HTTP/1.1");
        client.println("Host: harvest.soracom.io");
        client.println("User-Agent: Arduino/1.0");
        client.println("Connection: close");
        client.print("Content-Length: ");
        client.println(PostData.length());
        client.println();
        client.println(PostData);

        if(Serial) Serial.println("Works");

        // Read abd print the response from the server
        if(Serial) Serial.print("Receiving response...");
        boolean test = true;
        while (test) {
          // if there are incoming bytes available, print them
          if (client.available()) {
            char c = client.read();
            if(Serial) Serial.print(c);
          }

          // if the server's disconnected, stop the client:
          if (!client.connected()) {
            client.stop();
            test = false;
          }
        }
      } else {
        // if we didn't get a connection to the server
        if(Serial) Serial.println("Server Error");
      }
      delay(1000);
}

/*
  RF_Sniffer
  
  Hacked from http://code.google.com/p/rc-switch/
  
  by @justy to provide a handy RF code sniffer
*/

#include "RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

RCSwitch mySwitch;
 

int BinaryStringToDecimal(std::string a) 
{
    int Rslt = 0;
    int Mask = 1;
    int Neg  = 1;
    if (a.at(0) == '1') {
        for (int i = a.length()-1; i>=0; --i) {
            if (a.at(i) == '1') {
                 a.at(i) = '0';
            } else {
                 a.at(i) = '1';
            }
        }
        Neg = -1;
        std::cout << "XXX: " << a << "\n";
    }
    for (int i = a.length()-1; i >= 0; --i, Mask <<= 1) {
        if (a.at(i) != '0') {
            Rslt |= Mask;
        }
    }
    if (Neg < 0) {
        Rslt = -(Rslt+1);
    }
    return (Rslt);
}

int main(int argc, char *argv[]) {
  
     // This pin is not the first pin on the RPi GPIO header!
     // Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
     // for more information.
     int PIN = 2;
     
     if(wiringPiSetup() == -1)
       return 0;

     mySwitch = RCSwitch();
     mySwitch.enableReceive(PIN);  // Receiver on inerrupt 0 => that is pin #2
     
     std::ofstream tempFile;
     std::ofstream humiFile;

     while(1) {
  
      if (mySwitch.available()) {
    
        int value = mySwitch.getReceivedValue();
    
        if (value == 0) {
          printf("Unknown encoding");
        } else {    
   
          printf("Received %i - %i - %s - %s\n", mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedValueBin(), mySwitch.getReceivedValueBin());
          if (mySwitch.getReceivedBitlength() == 28) {
              char* valueC = mySwitch.getReceivedValueBin();
              std::string value (valueC,28);
              std::string devIdMask = value.substr(0,4);
              std::cout << "Dev: " << devIdMask << "\n";
              if (devIdMask.compare("1011") == 0) {
                  float humi = (float)BinaryStringToDecimal(value.substr(4,12))/10;
                  float temp = (float)BinaryStringToDecimal(value.substr(16,12))/10;
                  std::cout << "Humi: " << humi << "\n";
                  std::cout << "Temp: " << temp << "\n";
                  humiFile.open("/home/pi/RC-humi.txt", std::ios::out | std::ios::trunc );
                  tempFile.open("/home/pi/RC-temp.txt", std::ios::out | std::ios::trunc );
                  humiFile << humi << std::endl;
                  tempFile << temp << std::endl;
                  humiFile.close();
                  tempFile.close();
              } else {
                  printf("Unknown device id");
              }
          }
          
        }
    
        mySwitch.resetAvailable();
    
      }
      
  
  }

  exit(0);


}

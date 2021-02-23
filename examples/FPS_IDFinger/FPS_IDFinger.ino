/*
* Christian Barran
* Subscribe to my Youtube Channel
* www.youtube.com/c/justbarran
*
* Git: https://github.com/justbarran/Just-a-Fingerprint-Scanner-GT-521FXX
* 
*  Use this example to Check ID 
*  
*  
*/

#define PRINTS_MAX 200 //200=GT_521F32 3000=GT_521F52
#define FINGER_TRYS 3

#include <GT_521F.h>

GT_521F fps(Serial1); // Use any Hardware Serial object 
                      // Will add Software Serial support later

void setup()
{
  delay(5000);
  Serial.begin(9600);
  while(!fps.begin(9600))
  {
    Serial.print(".");
  }
  int openStatus = fps.open(true);
  if(openStatus == NO_ERROR) {
    // Print extra info from SRAM data buffer
    Serial.print("\nDevice information:\n");
    for(uint8_t i=0; i<24; i++) {
      if(fps.dataBuf[i] < 0x10) {
        Serial.print(0x00, HEX);
        Serial.flush();
      }
      Serial.print(fps.dataBuf[i], HEX);
      Serial.flush();
    }
  } else {
    Serial.print("\nInitialization failed!\nstatus: ");
    Serial.print(openStatus, HEX);
    Serial.flush();
  }
}

void loop()
{
	// Identify fingerprint test
  Serial.println("PUT FINGER ON SENSOR");
	uint16_t openStatus = fps.open(true);
    if(openStatus == NO_ERROR) 
    {
      uint16_t checkLED = fps.cmosLed(true);
      if(checkLED == NO_ERROR)
      { 
        
        uint8_t FingerCount = 0;
        uint16_t checkFinger = FINGER_IS_NOT_PRESSED;
        while((checkFinger==FINGER_IS_NOT_PRESSED) && (FingerCount<FINGER_TRYS))
        {
          delay(5); 
          checkFinger = fps.isPressFinger();
          FingerCount++;                   
        }
        if(checkFinger == FINGER_IS_PRESSED)
        {
          checkFinger = fps.captureFinger();
          if(checkFinger == NO_ERROR)
          {
            checkFinger = fps.identify();
            if(checkFinger < PRINTS_MAX)
            {
              Serial.print("FINGER FOUND ID: ");
              Serial.println(checkFinger);
            }
            else
            {
              Serial.print("FINGER NOT FOUND ERROR: ");
              Serial.println(checkFinger,HEX);
            }
          }
        }
        else
        {
          Serial.print("FINGER FAIL: ");
          Serial.println(checkFinger,HEX);
        }
      }
      else
      {
        Serial.print("LED FAIL: ");
        Serial.println(checkLED,HEX);
      }
    } 
    else 
    {
      Serial.print("Initialization failed!\nstatus: ");
      Serial.print(openStatus, HEX);
      Serial.println();
    }
    fps.cmosLed(false);
	delay(3000);
}

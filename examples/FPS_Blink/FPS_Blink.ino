/*
* Christian Barran
* Subscribe to my Youtube Channel
* www.youtube.com/c/justbarran
*
* Git: https://github.com/justbarran/Just-a-Fingerprint-Scanner-GT-521FXX
* 
* Use this example to Blink 
* 
*/

#include <GT_521F.h>

GT_521F fps(Serial1); // Use any Hardware Serial object 
                      // Will add Software Serial support later

void setup()
{
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
	// FPS Blink LED Test
	while(fps.cmosLed(true) != NO_ERROR);
	delay(1000);
	while(fps.cmosLed(false) != NO_ERROR);
	delay(1000);
}

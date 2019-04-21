#include "PPMReader.h"

PPMReader ppmReader(2, 0, false);

void setup()
{
  Serial.begin(115200);
  Serial.println("ready");
  ppmReader.start();
}

void loop()
{

  uint8_t count = 0;
  while(ppmReader.isReceiving()){  
    // Print out the servo values
    Serial.print(ppmReader.get(count));
    Serial.print("  ");
    count++;
  }
  Serial.println("");

  delay(500); //you can even use delays!!!
}

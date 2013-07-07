//   [Ground] -- [10k-pad-resistor] -- | -- [thermistor] --[Vcc (5 or 3.3v)]
//                                     |
//                                 Analog Pin 0
 
//on my setup for this:
// Red wire : 10k - then thermister (should be ground)
// Black : goes to thermister (should be vcc)
// green goes to thermister  (should be A0)


float Thermistor(int RawADC) {
  long Resistance;  
  float Temp;  // Dual-Purpose variable to save space.

  Resistance=((1024 * pad / RawADC) - pad); 
  Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius                      

  // BEGIN- Remove these lines for the function not to display anything
//  Serial.print("ADC: "); 
//  Serial.print(RawADC); 
//  Serial.print("/1024");                           // Print out RAW ADC Number
//  Serial.print(", vcc: ");
//  Serial.print(vcc,2);
//  Serial.print(", pad: ");
//  Serial.print(pad/1000,3);
//  Serial.print(" Kohms, Volts: "); 
//  Serial.print(((RawADC*vcc)/1024.0),3);   
//  Serial.print(", Resistance: "); 
//  Serial.print(Resistance);
//  Serial.print(" ohms, ");
  // END- Remove these lines for the function not to display anything

  // Uncomment this line for the function to return Fahrenheit instead.
  //temp = (Temp * 9.0)/ 5.0 + 32.0;                  // Convert to Fahrenheit
  return Temp;                                      // Return the Temperature
}


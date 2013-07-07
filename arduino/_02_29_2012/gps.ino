//////////////////////////////
/////     GPS             ////
//////////////////////////////
//
//void getgps(TinyGPS &gps)
//{
//
//  Serial.print(",");
//
//  // To get all of the data into varialbes that you can use in your code, 
//  // all you need to do is define variables and query the object for the 
//  // data. To see the complete list of functions see keywords.txt file in 
//  // the TinyGPS and NewSoftSerial libs.
//
//  // Define the variables that will be used
//  float latitude, longitude;
//  // Then call this function
//  gps.f_get_position(&latitude, &longitude);
//  // You can now print variables latitude and longitude
//  //  Serial.print("Lat/Long: "); 
//  Serial.print(latitude,5); 
//  Serial.print(","); 
//  Serial.print(longitude,5);
//  Serial.print(","); 
//  // Same goes for date and time
//  int year;
//  byte month, day, hour, minute, second, hundredths;
//  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
//  // Print data and time
//  Serial.print(month, DEC); 
//  Serial.print(","); 
//  Serial.print(day, DEC); 
//  Serial.print(","); 
//  Serial.print(year);
//  Serial.print(","); 
//  Serial.print(hour, DEC); 
//  Serial.print(","); 
//  Serial.print(minute, DEC); 
//  Serial.print(","); 
//  Serial.print(second, DEC); 
//  Serial.print(","); 
//  Serial.print(hundredths, DEC);
//  //Since month, day, hour, minute, second, and hundr
//
//  // Here you can print the altitude and course values directly since 
//  // there is only one value for the function
//  //  Serial.print("Altitude (meters): "); 
//  //  Serial.println(gps.f_altitude());  
//  // Same goes for course
//  //  Serial.print("Course (degrees): "); 
//  //  Serial.println(gps.f_course()); 
//  // And same goes for speed
//  //  Serial.print("Speed(kmph): "); 
//  //  Serial.println(gps.f_speed_kmph());
//  //  Serial.println();
//
//  // Here you can print statistics on the sentences.
//  unsigned long chars;
//  unsigned short sentences, failed_checksum;
//  gps.stats(&chars, &sentences, &failed_checksum);
//  //Serial.print("Failed Checksums: ");Serial.print(failed_checksum);
//  //Serial.println(); Serial.println();
//
//}


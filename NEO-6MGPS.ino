/*********************
 *D2 to GPS Module TX*
 *D1 to GPS Module RX*
 *********************/
 /*
  SD card read/write
 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin D7
 ** MISO - pin D6
 ** CLK  - pin D5
 ** CS   - pin D8
 This example code is in the public domain.
 */
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
 #define CS_PIN  D8   
#include <SPI.h>
#include <SD.h>

SoftwareSerial mySerial(4,5);
TinyGPS gps;
File dataFile;
unsigned int move_index = 1;  
WidgetMap myMap(V0);  // V0 for virtual pin of Map Widget
  
BlynkTimer timer;

void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);

char auth[] = "dd22a1f2e2b94d98ac723b383b62c9c6";                   //Your Project authentication key
char ssid[] = "HoangTrang";                                       // Name of your network (HotSpot or Router name)
char pass[] = "n14dcdt112";   


void setup()  
{
  
  // Oploen serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.print("Inicializando o cartão SD...");
  Serial.println();
  pinMode (D0,OUTPUT);
  pinMode (D3,OUTPUT);
  pinMode(D4,INPUT_PULLUP);
   //ss.begin(GPSBaud);
  // set the data rate for the SoftwareSerial port
 
  mySerial.begin(9600);
  
  Blynk.begin(auth, ssid, pass);
  delay(100);
  //notify to Email.
  Blynk.email("huynhduclinh10c4@gmail.com", " >> My systems is beginning Work <<", "My Blynk is wifi.  ");
  Blynk.notify("My systems is beginning Work ");
  attachInterrupt(digitalPinToInterrupt(D4), emailOnButtonPress, CHANGE);
  Serial.println("uBlox Neo 6M");
  Serial.print("Testing TinyGPS library v. "); Serial.println(TinyGPS::library_version());
  Serial.println("by Mikal Hart");
  Serial.println();
  Serial.print("Sizeof(gpsobject) = "); 
  Serial.println(sizeof(TinyGPS));
  Serial.println(); 
 if (!SD.begin(CS_PIN)) {
    Serial.println("Falha, verifique se o cartão está presente.");
    //programa encerrrado
    return;
   
  }
}

  
void loop() // run over and over
{
 dataFile = SD.open("LOG.txt", FILE_WRITE);
  // se o arquivo foi aberto corretamente, escreve os dados nele
  bool newdata = false;
  unsigned long start = millis();
  // Every 5 seconds we print an update
  while (millis() - start < 5000) 
  {
    if (mySerial.available()) 
    
    {
      char c = mySerial.read();
      //Serial.print(c);  // uncomment to see raw GPS data
      if (gps.encode(c)) 
      {
        newdata = true;
        break;  // uncomment to print new data immediately!
      }
    }
     Blynk.run();
     
  }
 
  if (newdata) 
  {
    Serial.println("Acquired Data");
    Serial.println("-------------");
    gpsdump(gps);
    Serial.println("-------------");
    Serial.println();
  }
 //Serial.println("initialization done.");
  
}


void gpsdump(TinyGPS &gps)
{float flat, flon;
 
long lat, lon;

unsigned long age, date, time, chars;
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned short sentences, failed;

  gps.get_position(&lat, &lon, &age);

  Serial.print("Lat/Long(10^-5 deg): "); Serial.print(lat); Serial.print(", "); Serial.print(lon); 
  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");

  // On Arduino, GPS characters may be lost during lengthy Serial.print()
  // On Teensy, Serial prints to USB, which has large output buffering and
  //   runs very fast, so it's not necessary to worry about missing 4800
  //   baud GPS characters.
  //Blynk.virtualWrite(V4,gps.satellites());
  gps.f_get_position(&flat, &flon, &age);
   Blynk.virtualWrite(V1, String(flat, 6),String(flon, 6),age); 
    myMap.location(move_index, flat, flon, "Your_Location");
  Serial.print("Lat/Long(float): "); printFloat(flat, 5); Serial.print(", "); printFloat(flon, 5);
    Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");
  //Blynk.virtualWrite(V3, age);
  gps.get_datetime(&date, &time, &age);
  Serial.print("Date(ddmmyy): "); Serial.print(date); Serial.print(" Time(hhmmsscc): ");
    Serial.print(time);
  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");

  // se o arquivo foi aberto corretamente, escreve os dados nele
 
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  Serial.print("Date: "); Serial.print(static_cast<int>(month)); Serial.print("/"); 
    Serial.print(static_cast<int>(day)); Serial.print("/"); Serial.print(year);
  Serial.print("  Time: "); Serial.print(static_cast<int>(hour+8));  Serial.print(":"); //Serial.print("UTC +08:00 Malaysia");
    Serial.print(static_cast<int>(minute)); Serial.print(":"); Serial.print(static_cast<int>(second));
    Serial.print("."); Serial.print(static_cast<int>(hundredths)); Serial.print(" UTC +08:00 Malaysia");
  Serial.print("  Fix age: ");  Serial.print(age); Serial.println("ms.");

  Serial.print("Alt(cm): "); Serial.print(gps.altitude()); Serial.print(" Course(10^-2 deg): ");
    Serial.print(gps.course()); Serial.print(" Speed(10^-2 knots): "); Serial.println(gps.speed());
  Serial.print("Alt(float): "); printFloat(gps.f_altitude()); Serial.print(" Course(float): ");
    printFloat(gps.f_course()); Serial.println();
  Serial.print("Speed(knots): "); printFloat(gps.f_speed_knots()); Serial.print(" (mph): ");
    printFloat(gps.f_speed_mph());
  Serial.print(" (mps): "); printFloat(gps.f_speed_mps()); Serial.print(" (kmph): ");
    printFloat(gps.f_speed_kmph()); Serial.println();

  gps.stats(&chars, &sentences, &failed);
  Serial.print("Stats: characters: "); Serial.print(chars); Serial.print(" sentences: ");
    Serial.print(sentences); Serial.print(" failed checksum: "); Serial.println(failed);
 if (dataFile) {
 
    Serial.println("O arquivo foi aberto com sucesso.");
      //Data has been successfully written to file.txt
      dataFile.println(" Toa do : ");
      dataFile.print(String(flat, 6));
      dataFile.print(" , ");
      dataFile.print(String(flon, 6));
      dataFile.print(" Ngay/thang/nam :  ");
      dataFile.print(day);
       dataFile.print("/ ");
        dataFile.print(month);
        dataFile.print("/ ");
        dataFile.print(year);
      //close SD card
      dataFile.close();
  }
  // The data was not successful
  else {
    Serial.println("The data was not successful.txt");
  }
 
  delay(200); 
}

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0) 
  {
     Serial.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0) 
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint;
  }
}
//Power
BLYNK_WRITE(V2)
{
  Serial.println(D0,!param.asInt());
}

//whistle signaling

BLYNK_WRITE(V3)
{
  Serial.println(D3,!param.asInt());
}

void emailOnButtonPress()
{
  // *** WARNING: You are limited to send ONLY ONE E-MAIL PER 15 SECONDS! ***

  // Let's send an e-mail when you press the button
  // connected to digital pin 2 on your Arduino

  int isButtonPressed = !digitalRead(D4); // Invert state, since button is "Active LOW"

  if (isButtonPressed) // You can write any condition to trigger e-mail sending
  {
    Serial.println("Button is pressed."); // This can be seen in the Serial Monitor
   // Blynk.email("huynhduclinh10c4@mail.com", " In special cases >> EMERGENCY HAZARD << ", "YOU SHOULD POSITION YOUR CHILD IMMEDIATELY ...");
  Blynk.email("huynhduclinh10c4@gmail.com", " >>  In special cases >> EMERGENCY HAZARD <<", "YOU SHOULD POSITION YOUR CHILD IMMEDIATELY ...  ");
  Blynk.notify("In special cases >> EMERGENCY HAZARD << ");
    // Or, if you want to use the email specified in the App (like for App Export):
    //Blynk.email("Subject: Button Logger", "You just pushed the button...");
  }
}




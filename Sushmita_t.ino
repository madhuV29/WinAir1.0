#include <dht.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 10, 7);
SoftwareSerial pmsSerial1(2, 3);
dht DHT;

#define DHT11_PIN 13

void setup()
{
  Serial.begin(9600);
  // our debugging output
  Serial.begin(115200);
  lcd.begin(16, 2);
  // sensor baud rate is 9600
  pmsSerial1.begin(9600);
}

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
 
struct pms5003data data;


void loop()
{
  int chk;
  float temp,hum;
  float pm1o,pm2o,pm10o;

  //displaying initial text
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("****WELCOME****");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    WinAir   ");
  lcd.setCursor(0, 1);
  lcd.print(" (Version 1.0) ");
  delay(10000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setting up the");
  lcd.setCursor(0, 1);
  lcd.print("sensors...");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reading data");
  lcd.setCursor(0, 1);
  lcd.print("from sensors...");
  delay(3000);
  lcd.clear();

  //reading the data
  chk = DHT.read11(DHT11_PIN);
  temp = DHT.temperature;
  hum = DHT.humidity;
  
  if (readPMSdata(&pmsSerial1)) 
  {
    pm1o = data.pm10_standard;
    pm2o = data.pm25_standard;
    pm10o = data.pm100_standard; 
  }

  //Displaying the results
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: " + String(temp) + "C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: " + String(hum) + "%");
  delay(15000);


  //SPMO
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SPM[O
  ] (ug/m3):");
  lcd.setCursor(0, 1);
  lcd.print("PM1.0: "+String(pm1o));
  delay(15000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PM2.5: "+String(pm2o));
  lcd.setCursor(0, 1);
  lcd.print("PM10: "+String(pm10o));
  delay(15000);

  
}

boolean readPMSdata(Stream *s) 
{
  if (! s->available())
    return false;

  if (s->peek() != 0x42) 
  {
    s->read();
    return false;
  }
 
  if (s->available() < 32) 
      return false;
      
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
 
  for (uint8_t i=0; i<30; i++) 
      sum += buffer[i];
 
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) 
  {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
 
  memcpy((void *)&data, (void *)buffer_u16, 30);
 
  if (sum != data.checksum) 
  {
    Serial.println("Checksum failure");
    return false;
  }
  return true;
}






    

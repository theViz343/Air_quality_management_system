#include "DHT.h"
#include <MQ2.h>
#include<WiFi.h>
#include<FirebaseESP32.h>

const char WiFi_name="name";
const char pass="password";

int gasin = A5;
int tempin = A4;

int temp;
int gasconc; 
int notify=7;
float smoke_arr[10]={0,0,0,0,0,0,0,0,0,0};
float temp_arr[10]={0,0,0,0,0,0,0,0,0,0};
float humidity_arr[10]={0,0,0,0,0,0,0,0,0,0};
float prev_smoke=0;
float prev_temp=0;
float prev_humidity=0;

//thresholds
int threshT1=100;
int threshT2=200;
int threshT3=300;
int threshG1=100;
int threshG2=200;
int threshG3=300;

#define DHTPIN 2    
#define DHTTYPE DHT11   // DHT 11
MQ2 mq2(pin);

int lpg, co, smoke;
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  // put your setup code here, to run once:
  pinMode(gasin,INPUT);
  pinMode(tempin,INPUT);
  
  dht.begin();
  mq2.begin();
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
   //Serial.print(temp);
   //Serial.print(gasin);
    float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temp = dht.readTemperature();
  float* values= mq2.read(false);
  
  //lpg = values[0];
  lpg = mq2.readLPG();
  //co = values[1];
  //smoke = values[2];
  smoke = (mq2.readSmoke()+mq2.readCO())/2; 
  if (smoke<0)
  {
    smoke=10000;  
  }
  if (isnan(humidity) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  for(int i=9;i>0;i--)
  {
    smoke_arr[i]=smoke_arr[i-1];
    temp_arr[i]=temp_arr[i-1];
    humidity_arr[i]=humidity_arr[i-1];
  }
  smoke_arr[0]=smoke;
  temp_arr[0]=temp;
  humidity_arr[0]=humidity;
  float norm_smoke=0;
  float mult=1;
  float sum=0;
  for(int i=0;i<10;i++)
  {
    norm_smoke+=smoke_arr[i]*mult;
    norm_temp+=temp_arr[i]*mult;
    norm_humidity+=humidity_arr[i]*mult;
    sum=sum+mult;
    mult*=0.8;
  }
  norm_smoke/=sum;
  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temp);
  Serial.print(F("°C Norm Smoke: "));
  Serial.print(norm_smoke);
  Serial.print(" ppm Smoke: ");
  Serial.print(smoke);
  Serial.print(" ppm");
  Serial.println("");
  if (norm_humidity>60)
  {
   // humidity dangerous
   // fan on
  } 
  else if (norm_humidity>50)
  {
   // humidity high
   if (prev_humidity<=50)
   {
    // send notification
   }
  } 
  else
  {
    // humidity safe
  }
  if(norm_temp>60)
  {
    // temperature dangerous
    // fan on
  }
  else if(norm_temp>50)
  {
    // temperature high
    if (prev_temp<=50)
    {
      // send notification
    }
  }
  else
  {
    // temperature normal
  }
  
  if(norm_smoke>100)
  {
    // smoke dangerous
    // fan on
  }
  else if(norm_smoke>80)
  {
    // smoke high
    if (prev_smoke<=80)
    {
      // send notification
    }
  }
  else
  {
    // smoke normal
  }
  prev_smoke=norm_smoke;
  prev_temp=norm_temp;
  prev_humidity=norm_humidity;
   delay(500);
   
}

int gasin = A5;
int tempin = A4;
int pin = A0;
int buzzer = 4;
int led1 = 7;
int led2 = 6;
int led3 = 5;
int led4 = 8;
int temp;
int gasconc; 
int notify=7;
int smoke_arr[10]={0,0,0,0,0,0,0,0,0,0};
//thresholds
int threshT1=100;
int threshT2=200;
int threshT3=300;
int threshG1=100;
int threshG2=200;
int threshG3=300;
#include "DHT.h"
#include <MQ2.h>
#define DHTPIN 2    
#define DHTTYPE DHT11   // DHT 11
MQ2 mq2(pin);

int lpg, co, smoke;
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  // put your setup code here, to run once:
  pinMode(gasin,INPUT);
  pinMode(tempin,INPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
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
  
  if (isnan(humidity) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  for(int i=9;i>0;i--)
  {
    smoke_arr[i]=smoke_arr[i-1];
  }
  smoke_arr[0]=smoke;
  int norm_smoke=0;
  int mult=1;
  int sum=0;
  for(int i=0;i<10;i++)
  {
    norm_smoke+=smoke_arr[i]*mult;
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
  if (humidity>60)
  {
   digitalWrite(buzzer,HIGH);
  } 
  if(temp<=35)
  {
    digitalWrite(led1,HIGH);
    digitalWrite(led2,LOW);
    digitalWrite(led3,LOW);
  }
  if(temp>35&&temp<=40)
  {
    digitalWrite(led1,LOW);
    digitalWrite(led2,HIGH);
    digitalWrite(led3,LOW);
  }
  if(temp>40)
  {
    digitalWrite(led1,LOW);
    digitalWrite(led2,LOW);
    digitalWrite(led3,HIGH);
  }
  if(smoke>100)
  {
    digitalWrite(led4,HIGH);
  }
  else
  {
    digitalWrite(led4,LOW);
  }
   delay(500);
   digitalWrite(buzzer,LOW);
}

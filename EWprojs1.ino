int gasin = A5;
int tempin = A4;
int mq2 = A0;
int buzzer = 4;
int led1 = 7;
int led2 = 6;
int led3 = 5;

int temp;
int gasconc; 
int notify=7;
int smoke_arr[10]={0,0,0,0,0,0,0,0,0,0};
#include "MQ2.h"
#include "DHT.h"

#define DHTPIN 2    
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  pinMode(gasin,INPUT);
  pinMode(tempin,INPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
  dht.begin();
  Serial.begin(9600);
}

void loop() {
  delay(500);
    float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temp = dht.readTemperature();
  float smoke=analogRead(mq2); 
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
  Serial.print(F("°C Smoke: "));
  Serial.print(norm_smoke);
  Serial.println("");
  if (humidity>60)
  {
   digitalWrite(buzzer,HIGH);
   delay(500);
  }
  digitalWrite(buzzer,LOW); 
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
   
}

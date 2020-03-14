#include <FirebaseESP32.h>

#include "DHT.h"
#include <MQ2.h>
#include <WiFi.h>

#define FIREBASE_HOST "https://air-quality-a7167.firebaseio.com/"   
#define FIREBASE_AUTH "DPikF7rxmpNCVJqfUzbDNuQHbyqIlJBooZjUDRAY"   
#define WIFI_SSID "Gangs of WiFipur"               
#define WIFI_PASSWORD "12345678"
FirebaseData firebaseData;
int gasin = A5;
int tempin = A4;
int pinmq2 = 35;
int temp;
int gasconc; 
float smoke_arr[10]={0,0,0,0,0,0,0,0,0,0};
float temp_arr[10]={0,0,0,0,0,0,0,0,0,0};
float humidity_arr[10]={0,0,0,0,0,0,0,0,0,0};
float prev_smoke=0;
float prev_temp=0;
float prev_humidity=0;

//thresholds
int threshT1=30;
int threshT2=32;
int threshT3=35;
int threshT4=40;

int threshG1=60;
int threshG2=100;
int threshG3=120;
int threshG4=150;

int threshH1=40;
int threshH2=45;
int threshH3=50;
int threshH4=60;

#define DHTPIN 34   
#define DHTTYPE DHT11   // DHT 11
MQ2 mq2(pinmq2);

int lpg, co, smoke;
DHT dht(DHTPIN, DHTTYPE);

int isFanRequired(float val, float prev_val, float T1, float T2, float T3, float T4)
{
  if(val>T4)
  {
    //Turn fan on  
    return 0;
  }  
  else if(val>T3 && prev_val<=val)
  {
    //notify on
    return 1;  
  }
  else if(val<T2 && prev_val>=val)
  {
    //notify off
    return 2;  
  }
  else if(val<T1)
  {
    //Turn fan off
    return 3;  
  }
  else
  {
    //Do nothing
    return -1;  
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(gasin,INPUT);
  pinMode(tempin,INPUT);
  dht.begin();
  mq2.begin();
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}

void loop() {
  // put your main code here, to run repeatedly:
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    
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

FirebaseJson updateData;
updateData.set("data1","value1");
if (Firebase.updateNode(firebaseData, "/sensor-values", updateData)) {

  Serial.println(firebaseData.dataPath());

  Serial.println(firebaseData.dataType());

  Serial.println(firebaseData.jsonString()); 

} else {
  Serial.println(firebaseData.errorReason());
}
  if (isnan(smoke)){
    Serial.println(F("Failed to read from mq2 sensor!"));
  }
  if (isnan(humidity) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
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
  float norm_temp=0;
  float norm_humidity=0;
  float mult=1;
  float sum=0;
  int flag=0;
  for(int i=0;i<10;i++)
  {
    norm_smoke+=smoke_arr[i]*mult;
    norm_temp+=temp_arr[i]*mult;
    norm_humidity+=humidity_arr[i]*mult;
    sum=sum+mult;
    mult*=0.8;
  }
  norm_smoke/=sum;
  norm_temp/=sum;
  norm_humidity/=sum;


  //Serial.print(F("Humidity: "));
  //Serial.print(humidity);
  //Serial.print(F("%  Temperature: "));
  //Serial.print(temp);
  Serial.print(F("°C Norm Smoke: "));
  Serial.print(norm_smoke);
  Serial.print(" ppm Smoke: ");
  Serial.print(smoke);
  Serial.print(" ppm"); 
  Serial.println("");

  int smoke_status= isFanRequired(norm_smoke,prev_smoke,threshG1,threshG2,threshG3,threshG4);
  int temp_status= isFanRequired(norm_temp,prev_temp,threshT1,threshT2,threshT3,threshT4);
  int humidity_status= isFanRequired(norm_humidity,prev_humidity,threshH1,threshH2,threshH3,threshH4);

  boolean notifyON= (smoke_status==1 || temp_status==1 || humidity_status==1);
  boolean notifyOFF= (smoke_status==2 || temp_status==2 || humidity_status==2);

  boolean fanON = (smoke_status==0 || temp_status==0 || humidity_status==0);
  
  prev_smoke=norm_smoke;
  prev_temp=norm_temp;
  prev_humidity=norm_humidity;
  delay(500);
   
}

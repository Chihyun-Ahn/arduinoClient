#include <SoftwareSerial.h>
#include <avr/sleep.h> 
#include <SimpleTimer.h>

SimpleTimer timer;
const int wakeUpPin = 2; // pin of interrupt 0
const int LED = 8;
const int numMsg = 1; // For the safety, send multiple messages, everytime.  
boolean armed = false;
SoftwareSerial mySerial(4,5);
String ID = "1001";
String code = "01";
String msg = "xxxx";
String rcvMsg = "xxxx";
int timerID = 0;
boolean isWaked = false;
int pulseWidth = 0;
int pinVal=3;
int softwareFilter[3] = {-1,-1,-1};

/*
  Communication protocol
  01: powered on
  02: Sensor touched (Feed is full)
  03: alive
 */

void setup()
{
  String msg = String(ID + code);
  pinMode(wakeUpPin, INPUT);
  pinMode(LED, OUTPUT);
  timerID = timer.setInterval(1000, sendMsg);
  
  Serial.begin(9600);  
  mySerial.begin(9600); 
  Serial.println(msg);
  delay(100);
}

void loop() // 반복
{
//  pulseWidth = pulseIn(wakeUpPin,LOW,100000);
  Serial.println("start loop.");
  armed = true;
  delay(100);
  Serial.println("Now sleeping.."); 
  delay(200);
  sleepNow(); 

// Software Filtering
  delay(200);
  softwareFilter[0] = digitalRead(wakeUpPin);
  delay(50);
  softwareFilter[1] = digitalRead(wakeUpPin);
  delay(50);
  softwareFilter[2] = digitalRead(wakeUpPin);
  delay(50);
  
//  pinVal = digitalRead(wakeUpPin);
  if(softwareFilter[0]==0 && softwareFilter[1]==0 && softwareFilter[2]==0){
    if(armed){
      Serial.println("Waked up. Send Feed full msg.");
      
      delay(200);
      code = "02";
      msg = String(ID + code);
      sendMsg();
      int count = 0;
      while(1){
        count++;
        if(count>20000){      //prevent infinite loop
          count = 0;
          break;
        }
        Serial.print('*');
        if(mySerial.available()>0){
          Serial.println("RECEIVED!!!!");
          rcvMsg = mySerial.readStringUntil('\n');
          if(rcvMsg.substring(0,4).equals(ID)){
  //          timer.deleteTimer(timerID);
            Serial.println("My msg is received successfully. Now sleep again..\n");
            break;
          }
        }
        delay(50);  
      }
      armed = false;
    }
    
  }else if(pinVal == 1){
    Serial.println("Awake by noise!!");
  }
 
}  

void wakeUpNow(){
  isWaked = true;
  return;
}

void sleepNow(){
  Serial.println("Entered sleep now()");
  delay(100);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
  sleep_enable(); 
  attachInterrupt(digitalPinToInterrupt(2),wakeUpNow, LOW); 
  sleep_mode(); 
  sleep_disable(); 
  
  
  detachInterrupt(0);   
  return;
}

void sendMsg(){
  for (int i=0;i<numMsg;i++){
    mySerial.println(msg);
  }
  return;
}

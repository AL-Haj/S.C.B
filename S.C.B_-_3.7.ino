#define BLYNK_PRINT Serial                          
#define BLYNK_TEMPLATE_ID "XXXXXXXXXX"            
#define BLYNK_DEVICE_NAME "XXXXXXXXXX"                    
char auth[] = "XXXXXXXXXXXXXXXXXXXXXXX"; 
                                                         
#include "WiFi.h"                                   
#include "WiFiClient.h"                             
#include "BlynkSimpleEsp32.h"                       
#include "PZEM004Tv30.h"                            

BlynkTimer timer1;
BlynkTimer timer2;

#define RELAY_PIN      2    
#define PUSH_BUTTON    13   
#define V_BUTTON       V18

#define  RX  19
#define  TX  21

#define  RX1 16
#define  TX1 17

#define  VOLT    V0
#define  CT      V1
#define  POW     V2
#define  ENY     V3
#define  FRQ     V4
#define  PF      V5

#define BAUD 9600
#define SE   SERIAL_8N1

#define  r    12
#define  g    14
#define  b    27
#define  HI   26
#define  LO   25

WidgetLCD lcd(V25);
PZEM004Tv30 pzem (&Serial2); 
PZEM004Tv30 pzem1(&Serial2);

double c1  ;
double p1  ;
double e1  ;
double f1  ;
double pf1 ;

double Vout ;
double L ;
double N ;

double SC ;
double OL ;
double CL ;
double HV ;
double LV ;

int TSC ;
int TOL ;
int TCL ;
int THV ;
int TLV ;

int toggleState = 1;
int pushButtonState = HIGH;

BLYNK_WRITE(V8) 
{
   SC  = param.asDouble();
}
BLYNK_WRITE(V9) 
{
   TSC  = param.asInt();
}

BLYNK_WRITE(V10) 
{
   OL  = param.asDouble();
}
BLYNK_WRITE(V11) 
{
   TOL  = param.asInt();
}

BLYNK_WRITE(V6) 
{
   CL  = param.asDouble();
}
BLYNK_WRITE(V7) 
{
   TCL  = param.asInt();
}

BLYNK_WRITE(V12) 
{
   HV  = param.asDouble();
}
BLYNK_WRITE(V13) 
{
   THV  = param.asInt();
}

BLYNK_WRITE(V14) 
{
   LV  = param.asDouble();
}
BLYNK_WRITE(V16) 
{
   TLV  = param.asInt();
}

void loop1(void *pvParameters)
{
  while (1)
  { 
    Blynk.run();
    timer1.run();
    timer2.run();
    pzem_reading();  
    Blynk.virtualWrite(VOLT,Vout);    
    Blynk.virtualWrite(CT,c1);    
    Blynk.virtualWrite(POW,p1);    
    Blynk.virtualWrite(ENY,e1);   
    Blynk.virtualWrite(FRQ,f1);    
    Blynk.virtualWrite(PF,pf1);

    if (digitalRead(RELAY_PIN) == LOW)
    delay(1000);
    if (digitalRead(RELAY_PIN) == LOW)
    {
      lcd.clear();     
      digitalWrite(r,  LOW);
      digitalWrite(g,  LOW);
      digitalWrite(b,  LOW); 
      digitalWrite(HI, LOW);
      digitalWrite(LO, LOW);   
    }     
  }  
} 

void loop2(void *pvParameters)
{
  while (1)
  {   
        
    if((L>SC)||(N>SC))
  {
    delay(TSC);
    if((L>SC)||(N>SC))
    {
    lcd.clear();
    lcd.print(1, 0, "Short-Circuit"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")           
    digitalWrite(r,  LOW);
    digitalWrite(g,  HIGH);
    digitalWrite(b,  LOW); 
    digitalWrite(HI, LOW);
    digitalWrite(LO, LOW); 
    Blynk.virtualWrite(V_BUTTON, HIGH);
    digitalWrite(RELAY_PIN, HIGH);
    }     
  } 
       
    if(( L > OL)||( N > OL ))
  {
    delay(TOL);
    if(( L > OL)||( N > OL ))
    {            
    lcd.clear();
    lcd.print(1, 0, "Overload"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")               
    digitalWrite(r,  LOW);
    digitalWrite(g,  LOW);
    digitalWrite(b,  HIGH); 
    digitalWrite(HI, LOW);
    digitalWrite(LO, LOW);  
    Blynk.virtualWrite(V_BUTTON, HIGH);
    digitalWrite(RELAY_PIN, HIGH); 
    }
  }
     
    if(((L/N)<CL)||((N/L)<CL))
  {
    delay(TCL); 
    if(((L/N)<CL)||((N/L)<CL))
    {
    lcd.clear();    
    lcd.print(1, 0, "Circuit-Leakage"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")       
    digitalWrite(r,  HIGH);
    digitalWrite(g,  LOW);
    digitalWrite(b,  LOW); 
    digitalWrite(HI, LOW);
    digitalWrite(LO, LOW);
    Blynk.virtualWrite(V_BUTTON, HIGH);
    digitalWrite(RELAY_PIN, HIGH);
    }   
  } 

   if(Vout>HV)
  {
    delay(THV);
    if(Vout>HV)
    {
    lcd.clear();
    lcd.print(1, 0, "High-Voltage"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")    
    digitalWrite(r,  LOW);
    digitalWrite(g,  LOW);
    digitalWrite(b,  LOW); 
    digitalWrite(HI, HIGH);
    digitalWrite(LO, LOW);
    Blynk.virtualWrite(V_BUTTON, HIGH);
    digitalWrite(RELAY_PIN, HIGH);
    }    
  }
  
    if(Vout<LV)
  {  
    delay(TLV);
    if(Vout<LV)
    {
    lcd.clear();
    lcd.print(1, 0, "Low-Voltage"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")          
    digitalWrite(r,  LOW);
    digitalWrite(g,  LOW);
    digitalWrite(b,  LOW); 
    digitalWrite(HI, LOW);
    digitalWrite(LO, HIGH); 
    Blynk.virtualWrite(V_BUTTON, HIGH);
    digitalWrite(RELAY_PIN, HIGH);
    }    
  }
  
    if(Vout>250)
  {
    delay(100);
    if(Vout>250)
    {
    lcd.clear();
    lcd.print(1, 0, "High-Voltage"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")       
    digitalWrite(r,  LOW);
    digitalWrite(g,  LOW);
    digitalWrite(b,  LOW); 
    digitalWrite(HI, HIGH);
    digitalWrite(LO, LOW);
    Blynk.virtualWrite(V_BUTTON, HIGH);
    digitalWrite(RELAY_PIN, HIGH);
    }    
  }
  
    if(Vout<180)
  {  
    delay(100);
    if(Vout<180)
    {
    lcd.clear();
    lcd.print(1, 0, "Low-Voltage"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")    
    digitalWrite(r,  LOW);
    digitalWrite(g,  LOW);
    digitalWrite(b,  LOW); 
    digitalWrite(HI, LOW);
    digitalWrite(LO, HIGH); 
    Blynk.virtualWrite(V_BUTTON, HIGH);
    digitalWrite(RELAY_PIN, HIGH);        
    }    
  }    
  }  
}              
                
void setup()
{     
  Serial.begin(BAUD);
  Blynk.begin(auth, "XXX", "XXXXXXXXXX");
   
  xTaskCreatePinnedToCore(loop1, "loop1", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 1, NULL, 1);
  
  pinMode(r,  OUTPUT);
  pinMode(g,  OUTPUT);
  pinMode(b,  OUTPUT);    
  pinMode(HI, OUTPUT);
  pinMode(LO, OUTPUT);  
  pinMode(RELAY_PIN, OUTPUT); 
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  
  digitalWrite(RELAY_PIN, toggleState); 
  timer1.setInterval(500L, check_PUSH_BUTTON );  
  timer2.setInterval(1000L, check_LED );
  
  Blynk.syncVirtual(V6);
  Blynk.syncVirtual(V7);  
  Blynk.syncVirtual(V8);
  Blynk.syncVirtual(V9);
  Blynk.syncVirtual(V10);
  Blynk.syncVirtual(V11);   
  Blynk.syncVirtual(V12);
  Blynk.syncVirtual(V13);
  Blynk.syncVirtual(V14);
  Blynk.syncVirtual(V16);    
}

void loop()
{             
}

void check_LED()
{
    if (digitalRead(g) == HIGH)
    {    
    lcd.print(1, 0, "Short-Circuit");    
    }  
    if (digitalRead(r) == HIGH)
    {  
    lcd.print(1, 0, "Circuit-Leakage");
    }       
    if (digitalRead(b) == HIGH)
    {            
    lcd.print(1, 0, "Overload");  
    }        
    if (digitalRead(HI) == HIGH)
    {       
    lcd.print(1, 0, "High-Voltage"); 
    }   
    if (digitalRead(LO) == HIGH)
    {
    lcd.print(1, 0, "Low-Voltage");   
    }   
}

void pzem_reading()
{
    Serial2.begin(BAUD, SE, TX, RX); 
    c1=pzem.current();
    N=pzem.current();        
    p1=pzem.power();  
    e1=pzem.energy();    
    f1=pzem.frequency();   
    pf1=pzem.pf();
    delay(25);
    
    Serial2.begin(BAUD, SE, TX1, RX1);
    Vout=pzem1.voltage();    
    L=pzem1.current();                     
    delay(25);
}

void check_PUSH_BUTTON()
{
      if (digitalRead(PUSH_BUTTON) == LOW) 
    {
      relayOnOff(1);
      Blynk.virtualWrite(V_BUTTON, toggleState);
    }        
}


void relayOnOff(int relay)
{
  switch(relay){
      case 1: 
             if(toggleState == 0)
             {
              digitalWrite(RELAY_PIN, HIGH); // turn on relay 
              toggleState = 1;
              }
             else
             {
              digitalWrite(RELAY_PIN, LOW); // turn off relay 
              toggleState = 0;
              }
      break;
      } 
}
BLYNK_CONNECTED() // Request the latest state from the server
{ 
  if (digitalRead(RELAY_PIN) == LOW)
  {
  Blynk.virtualWrite(V_BUTTON,LOW );
  Blynk.syncVirtual(V_BUTTON);
  }  
  
  else 
  {
  Blynk.virtualWrite(V_BUTTON,HIGH );
  Blynk.syncVirtual(V_BUTTON);
  }  
}
BLYNK_WRITE(V_BUTTON) // When App button is pushed - switch the state
{
  toggleState = param.asInt();
  digitalWrite(RELAY_PIN, toggleState);
}

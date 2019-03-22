/*This Program controls one output, in this case a heater, based on two inputs, in this case temperature sensors.
Inside its main loop, there are two sections, a Control section and a Screen section.
The computor will continually run the screen section until it is notified by means of a bool flag that a desired amount of time has passed.
It will then enter the control section, and decide if the heater should be on or off.
then it will reset the flag and return to looping through the screen section until the flag is tripped again.
*/

int heatdialval;
int offsetdialval;
long int lastcontrolTime = millis();
bool heatflag = true;
long waitperiod = 5000;
// define global variables

#include <LiquidCrystal.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

  pinMode(13, INPUT); 
  pinMode(8, INPUT);
  pinMode(6, OUTPUT); 
 //define inputs and outputs
lcd.begin(16, 2);
 // set up the LCD's number of columns and rows:
}//end of setup




void loop() {
unsigned long elapsedtime;
int modeswitchValue = digitalRead(13); 
int statusswitch = digitalRead(8);
// define variables


Serial.println("At Beginning of Loop");

if (heatflag == true){
// Enter Control Section
Serial.print("In Control Section");
      if (modeswitchValue == HIGH){
      //Enter Heat Mode
      Serial.println("    Heat");
      heatdialval  = (((analogRead(A3))/(40.92))+50);
             if (intempread() <= (heatdialval)){
               digitalWrite(6, HIGH);
             }
             else{
             digitalWrite(6, LOW);
             }
      }
      else {
      // enter Offset Mode
      Serial.println("    Offset");
      offsetdialval = ((analogRead(A5))/(102.3));
            if (intempread() <= (outtempread()+offsetdialval)){
            digitalWrite(6, HIGH);
            }
            else{
            digitalWrite(6, LOW);
            }
      }
      
//Change heatflag to false and set lastcontrolTime to equal count
lastcontrolTime = millis();
heatflag = false;    
}
  
else{
//enter Screen Section
Serial.print("In Screen Section");
      if (statusswitch == LOW){
      //status screen
      Serial.println("    Status");
      statusscreen();
      }
      else if (modeswitchValue == HIGH){
      // heat screen
      Serial.println("    Heat");
      heatmodescreen();
      }
      else{
      // offset screen
      Serial.println("    Offset");
      offsetmodescreen();
      }  
}


//Change heatflag if needed based on time since last control.
elapsedtime = (millis() - (lastcontrolTime));
if (elapsedtime > waitperiod){
heatflag = true;}
Serial.print("Time until next control - ");
Serial.println((waitperiod/1000)-((elapsedtime)/1000));
//Info prints for diagnostics



}//end of loop








// Temp and Screen Functions

//------------------------------------------------------------------------
//Inside temp function

double intempread(){
  double tempF;
 // which analog pin to connect
 pinMode(A0, INPUT);        
// resistance at 25 degrees C
const int THERMISTORNOMINAL = 8430;      
// temp. for nominal resistance (almost always 25 C)
const int TEMPERATURENOMINAL = 25;   
// how many samples to take and average, more takes longer
// but is more 'smooth'
const int NUMSAMPLES = 20;
// The beta coefficient of the thermistor (usually 3000-4000)
const int BCOEFFICIENT = 3435;
// the value of the 'other' resistor
const int SERIESRESISTOR = 8800;   

 uint16_t samples[NUMSAMPLES];
analogReference(EXTERNAL);

 uint8_t i;
  float average;
 
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(A0);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
 // Serial.print("Thermistor resistance "); 
 // Serial.println(average);
 
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
 
  tempF  = ((steinhart * 1.8) + 32.0);
 
//Serial.print("Temperature ");
//Serial.println(tempF);
return tempF;
}
//------------------------------------------------------------------------
//Outside temp function

double outtempread(){
  double tempF;
 // which analog pin to connect
 pinMode(A1, INPUT);        
// resistance at 25 degrees C
const int THERMISTORNOMINAL = 8430;      

// temp. for nominal resistance (almost always 25 C)
const int TEMPERATURENOMINAL = 25;   
// how many samples to take and average, more takes longer
// but is more 'smooth'
const int NUMSAMPLES = 5;
// The beta coefficient of the thermistor (usually 3000-4000)
const int BCOEFFICIENT = 3435;
// the value of the 'other' resistor
const int SERIESRESISTOR = 8800;   

 uint16_t samples[NUMSAMPLES];
analogReference(EXTERNAL);

 uint8_t i;
  float average;
 
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(A1);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  //Serial.print("Average analog reading "); 
  //Serial.println(average);
 
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;

 
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
 
  tempF  = ((steinhart * 1.8) + 32.0);

//Serial.println(tempF);
return tempF;
}  
  
//------------------------------------------------------------------------
//Status screen function

  void statusscreen (){
bool heatpinstatus = digitalRead(6);
// Check output status

lcd.setCursor(0, 0);
lcd.print("HEAT OT  IT DIFF");

lcd.setCursor(0, 1);
lcd.print("   ");

lcd.setCursor(0, 1);
if (heatpinstatus == 1){lcd.print("ON");}
else{lcd.print("OFF");}

lcd.setCursor(5, 1);
lcd.print(outtempread(),0);

lcd.setCursor(7, 1);
lcd.print("  ");


lcd.setCursor(9, 1);
lcd.print(intempread(),0);

lcd.setCursor(11, 1);
lcd.print("  ");


lcd.setCursor(13, 1);
lcd.print((intempread()-outtempread()));
//print to lcd
  }

//------------------------------------------------------------------------
//Heat screen function

  void heatmodescreen(){
heatdialval  = (((analogRead(A3))/(40.92))+50);
bool heatpinstatus = digitalRead(6);   
//Check value of dial and output status.

lcd.setCursor(0, 0);
lcd.print("HEAT Goal curTMP");

lcd.setCursor(0, 1);
lcd.print("   ");
    
lcd.setCursor(0, 1);
if (heatpinstatus == 1){lcd.print("ON");}
else{lcd.print("OFF");}
  
lcd.setCursor(5, 1);
lcd.print(heatdialval);
   
lcd.setCursor(7, 1);
lcd.print("    ");
    
lcd.setCursor(11, 1);
lcd.print(intempread(),1);

lcd.setCursor(15, 1);
lcd.print("  ");
//print to lcd
    
}

//------------------------------------------------------------------------
//Offset screen function

  void offsetmodescreen(){
offsetdialval = ((analogRead(A5))/(102.3));
//Check dial value

lcd.setCursor(0, 0);
lcd.print("DIFF ITMP  OTMP ");

lcd.setCursor(0, 1);
lcd.print("+  ");

lcd.setCursor(1, 1);
lcd.print(offsetdialval);

lcd.setCursor(5, 1);
lcd.print(intempread(), 1);

lcd.setCursor(9, 1);
lcd.print("  ");

lcd.setCursor(11, 1);
lcd.print(outtempread(),1);

lcd.setCursor(15, 1);
lcd.print("  ");
//print to lcd
}  
    
    
    
    

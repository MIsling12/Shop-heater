/*
This program is designed to run a heating system to maintain reasonable temperatures in an outdoor shop or to prevent condensation. 
It runs on an arduino nano microcontroller, which can be programmed from a laptop or pc.

This program controls one output, in this case a heater, based on two inputs, in this case temperature sensors.
Inside its main loop, there are two sections, a Control section and a Screen section.
The computor will continually run the screen section until it is notified by means of a built in timer that a desired amount of time has passed.
It will then enter the control section, and decide if the heater should be on or off.
Then it will reset the timer and return to looping through the screen section until the timer is tripped again.
 
The device has two modes, Heat, and Offset. You can change between modes with the central mode switch on the box.
In Heat mode the controller will maintain a desired temperature setting.
In Offset mode the controller will keep the temperature a set amount above the outside temperature. This prevents
the shop temp from falling below the outside temp when it warms up in the spring. If the shop is colder than the
outside air when someone enters the shop the warm air from outside will rush in and condense on the cold tools inside.
The heat and offset settings are adjusted with the knobs on either side of the mode switch.

The box has an outlet built in, allowing the use of any standard space heater as a heat source. 
The device can be turned on and off with the switch next to the outlet.
There is a replaceable 6 amp fuse underneath the outlet to protect the circuit.
There are two temperature sensors attached to the box. The inside senser should be placed
in a central location in whatever area you are trying to heat. The outside sensor should be put
outside of the area you are trying to heat. 

The switch on the left of the box toggles a status screen on and off. The status screen shows a countdown
with the time until the next control, whether the heater is on or off, both temp readings, and the current difference between them.
*/



// define global variables for dials and the timer.
int heatdialval;
int offsetdialval;
long int lastcontrolTime = millis();
unsigned long elapsedtime;
bool controlflag = true;// A "Flag" that is turned on if the timer has run down. 
long waitperiod = 300000;// The amount of time between controls in milliseconds.


#include <LiquidCrystal.h>
// initialize the 1602 LCD library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// put your setup code here, to run once:
void setup() {
Serial.begin(9600);

  //define inputs and outputs
  pinMode(13, INPUT); 
  pinMode(8, INPUT);
  pinMode(6, OUTPUT); 
 
// set up the LCD's number of columns and rows:
lcd.begin(16, 2);
 
}//end of setup




void loop() {
  
// define variables for both switches.
int modeswitchValue = digitalRead(13); 
int statusswitchValue = digitalRead(8);



Serial.println("At Beginning of Loop");

if (controlflag == true){
// Enter Control Section
Serial.print("In Control Section");
      if (modeswitchValue == HIGH){
      //Enter Heat Mode if the switch is on Heat.
      Serial.println("    Heat");
      heatdialval  = (((analogRead(A3))/(40.92))+50);
             if (intempread() <= (heatdialval)){              //compare the temp to the heat dial setting.
               digitalWrite(6, HIGH);                    //Turn the heater on.
             }
             else{
             digitalWrite(6, LOW);            //Turn the heater off.
             }
      }
      else {
      // Enter Offset Mode if the switch is on Offset.
      Serial.println("    Offset");
      offsetdialval = ((analogRead(A5))/(102.3));
            //compare the temp to the offset dial setting.
            if (intempread() <= (outtempread()+offsetdialval)){
            digitalWrite(6, HIGH);          //Turn the heater on.
            }
            else{
            digitalWrite(6, LOW);         //Turn the heater off.
            }
      }
      
//Change controlflag to false and set lastcontrolTime to equal count. (Turn the flag off and reset the timer.)
lastcontrolTime = millis();
controlflag = false;    
}
  
else{
//enter Screen Section
Serial.print("In Screen Section");
      if (statusswitchValue == LOW){
      Serial.println("    Status");
      statusscreen();         //Run the status screen function if the status switch is on.
      
      }
      else if (modeswitchValue == HIGH){
      Serial.println("    Heat");
      heatmodescreen();       //Run the heat screen function if the mode switch is on heat.
      
      }
      else{
      Serial.println("    Offset");
      offsetmodescreen();       //Run the offset screen function if the mode switch is on offset.
      
      }  
}


//Change controlflag if needed based on time since last control.(turn the flag on if the timer has run down.)
elapsedtime = (millis() - (lastcontrolTime));
if (elapsedtime > waitperiod){
controlflag = true;}


//Info prints for diagnostics
Serial.print("Time until next control - ");
Serial.println((waitperiod/1000)-((elapsedtime)/1000));


}//end of loop








// Temp and Screen Functions

//------------------------------------------------------------------------
//Inside temp function to convert information from the sensor.

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
const int NUMSAMPLES = 25;
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
 

return tempF;
}






//------------------------------------------------------------------------
//Outside temp function to convert information from the sensor

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
const int NUMSAMPLES = 25;
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
//Status screen function.

  void statusscreen (){
bool heatpinstatus = digitalRead(6);
// Check output status

lcd.setCursor(0, 0);
lcd.print("     OT  IT DIFF");

lcd.setCursor(0, 0);
lcd.print((waitperiod/1000)-((elapsedtime)/1000));

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
  

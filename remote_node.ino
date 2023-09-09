/*
 *  © 2023, Harold Downe <hdowne@yahoo.ca>. All rights reserved.
 *  
 *  This file is part of DCC++EX API
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 

/*
*

 *  
 *  require RF24Network and RF24 library from 
   https://github.com/nRF24/RF24
   https://github.com/nRF24/RF24Network            

see https://nrf24.github.io/RF24Network/ for how to configure nodes.

  upload to arduino after changing eser parameters    
NRF24L01 Wired as follows for uno
            uno     pro micro
ce/cs         9
csn          10    
sck          13
mo           11
mi           12

*/
/*
 At moment shetch does no work with analog pins
  
 * */
 
 */
//***************************************
// USER PARAMETERS  (change as needed)

 int firstVpin=400;   // FIRST PIN OF THIS NODE
int numbpins = 8;
// digital pins D0 - D7
const uint16_t this_node = 01; 
//********************************************



#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <Servo.h>


RF24 radio(9, 10);  // nRF24L01(+) radio attached using Getting Started board UNO
RF24Network network(radio);      // Network uses that radio
 
  // Address of our node in Octal format (04, 031, etc)
const uint16_t other_node = 00;  // Address of the other node in Octal format

Servo myservo;

 

 typedef struct payload_t {  // Structure of our payload VPIN:%d Apos:%d Ipos:%d Profile:%d Duration:%d state:%d
    int p_firstVpin;
    int p_numbpins;
    int p_pin;
    int p_value;
    int p_mode;
    int p_Apos;
    int p_Ipos;
    int p_profile;
    int p_Duration;
    int p_State;
    
  };
  payload_t payload;
      
      int _inputPin[8];
      int _firstpin;
      int _npins; 

void setup(void) {
  Serial.begin(115200);

   if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
 
 while (1) {
  //  hold in infinite loop
    }
  }
  
    radio.setDataRate(RF24_250KBPS);
    network.begin(54, /*node address*/ this_node);

     
  }
void loop(void) {
 
 if (_firstpin == 0){
   payload_Init(firstVpin);//
    delay(500);
  }
  
   network.update();  // Check the network regularly
     while (network.available()) {
     payload_R(&payload);
    }
  
checkSensors();



}

void writeOut(int _pin, int _value){     // WORKING
   pinMode(_pin, OUTPUT);
   digitalWrite(_pin, _value);
  }

void payload_Init(int _pin){//
  delay(1000);
  
  network.update();
    payload_t payload = { _pin ,8,1,2,9,3,4,5,6,7};
    RF24NetworkHeader header(00);//to node
    bool ok = network.write(header, &payload, sizeof(payload));
    Serial.println(ok ? F("ok.") : F("failed."));
}

void payload_R(payload_t* r ){     //WORKING
    //while (network.available()) {  // Is there anything ready for us?
     RF24NetworkHeader header;  // If so, grab it and print it out
     payload_t payload;
     network.read(header, &payload, sizeof(payload)); 
    
    _firstpin = payload.p_firstVpin;
    _npins = payload.p_numbpins;
 
     
 if (payload.p_mode ==1 && payload.p_value ==1){  //
     pinMode(payload.p_pin, INPUT_PULLUP);
     _inputPin[payload.p_pin] = payload.p_value +1;
     //_Apos
    }
   if  (payload.p_mode ==1 && payload.p_value ==0){
     pinMode(payload.p_pin, INPUT);
     _inputPin[payload.p_pin] = payload.p_value+1;
    }
 
else if (payload.p_mode ==3 ){
 
writeOut(payload.p_pin , payload.p_value);
 }
  
 if (payload.p_mode ==4 ){  //
      myservo.attach(payload.p_pin); 
      myservo.write(payload.p_Apos);   
   }
 
}

void checkSensors() {

  for (int count=0; count <= _npins-1; count++){
      if ( _inputPin[count]!= 0){
      if ( _inputPin[count]!= digitalRead(count)+1){
     _inputPin[count] =digitalRead(count)+1;
    
    network.update(); 
      payload_t payload = {_firstpin,_npins, count + _firstpin,digitalRead(count),4};
      RF24NetworkHeader header( other_node);//to node
      bool ok = network.write(header, &payload, sizeof(payload));
      Serial.println(ok ? F("ok.") : F("failed."));
 }  }   // loop
}   // void

 }

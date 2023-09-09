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

        
NRF24L01 Wired as follows for uno
            uno     pro micro
ce/cs         9
csn         10    
sck        13
mo          11
mi         12

*/
//***************************************
// USER PARAMETRICS

 int firstVpin=400;   // FIRST PIN OF THIS NODE
int numbpins = 8;
const uint16_t this_node = 01; 
//********************************************


//#define LCD;
#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <Servo.h>
#ifdef LCD 
#include <LCD_I2C.h>
LCD_I2C lcd(0x27, 20, 4); // Default address of most PCF8574 modules, change according
#endif
//RF24 radio(9, 10);  // nRF24L01(+) radio attached using Getting Started board
RF24 radio(9, 10);  // nRF24L01(+) radio attached using Getting Started board UNO
RF24Network network(radio);      // Network uses that radio
  // Address of our node in Octal format (04, 031, etc)
const uint16_t other_node = 00;  // Address of the other node in Octal format

Servo myservo;

 
/*
struct payload_t {  // Structure of our payload
 int _pin;
  int _value;
  int _mode;
};
*/
//int test;
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
/*
 struct payload_SERVO {  // Structure of our payload VPIN:%d Apos:%d Ipos:%d Profile:%d Duration:%d state:%d
      int _firstVpin;
      int _numbpins;
      int _pin;
      int _value;
      int _mode;
      int _Apos;
      int _Ipos;
      int _profile;
      int _Duration;
      int _State;

    };
    */
void setup(void) {
  Serial.begin(115200);
//Serial.println("TEST");
 #ifdef LCD  
  lcd.begin(); // If you are using more I2C devices using the Wire library use lcd.begin(false)
                 // this stop the library(LCD_I2C) from calling Wire.begin()
    lcd.backlight();
 #endif 
    //  while (!Serial) {
    // some boards need this because of native USB capability
// }
  Serial.println(F("RF24Network/examples/helloworld_rx/"));
//
  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
  //  lcd.setCursor(0 , 0); 
  //   lcd.print("Radio not responding!");
 while (1) {
  //  hold in infinite loop
    }
  }
   // network.begin(90, 0);
 //radio.setChannel(90);
  radio.setDataRate(RF24_250KBPS);
  network.begin(54, /*node address*/ this_node);

     
  }
void loop(void) {
  //Serial.println("loop");
 if (_firstpin == 0){
 payload_Init(firstVpin);//
Serial.print("setup _firstpin = ");
 Serial.println(_firstpin);
  delay(500);
  }
  
   network.update();  // Check the network regularly
     while (network.available()) {
     payload_R(&payload);
    //  Serial.print ("payload.p_mode = " ); 
// Serial.println (payload.p_mode ); 
  }
  //Serial.println("SENSORS loop ");
checkSensors();
 //Serial.print(" loop _firstpin = ");
// Serial.println(_firstpin);

//Serial.println("after SENSORS loop ");
//readpin(5);
 /*
       while (network.available()) {  // Is there anything ready for us?
     RF24NetworkHeader header;  // If so, grab it and print it out
     payload_t payload;
        network.read(header, &payload, sizeof(payload)); 
*/
#ifdef LCD 
  //   lcd.clear();
     lcd.setCursor(0, 0); 
     lcd.print("Received packet: ");
     lcd.setCursor(0, 1); 
     lcd.print("PIN=");
     lcd.print(payload.p_pin);

     lcd.setCursor(8, 1);
     lcd.print("VALUE=");
   lcd.println(payload.p_value);
     lcd.setCursor(0, 2);
     lcd.print("MODE=");
   lcd.println(payload.p_mode);
    
 lcd.setCursor(0, 3);
  lcd.print(payload.p_Apos);lcd.print(" ");
  lcd.print(payload.p_Ipos);lcd.print(" ");
 lcd.print(payload.p_profile);lcd.print(" ");
  lcd.print(payload.p_Duration);lcd.print(" ");
 lcd.print(payload.p_State);
#endif
//if (payload._mode == 1) readpin(payload._pin, payload._value);
//if (payload.p_mode == 3){ 
//  writeOut(payload.p_pin , payload.p_value);

// Serial.println(" payload.p_mode = ");
//Serial.println(payload.p_mode);
 //Serial.println(payload.p_value);
}//}
//}
/*
 * mode 1 set pinmode to input with 1 enable pullup 0 disables pullup
 * mode 3 writes output to pin
 */

void writeOut(int _pin, int _value){     // WORKING
   pinMode(_pin, OUTPUT);
  digitalWrite(_pin, _value);
}
/*
void readpin(int _pin){
 // lcd.setCursor(15, 3);
  //   lcd.print(_pin);
  pinMode(_pin, INPUT);
  digitalRead(_pin);
 lcd.setCursor(12, 3);
 lcd.print(_pin);
    lcd.print(digitalRead(_pin));
  
//payload_W(_pin, other_node);

  Serial.print(F("Sending... "));
    payload_t payload = { _pin ,digitalRead(_pin)};
    RF24NetworkHeader header( other_node);//to node
    bool ok = network.write(header, &payload, sizeof(payload));
    Serial.println(ok ? F("ok.") : F("failed."));


}
*/
void payload_Init(int _pin){//
  delay(1000);
  //lcd.setCursor(12 , 2); 
//     lcd.print(other_node, OCT);
 network.update();
  Serial.print(F("Sending... "));
    payload_t payload = { _pin ,8,1,2,9,3,4,5,6,7};
    RF24NetworkHeader header(00);//to node
    bool ok = network.write(header, &payload, sizeof(payload));
 //    lcd.setCursor(10 , 2); 
 //    lcd.print("d");
   
    Serial.println(ok ? F("ok.") : F("failed."));
}

void payload_R(payload_t* r ){     //WORKING
//while (network.available()) {  // Is there anything ready for us?
     RF24NetworkHeader header;  // If so, grab it and print it out
  payload_t payload;
 //Serial.println("Received ");
     network.read(header, &payload, sizeof(payload)); 
    
 Serial.println("payload read");
      Serial.print("payload.p_firstVpin = ");
      Serial.print(payload.p_firstVpin);
      Serial.print(" payload.p_numbpins = ");
      Serial.println(payload.p_numbpins);

 _firstpin = payload.p_firstVpin;
       _npins = payload.p_numbpins;
  //     Serial.println("received _firstpin = ");
 //Serial.println(_firstpin);
     
 if (payload.p_mode ==1 && payload.p_value ==1){  //
     pinMode(payload.p_pin, INPUT_PULLUP);
     _inputPin[payload.p_pin] = payload.p_value +1;
     //_Apos
 /* 
     lcd.setCursor(10, 3);
     lcd.print("   PULL");
     lcd.setCursor(8, 2);
     lcd.print("setpin ");
     lcd.print(payload._mode);
     lcd.print(_inputPin[payload._pin]);
     lcd.print(payload._pin);
     lcd.print(payload._value);
 */
   } if  (payload.p_mode ==1 && payload.p_value ==0){
     pinMode(payload.p_pin, INPUT);
     _inputPin[payload.p_pin] = payload.p_value+1;
   /*
     lcd.setCursor(10, 3);
     lcd.print("NO PULL");
     lcd.setCursor(8, 2);
     lcd.print("setpin ");
     lcd.print(payload._mode);
     lcd.print(_inputPin[payload._pin]);
     lcd.print(payload._pin);
     lcd.print(payload._value);
*/
//    lcd.setCursor(10, 3);
//    lcd.print("         ");
   
   }
 
else if (payload.p_mode ==3 ){
  /*
    Serial.print(" payload.p_pin = ");
Serial.print(payload.p_pin);
Serial.print(" payload.p_value = ");
  Serial.println(payload.p_value);
 */
writeOut(payload.p_pin , payload.p_value);
 }
  
 if (payload.p_mode ==4 ){  //
      myservo.attach(payload.p_pin); 
   myservo.write(payload.p_Apos);   
 /*
  lcd.setCursor(10, 2);
  lcd.print("SERVO");
   lcd.setCursor(0, 3);
  lcd.print(payload._Apos);lcd.print(" ");
  lcd.print(payload._Ipos);lcd.print(" ");
 lcd.print(payload._profile);lcd.print(" ");
  lcd.print(payload._Duration);lcd.print(" ");
 lcd.print(payload._State);*/
  }
 //  return
 // }
}

void checkSensors() {
//delay (500);
//Serial.println(_npins);
for (int count=0; count <= _npins-1; count++){
//Serial.printLN("SENSORS loop ");

if ( _inputPin[count]!= 0){
  //Serial.print(" count = ");
//Serial.println(count);
   if ( _inputPin[count]!= digitalRead(count)+1){
    _inputPin[count] =digitalRead(count)+1;
    Serial.println(count +_firstpin);
    network.update(); 
      Serial.print(F("SENSORS Sending... "));
      payload_t payload = {_firstpin,_npins, count + _firstpin,digitalRead(count),4};
      RF24NetworkHeader header( other_node);//to node
      bool ok = network.write(header, &payload, sizeof(payload));
      Serial.println(ok ? F("ok.") : F("failed."));
 }  }   // loop
}   // void

 }
//lcd.setCursor(17, 0);
//lcd.print(count);
//lcd.print(temp);
 //   lcd.setCursor(17, 1);
  //  lcd.print(digitalRead(5));
  //  lcd.print(digitalRead(6));
   // lcd.setCursor(0+count, 3);
  //  lcd.print(_inputPin[count]);
 //   lcd.setCursor(17, 3);
//    lcd.print(count);

//else{
 //   lcd.setCursor(0+count, 3);
 //   lcd.print(_inputPin[count]);
 //}    // else in loop

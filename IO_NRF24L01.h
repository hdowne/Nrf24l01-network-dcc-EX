/*
 *  © 2023, Harold Downe. All rights reserved.
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
 
   require RF24Network and RF24 library from 
   https://github.com/nRF24/RF24
   https://github.com/nRF24/RF24Network            

  see https://nrf24.github.io/RF24Network/ for how to configure nodes.

WORKS WITH DCC EX VER 4.1.5
WORKS WITH DCC EX VER 5.0.2

NRF24L01 Wired as follows

ce/cs  48       
csn    49      
sck    52     
mo     51     
mi     50    

=======================================================================
  The following directive defines a NRF24L01 module in the myHAL.cpp file.
=======================================================================
   The parameters are: 
     First Vpin=400
     Number of VPINs=8 (numbered 400 - 407)
     node number in oct format = 01
     **  No need for a line for parent (master) node (node 00)  **
  
    NRF24L01::create(400, 8, 01);  //  child node
    NRF24L01::create(410, 8, 011);  // grandchild node


*/



#ifndef IO_NRF24L01_H
#define IO_NRF24L01_H


//#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

#include "printf.h"
#include "IODevice.h"
#include "DIAG.h"
 

  
   int _addressptr[8];
   int ptr = 0 ;
   int respond = 0;
   
   RF24 radio(48,49);  // (9,53)   using pin 48 for the CE pin, and pin 49 for the CSN pin
   RF24Network network(radio);//

class NRF24L01 : public IODevice {
  private:
   
    uint16_t _firstVpin;
    uint16_t _nPins;
    uint16_t _node;
    int _ce = 48;
    int _cs = 49;

    const uint16_t this_node = 00;   // Address of our node in Octal format
  
    typedef struct payload_SERVO {  // Structure of our payload VPIN:%d Apos:%d Ipos:%d Profile:%d Duration:%d state:%d
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

  public:
    NRF24L01(VPIN firstVpin, uint8_t nPins, uint16_t node):  //, int ce=-1, int cs=-1
      IODevice(firstVpin, min(nPins, 8)) {

      _firstVpin = firstVpin;
      _nPins = min(nPins, 8);
      _node = node;
  
       addDevice(this);
    }
 
  struct PinData {
    uint16_t _Virtualpin = 0;
    uint8_t _numb_Pins = 0;
    uint16_t _recNode = 0 ; // Config parameter
    uint16_t _pinMode[8] ; // Config parameter 1 for input 0 for output
    uint16_t _pinState[8] ; //  1 = high 0 = low
  }PinData;
  
   struct PinData *_PinData;
   struct PinData  _PinData0;
    
    static void create(VPIN firstVpin, uint8_t nPins, uint16_t node) {  //, int ce=-1, int cs=-1
      new NRF24L01(firstVpin, nPins, node); //, ce,  cs
    }

 protected:
    void _begin() override {
 
      if (!radio.begin() && respond != 1) {
        Serial.println(F("Radio hardware not responding!"));
      // while (1) {
        // hold in infinite loop
       }//}
       else if (respond != 1){
         Serial.println(F("Radio hardware responding"));
       DIAG(F("nrf24L01-SPI:%d configured"), 00);
        respond = 1;
    }

     radio.setPALevel(RF24_PA_MIN);    // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
     radio.setDataRate(RF24_250KBPS);   //  RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
     //Serial.println( radio.getPALevel()) ;  // 0=min  1 = low  2 = med  3 = max
    //Serial.println(   radio.getDataRate());  // 0 = 1Mbps   1 = 2Mbps  2 = 250kbs
       network.begin(54, 0);
   //printf_begin();        // needed for RF24* libs' internal printf() calls
   //radio.printDetails();  // requires printf support
       
     //#ifdef DIAG_IO
     DIAG(F("nrf24L01-SPI:%d configured on VPins:%d-%d CE Pin:%d CS Pin:%d"), (_node), _firstVpin, _firstVpin + _nPins - 1, _ce, _cs);
    _addressptr[ptr] = &_PinData0;
    _PinData = _addressptr[ptr];
    _PinData->_Virtualpin = _firstVpin;
    _PinData->_numb_Pins = _nPins;
    _PinData->_recNode = _node;
    _addressptr[ptr+1] = NULL;
    ptr++;
    // #endif
  
    }
 
   void _loop(unsigned long currentMicros) override {
   
  }

  void _readRadio()  {//int vpin
   
    network.update();  // Check the network regularly
    
   while (network.available()) {  // Is there anything ready for us?     
       payload_SERVO payload;
       RF24NetworkHeader header;  // If so, grab it and print it out  
       network.read(header, &payload, sizeof(payload));


   for (int temp = 0; temp <ptr; temp++) {   // 400 -  408
      _PinData = _addressptr[temp];
        if(payload._pin >= _PinData-> _Virtualpin && payload._pin< _PinData-> _Virtualpin+ _PinData->  _numb_Pins && payload._mode == 4 ){
           _PinData-> _pinState[payload._pin-payload._firstVpin] = payload._value;
        }
     }
 
   switch (payload._mode ){
  //   case 1:
 //   case 2:
 //   case 3:
case 4:
  break;
  
     case 9:   // payload.mode = 9 
         
       for (int temp0 = 0; temp0 < ptr; temp0++) { 
         _PinData = _addressptr[temp0];
              if( payload._firstVpin >= _PinData-> _Virtualpin && payload._firstVpin < _PinData-> _Virtualpin+ _PinData->  _numb_Pins){
                    for (int temp = payload._firstVpin; temp < payload._firstVpin + payload._numbpins; temp++) {   // 400 -  408
                         if( _PinData->_pinMode[temp - payload._firstVpin] == 1 || _PinData->_pinMode[temp - payload._firstVpin] == 2){   //&& payload._mode == 9
                        delay(100);
           IODevice::configureInput(temp, _PinData->_pinState[temp - payload._firstVpin]   );///_inputPin[temp-payload._pin
      }  
       
  else if   (_PinData->_pinMode[temp - payload._firstVpin] ==0){   // 0 = configure Output
 
          delay(100);
      IODevice::write(temp, _PinData->_pinState[temp - payload._firstVpin]);
     }    
     
  else if   (_PinData->_pinMode[temp - payload._firstVpin] ==3){  // 3 = configure servo

   IODevice::writeAnalogue(temp, _PinData->_pinState[temp - payload._firstVpin], 25, 45);
  }  } 
  } }  

DIAG(F("nrf24l01 Node %d Online"),header.from_node);
   
    break;  
 }
    } 
  }
// int _hasCallback {
 
//Serial.println("_hasCallback")};
   

 int _read(VPIN vpin)override {
    (void) vpin;
      int _value;
      int _pin = vpin - _firstVpin;
      _readRadio();
   for (int temp = 0; temp <ptr; temp++) {   // 400 -  408
     _PinData = _addressptr[temp];
       if(vpin >= _PinData-> _Virtualpin && vpin<= _PinData-> _Virtualpin+ _PinData->  _numb_Pins-1 ){
 
    return _PinData-> _pinState[_pin];
       }
   }  
}


   
   void _write(VPIN vpin, int value) override {
       delay(100);
      int _value = value;
      int _pin = vpin - _firstVpin;
      int _mode = 3; //output
  
   for (int temp = 0; temp <ptr; temp++) {   // 400 -  408
      _PinData = _addressptr[temp];
         if(vpin >= _PinData-> _Virtualpin && vpin< _PinData-> _Virtualpin+ _PinData->  _numb_Pins ){
            _PinData-> _Virtualpin = _firstVpin;
            _PinData-> _numb_Pins = _nPins;
            _PinData-> _recNode = _node;
            _PinData-> _pinState[_pin] = _value ;
            _PinData-> _pinMode[_pin] = 0 ;
  
   network.update();
     payload_SERVO payload = {_firstVpin,_nPins,  _pin, _value, _mode };
          RF24NetworkHeader header( _node, OCT);   //to node, OCT
         bool ok = network.write(header, &payload, sizeof(payload));
        Serial.println(ok ? F("ok.") : F("failed."));
     
DIAG(F("nrf24l01 OUTPUT Configure VPIN:%d Value:%d"),// Ipos:%d Profile:%d Duration:%d state:%d
     vpin,  _PinData->_pinState[_pin]);   //, params[1], params[2], params[3], params[4]);
    }
 }
 }

void _writeAnalogue(VPIN vpin, int value, uint8_t param1, uint16_t param2) override { 
   
    uint8_t _pin = vpin - _firstVpin;
     for (int temp = 0; temp <ptr; temp++) {   // 400 -  408
      _PinData = _addressptr[temp];
         if(vpin >= _PinData-> _Virtualpin && vpin<= _PinData-> _Virtualpin+ _PinData->  _numb_Pins-1 ){
      _PinData->_pinMode[ _pin   ] = 3;


   payload_SERVO payload = { _firstVpin,_nPins, _pin, 0,4, value,0, param1,0, 0 };
   
      RF24NetworkHeader header( _node, OCT);   //to node, OCT
      bool ok = network.write(header, &payload, sizeof(payload));
      Serial.println(ok ? F("ok.") : F("failed."));
//DIAG(F("nrf24l01 servo Configure VPIN:%d Apos:%d Ipos:%d Profile:%d Duration:%d state:%d"),
   //      vpin, params[0], params[1], params[2], params[3], params[4]);
         }
     }
}
    // Configure a input on the NRF24L01.
    bool _configure(VPIN vpin, ConfigTypeEnum configType, int paramCount, int params[]) {
      delay(100);
       
    //  network.update();
    for (int temp = 0; temp <ptr; temp++) {   // 400 -  408
      _PinData = _addressptr[temp];
        if(vpin >= _PinData-> _Virtualpin && vpin< _PinData-> _Virtualpin+ _PinData->  _numb_Pins ){
          if (configType == CONFIGURE_INPUT) { // return false;
               int _value = params[0];   //Pullup
               int _pin = vpin - _firstVpin;
               _PinData-> _pinState[_pin] = _value ;
               _PinData-> _pinMode[_pin] = 1 ;
        //network.update();
        int _mode = 1;  //input
     payload_SERVO payload = {_firstVpin,_nPins, _pin, _value, _mode };
        RF24NetworkHeader header( _node, OCT);   //to node
        bool ok = network.write(header, &payload, sizeof(payload));
        Serial.println(ok ? F("ok.") : F("failed."));
       
        //#ifdef DIAG_IO
        DIAG(F("NRF24L01 Sensor Configure VPIN:%d Pullup:%d" ), vpin, params[0]);
        // #endif
       
   }  
      
 else if (configType == CONFIGURE_SERVO) { //return false;
     
        network.update();
              int _value = 0;
              int _Apos = params[0]; int _Ipos = params[1]; int _profile = params[2]; int _Duration = params[3]; int _State = params[4];
              int _pin = vpin + 1 - _firstVpin;
              int _mode = 2;  //servo
     
        payload_SERVO payload = {_firstVpin,_nPins, _pin, _value, _mode,  _Apos, _Ipos, _profile, _Duration, _State};
        RF24NetworkHeader header( _node);   //to node
        bool ok = network.write(header, &payload, sizeof(payload));
        Serial.println(ok ? F("ok.") : F("failed."));
        DIAG(F("nrf24l01 servo Configure VPIN:%d Apos:%d Ipos:%d Profile:%d Duration:%d state:%d"),
             vpin, params[0], params[1], params[2], params[3], params[4]);
      }

   else if (configType == CONFIGURE_OUTPUT) { //return false;
  
       int _value = params[0];
       int _pin = vpin - _firstVpin;
  
  struct PinData *s = _PinData;
  if (s == NULL) { 
    _PinData = (struct PinData *)calloc(1, sizeof(struct PinData));
    s = _PinData;
    if (!s) return false; // Check for failed memory allocation
  }
  
    _PinData-> _Virtualpin = _firstVpin;
    _PinData-> _numb_Pins = _nPins;
    _PinData-> _recNode = _node;
    _PinData-> _pinState[_pin] = _value ;
    _PinData-> _pinMode[_pin] = 2 ;
       int _mode = 2;  //servo
            payload_SERVO payload = {_firstVpin,_nPins, _pin, _value, _mode };
         RF24NetworkHeader header( _node, OCT);   //to node
        bool ok = network.write(header, &payload, sizeof(payload));
        Serial.println(ok ? F("ok.") : F("failed."));

// DIAG(F("nrf24l01 OUTPUT Configure VPIN:%d Apos:%d"),// Ipos:%d Profile:%d Duration:%d state:%d
          }
      }
   }
}
     /* 
 * mode 1 set pinmode to input with 1 enable pullup 0 disables pullup
 * mode 3 writes output to pin
 * mode 4  read input pin
 */ 
        
     
 }; //PUBLIC io DEVICE
#endif

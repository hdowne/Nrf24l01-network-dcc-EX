THIS IS WORK IN PROGRESS

# Nrf24l01-network-dcc-EX
# Nrf24l01 network library driver for dcc++ ex

With this driver you can connect the Nrf24l01 to the latest version of dcc++ ex. 
Add a couple of lines in the myHal.cpp file.


   require RF24Network and RF24 library from 
   
   https://github.com/nRF24/RF24
   
   https://github.com/nRF24/RF24Network            

  see https://nrf24.github.io/RF24Network/ for how to configure nodes.

Also upload Remote_node.ino to second arduino after changiing parameters in file for

spi pin (if needed)

USER parameters

int firstVpin=400;   // FIRST PIN OF THIS NODE

int numbpins = 8;

const uint16_t this_node = 01; 


NRF24L01 Wired as follows
for Arduino Mega

ce/cs  48       
csn    49      
sck    52     
mo     51     
mi     50    

==========================

  The following directive defines a NRF24L01 module in the myHAL.cpp file.

==========================

   The parameters are: 
     First Vpin=400
     Number of VPINs=8 (numbered 400 - 407)
     node number in oct format = 01
     
     **  No need for a line for parent (master) node (node 00)  **
  
    NRF24L01::create(400, 8, 01);  //  child node
    NRF24L01::create(410, 8, 011);  // grandchild node

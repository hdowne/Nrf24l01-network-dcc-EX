THIS IS WORK IN PROGRESS

# Nrf24l01-network-dcc-EX
Nrf24l01 network library driver for dcc++ ex

With this driver you can connect the Nrf24l01 to the latest version of dcc++ ex. Add a couple of line in the myHal.cpp file.

This will configure the Nrf24l01 with the first pin, number of pins, node address, ce pin, cs pin.

NRF24L01::create(399, 1, 00, 47, 48); // For master node
NRF24L01::create(400, 8, 01, 47, 48); // For all other nodes

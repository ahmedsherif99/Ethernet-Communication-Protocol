# Ethernet-Communication-Protocol
An ethernet communication project between two microcontrollers by sending an ethernet frame through UART. Sender takes text from user through terminal then puts the data into an ethernet frame by adding source and destination mac addresses to the header, then calculates the CRC and send the decoded message to the receiver. The receiver on the other side waits for the message and checks for the source and destination mac addresses then the CRC and decode the message and show its content to the user on the terminal. Code was written on Arduino IDE using C language and flashed to an arduino mega and arduino uno.

Youtube Link for Project explanation and demonstration: https://www.youtube.com/watch?v=O7DGmUS7JsM

![image](https://user-images.githubusercontent.com/68401714/150700956-ffb4cf9b-1b90-4388-ac92-8d50f63e9f89.png)

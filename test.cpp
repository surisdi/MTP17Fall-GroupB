#include <iostream>
#include <cstdlib>
#include "RF24/RF24.h"

using namespace std;

//Using GPIO25 (Pin 22) as CE, and CE0 as CS.
RF24 radio(25,0);

int main(int argc, char** argv){

        /************SETUP***************/

        cout << "Setting up the module.\n";

        radio.begin();
        // setChannel as 2400 + n (MHz)
        radio.setChannel(97);
        // Power Amplifier Level: MIN (-18dBm), LOW (-12dBm), HIGH (-6dBm), MAX (0dBm)
        radio.setPALevel(RF24_PA_LOW);
        // Data rate: 250KBPS, 1MBPS, 2MBPS
        radio.setDataRate(RF24_2MBPS);
        // AutoACK: Not allowed
        radio.setAutoAck(1);
        // CRC: Not allowed
        radio.disableCRC();

        // Print settings
        cout << "Getting the channel:";
        cout << radio.getChannel();
        cout << "\n";
        //radio.printDetails();

        cout << "Exiting...\n";
        return 0;
}


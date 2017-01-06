#include <SPI.h>        
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <common.h>
// :|
// included because otherwise refuses to build.
#include <common.c>

//Adruino Pin definitions
const static int steer_dir = 2;
const static int steer_pwm = 3;
const static int drive_dir = 4;
const static int drive_pwm = 5;


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static IPAddress ip(192,168,1,101); 
static unsigned int localPort = 1340;      // local port to listen on

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet,

void setup();

void handle_axis(int pwm, int dir, int16_t reading, int max_pwm);

// gp_state is a global variable to keep it from going out of scope
gp_state state;

void loop() {

	int packetSize = Udp.parsePacket();
	if(packetSize) {
		// read the packet into packetBufffer
		Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
		// read the buffer into gp_state
		structize_gpbuffer(&state, packetBuffer, sizeof (gp_state));
	}

	// handle the changed state here
	handle_axis(steer_pwm, steer_dir, state.x0, 127);
	handle_axis(drive_pwm, drive_dir, state.y1, 63);
}

void setup() {
   	// scale the prescaler down to achieve the PWM
    // necessary for moving the drive motor
	// NOTE: affects pins 3 and 11

	pinMode(drive_dir, OUTPUT);
	pinMode(drive_pwm, OUTPUT);
	pinMode(steer_dir, OUTPUT);
	pinMode(steer_pwm, OUTPUT);

	// start the Ethernet and UDP:
	Ethernet.begin(mac,ip);
	Udp.begin(localPort);
	Serial.begin(9600);
	Serial.println("Initializing");
	Serial.println(Ethernet.localIP());
}

void handle_axis(int pwm_pin, int dir_pin, int16_t reading, int max_pwm) {
	const static int input_max = 32768;
	const static int divisor = input_max / max_pwm;
	int scaled;

	if(reading < 0) { // if it's negative
		reading = abs(reading);
		digitalWrite(dir_pin, LOW);
		scaled = (reading-1)/divisor;
		analogWrite(pwm_pin, scaled);
	} 
	else { // it's positive
		digitalWrite(dir_pin, HIGH);
		scaled = reading/divisor;
		analogWrite(pwm_pin, scaled );
	}
}

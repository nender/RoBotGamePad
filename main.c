#include <stdbool.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <sys/socket.h>
#include <netdb.h>
#include "common.h"

#define GP_X0 0
#define GP_Y0 1
#define GP_X1 2
#define GP_Y1 3

// Globals
const static char arduino_ip[] = "192.168.1.101";
const static char arduino_port[] = "1340";

// struct returned by init_network, basically a container for network state
typedef struct {
    int sockfd;
    struct addrinfo *info;
} netinfo;

// prints joystick attributes, useful for debugging
void joystick_attr(SDL_Joystick* joystick) {
    printf("Axes: %d\n", SDL_JoystickNumAxes(joystick));
    printf("Buttons: %d\n", SDL_JoystickNumButtons(joystick));
    printf("Balls: %d\n", SDL_JoystickNumBalls(joystick));
    printf("Hats: %d\n", SDL_JoystickNumHats(joystick));
}

// for a given buttonup, make sure the state struct is updated
void process_buttonup(SDL_Event *event, gp_state *state) {
    /* the bit magic here is to AND the current bit with the negation of 
     * the pressed button */
    switch (event->button.button) {
	case 0:
	    state->buttons &= ~(1 << GP_B0);
	    break;
	case 1:
	    state->buttons &= ~(1 << GP_B1);
	    break;
	case 2:
	    state->buttons &= ~(1 << GP_B2);
	    break;
	case 3:
	    state->buttons &= ~(1 << GP_B3);
	    break;
	case 4:
	    state->buttons &= ~(1 << GP_B4);
	    break;
	case 5:
	    state->buttons &= ~(1 << GP_B5);
	    break;
	case 6:
	    state->buttons &= ~(1 << GP_B6);
	    break;
	case 7:
	    state->buttons &= ~(1 << GP_B7);
	    break;
	case 8:
	    state->buttons &= ~(1 << GP_B8);
	    break;
	case 9:
	    state->buttons &= ~(1 << GP_B9);
	    break;
	case 10:
	    state->buttons &= ~(1 << GP_B10);
	    break;
	case 11:
	    state->buttons &= ~(1 << GP_B11);
	    break;
    }
    return;
}

// for a given buttondown, make sure the state is updated
void process_buttondown(SDL_Event* event, gp_state* state) {
    /* the bit magic here is to OR the bit with the pressed state*/
    switch (event->button.button) {
	case 0:
	    state->buttons |= (1 << GP_B0);
	    break;
	case 1:
	    state->buttons |= (1 << GP_B1);
	    break;
	case 2:
	    state->buttons |= (1 << GP_B2);
	    break;
	case 3:
	    state->buttons |= (1 << GP_B3);
	    break;
	case 4:
	    state->buttons |= (1 << GP_B4);
	    break;
	case 5:
	    state->buttons |= (1 << GP_B5);
	    break;
	case 6:
	    state->buttons |= (1 << GP_B6);
	    break;
	case 7:
	    state->buttons |= (1 << GP_B7);
	    break;
	case 8:
	    state->buttons |= (1 << GP_B8);
	    break;
	case 9:
	    state->buttons |= (1 << GP_B9);
	    break;
	case 10:
	    state->buttons |= (1 << GP_B10);
	    break;
	case 11:
	    state->buttons |= (1 << GP_B11);
	    break;
    }
    return;
}

// simple function to place the value from the event to the state struct 
void process_joyhat(SDL_Event* event, gp_state* state) {
    state->hat = event->jhat.value;
    return;
}

/* given an axis event this function places the new axis value in it's proper
 * place. */
void process_axis(SDL_Event* event, gp_state* state) {
    switch (event->jaxis.axis) {
	case GP_X0:
	    state->x0 = event->jaxis.value;
	    break;

	case GP_Y0:
	    state->y0 = event->jaxis.value;
	    break;

	case GP_X1:
	    state->x1 = event->jaxis.value;
	    break;

	case GP_Y1:
	    state->y1 = event->jaxis.value;
	    break;
    }
    return;
}

/* returns a netinfo struct pointer which contains the information for using
 * the connection. Most of this code is adapted from beej's network programming
 * guide */
netinfo* net_init() {
    netinfo *ret = (netinfo*) malloc(sizeof (netinfo));
    int status, sockfd;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0x00, sizeof (hints)); // make sure struct is empty
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM; // for udp
    if ((status = getaddrinfo(arduino_ip, arduino_port, &hints, &res)) != 0)
		fprintf(stderr, "Error: getaddrinfo() failed \n");

    // loop through all result addresses and create socket
    for (p = res; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family,
			p->ai_socktype, p->ai_protocol)) == -1) {
			fprintf(stderr, "Error: socket() returned error\n");
			continue;
		}
		break;
    }

    // make sure we have a valid address
    if (p == NULL)
		fprintf(stderr, "Error: failed to bind socket\n");

    ret->info = p;
    ret->sockfd = sockfd;
    return ret;
}

/* function to send _ALL_ of a given chunk of data. Normally could exit early
 with arbitrary amount of information sent.*/
int net_send(netinfo* p, void* data, size_t len) {
    int towrite, code;
    towrite = len;
    
    // while there is still data to write
    while (towrite > 0) {
		code = sendto(p->sockfd, data, len, 0x00,
			p->info->ai_addr, p->info->ai_addrlen);

		// if code is negative there is an error
		if (code < 0) {
			fprintf(stderr, "Error: sendto returned error code\n");
			/* if amount written is greater than amount we had left to write then
			something is wrong. */
		} else if (code > towrite) {
			fprintf(stderr, "Error: sendto returned send more than was asked?\n");
		}
		
		// subtract what we wrote from what we have left to write
		towrite -= code;
		code = 0;
    }
    
    return 0;
}

// SDL defines it's own main, so this is our "shadow" main.
int SDL_main(int argc, char** argv) {
    // get the info for out network connection first
    netinfo* net = net_init();

    // set up SDL and exit early if it fails
    if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Failed to init SDL, quitting\n");
    } else {
		SDL_JoystickEventState(SDL_ENABLE);

		// otherwise prep for joystick events
		SDL_Joystick *joystick;
		if ((joystick = SDL_JoystickOpen(0)) == NULL)
			fprintf(stderr, "Error: failed to get joystick\n");
		
		// gp_state is the main state struct for the gamepad
		SDL_Event event;
		gp_state state;
		memset(&state, 0x00, sizeof (state));
		char gpbuffer[sizeof (gp_state)];
		memset(&state, 0x00, sizeof (gp_state));
		
		// get the next event and apply it to gp_state
		while (true) {
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_JOYBUTTONUP:
						process_buttonup(&event, &state);
						break;
					case SDL_JOYBUTTONDOWN:
						process_buttondown(&event, &state);
					break;
					case SDL_JOYHATMOTION:
						process_joyhat(&event, &state);
					break;
					case SDL_JOYAXISMOTION:
						process_axis(&event, &state);
					break;
				}
			}
			// dump gp_state to buffer and then send it off to the arduino
			bufferize_gpstate(gpbuffer, &state, sizeof(gp_state));
			net_send(net, (void*) gpbuffer, sizeof (gp_state));
		}
    }
}

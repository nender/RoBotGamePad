/* 
 * File:   common.h
 * Author: nathan
 *
 * Created on November 2, 2012, 5:21 PM
 */

#ifndef COMMON_H
#define	COMMON_H

#include <sys/types.h>
#include <stdint.h>
#include <string.h>

// holds current stats for all used elements of gamepad
typedef struct {
    uint16_t buttons;
    uint8_t hat;
    int16_t x0;
    int16_t y0;
    int16_t x1;
    int16_t y1;
} gp_state;

int bufferize_gpstate(char* buffer, gp_state* state,  size_t len);
int structize_gpbuffer(gp_state* state, char* buffer,  size_t len);

#define GP_B0 0
#define GP_B1 1
#define GP_B2 2
#define GP_B3 3
#define GP_B4 4
#define GP_B5 5
#define GP_B6 6
#define GP_B7 7
#define GP_B8 8
#define GP_B9 9
#define GP_B10 10
#define GP_B11 11

#endif
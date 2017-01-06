#include "common.h"

int bufferize_gpstate(char* buffer, gp_state* state, size_t len) {
    int offset = 0;
    // don't use offset first read
    memcpy(buffer, &state->buttons, sizeof (state->buttons));
    offset += sizeof (state->buttons);

    memcpy(&buffer[offset], &state->hat, sizeof (state->hat));
    offset += sizeof (state->hat);

    memcpy(&buffer[offset], &state->x0, sizeof (state->x0));
    offset += sizeof (state->x0);

    memcpy(&buffer[offset], &state->y0, sizeof (state->y0));
    offset += sizeof (state->y0);

    memcpy(&buffer[offset], &state->x1, sizeof (state->x1));
    offset += sizeof (state->x1);

    memcpy(&buffer[offset], &state->y1, sizeof (state->y1));
    offset += sizeof (state->y1);

    return 0;
}

int structize_gpbuffer(gp_state* state, char* buffer, size_t len) {
    int offset = 0;
    // don't use offset for first write
    memcpy(&state->buttons, buffer, sizeof (state->buttons));
    offset += sizeof (state->buttons);

    memcpy(&state->hat, &buffer[offset], sizeof (state->hat));
    offset += sizeof (state->hat);

    memcpy(&state->x0, &buffer[offset], sizeof (state->x0));
    offset += sizeof (state->x0);

    memcpy(&state->y0, &buffer[offset],  sizeof (state->y0));
    offset += sizeof (state->y0);

    memcpy(&state->x1, &buffer[offset],  sizeof (state->x1));
    offset += sizeof (state->x1);

    memcpy(&state->y1, &buffer[offset], sizeof (state->y1));
    offset += sizeof (state->y1);
    return 0;
}
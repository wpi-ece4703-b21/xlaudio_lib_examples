#include "xlaudio.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "xlaudio_armdsp.h"

typedef enum {IDLE, RECORDING, PLAYBACK_1, PLAYBACK_2, PLAYBACK_3} audiostate_t;

#define BUFSIZE 16384

int8_t    buffer[BUFSIZE];
uint32_t  bufindex  = 0;
audiostate_t pbmode = PLAYBACK_1;

audiostate_t next_state(audiostate_t current) {
    audiostate_t next;

    next = current;    // by default, maintain the state

    switch (current) {
    case IDLE:
        xlaudio_colorledoff();
        xlaudio_errorledoff();
        bufindex = 0;
        if (xlaudio_pushButtonLeftDown())
            next = RECORDING;
        else if (xlaudio_pushButtonRightDown())
            next = pbmode;
        break;

    case RECORDING:
        xlaudio_errorledon();
        bufindex++;
        if (bufindex == BUFSIZE) {
            bufindex = 0;
            next = IDLE;
        }
        break;

    case PLAYBACK_1:
        xlaudio_colorledgreen();
        bufindex++;
        if (bufindex == BUFSIZE) {
            bufindex = 0;
            if (xlaudio_pushButtonRightUp()) {
                pbmode = PLAYBACK_2;
                next = IDLE;
            }
        }
        break;

    case PLAYBACK_2:
        xlaudio_colorledblue();
        bufindex++;
        bufindex++;
        if (bufindex == BUFSIZE) {
            bufindex = 0;
            if (xlaudio_pushButtonRightUp()) {
                pbmode = PLAYBACK_3;
                next = IDLE;
            }
        }
        break;

    case PLAYBACK_3:
        xlaudio_colorledred();
        bufindex = (bufindex + BUFSIZE - 1) % BUFSIZE; // play backward
        if (bufindex == 0) {
            if (xlaudio_pushButtonRightUp()) {
                pbmode = PLAYBACK_1;
                next = IDLE;
            }
        }
        break;

    default:
        next = IDLE;
        break;
    }

    return next;
}

audiostate_t glbAudioState = IDLE;

uint16_t processSample(uint16_t x) {
    uint16_t y;

    glbAudioState = next_state(glbAudioState);         // called for every new sample @16KHz

    switch (glbAudioState) {
    case RECORDING:
             buffer[bufindex] = xlaudio_adc14_to_q15(x) >> 6;  // only keep upper byte to save storage
             y = xlaudio_q15_to_dac14(0);                      // keep silent during recording
             break;
    case PLAYBACK_1:
    case PLAYBACK_2:
    case PLAYBACK_3:
             y = xlaudio_q15_to_dac14(((int16_t) buffer[bufindex]) << 6);
             break;
    default:
             y = xlaudio_q15_to_dac14(0);                     // keep silent in IDLE
             break;
    }
    return y;
}

#include <stdio.h>

int main(void) {
    WDT_A_hold(WDT_A_BASE);

    xlaudio_init_intr(FS_16000_HZ, XLAUDIO_MIC_IN, processSample);
    xlaudio_run();

    return 1;
}

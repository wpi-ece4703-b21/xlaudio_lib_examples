#include "xlaudio.h"
#include "xlaudio_armdsp.h"

   uint16_t processSample(uint16_t x) {
       static float32_t tap = 0.0;
       float32_t sx, sy;

       sx  = xlaudio_adc14_to_f32(x);
       tap = 0.9 * tap + 0.1 * sx;
       sy  = tap;

       return xlaudio_f32_to_dac14(sy);
   }

   #include <stdio.h>

   int main(void) {
       WDT_A_hold(WDT_A_BASE);

       xlaudio_init_intr(FS_16000_HZ, XLAUDIO_MIC_IN, processSample);
       xlaudio_run();

       return 1;
   }

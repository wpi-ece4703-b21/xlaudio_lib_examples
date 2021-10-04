#include "xlaudio.h"

   uint16_t processSample(uint16_t x) {
       blockingerror();
       return x;
   }

   #include <stdio.h>

   int main(void) {
       WDT_A_hold(WDT_A_BASE);

       xlaudio_init_intr(FS_32000_HZ, XLAUDIO_MIC_IN, processSample);
       xlaudio_run();

       return 1;
   }

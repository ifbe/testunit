#define RME32_WCR_START     (1 << 0)    /* startbit */
#define RME32_WCR_MONO      (1 << 1)    /* 0=stereo, 1=mono
                                           Setting the whole card to mono
                                           doesn't seem to be very useful.
                                           A software-solution can handle 
                                           full-duplex with one direction in
                                           stereo and the other way in mono. 
                                           So, the hardware should work all 
                                           the time in stereo! */

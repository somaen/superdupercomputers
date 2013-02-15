#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
struct Precision_Timer {
	struct timeval time_start;
	struct timeval time_curr;
	struct timeval time_diff;
	struct timezone tz;
};
void PT_start ( struct Precision_Timer *pt);
void PT_stop ( struct Precision_Timer *pt);
void diffTime( struct Precision_Timer *pt );
char *print_timeval( struct Precision_Timer *pt);

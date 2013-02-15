#include "PrecisionTimer.h"

void PT_start ( struct Precision_Timer *pt){
	gettimeofday(&pt->time_start, &pt->tz); 
}
void PT_stop ( struct Precision_Timer *pt){
	gettimeofday(&pt->time_curr, &pt->tz);
}
void diffTime( struct Precision_Timer *pt ){
	pt->time_diff.tv_sec = pt->time_curr.tv_sec -pt->time_start.tv_sec;
	pt->time_diff.tv_usec = pt->time_curr.tv_usec -pt->time_start.tv_usec;
	if ( pt->time_diff.tv_usec < 0 ){
		pt->time_diff.tv_usec += 1000000;
		pt->time_diff.tv_sec--;
	}
	return;
}
char *print_timeval( struct Precision_Timer *pt){
	char *mystr = calloc(1000, sizeof(char));
	snprintf(mystr, 1000, "%ld seconds and %ld µseconds", pt->time_diff.tv_sec, pt->time_diff.tv_usec );
	return mystr;
}

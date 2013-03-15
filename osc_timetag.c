#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
/*
#ifdef WIN_VERSION
#include <Winsock.h>
#else
#include <arpa/inet.h>
#endif
*/
#include <time.h>
#include <sys/time.h>

// crossplatform gettimeofday
//#include "contrib/timeval.h"

// this is for windows which doesn't have
// strptime
#include "contrib/strptime.h"

#include "osc.h"
#include "osc_byteorder.h"
#include "osc_timetag.h"

#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP
typedef struct _osc_timetag_ntptime{
	uint32_t sec;
	uint32_t frac_sec;
} t_osc_timetag_ntptime;
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
typedef struct _osc_ptptime{

} t_osc_ptptime;
#else
#error Unrecognized timetag format in osc_timetag.c!
#endif

void osc_timetag_ut_to_ntp(long int ut, t_osc_timetag_ntptime *n);
long int osc_timetag_ntp_to_ut(t_osc_timetag_ntptime n);
void osc_timetag_float_to_ntp(double d, t_osc_timetag_ntptime *t);
double osc_timetag_ntp_to_float(t_osc_timetag_ntptime t);
int osc_timetag_ntp_to_iso8601(t_osc_timetag t, char *s);

// add or subtract

t_osc_timetag osc_timetag_add(t_osc_timetag t1, t_osc_timetag t2)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP
	t_osc_timetag_ntptime t1_ntp = *((t_osc_timetag_ntptime *)(&t1));
	t_osc_timetag_ntptime t2_ntp = *((t_osc_timetag_ntptime *)(&t2));
	t_osc_timetag_ntptime r;
	r.sec = t1_ntp.sec + t2_ntp.sec;
	r.frac_sec = t1_ntp.frac_sec + t2_ntp.frac_sec;
    
	if(r.frac_sec < t1_ntp.frac_sec) { // rollover occurred
		r.sec += 1;
	}
	return *((t_osc_timetag *)(&r));
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
#endif
}

t_osc_timetag osc_timetag_subtract(t_osc_timetag t1, t_osc_timetag t2)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP
	t_osc_timetag_ntptime t1_ntp = *((t_osc_timetag_ntptime *)(&t1));
	t_osc_timetag_ntptime t2_ntp = *((t_osc_timetag_ntptime *)(&t2));
	t_osc_timetag_ntptime r;

	int64_t sec = t1_ntp.sec - t2_ntp.sec;
	int64_t frac_sec = t1_ntp.frac_sec - t2_ntp.frac_sec;
	if(sec < 0){
		sec *= -1;
	}
	if(frac_sec < 0){
		sec -= 1;
		frac_sec += 1;
	}
	r.sec = (uint32_t)sec;
	r.frac_sec = (uint32_t)frac_sec;

	return *((t_osc_timetag *)(&r));
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
#endif
}

int osc_timetag_cmp(t_osc_timetag t1, t_osc_timetag t2)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP
	t_osc_timetag_ntptime t1_ntp = *((t_osc_timetag_ntptime *)(&t1));
	t_osc_timetag_ntptime t2_ntp = *((t_osc_timetag_ntptime *)(&t2));
	if(t1_ntp.sec < t2_ntp.sec || (t1_ntp.sec == t2_ntp.sec && t1_ntp.frac_sec < t2_ntp.frac_sec)) {
		return -1;
	}
  
	if(t1_ntp.sec > t2_ntp.sec || (t1_ntp.sec == t2_ntp.sec && t1_ntp.frac_sec > t2_ntp.frac_sec)) {
		return 1;
	}
  
	return 0;
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
#endif
}

/*
int osc_timetag_is_immediate(char *buf){
	int offset = 0;
	if(!(strcmp(buf, "#bundle"))){
		offset = 8;
	}
	if(*(unsigned long long *)(buf + offset) == 0x100000000000000LL){
		return 1;
	}else{
		return 0;
	}
}
*/
// conversion functions

// timegm is not portable.

static void osc_timetag_setenv(const char *name, const char *value)
{
#if !(defined _WIN32) || defined HAVE_SETENV
  	setenv(name, value, 1);
#else
  	int len = strlen(name) + 1 + strlen(value) + 1;
  	char str[len];
  	sprintf(str, "%s=%s", name, value);
  	putenv(str);
#endif
}

static void osc_timetag_unsetenv(const char *name)
{
#if !(defined _WIN32) || defined HAVE_SETENV
	unsetenv(name);
#else
    	int len = strlen(name) + 2;
  	char str[len];
  	sprintf(str, "%s=", name);
  	putenv(str);
#endif
}

time_t osc_timetag_timegm (struct tm *tm)
{
	time_t ret;
	char *tz;

	tz = getenv("TZ");
#ifdef WIN_VERSION
	osc_timetag_setenv("TZ", "UTC");
#else
	osc_timetag_setenv("TZ", "");
#endif
	tzset();
	ret = mktime(tm);
	if(tz){
		osc_timetag_setenv("TZ", tz);
	}else{
		osc_timetag_unsetenv("TZ");
	}
	tzset();
	return ret;
}

int osc_timetag_toISO8601(t_osc_timetag timetag, char *s)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP  
	time_t i;
	struct tm *t;
	char s1[24];
	char s2[10];
	double d;

	t_osc_timetag_ntptime n = *((t_osc_timetag_ntptime *)&timetag);
    
	i = (time_t)osc_timetag_ntp_to_ut(n);
	d = osc_timetag_ntp_to_float(n);
	//t = gmtime(&i);
    	t = localtime(&i);

	strftime(s1, 24, "%Y-%m-%dT%H:%M:%S", t);
	sprintf(s2, "%05fZ", fmod(d, 1.0));
	return sprintf(s, "%s.%s", s1, s2+2);
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
#endif
}

void osc_timetag_fromISO8601(char *s, t_osc_timetag timetag)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP  
	struct tm t;
	float sec;
	char s1[20];
    
	// read out the fractions part
	sscanf(s, "%*d-%*d-%*dT%*d:%*d:%fZ", &sec);
    
	// null-terminate the string
	strncat(s1, s, 19);

	// parse the time
	strptime(s1, "%Y-%m-%dT%H:%M:%S", &t);
    
	t_osc_timetag_ntptime n;
	osc_timetag_ut_to_ntp(osc_timetag_timegm(&t), &n);
	n.frac_sec = (unsigned long int)(fmod(sec, 1.0) * 4294967295.0);
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
#endif
}

void osc_timetag_float_to_ntp(double d, t_osc_timetag_ntptime *n)
{
	double sec;
	double frac_sec;

	/*
	if(d > 0) {
		n->sign = 1;
	} else {
		d *= -1;
		n->sign = -1;
	}
	*/
	frac_sec = fmod(d, 1.0);
	sec = d - frac_sec;
    
	n->sec = (unsigned long int)(sec);
	n->frac_sec= (unsigned long int)(frac_sec * 4294967295.0);
	//n->type = TIME_STAMP;
}

double osc_timetag_ntp_to_float(t_osc_timetag_ntptime n)
{
	return ((double)(n.sec)) + ((double)((unsigned long int)(n.frac_sec))) / 4294967295.0;
}

t_osc_timetag osc_timetag_floatToTimetag(double d)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP
	t_osc_timetag_ntptime n;
	osc_timetag_float_to_ntp(d, &n);
	return *((t_osc_timetag *)&n);
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
#endif
}

double osc_timetag_timetagToFloat(t_osc_timetag timetag)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP
	return osc_timetag_ntp_to_float(*((t_osc_timetag_ntptime *)&timetag));
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
#endif
}

void osc_timetag_ut_to_ntp(long int ut, t_osc_timetag_ntptime *n)
{
	struct timeval tv;
	struct timezone tz;
    
	gettimeofday(&tv, &tz); // this is just to get the timezone...
    
	n->sec = (unsigned long)2208988800UL + 
		(unsigned long)ut - 
		(unsigned long)(60 * tz.tz_minuteswest) +
		(unsigned long)(tz.tz_dsttime == 1 ? 3600 : 0);

	n->frac_sec = 0;
}

long int osc_timetag_ntp_to_ut(t_osc_timetag_ntptime n)
{
	struct timeval tv;
	struct timezone tz;
    
	gettimeofday(&tv, &tz); // this is just to get the timezone...
    
	return n.sec - (unsigned long)2208988800UL + (unsigned long)(60 * tz.tz_minuteswest) - (unsigned long)(tz.tz_dsttime == 1 ? 3600 : 0);
}


t_osc_timetag osc_timetag_now(void)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP
	struct timeval tv;
	struct timezone tz;
	t_osc_timetag_ntptime n;

	gettimeofday(&tv, &tz);
    
	n.sec = (unsigned long)2208988800UL + 
		(unsigned long) tv.tv_sec - 
		(unsigned long)(60 * tz.tz_minuteswest) +
		(unsigned long)(tz.tz_dsttime == 1 ? 3600 : 0);
    
	n.frac_sec = (unsigned long)(tv.tv_usec * 4295); // 2^32-1 / 1.0e6

	return *((t_osc_timetag *)(&n));
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
#endif
}

void osc_timetag_toBytes(t_osc_timetag t, char *ptr)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP
	memcpy(ptr, (char *)&t, OSC_TIMETAG_SIZEOF_NTP);
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
#endif
}

int osc_timetag_format(t_osc_timetag t, char *buf)
{
	return osc_timetag_toISO8601(t, buf);
}

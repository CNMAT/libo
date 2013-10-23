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
#include "osc_strfmt.h"

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

void osc_timetag_ut_to_ntp(uint32_t ut, t_osc_timetag_ntptime *n);
int32_t osc_timetag_ntp_to_ut(t_osc_timetag_ntptime n);
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

	if(t1_ntp.sec > t2_ntp.sec || (t1_ntp.sec == t2_ntp.sec && t1_ntp.frac_sec >= t2_ntp.frac_sec)){
		r.sec = t1_ntp.sec - t2_ntp.sec;
		if(t1_ntp.frac_sec >= t2_ntp.frac_sec){
			r.frac_sec = t1_ntp.frac_sec - t2_ntp.frac_sec;
		}else{
			if(r.sec == 0){
				r.frac_sec = t2_ntp.frac_sec - t1_ntp.frac_sec;
			}else{
				r.sec--;
				r.frac_sec = t1_ntp.frac_sec - t2_ntp.frac_sec;
			}
		}
	}else{
		r.sec = t2_ntp.sec - t1_ntp.sec;
		if(t1_ntp.frac_sec >= t2_ntp.frac_sec){
			r.frac_sec = t1_ntp.frac_sec - t2_ntp.frac_sec;
		}else{
			r.frac_sec = t2_ntp.frac_sec - t1_ntp.frac_sec;
		}
	}

	return *((t_osc_timetag *)(&r));
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
#endif
}

int osc_timetag_compare(t_osc_timetag t1, t_osc_timetag t2)
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

int osc_timetag_isImmediate(t_osc_timetag timetag)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP
	t_osc_timetag_ntptime ntp = *((t_osc_timetag_ntptime *)&timetag);
	return (ntp.sec == 0 && ntp.frac_sec == 1);
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
#endif
}

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

//long osc_timetag_toISO8601(t_osc_timetag timetag, char *s)
long osc_timetag_toISO8601(char *buf, long n, t_osc_timetag timetag)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP  
	time_t i;
	struct tm *t;
	char s1[24];
	//char s2[10];
	double d;

	t_osc_timetag_ntptime ntptime = *((t_osc_timetag_ntptime *)&timetag);
    
	i = (time_t)osc_timetag_ntp_to_ut(ntptime);
	d = osc_timetag_ntp_to_float(ntptime);
	//t = gmtime(&i);
    	t = localtime(&i);

	strftime(s1, 24, "%Y-%m-%dT%H:%M:%S", t);
	double dm1 = fmod(d, 1.0);
	int l = osc_strfmt_float64(NULL, 0, dm1);
	char s2[l + 1];
	//sprintf(s2, "%05fZ", fmod(d, 1.0));
	osc_strfmt_float64(s2, l + 1, dm1);
	return snprintf(buf, n, "%s.%sZ", s1, s2+2);
#elif OSC_TIMETAG_FORMAT == OSC_TIMETAG_PTP
#endif
}

void osc_timetag_fromISO8601(char *s, t_osc_timetag *timetag)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP  
	struct tm t;
	memset(&t, '\0', sizeof(struct tm));
	double sec;
	//char s1[];
	//memset(s1, '\0', sizeof(s1));
    
	// read out the fractions part
	sscanf(s, "%*d-%*d-%*dT%*d:%*d:%lfZ", &sec);

	time_t now = time(NULL);
	struct tm *unsafe = localtime(&now);
	if(unsafe){
		t = *unsafe;
	} 
	// null-terminate the string
	//strncat(s1, s, OSC_TIMETAG_MAX_STRING_LENGTH - 1);

	// parse the time
	//strptime(s1, "%Y-%m-%dT%H:%M:%S", &t);
	strptime(s, "%Y-%m-%dT%H:%M:%S", &t);

	t_osc_timetag_ntptime n;
	osc_timetag_ut_to_ntp(mktime(&t), &n);

	/**************************************************
	 * The connversion from double precision float to 32-bit int obviously 
	 * results in a loss of precision that I don't think can be overcome...
	 **************************************************/
	n.frac_sec = (uint32_t)(fmod(sec, 1.0) * 4294967295.0);
	*timetag = *((t_osc_timetag *)(&n));
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
    
	n->sec = (uint32_t)(sec);
	n->frac_sec= (uint32_t)(frac_sec * 4294967295.0);
	//n->type = TIME_STAMP;
}

double osc_timetag_ntp_to_float(t_osc_timetag_ntptime n)
{
	return ((double)(n.sec)) + ((double)((uint32_t)(n.frac_sec))) / 4294967295.0;
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

void osc_timetag_ut_to_ntp(uint32_t ut, t_osc_timetag_ntptime *n)
{
	struct timeval tv;
	struct timezone tz;
    
	gettimeofday(&tv, &tz); // this is just to get the timezone...
    
	n->sec = (uint32_t)2208988800UL + 
		(uint32_t)ut - 
		(uint32_t)(60 * tz.tz_minuteswest) + 
		(uint32_t)(tz.tz_dsttime == 1 ? 3600 : 0);
	//printf("%s ut: %lu n->sec: %u minuteswest: %d dst: %d\n", __func__, ut, n->sec, 60 * tz.tz_minuteswest, tz.tz_dsttime == 1 ? 3600 : 0);

	n->frac_sec = 0;
}

int32_t osc_timetag_ntp_to_ut(t_osc_timetag_ntptime n)
{
	struct timeval tv;
	struct timezone tz;
    
	gettimeofday(&tv, &tz); // this is just to get the timezone...
    
	int32_t ut =  n.sec - (uint32_t)2208988800UL + (uint32_t)(60 * tz.tz_minuteswest) - (uint32_t)(tz.tz_dsttime == 1 ? 3600 : 0);
	//printf("%s: ut: %lu n.sec: %u minuteswest: %d dst: %d\n", __func__, ut, n.sec, 60 * tz.tz_minuteswest, tz.tz_dsttime == 1 ? 3600 : 0);
	return ut;
}


t_osc_timetag osc_timetag_now(void)
{
#if OSC_TIMETAG_FORMAT == OSC_TIMETAG_NTP
	struct timeval tv;
	struct timezone tz;
	t_osc_timetag_ntptime n;

	gettimeofday(&tv, &tz);
    
	n.sec = (uint32_t)2208988800UL + 
		(uint32_t) tv.tv_sec - 
		(uint32_t)(60 * tz.tz_minuteswest) +
		(uint32_t)(tz.tz_dsttime == 1 ? 3600 : 0);
    
	n.frac_sec = (uint32_t)(tv.tv_usec * 4295); // 2^32-1 / 1.0e6

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

//int osc_timetag_format(t_osc_timetag t, char *buf)
long osc_timetag_format(char *buf, long n, t_osc_timetag t)
{
	return osc_timetag_toISO8601(buf, n, t);
}

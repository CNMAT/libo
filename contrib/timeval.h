/*
 * timeval.h    1.0 01/12/19
 *
 * Defines gettimeofday, timeval, etc. for Win32
 *
 * By Wu Yongwei
 *
 */

#ifdef _TIMEVAL_H
#error "have timeval.h!"
#endif
#ifndef _TIMEVAL_H
#define _TIMEVAL_H

#if defined(_WIN32) || defined(_WIN64)

//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>

#ifndef __GNUC__
#define EPOCHFILETIME (116444736000000000i64)
#else
#define EPOCHFILETIME (116444736000000000LL)
#endif

/* struct timezone { */
/*     int tz_minuteswest; /\* minutes W of Greenwich *\/ */
/*     int tz_dsttime;     /\* type of dst correction *\/ */
/* }; */

__inline int gettimeofday_win(struct timeval *tv, struct timezone *tz)
{
    FILETIME        ft;
    LARGE_INTEGER   li;
    __int64         t;
    static int      tzflag;

    if (tv)
    {
        GetSystemTimePreciseAsFileTime(&ft);
        li.LowPart  = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        t  = li.QuadPart;       /* In 100-nanosecond intervals */
        t -= EPOCHFILETIME;     /* Offset to the Epoch time */
        t /= 10;                /* In microseconds */
        tv->tv_sec  = (long)(t / 1000000);
        tv->tv_usec = (long)(t % 1000000);
    }

    if (tz)
    {
        if (!tzflag)
        {
            tzset();
            tzflag++;
        }
        tz->tz_minuteswest = timezone / 60;
        tz->tz_dsttime = daylight;
    }

    return 0;
}

#else  /* _WIN32 */

#include <sys/time.h>

#endif /* _WIN32 */

#endif /* _TIMEVAL_H */

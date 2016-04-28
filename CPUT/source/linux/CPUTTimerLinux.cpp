//--------------------------------------------------------------------------------------
// Copyright 2013 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//--------------------------------------------------------------------------------------
#include "CPUTTimerLinux.h"

//
// Timer is initially not running and set to a zero state.
// Performance counter frequency is obtained.
//
CPUTTimerLinux::CPUTTimerLinux():mbCounting(false)
{
    //
    // Determine which clock is supported and collect the frequency
    //
#ifndef CPUT_OS_ANDROID
    if (clock_getres(CLOCK_MONOTONIC_RAW, &res) == 0) {
        clockType = CLOCK_MONOTONIC_RAW;
    } else 
#endif
        if (clock_getres(CLOCK_MONOTONIC, &res) == 0) {
        clockType = CLOCK_MONOTONIC;        
    } else {
        clock_getres(CLOCK_REALTIME, &res);
        clockType = CLOCK_REALTIME;        
    }
    ResetTimer();
}

//
// Reset timer to zero
//
void CPUTTimerLinux::ResetTimer()
{
    startTime.tv_sec     = 0;
    startTime.tv_nsec    = 0;
    lastMeasured.tv_sec  = 0;
    lastMeasured.tv_nsec = 0;
    mbCounting           = false;
}

//
// Starts timer and resets everything. If timer is already running,
// nothing happens.
//
void CPUTTimerLinux::StartTimer()
{
    if(!mbCounting)
    {
        ResetTimer();
        mbCounting = true;
        clock_gettime(clockType, &lastMeasured);
        startTime = lastMeasured;
    }
}

//
// If the timer is running, stops the timer and returns the time in seconds since StartTimer() was called.
//
// If the timer is not running, returns the time in seconds between the
// last start/stop pair.
//
//
double CPUTTimerLinux::StopTimer()
{
    if(mbCounting)
    {
        mbCounting = false;
        clock_gettime(clockType, &lastMeasured);
    }

    timespec timePassed = timeDifference(startTime, lastMeasured);
    return (double)(timePassed.tv_sec + ((double)timePassed.tv_nsec / 1E9));
}

//
// If the timer is running, returns the total time in seconds since StartTimer was called.
//
// If the timer is not running, returns the time in seconds between the
// last start/stop pair.
//
double CPUTTimerLinux::GetTotalTime()
{
    timespec timePassed;
    if (mbCounting) {
        timespec tempTime;
        clock_gettime(clockType, &tempTime);
        timePassed = timeDifference(startTime, tempTime);
        
    } else {
        timePassed = timeDifference(startTime, lastMeasured);
    }
    
    return (double)(timePassed.tv_sec + ((double)timePassed.tv_nsec / 1E9));
}

//
// If the timer is running, returns the total time in seconds that the timer
// has run since it was last started or elapsed time was read from. Updates last measured time.
//
// If the timer is not running, returns 0.
//
double CPUTTimerLinux::GetElapsedTime()
{
    struct timespec tempTime;
    struct timespec elapsedTime;
    elapsedTime.tv_sec  = 0;
    elapsedTime.tv_nsec = 0;

    if (mbCounting) {
        clock_gettime(clockType, &tempTime);
        elapsedTime = timeDifference(lastMeasured, tempTime);
        lastMeasured = tempTime;
    }

    return (double)(elapsedTime.tv_sec + ((double)elapsedTime.tv_nsec / 1E9));
}

//
// Calculating the difference between two times in timespec structs is a little
// complicated so we use a separate function
//
timespec CPUTTimerLinux::timeDifference(timespec startTime, timespec endTime)
{
    timespec diff;
    
    if (startTime.tv_nsec > endTime.tv_nsec) {
        diff.tv_sec  = (endTime.tv_sec - startTime.tv_sec) - 1;
        diff.tv_nsec = (endTime.tv_nsec + 1000000000) - startTime.tv_nsec;
    } else {
        diff.tv_sec = endTime.tv_sec - startTime.tv_sec;
        diff.tv_nsec = endTime.tv_nsec - startTime.tv_nsec;
    }
    
    return diff;
}
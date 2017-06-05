/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or imlied.
// See the License for the specific language governing permissions and
// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
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
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
#ifndef CPUTTIMERLINUX_H
#define CPUTTIMERLINUX_H

#include "CPUT.h"
#include "CPUTTimer.h"

// Simple QueryPerformanceCounter()-based timer class
//-----------------------------------------------------------------------------
class CPUTTimerLinux : public CPUTTimer
{
public:
    virtual void   StartTimer();
    virtual double StopTimer();
    virtual double GetTotalTime();
    virtual double GetElapsedTime();
    virtual void   ResetTimer();

	static CPUTTimerLinux* Create() {
		return new CPUTTimerLinux();
	}
private:

    CPUTTimerLinux();
    timespec  timeDifference(timespec startTime, timespec endTime); 
    bool      mbCounting;
    timespec  res;
    clockid_t clockType;
    timespec  startTime;
    timespec  lastMeasured;
};


#endif // CPUTTIMERLINUX_H
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
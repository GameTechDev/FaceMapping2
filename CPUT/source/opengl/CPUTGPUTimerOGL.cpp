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


#include "CPUTGPUTimerOGL.h"
#include "CPUT_OGL.h"

#include <algorithm>

std::vector<CPUTGPUTimerOGL*>     CPUTGPUTimerOGL::s_instances;
unsigned int                     CPUTGPUTimerOGL::s_lastFrameID = 0;
bool                             CPUTGPUTimerOGL::s_frameActive = false;

std::vector<GLuint>				CPUTGPUTimerOGL::s_timerQueries;




#ifdef CPUT_OS_WINDOWS
#define OSSleep Sleep
#else
#include <unistd.h>
#define OSSleep sleep
#endif

CPUTGPUTimerOGL::CPUTGPUTimerOGL()
{
   s_instances.push_back( this );

   memset( m_history, 0, sizeof(m_history) );
   for( int i = 0; i < (int)c_historyLength; i++ )
   {
      m_history[i].TimingResult   = -1.0;
	  m_history[i].StartData = 0;
	  m_history[i].StopData = 0;
   }

   m_historyLastIndex         = 0;
   m_active                   = false;
   m_lastTime                 = 0.0f;
   m_avgTime                  = 0.0f;
}

CPUTGPUTimerOGL::~CPUTGPUTimerOGL()
{
    std::vector<CPUTGPUTimerOGL*>::iterator it = std::find(s_instances.begin(), s_instances.end(), this);
   if( it != s_instances.end() )
   {
      s_instances.erase( it );
   }
   else
   {
      // this instance not found in the global list?
      assert( false );
   }
}



void CPUTGPUTimerOGL::OnDeviceAndContextCreated()

{
   s_lastFrameID                 = 0;
   s_frameActive                 = false;
}

void CPUTGPUTimerOGL::OnDeviceAboutToBeDestroyed()
{
   FrameFinishQueries( true );

   s_timerQueries.clear();
}

void CPUTGPUTimerOGL::FrameFinishQueries(bool forceAll)
{
   assert( !s_frameActive );

   for( int i = 0; i < (int)s_instances.size(); i++ )
      s_instances[i]->FinishQueries( forceAll );
}


GLuint  CPUTGPUTimerOGL::GetTimerQuery2()
{
	GLuint queryID;
	if( s_timerQueries.size() == 0 )
	{
	  GL_CHECK(glGenQueries(1,&queryID));
   }
   else
   {
      queryID = s_timerQueries.back();
      s_timerQueries.pop_back();
   }
   return queryID;
}


void CPUTGPUTimerOGL::OnFrameStart()
{
   FrameFinishQueries( false );

   assert( !s_frameActive );

   s_frameActive = true;
   s_lastFrameID++;
}

void CPUTGPUTimerOGL::OnFrameEnd()
{
   assert( s_frameActive );
   s_frameActive = false;
}


static bool GetQueryDataHelper( bool loopUntilDone, GLuint query, GLuint64* startTime )
{
   if( query == 0 )
      return false;

   int attempts = 0;
   do
   {
		// wait until the results are available
		GLint stopTimerAvailable = 0;

		glGetQueryObjectiv(query,GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);

		if (stopTimerAvailable == 0)
		{
			// results not available yet - should try again later
		}
		else
		{
			glGetQueryObjectui64v(query, GL_QUERY_RESULT, startTime);
			return true;
		}
		attempts++; 
		if( attempts > 1000 ) 
            OSSleep(0);
		if( attempts > 100000 ) 
        { 
            ASSERT( false, "CPUTGPUTimerOGL.cpp - Infinite loop while doing s_immediateContext->GetData()\n"); 
            return false; 
        }
   } while ( loopUntilDone);
   return false;
}

void CPUTGPUTimerOGL::FinishQueries(bool forceAll)
{
	
   assert( !m_active );

   int dataGathered = 0;
   m_avgTime = 0.0;
   m_NewRetires = 0;

   // get data from previous frames queries if available
   for( int i = 0; i < (int)c_historyLength; i++ )
   {
      int safeIndex = (i % c_historyLength);

      GPUTimerInfo & item = m_history[safeIndex];

      bool tryGather = true;

      if( item.QueryActive )
      {
         bool loopUntilDone = ((s_lastFrameID - item.FrameID) >= c_dataQueryMaxLag) || forceAll;

		GLuint64 startTime = 0, stopTime = 0;
		 

		 GetQueryDataHelper( loopUntilDone, item.StartQuery2, &startTime );
		 GetQueryDataHelper( loopUntilDone, item.StopQuery2, &stopTime );

		 if(startTime && stopTime )
		 {
			 item.TimingResult = (double)(stopTime-startTime)/1000000000.0f;
			item.QueryActive = false;
			m_NewRetires++;
			if(item.FrameID>m_lastFrameID)
			{
				m_lastFrameID = item.FrameID;
				m_lastTime = item.TimingResult;
			}
		 }
      }

	if( (!item.QueryActive) && (item.TimingResult != -1.0) )
	{
		dataGathered++;
		m_avgTime += item.TimingResult;
		if(item.StartQuery2 && item.StopQuery2)
		{
			s_timerQueries.push_back(item.StartQuery2);
			s_timerQueries.push_back(item.StopQuery2);
			item.StartQuery2 = 0;
			item.StopQuery2 = 0;
		}
	}
   }



   if( dataGathered == 0 )
      m_avgTime = 0.0f;
   else
      m_avgTime /= (double)dataGathered;
}



void CPUTGPUTimerOGL::Start()
{
	assert( s_frameActive );
	assert( !m_active );

	m_historyLastIndex = (m_historyLastIndex+1)%c_historyLength;
   
	assert( !m_history[m_historyLastIndex].QueryActive );

   m_history[m_historyLastIndex].StartQuery2     = GetTimerQuery2();
   glQueryCounter(m_history[m_historyLastIndex].StartQuery2, GL_TIMESTAMP);

	m_history[m_historyLastIndex].QueryActive    = true;
	m_history[m_historyLastIndex].FrameID        = s_lastFrameID;
	m_active = true;
    m_history[m_historyLastIndex].TimingResult   = -1.0;
}

void CPUTGPUTimerOGL::Stop()
{
   assert( s_frameActive );
   assert( m_active );
   assert( m_history[m_historyLastIndex].QueryActive );

   m_history[m_historyLastIndex].StopQuery2      = GetTimerQuery2();
   glQueryCounter(m_history[m_historyLastIndex].StopQuery2, GL_TIMESTAMP);

   m_active = false;
}




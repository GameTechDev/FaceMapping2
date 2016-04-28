
#include "CPUTGPUTimerDX11.h"

#include "CPUT.h"

#include <algorithm>

ID3D11DeviceContext *                  CPUTGPUTimerDX11::s_immediateContext           = NULL;
ID3D11Device *                         CPUTGPUTimerDX11::s_device                     = NULL;

__int64                                CPUTGPUTimerDX11::s_lastFrameID                = 0;
bool                                   CPUTGPUTimerDX11::s_frameActive                = false;

std::vector<ID3D11Query*>              CPUTGPUTimerDX11::s_disjointQueries;
std::vector<ID3D11Query*>              CPUTGPUTimerDX11::s_timerQueries;

std::vector<CPUTGPUTimerDX11*>     CPUTGPUTimerDX11::s_instances;

void CPUTGPUTimerDX11::OnDeviceAndContextCreated( ID3D11Device* device, ID3D11DeviceContext * immediateContext )
{
   assert( s_immediateContext == NULL );
   s_device                      = device;
   s_immediateContext            = immediateContext;
   s_lastFrameID                 = 0;
   s_frameActive                 = false;
}

void CPUTGPUTimerDX11::OnDeviceAboutToBeDestroyed( )
{
   FrameFinishQueries( true );

   assert( s_immediateContext != NULL );
   s_immediateContext                  = NULL;
   s_device                            = NULL;

   for( int i = 0; i < (int)s_disjointQueries.size(); i++ )
      SAFE_RELEASE( s_disjointQueries[i] );
   s_disjointQueries.clear();

   for( int i = 0; i < (int)s_timerQueries.size(); i++ )
      SAFE_RELEASE( s_timerQueries[i] );
   s_timerQueries.clear();
}

void CPUTGPUTimerDX11::FrameFinishQueries( bool forceAll )
{
   assert( s_immediateContext != NULL );
   assert( !s_frameActive );

   for( int i = 0; i < (int)s_instances.size(); i++ )
      s_instances[i]->FinishQueries( forceAll );
}

void CPUTGPUTimerDX11::OnFrameStart()
{
   FrameFinishQueries( false );

   assert( s_immediateContext != NULL );
   assert( !s_frameActive );

   s_frameActive = true;
   s_lastFrameID++;
}

void CPUTGPUTimerDX11::OnFrameEnd()
{
   assert( s_immediateContext != NULL );
   assert( s_frameActive );
   s_frameActive = false;
}

ID3D11Query * CPUTGPUTimerDX11::GetDisjointQuery()
{
   ID3D11Query * ret = NULL;
   if( s_disjointQueries.size() == 0 )
   {
      CD3D11_QUERY_DESC tqd( D3D11_QUERY_TIMESTAMP_DISJOINT, 0 );
      if( FAILED( s_device->CreateQuery( &tqd, &ret ) ) )
      {
         assert( false );
         return NULL;
      }
   }
   else
   {
      ret = s_disjointQueries.back();
      s_disjointQueries.pop_back();
   }
   return ret;
}

void CPUTGPUTimerDX11::ReleaseDisjointQuery( ID3D11Query * q )
{
   s_disjointQueries.push_back( q );
}

ID3D11Query * CPUTGPUTimerDX11::GetTimerQuery()
{
   ID3D11Query * ret = NULL;
   if( s_timerQueries.size() == 0 )
   {
      CD3D11_QUERY_DESC tqd( D3D11_QUERY_TIMESTAMP, 0 );
      if( FAILED( s_device->CreateQuery( &tqd, &ret ) ) )
      {
         assert( false );
         return NULL;
      }
   }
   else
   {
      ret = s_timerQueries.back();
      s_timerQueries.pop_back();
   }
   return ret;
}

void CPUTGPUTimerDX11::ReleaseTimerQuery( ID3D11Query * q )
{
   s_timerQueries.push_back( q );
}


CPUTGPUTimerDX11::CPUTGPUTimerDX11()
{
   s_instances.push_back( this );

   memset( m_history, 0, sizeof(m_history) );
   for( int i = 0; i < (int)c_historyLength; i++ )
      m_history[i].TimingResult   = -1.0;

   m_historyLastIndex         = 0;
   m_active                   = false;
   m_lastTime                 = 0.0f;
   m_avgTime                  = 0.0f;
}

CPUTGPUTimerDX11::~CPUTGPUTimerDX11()
{
   std::vector<CPUTGPUTimerDX11*>::iterator it = std::find( s_instances.begin(), s_instances.end(), this );
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

void CPUTGPUTimerDX11::Start()
{
   assert( s_frameActive );
   assert( s_device != NULL );
   assert( !m_active );

   m_historyLastIndex = (m_historyLastIndex+1)%c_historyLength;
   
   assert( !m_history[m_historyLastIndex].QueryActive );

   m_history[m_historyLastIndex].DisjointData.Frequency  = 0;
   m_history[m_historyLastIndex].DisjointData.Disjoint   = 1;
   m_history[m_historyLastIndex].StartData      = -1;
   m_history[m_historyLastIndex].StopData       = -1;
   m_history[m_historyLastIndex].TimingResult   = -1.0;
   m_history[m_historyLastIndex].DisjointQuery  = GetDisjointQuery();
   m_history[m_historyLastIndex].StartQuery     = GetTimerQuery();
   m_history[m_historyLastIndex].StopQuery      = GetTimerQuery();
   m_history[m_historyLastIndex].QueryActive    = true;
   m_history[m_historyLastIndex].FrameID        = s_lastFrameID;
   m_active = true;
   s_immediateContext->Begin( m_history[m_historyLastIndex].DisjointQuery );
   s_immediateContext->End( m_history[m_historyLastIndex].StartQuery );
}

void CPUTGPUTimerDX11::Stop()
{
   assert( s_device != NULL );
   assert( s_frameActive );
   assert( m_active );
   assert( m_history[m_historyLastIndex].QueryActive );

   s_immediateContext->End( m_history[m_historyLastIndex].StopQuery );
   s_immediateContext->End( m_history[m_historyLastIndex].DisjointQuery );
   m_active = false;
}

static bool GetQueryDataHelper( ID3D11DeviceContext * context, bool loopUntilDone, ID3D11Query * query, void * data, int dataSize )
{
   if( query == NULL )
      return false;

   HRESULT hr = 0;
   int attempts = 0;
   do
   {
      hr = context->GetData( query, data, dataSize, 0 ); //(loopUntilDone)?(0):(D3D11_ASYNC_GETDATA_DONOTFLUSH) );
      if( hr == S_OK )
      {
         return true;
      }
      attempts++; 
      if( attempts > 100 ) 
          Sleep(1);
      if( attempts > 1000 ) 
      {
          ASSERT(false, "CPUTGPUTimerDX11.cpp - Infinite loop while doing s_immediateContext->GetData()\n");
          return false;
      }
   } while ( loopUntilDone && (hr == S_FALSE) );
   return false;
}

void CPUTGPUTimerDX11::FinishQueries( bool forceAll )
{
   assert( !m_active );
   assert( s_device != NULL );

   int dataGathered = 0;
   m_avgTime = 0.0;

   // get data from previous frames queries if available
   for( int i = 0; i < (int)c_historyLength; i++ )
   {
      int safeIndex = (i % c_historyLength);

      GPUTimerInfo & item = m_history[safeIndex];

      bool tryGather = true;

      if( item.QueryActive )
      {
         bool loopUntilDone = ((s_lastFrameID - item.FrameID) >= c_dataQueryMaxLag) || forceAll;

         if( GetQueryDataHelper( s_immediateContext, loopUntilDone, item.DisjointQuery, &item.DisjointData, sizeof(item.DisjointData) ) )
         {
            ReleaseDisjointQuery( item.DisjointQuery );
            item.DisjointQuery = NULL;
         }
      
         if( GetQueryDataHelper( s_immediateContext, loopUntilDone, item.StartQuery, &item.StartData, sizeof(item.StartData) ) )
         {
            ReleaseTimerQuery( item.StartQuery );
            item.StartQuery = NULL;
         }

         if( GetQueryDataHelper( s_immediateContext, loopUntilDone, item.StopQuery, &item.StopData, sizeof(item.StopData) ) )
         {
            ReleaseTimerQuery( item.StopQuery );
            item.StopQuery = NULL;
         }

         if( ( item.StartQuery == NULL ) && ( item.StopQuery == NULL ) && ( item.DisjointQuery == NULL ) )
         {
            if( (item.DisjointData.Disjoint) || ((item.StartData & 0xFFFFFFFF)==0xFFFFFFFF) || ((item.StopData & 0xFFFFFFFF)==0xFFFFFFFF) )
            {
               // discard data
               item.TimingResult = -1.0;
            }
            else
            {
               item.TimingResult = (item.StopData - item.StartData) / (double)item.DisjointData.Frequency;
            }

            item.QueryActive = false;
         }
      }

      if( (!item.QueryActive) && (item.TimingResult != -1.0) )
      {
         dataGathered++;
         m_lastTime = item.TimingResult;
         m_avgTime += item.TimingResult;
      }
   }

   if( dataGathered == 0 )
      m_avgTime = 0.0f;
   else
      m_avgTime /= (double)dataGathered;
}


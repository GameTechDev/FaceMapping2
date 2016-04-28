
#ifndef __CPUTGPUTimer_h__
#define __CPUTGPUTimer_h__

#include "CPUT_DX11.h"
#include "CPUTGPUTimer.h"

#include <vector>

class CPUTGPUTimerDX11 : public CPUTGPUTimer
{
    // STATIC bit
    static const int                          c_dataQueryMaxLag       = 8;                       // in frames
    static const int                          c_historyLength         = 16+c_dataQueryMaxLag;
    static __int64                            s_lastFrameID;
    static bool                               s_frameActive;
    
    static ID3D11Device *                     s_device;
    static ID3D11DeviceContext *              s_immediateContext;
    
    static std::vector<ID3D11Query*>          s_disjointQueries;
    static std::vector<ID3D11Query*>          s_timerQueries;
    
    
    static std::vector<CPUTGPUTimerDX11*> s_instances;

protected:

                                              CPUTGPUTimerDX11();
public:

    static void                               OnDeviceAndContextCreated( ID3D11Device* device, ID3D11DeviceContext * immediateContext );
    static void                               OnDeviceAboutToBeDestroyed( );
    static void                               OnFrameStart();
    static void                               OnFrameEnd();

private:
    static void                               FrameFinishQueries( bool forceAll );
    
    ID3D11Query *                             GetDisjointQuery();
    void                                      ReleaseDisjointQuery( ID3D11Query * q );
    ID3D11Query *                             GetTimerQuery();
    void                                      ReleaseTimerQuery( ID3D11Query * q );

public:
   
    // NON-STATIC bit
    struct GPUTimerInfo
    {
       D3D11_QUERY_DATA_TIMESTAMP_DISJOINT    DisjointData;
       UINT64                                 StartData;
       UINT64                                 StopData;
       ID3D11Query *                          DisjointQuery;
       ID3D11Query *                          StartQuery;
       ID3D11Query *                          StopQuery;
       bool                                   QueryActive;
       double                                 TimingResult;
       __int64                                FrameID;
    };
   
    int                                       m_historyLastIndex;
    GPUTimerInfo                              m_history[c_historyLength];
    bool                                      m_active;
   
    double                                    m_lastTime;
    double                                    m_avgTime;
   
   
                                              ~CPUTGPUTimerDX11();
   
    void                                      Start();
    void                                      Stop();
   
    double                                    GetLastTime( ) const    { return m_lastTime; }
    double                                    GetAvgTime( ) const     { return m_avgTime; }

private:
    void                                      FinishQueries( bool forceAll );
};


class CPUTGPUProfilerDX11_AutoScopeProfile
{
    CPUTGPUTimerDX11 &                    m_profiler;
    const bool                            m_doProfile;

public:
    CPUTGPUProfilerDX11_AutoScopeProfile( CPUTGPUTimerDX11 & profiler, bool doProfile = true ) : m_profiler(profiler), m_doProfile(doProfile)       
    { 
        if( m_doProfile ) m_profiler.Start(); 
    }
    ~CPUTGPUProfilerDX11_AutoScopeProfile( )                                             
    { 
        if( m_doProfile ) m_profiler.Stop(); 
    }
};

#endif // __CPUTGPUTimer_h__

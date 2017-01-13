/*
 * Copyright 2015 Your Name <your@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <comphelper/solarmutex.hxx>

#include "haiku/salinst.hxx"
#include "haiku/salframe.hxx"
#include "haiku/saltimer.hxx"

void InitSalData()   {}
void DeInitSalData() {}
void InitSalMain()   { fprintf(stderr, "InitSalMain\n"); }

void SalAbort( const OUString& rErrorText, bool bDumpCore )
{
    OUString aError( rErrorText );
    if( aError.isEmpty() )
        aError = "Unknown application error";
    fprintf(stderr, "%s", OUStringToOString(rErrorText, osl_getThreadTextEncoding()).getStr() );

    fprintf(stderr, "SalAbort: '%s'",
         OUStringToOString(aError, RTL_TEXTENCODING_ASCII_US).getStr());
    if( bDumpCore )
        abort();
    else
        _exit(1);
}

const OUString& SalGetDesktopEnvironment()
{
    static OUString aEnv( "haiku" );
    return aEnv;
}

// This is our main entry point:
SalInstance *CreateSalInstance()
{
    fprintf(stderr, "Haiku: CreateSalInstance!\n");
    HaikuSalInstance* pInstance = new HaikuSalInstance();
    //new HaikuSalData( pInstance );
    pInstance->AcquireYieldMutex(1);
    return pInstance;
}

void DestroySalInstance( SalInstance *pInst )
{
    pInst->ReleaseYieldMutex();
    delete pInst;
}


// SalYieldMutex

SalYieldMutex::SalYieldMutex()
{
    mnCount     = 0;
    mnThreadId  = 0;
    ::comphelper::SolarMutex::setSolarMutex( this );
}

SalYieldMutex::~SalYieldMutex()
{
    ::comphelper::SolarMutex::setSolarMutex( nullptr );
}

void SalYieldMutex::acquire()
{
    m_mutex.acquire();
    mnThreadId = osl::Thread::getCurrentIdentifier();
    mnCount++;
}

void SalYieldMutex::release()
{
    assert(mnCount != 0);
    assert(mnThreadId == osl::Thread::getCurrentIdentifier());
    if ( mnCount == 1 )
    {
        mnThreadId = 0;
    }
    mnCount--;
    m_mutex.release();
}

bool SalYieldMutex::tryToAcquire()
{
    if ( m_mutex.tryToAcquire() )
    {
        mnThreadId = osl::Thread::getCurrentIdentifier();
        mnCount++;
        return true;
    }
    else
        return false;
}

comphelper::SolarMutex* HaikuSalInstance::GetYieldMutex()
{
    return mpSalYieldMutex;
}

sal_uLong HaikuSalInstance::ReleaseYieldMutex()
{
    SalYieldMutex* pYieldMutex = mpSalYieldMutex;
    if ( pYieldMutex->GetThreadId() ==
         osl::Thread::getCurrentIdentifier() )
    {
        sal_uLong nCount = pYieldMutex->GetAcquireCount();
        sal_uLong n = nCount;
        while ( n )
        {
            pYieldMutex->release();
            n--;
        }

        return nCount;
    }
    else
        return 0;
}


void HaikuSalInstance::AcquireYieldMutex( sal_uLong nCount )
{
    SalYieldMutex* pYieldMutex = mpSalYieldMutex;
    while ( nCount )
    {
        pYieldMutex->acquire();
        nCount--;
    }
}

bool HaikuSalInstance::CheckYieldMutex()
{
    bool bRet = true;

    SalYieldMutex* pYieldMutex = mpSalYieldMutex;
    if ( pYieldMutex->GetThreadId() != osl::Thread::getCurrentIdentifier() )
    {
        SAL_WARN("vcl", "CheckYieldMutex: " << pYieldMutex->GetThreadId() << "!=" << osl::Thread::getCurrentIdentifier() );
        bRet = false;
    }

    return bRet;
}

HaikuSalInstance::HaikuSalInstance()
{
    fprintf(stderr, "HaikuSalInstance::HaikuSalInstance()\n");
    mpSalYieldMutex = new SalYieldMutex();
    mpApplication = new HaikuApplication();
}

HaikuSalInstance::~HaikuSalInstance()
{
    delete mpApplication;
    delete mpSalYieldMutex;
    fprintf(stderr, "HaikuSalInstance::~HaikuSalInstance()\n");
}

SalFrame* HaikuSalInstance::CreateChildFrame( SystemParentData* pParent, SalFrameStyleFlags nStyle )
{
    fprintf(stderr, "HaikuSalInstance::CreateChildFrame()\n");
    return new HaikuSalFrame();
}

SalFrame* HaikuSalInstance::CreateFrame( SalFrame* pParent, SalFrameStyleFlags nStyle )
{
    fprintf(stderr, "HaikuSalInstance::CreateFrame()\n");
    return new HaikuSalFrame();
}

void HaikuSalInstance::DestroyFrame( SalFrame* pFrame )
{
    fprintf(stderr, "HaikuSalInstance::DestroyFrame()\n");
    delete pFrame;
}

SalObject* HaikuSalInstance::CreateObject( SalFrame* pParent, SystemWindowData* pWindowData, bool bShow )
{
    fprintf(stderr, "HaikuSalInstance::CreateObject()\n");
    return nullptr;
}

void HaikuSalInstance::DestroyObject( SalObject* pObject )
{
    fprintf(stderr, "HaikuSalInstance::DestroyObject()\n");
}

SalVirtualDevice* HaikuSalInstance::CreateVirtualDevice( SalGraphics* pGraphics,
                                                         long &nDX, long &nDY,
                                                         DeviceFormat eFormat, const SystemGraphicsData *pData )
{
    fprintf(stderr, "HaikuSalInstance::CreateVirtualDevice()\n");
    return nullptr;
}

SalInfoPrinter* HaikuSalInstance::CreateInfoPrinter( SalPrinterQueueInfo* pQueueInfo,
                                                     ImplJobSetup* pSetupData )
{
    fprintf(stderr, "HaikuSalInstance::CreateInfoPrinter()\n");
    return nullptr;
}

void HaikuSalInstance::DestroyInfoPrinter( SalInfoPrinter* pPrinter )
{
    fprintf(stderr, "HaikuSalInstance::DestroyInfoPrinter()\n");
}

SalPrinter* HaikuSalInstance::CreatePrinter( SalInfoPrinter* pInfoPrinter )
{
    fprintf(stderr, "HaikuSalInstance::CreatePrinter()\n");
    return nullptr;
}

void HaikuSalInstance::DestroyPrinter( SalPrinter* pPrinter )
{
    fprintf(stderr, "HaikuSalInstance::DestroyPrinter()\n");
}

void HaikuSalInstance::GetPrinterQueueInfo( ImplPrnQueueList* pList )
{
    fprintf(stderr, "HaikuSalInstance::GetPrinterQueueInfo()\n");
}

void HaikuSalInstance::GetPrinterQueueState( SalPrinterQueueInfo* pInfo )
{
    fprintf(stderr, "HaikuSalInstance::GetPrinterQueueState()\n");
}

void HaikuSalInstance::DeletePrinterQueueInfo( SalPrinterQueueInfo* pInfo )
{
    fprintf(stderr, "HaikuSalInstance::DeletePrinterQueueInfo()\n");
}

OUString HaikuSalInstance::GetDefaultPrinter()
{
    fprintf(stderr, "HaikuSalInstance::GetDefaultPrinter()\n");
    return OUString("default\n");
}

SalTimer* HaikuSalInstance::CreateSalTimer()
{
    fprintf(stderr, "HaikuSalInstance::CreateSalTimer()\n");
    return new HaikuSalTimer();
}

SalSystem* HaikuSalInstance::CreateSalSystem()
{
    fprintf(stderr, "HaikuSalInstance::CreateSalSystem()\n");
    return nullptr;
}

SalBitmap* HaikuSalInstance::CreateSalBitmap()
{
    fprintf(stderr, "HaikuSalInstance::CreateSalBitmap()\n");
    return nullptr;
}

SalYieldResult HaikuSalInstance::DoYield(bool bWait, bool bHandleAllCurrentEvents, sal_uLong nReleased)
{
    //fprintf(stderr, "HaikuSalInstance::DoYield()\n");
    return SalYieldResult::TIMEOUT;
}

bool HaikuSalInstance::AnyInput( VclInputFlags nType )
{
    fprintf(stderr, "HaikuSalInstance::AnyInput()\n");
    return false;
}

SalMenu* HaikuSalInstance::CreateMenu( bool bMenuBar, Menu* )
{
    fprintf(stderr, "HaikuSalInstance::CreateMenu()\n");
    return nullptr;
}

void HaikuSalInstance::DestroyMenu( SalMenu* )
{
    fprintf(stderr, "HaikuSalInstance::DestroyMenu()\n");
}

SalMenuItem* HaikuSalInstance::CreateMenuItem( const SalItemParams* pItemData )
{
    fprintf(stderr, "HaikuSalInstance::CreateMenuItem()\n");
    return nullptr;
}

void HaikuSalInstance::DestroyMenuItem( SalMenuItem* )
{
    fprintf(stderr, "HaikuSalInstance::DestroyMenuItem()\n");
}

SalSession* HaikuSalInstance::CreateSalSession()
{
    fprintf(stderr, "HaikuSalInstance::CreateSalSession()\n");
    return nullptr;
}

OpenGLContext* HaikuSalInstance::CreateOpenGLContext()
{
    fprintf(stderr, "HaikuSalInstance::CreateOpenGLContext()\n");
    return nullptr;
}

void* HaikuSalInstance::GetConnectionIdentifier( ConnectionIdentifierType& rReturnedType, int& rReturnedBytes )
{
    fprintf(stderr, "HaikuSalInstance::GetConnectionIdentifier()\n");
    return nullptr;
}

void HaikuSalInstance::AddToRecentDocumentList(const OUString& rFileUrl, const OUString& rMimeType, const OUString& rDocumentService)
{
    fprintf(stderr, "HaikuSalInstance::AddToRecentDocumentList()\n");
}

OUString HaikuSalInstance::getOSVersion()
{
    fprintf(stderr, "HaikuSalInstance::getOSVersion()");
    return OUString("beta1");
}

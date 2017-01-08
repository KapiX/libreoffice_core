/*
 * Copyright 2015 Your Name <your@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <comphelper/solarmutex.hxx>

#include "haiku/salinst.hxx"

void InitSalData()   {}
void DeInitSalData() {}
void InitSalMain()   { fprintf(stderr, "InitSalMain"); }

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
    fprintf(stderr, "Haiku: CreateSalInstance!");
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
    fprintf(stderr, "HaikuSalInstance ctor");
    mpSalYieldMutex = new SalYieldMutex();
}


HaikuSalInstance::~HaikuSalInstance()
{
    delete mpSalYieldMutex;
    fprintf(stderr, "HaikuSalInstance dtor");
}

SalFrame* HaikuSalInstance::CreateChildFrame( SystemParentData* pParent, SalFrameStyleFlags nStyle )
{
    fprintf(stderr, "CreateChildFrame()");
    return nullptr;
}

SalFrame* HaikuSalInstance::CreateFrame( SalFrame* pParent, SalFrameStyleFlags nStyle )
{
    fprintf(stderr, "CreateFrame()");
    return nullptr;
}

void HaikuSalInstance::DestroyFrame( SalFrame* pFrame )
{
    fprintf(stderr, "DestroyFrame()");
}

SalObject* HaikuSalInstance::CreateObject( SalFrame* pParent, SystemWindowData* pWindowData, bool bShow )
{
    fprintf(stderr, "CreateObject()");
    return nullptr;
}

void HaikuSalInstance::DestroyObject( SalObject* pObject )
{
    fprintf(stderr, "DestroyObject()");
}

SalVirtualDevice* HaikuSalInstance::CreateVirtualDevice( SalGraphics* pGraphics,
                                                         long &nDX, long &nDY,
                                                         DeviceFormat eFormat, const SystemGraphicsData *pData )
{
    fprintf(stderr, "CreateVirtualDevice()");
    return nullptr;
}

SalInfoPrinter* HaikuSalInstance::CreateInfoPrinter( SalPrinterQueueInfo* pQueueInfo,
                                                     ImplJobSetup* pSetupData )
{
    fprintf(stderr, "CreateInfoPrinter()");
    return nullptr;
}

void HaikuSalInstance::DestroyInfoPrinter( SalInfoPrinter* pPrinter )
{
    fprintf(stderr, "DestroyInfoPrinter()");
}

SalPrinter* HaikuSalInstance::CreatePrinter( SalInfoPrinter* pInfoPrinter )
{
    fprintf(stderr, "CreatePrinter()");
    return nullptr;
}

void HaikuSalInstance::DestroyPrinter( SalPrinter* pPrinter )
{
    fprintf(stderr, "DestroyPrinter()");
}

void HaikuSalInstance::GetPrinterQueueInfo( ImplPrnQueueList* pList )
{
    fprintf(stderr, "GetPrinterQueueInfo()");
}

void HaikuSalInstance::GetPrinterQueueState( SalPrinterQueueInfo* pInfo )
{
    fprintf(stderr, "GetPrinterQueueState()");
}

void HaikuSalInstance::DeletePrinterQueueInfo( SalPrinterQueueInfo* pInfo )
{
    fprintf(stderr, "DeletePrinterQueueInfo()");
}

OUString HaikuSalInstance::GetDefaultPrinter()
{
    fprintf(stderr, "GetDefaultPrinter()");
    return OUString("default");
}

SalTimer* HaikuSalInstance::CreateSalTimer()
{
    fprintf(stderr, "CreateSalTimer()");
    return nullptr;
}

SalSystem* HaikuSalInstance::CreateSalSystem()
{
    fprintf(stderr, "CreateSalSystem()");
    return nullptr;
}

SalBitmap* HaikuSalInstance::CreateSalBitmap()
{
    fprintf(stderr, "CreateSalBitmap()");
    return nullptr;
}

SalYieldResult HaikuSalInstance::DoYield(bool bWait, bool bHandleAllCurrentEvents, sal_uLong nReleased)
{
    fprintf(stderr, "DoYield()");
    return SalYieldResult::EVENT;
}

bool HaikuSalInstance::AnyInput( VclInputFlags nType )
{
    fprintf(stderr, "AnyInput()");
    return false;
}

SalMenu* HaikuSalInstance::CreateMenu( bool bMenuBar, Menu* )
{
    fprintf(stderr, "CreateMenu()");
    return nullptr;
}

void HaikuSalInstance::DestroyMenu( SalMenu* )
{
    fprintf(stderr, "DestroyMenu()");
}

SalMenuItem* HaikuSalInstance::CreateMenuItem( const SalItemParams* pItemData )
{
    fprintf(stderr, "CreateMenuItem()");
    return nullptr;
}

void HaikuSalInstance::DestroyMenuItem( SalMenuItem* )
{
    fprintf(stderr, "DestroyMenuItem()");
}

SalSession* HaikuSalInstance::CreateSalSession()
{
    fprintf(stderr, "CreateSalSession()");
    return nullptr;
}

OpenGLContext* HaikuSalInstance::CreateOpenGLContext()
{
    fprintf(stderr, "CreateOpenGLContext()");
    return nullptr;
}

void* HaikuSalInstance::GetConnectionIdentifier( ConnectionIdentifierType& rReturnedType, int& rReturnedBytes )
{
    fprintf(stderr, "GetConnectionIdentifier()");
    return nullptr;
}

void HaikuSalInstance::AddToRecentDocumentList(const OUString& rFileUrl, const OUString& rMimeType, const OUString& rDocumentService)
{
    fprintf(stderr, "AddToRecentDocumentList()");
}

OUString HaikuSalInstance::getOSVersion()
{
    fprintf(stderr, "getOSVersion()");
    return OUString("beta1");
}

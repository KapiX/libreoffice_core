/*
 * Copyright 2015 Your Name <your@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <comphelper/solarmutex.hxx>

#include "haiku/saldata.hxx"
#include "haiku/salinst.hxx"
#include "haiku/salgdi.hxx"
#include "haiku/salframe.hxx"
#include "haiku/saltimer.hxx"
#include "haiku/salsys.hxx"
#include <headless/svpbmp.hxx>
#include <headless/svpvd.hxx>

void InitSalData() {}
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
    //if( bDumpCore )
        debugger("BOOOOOOM");
    /*else
        _exit(1);*/
}

SalData::SalData() :
    m_pInstance( nullptr ),
    m_pPIManager( nullptr )
{
}

SalData::~SalData()
{
}

const OUString& SalGetDesktopEnvironment()
{
    static OUString aEnv( "haiku" );
    return aEnv;
}

// This is our main entry point:
SalInstance *CreateSalInstance()
{
    HaikuSalInstance* pInst = new HaikuSalInstance( new SvpSalYieldMutex() );
    new HaikuSalData( pInst );
    HaikuSalData* pSalData = GetHaikuSalData();

    pSalData->mpFirstInstance = pInst;
    pInst->AcquireYieldMutex();
    return pInst;
}

void DestroySalInstance( SalInstance *pInst )
{
    HaikuSalData* pSalData = GetHaikuSalData();
    pInst->ReleaseYieldMutexAll();

    if ( pSalData->mpFirstInstance == pInst )
        pSalData->mpFirstInstance = nullptr;

    delete pInst;
}

bool ImplSalYieldMutexTryToAcquire()
{
    HaikuSalInstance* pInst = GetHaikuSalData()->mpFirstInstance;
    if ( pInst )
        return pInst->GetYieldMutex()->tryToAcquire();
    else
        return false;
}

void ImplSalYieldMutexRelease()
{
    HaikuSalInstance* pInst = GetHaikuSalData()->mpFirstInstance;
    if ( pInst )
        pInst->GetYieldMutex()->release();
}

HaikuSalInstance::HaikuSalInstance( SalYieldMutex* pMutex )
    : SvpSalInstance( pMutex )
    , maUserEventListMutex()
{
    mpApplication = new HaikuApplication();
    mbWaitingYield = false;
    maWaitingYieldCond = osl_createCondition();
}

HaikuSalInstance::~HaikuSalInstance()
{
    osl_destroyCondition( maWaitingYieldCond );
    delete mpApplication;
}

void HaikuSalInstance::PostUserEvent( HaikuSalFrame* pFrame, SalEvent nType, void* pData )
{
    {
        osl::MutexGuard g( maUserEventListMutex );
        maUserEvents.push_back( SalUserEvent( pFrame, pData, nType ) );
    }
}

SalFrame* HaikuSalInstance::CreateChildFrame( SystemParentData* pParent, SalFrameStyleFlags nStyle )
{
    fprintf(stderr, "HaikuSalInstance::CreateChildFrame()\n");
    return new HaikuSalFrame(nullptr, nStyle);
}

SalFrame* HaikuSalInstance::CreateFrame( SalFrame* pParent, SalFrameStyleFlags nStyle )
{
    fprintf(stderr, "HaikuSalInstance::CreateFrame()\n");
    return new HaikuSalFrame(static_cast<HaikuSalFrame*>(pParent), nStyle);
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
    (void) pData;
    SvpSalGraphics *pSvpSalGraphics = dynamic_cast<SvpSalGraphics*>(pGraphics);
    assert(pSvpSalGraphics);
    SvpSalVirtualDevice* pNew = new SvpSalVirtualDevice(eFormat, pSvpSalGraphics->getScale());
    pNew->SetSize( nDX, nDY );
    return pNew;
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
    return new HaikuSalSystem();
}

SalBitmap* HaikuSalInstance::CreateSalBitmap()
{
    return SvpSalInstance::CreateSalBitmap();
}

bool HaikuSalInstance::DoYield(bool bWait, bool bHandleAllCurrentEvents)
{
    return SvpSalInstance::DoYield(bWait, bHandleAllCurrentEvents);
}

bool HaikuSalInstance::AnyInput( VclInputFlags nType )
{
    fprintf(stderr, "HaikuSalInstance::AnyInput()\n");
    /*if( nType & VclInputFlags::APPEVENT )
    {
        if( ! aAppEventList.empty() )
            return true;
        if( nType == VclInputFlags::APPEVENT )
            return false;
    }*/
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

OUString HaikuSalInstance::GetConnectionIdentifier()
{
    fprintf(stderr, "HaikuSalInstance::GetConnectionIdentifier()\n");
    return OUString();
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

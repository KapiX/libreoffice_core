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

void SalAbort(const OUString& rErrorText, bool bDumpCore)
{
    OUString aError(rErrorText);
    if (aError.isEmpty())
        aError = "Unknown application error";
    fprintf(stderr, "%s", OUStringToOString(rErrorText, osl_getThreadTextEncoding()).getStr());

    fprintf(stderr, "SalAbort: '%s'",
            OUStringToOString(aError, RTL_TEXTENCODING_ASCII_US).getStr());
    //if( bDumpCore )
    debugger("BOOOOOOM");
    /*else
        _exit(1);*/
}

SalData::SalData()
    : m_pInstance(nullptr)
    , m_pPIManager(nullptr)
{
}

SalData::~SalData() {}

HaikuSalInstance::HaikuSalInstance()
    : SalGenericInstance(o3tl::make_unique<SalYieldMutex>())
{
    mpApplication = new HaikuApplication();
}

HaikuSalInstance::~HaikuSalInstance() { delete mpApplication; }

void HaikuSalInstance::PostUserEvent(HaikuSalFrame* pFrame, SalEvent nType, void* pData)
{
    {
        //osl::MutexGuard g( maUserEventListMutex );
        //maUserEvents.push_back( SalUserEvent( pFrame, pData, nType ) );
    }
}

SalFrame* HaikuSalInstance::CreateChildFrame(SystemParentData* pParent, SalFrameStyleFlags nStyle)
{
    fprintf(stderr, "HaikuSalInstance::CreateChildFrame()\n");
    return new HaikuSalFrame(nullptr, nStyle);
}

SalFrame* HaikuSalInstance::CreateFrame(SalFrame* pParent, SalFrameStyleFlags nStyle)
{
    fprintf(stderr, "HaikuSalInstance::CreateFrame()\n");
    return new HaikuSalFrame(static_cast<HaikuSalFrame*>(pParent), nStyle);
}

void HaikuSalInstance::DestroyFrame(SalFrame* pFrame)
{
    fprintf(stderr, "HaikuSalInstance::DestroyFrame()\n");
    delete pFrame;
}

SalObject* HaikuSalInstance::CreateObject(SalFrame* pParent, SystemWindowData* pWindowData,
                                          bool bShow)
{
    fprintf(stderr, "HaikuSalInstance::CreateObject()\n");
    return nullptr;
}

void HaikuSalInstance::DestroyObject(SalObject* pObject)
{
    fprintf(stderr, "HaikuSalInstance::DestroyObject()\n");
}

std::unique_ptr<SalVirtualDevice>
HaikuSalInstance::CreateVirtualDevice(SalGraphics* pGraphics, long& nDX, long& nDY,
                                      DeviceFormat eFormat, const SystemGraphicsData* pData)
{
    fprintf(stderr, "HaikuSalInstance::CreateVirtualDevice()\n");
    (void)pData;
    SvpSalGraphics* pSvpSalGraphics = dynamic_cast<SvpSalGraphics*>(pGraphics);
    assert(pSvpSalGraphics);
    std::unique_ptr<SalVirtualDevice> pVD(
        new SvpSalVirtualDevice(eFormat, pSvpSalGraphics->getSurface()));
    pVD->SetSize(nDX, nDY);
    return pVD;
}

SalInfoPrinter* HaikuSalInstance::CreateInfoPrinter(SalPrinterQueueInfo* pQueueInfo,
                                                    ImplJobSetup* pSetupData)
{
    fprintf(stderr, "HaikuSalInstance::CreateInfoPrinter()\n");
    return nullptr;
}

void HaikuSalInstance::DestroyInfoPrinter(SalInfoPrinter* pPrinter)
{
    fprintf(stderr, "HaikuSalInstance::DestroyInfoPrinter()\n");
}

std::unique_ptr<SalPrinter> HaikuSalInstance::CreatePrinter(SalInfoPrinter* pInfoPrinter)
{
    fprintf(stderr, "HaikuSalInstance::CreatePrinter()\n");
    return nullptr;
}

void HaikuSalInstance::GetPrinterQueueInfo(ImplPrnQueueList* pList)
{
    fprintf(stderr, "HaikuSalInstance::GetPrinterQueueInfo()\n");
}

void HaikuSalInstance::GetPrinterQueueState(SalPrinterQueueInfo* pInfo)
{
    fprintf(stderr, "HaikuSalInstance::GetPrinterQueueState()\n");
}

OUString HaikuSalInstance::GetDefaultPrinter()
{
    fprintf(stderr, "HaikuSalInstance::GetDefaultPrinter()\n");
    return OUString("default\n");
}

void HaikuSalInstance::PostPrintersChanged() {}

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

std::shared_ptr<SalBitmap> HaikuSalInstance::CreateSalBitmap()
{
    return std::make_shared<SvpSalBitmap>();
}

bool HaikuSalInstance::IsMainThread() const { return true; }

void HaikuSalInstance::TriggerUserEventProcessing() {}

void HaikuSalInstance::ProcessEvent(SalUserEvent aEvent)
{
    aEvent.m_pFrame->CallCallback(aEvent.m_nEvent, aEvent.m_pData);
}

bool HaikuSalInstance::DoYield(bool bWait, bool bHandleAllCurrentEvents) { return false; }

bool HaikuSalInstance::AnyInput(VclInputFlags nType)
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

//std::unique_ptr<SalMenu> HaikuSalInstance::CreateMenu( bool bMenuBar, Menu* )
//{
//    fprintf(stderr, "HaikuSalInstance::CreateMenu()\n");
//    return std::unique_ptr<SalMenu>(nullptr);
//}
//
//std::unique_ptr<SalMenuItem> HaikuSalInstance::CreateMenuItem( const SalItemParams& pItemData )
//{
//    fprintf(stderr, "HaikuSalInstance::CreateMenuItem()\n");
//    return std::unique_ptr<SalMenuItem>(nullptr);
//}

//std::unique_ptr<SalSession> HaikuSalInstance::CreateSalSession()
//{
//    fprintf(stderr, "HaikuSalInstance::CreateSalSession()\n");
//    return std::unique_ptr<SalSession>();
//}

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

void HaikuSalInstance::AddToRecentDocumentList(const OUString& rFileUrl, const OUString& rMimeType,
                                               const OUString& rDocumentService)
{
    fprintf(stderr, "HaikuSalInstance::AddToRecentDocumentList()\n");
}

GenPspGraphics* HaikuSalInstance::CreatePrintGraphics() {}

OUString HaikuSalInstance::getOSVersion()
{
    fprintf(stderr, "HaikuSalInstance::getOSVersion()");
    return OUString("beta1");
}

extern "C" {
VCLPLUG_HAIKU_PUBLIC SalInstance* create_SalInstance()
{
    HaikuSalInstance* pInstance = new HaikuSalInstance();
    new HaikuSalData(pInstance);

    return pInstance;
}
}

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef INCLUDED_VCL_INC_HAIKU_SALINST_HXX
#define INCLUDED_VCL_INC_HAIKU_SALINST_HXX

#include <list>

#include <vclpluginapi.h>
#include <unx/geninst.h>
#include <salusereventlist.hxx>

#include <osl/conditn.hxx>

#include <svsys.h>

class HaikuApplication : public BApplication
{
public:
    HaikuApplication()
        : BApplication("application/x-vnd.TDF.LibreOffice")
    {
    }
    ~HaikuApplication() {}
};

class HaikuSalFrame;
enum class SalEvent;

class VCLPLUG_HAIKU_PUBLIC HaikuSalInstance : public SalGenericInstance, public SalUserEventList
{
private:
    HaikuApplication* mpApplication;
    osl::Condition m_aWaitingYieldCond;

public:
    explicit HaikuSalInstance();
    virtual ~HaikuSalInstance() override;

    virtual SalFrame* CreateChildFrame(SystemParentData* pParent,
                                       SalFrameStyleFlags nStyle) override;
    virtual SalFrame* CreateFrame(SalFrame* pParent, SalFrameStyleFlags nStyle) override;
    virtual void DestroyFrame(SalFrame* pFrame) override;
    virtual SalObject* CreateObject(SalFrame* pParent, SystemWindowData* pWindowData,
                                    bool bShow) override;
    virtual void DestroyObject(SalObject* pObject) override;
    virtual std::unique_ptr<SalVirtualDevice>
    CreateVirtualDevice(SalGraphics* pGraphics, long& nDX, long& nDY, DeviceFormat eFormat,
                        const SystemGraphicsData* pData = nullptr) override;
    virtual SalInfoPrinter* CreateInfoPrinter(SalPrinterQueueInfo* pQueueInfo,
                                              ImplJobSetup* pSetupData) override;
    virtual void DestroyInfoPrinter(SalInfoPrinter* pPrinter) override;
    virtual std::unique_ptr<SalPrinter> CreatePrinter(SalInfoPrinter* pInfoPrinter) override;
    virtual void GetPrinterQueueInfo(ImplPrnQueueList* pList) override;
    virtual void GetPrinterQueueState(SalPrinterQueueInfo* pInfo) override;
    virtual void PostPrintersChanged() override;
    virtual OUString GetDefaultPrinter() override;
    virtual SalTimer* CreateSalTimer() override;
    virtual SalSystem* CreateSalSystem() override;
    virtual std::shared_ptr<SalBitmap> CreateSalBitmap() override;

    virtual bool IsMainThread() const;
    virtual void TriggerUserEventProcessing() override;
    virtual void ProcessEvent(SalUserEvent aEvent) override;
    virtual bool DoYield(bool bWait, bool bHandleAllCurrentEvents) override;
    virtual bool AnyInput(VclInputFlags nType) override;
    //    virtual std::unique_ptr<SalMenu>            CreateMenu( bool bMenuBar, Menu* ) override;
    //    virtual std::unique_ptr<SalMenuItem>        CreateMenuItem( const SalItemParams&) override;
    //    virtual std::unique_ptr<SalSession>         CreateSalSession() override;
    virtual OpenGLContext* CreateOpenGLContext() override;
    virtual OUString GetConnectionIdentifier() override;
    virtual void AddToRecentDocumentList(const OUString& rFileUrl, const OUString& rMimeType,
                                         const OUString& rDocumentService) override;

    virtual GenPspGraphics* CreatePrintGraphics() override;

    virtual OUString getOSVersion() override;

    void PostUserEvent(HaikuSalFrame* pFrame, SalEvent nType, void* pData);
};

#endif // INCLUDED_VCL_INC_HAIKU_SALINST_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

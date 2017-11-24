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

#include <svsys.h>

#include <vcl/sysdata.hxx>

#include <haiku/saldata.hxx>
#include <haiku/salinst.hxx>
#include <haiku/salframe.hxx>
#include <haiku/salgdi.hxx>
#include <haiku/salvd.hxx>

HaikuSalVirtualDevice::HaikuSalVirtualDevice(HaikuSalGraphics *pGraphics)
{
    mbGraphics = false;
    mpGraphics = nullptr;
    mbForeign = false;
    if(pGraphics) {
        if(pGraphics->getView()->Window()->LockLooper()) {
            BView* view = new BView(pGraphics->getView()->Bounds(), "drawing",
                B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
            mpBmp = new BBitmap(pGraphics->getView()->Bounds(), B_RGB32, true);
            pGraphics->getView()->Window()->UnlockLooper();
            mpBmp->AddChild(view);
            mpGraphics = new HaikuSalGraphics(view, this);
        }
    } else {
        mbForeign = true;
        mpGraphics = new HaikuSalGraphics(pGraphics->getView(), this);
    }
    fprintf(stderr, "HaikuSalVirtualDevice::HaikuSalVirtualDevice()\n");
}

HaikuSalVirtualDevice::~HaikuSalVirtualDevice()
{
    if(mpGraphics) {
        if(mbForeign == false) {
            BWindow* window = mpGraphics->getView()->Window();
            if(window) {
                if(window->LockLooper()) {
                    mpGraphics->getView()->RemoveSelf();
                    window->UnlockLooper();
                }
            }
            delete mpGraphics->getView();
            delete mpBmp;
        }
        delete mpGraphics;
    }
    fprintf(stderr, "HaikuSalVirtualDevice::~HaikuSalVirtualDevice()\n");
}

SalGraphics* HaikuSalVirtualDevice::AcquireGraphics()
{
    fprintf(stderr, "HaikuSalVirtualDevice::AcquireGraphics()\n");

    if ( mbGraphics )
        return nullptr;

    if ( mpGraphics )
        mbGraphics = true;

    return mpGraphics;
}

void HaikuSalVirtualDevice::ReleaseGraphics( SalGraphics* pGraphics )
{
    fprintf(stderr, "HaikuSalVirtualDevice::ReleaseGraphics()\n");
    mbGraphics = false;
}

bool HaikuSalVirtualDevice::SetSize( long nNewDX, long nNewDY )
{
    fprintf(stderr, "HaikuSalVirtualDevice::SetSize(%d, %d)\n", nNewDX, nNewDY);
    if(mpGraphics) {
        if(mpGraphics->getView()->Window()->LockLooper()) {
            mpGraphics->getView()->ResizeTo(nNewDX, nNewDY);
            mpGraphics->getView()->Window()->UnlockLooper();
        }
    }
    return true;
}

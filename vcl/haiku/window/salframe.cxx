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

#include "osl/thread.h"

#include <haiku/saldata.hxx>
#include <haiku/salframe.hxx>
#include <haiku/salgdi.hxx>
#include <haiku/salinst.hxx>

#include <cstdio>

HaikuView::HaikuView(BRect rect, HaikuSalFrame* pFrame)
    :
    BView(rect, nullptr, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE),
    mpFrame(pFrame)
{
}

HaikuView::~HaikuView()
{
}

void HaikuView::Draw(BRect updateRect)
{
    SetViewColor(B_TRANSPARENT_COLOR);
    SalPaintEvent aPEvt(0, 0, Bounds().Width(), Bounds().Height());
    aPEvt.mbImmediateUpdate = false;
    mpFrame->CallCallback(SalEvent::Paint, &aPEvt);
    //GetSalData()->mpFirstInstance->PostUserEvent(mpFrame, SalEvent::Paint, &aPEvt);
}

void HaikuView::MouseMoved(BPoint point, uint32 transit, const BMessage* message)
{
    SalMouseEvent   aMouseEvt;
    aMouseEvt.mnX       = point.x;
    aMouseEvt.mnY       = point.y;
    aMouseEvt.mnCode    = 0;
    aMouseEvt.mnButton = 0;
    GetSalData()->mpFirstInstance->PostUserEvent(mpFrame, SalEvent::MouseMove, &aMouseEvt);
}

void HaikuView::MouseDown(BPoint point)
{
    SalMouseEvent   aMouseEvt;
    aMouseEvt.mnButton = MOUSE_LEFT;
    aMouseEvt.mnX       = point.x;
    aMouseEvt.mnY       = point.y;
    aMouseEvt.mnCode    = 0;
    GetSalData()->mpFirstInstance->PostUserEvent(mpFrame, SalEvent::MouseButtonDown, &aMouseEvt);

}

void HaikuView::MouseUp(BPoint point)
{
    SalMouseEvent   aMouseEvt;
    aMouseEvt.mnButton = MOUSE_LEFT;
    aMouseEvt.mnX       = point.x;
    aMouseEvt.mnY       = point.y;
    aMouseEvt.mnCode    = 0;
    GetSalData()->mpFirstInstance->PostUserEvent(mpFrame, SalEvent::MouseButtonUp, &aMouseEvt);
}

void HaikuView::FrameResized(float width, float height)
{
    GetSalData()->mpFirstInstance->PostUserEvent(mpFrame, SalEvent::Resize, nullptr);
}

void HaikuWindow::MessageReceived(BMessage* message)
{
    switch(message->what) {
        default: {
            BWindow::MessageReceived(message);
        } break;
    }
}

HaikuSalFrame::HaikuSalFrame(SalFrameStyleFlags nStyle)
{
    fprintf(stderr, "HaikuSalFrame::HaikuSalFrame()\n");
    mpWindow = new HaikuWindow(this);
    if(nStyle & SalFrameStyleFlags::TOOLTIP) {
        mpWindow->SetLook(B_NO_BORDER_WINDOW_LOOK);
        mpWindow->SetFlags(mpWindow->Flags() & B_AVOID_FOCUS);
    }

//    if(nStyle & SalFrameStyleFlags::MOVEABLE) {
//        mpWindow->SetLook(B_NO_BORDER_WINDOW_LOOK);
//        mpWindow->SetFlags(mpWindow->Flags() & B_AVOID_FOCUS);
//    }
}

HaikuSalFrame::~HaikuSalFrame()
{
    fprintf(stderr, "HaikuSalFrame::~HaikuSalFrame()\n");
    if(mpWindow->LockLooper()) {
        mpWindow->Quit();
    }
}

SalGraphics* HaikuSalFrame::AcquireGraphics()
{
    fprintf(stderr, "HaikuSalFrame::AcquireGraphics()\n");
    HaikuView* defView = new HaikuView(mpWindow->Bounds(), this);
    mpWindow->AddChild(defView);
    return new HaikuSalGraphics(defView);
}

void HaikuSalFrame::ReleaseGraphics( SalGraphics* pGraphics )
{
    fprintf(stderr, "HaikuSalFrame::ReleaseGraphics()\n");
    delete pGraphics;
}

bool HaikuSalFrame::PostEvent(ImplSVEvent* pData)
{
    GetSalData()->mpFirstInstance->PostUserEvent( this, SalEvent::UserEvent, pData );
    return true;
}

void HaikuSalFrame::SetTitle( const OUString& rTitle )
{
    OString aTitle(OUStringToOString(rTitle, osl_getThreadTextEncoding()));
    mpWindow->SetTitle(aTitle.getStr());
}

void HaikuSalFrame::SetIcon( sal_uInt16 nIcon )
{
    fprintf(stderr, "HaikuSalFrame::SetIcon()\n");
}

void HaikuSalFrame::SetMenu( SalMenu* pSalMenu )
{
    fprintf(stderr, "HaikuSalFrame::SetMenu()\n");
}

void HaikuSalFrame::DrawMenuBar()
{
    fprintf(stderr, "HaikuSalFrame::DrawMenuBar()\n");
}

void HaikuSalFrame::SetExtendedFrameStyle( SalExtStyle )
{
    fprintf(stderr, "HaikuSalFrame::SetExtendedFrameStyle()\n");
}

void HaikuSalFrame::Show( bool bVisible, bool bNoActivate )
{
    mpWindow->Show();
}

void HaikuSalFrame::SetMinClientSize( long nWidth, long nHeight )
{
    fprintf(stderr, "HaikuSalFrame::SetMinClientSize()\n");
}

void HaikuSalFrame::SetMaxClientSize( long nWidth, long nHeight )
{
    fprintf(stderr, "HaikuSalFrame::SetMaxClientSize()\n");
}

void HaikuSalFrame::SetPosSize( long nX, long nY, long nWidth, long nHeight,
                                sal_uInt16 nFlags )
{
    fprintf(stderr, "HaikuSalFrame::SetPosSize(%d, %d, %d, %d, %d)\n", nX, nY, nWidth, nHeight, nFlags);
    SalEvent nEvent = SalEvent::NONE;

    BRect aWindowRect = mpWindow->Frame();

    if(nFlags & (SAL_FRAME_POSSIZE_X | SAL_FRAME_POSSIZE_Y)) {
        nEvent = SalEvent::Move;
        mpWindow->MoveTo(nX, nY);
    }
    /*if(nFlags & (SAL_FRAME_POSSIZE_WIDTH | SAL_FRAME_POSSIZE_HEIGHT)) {
        nEvent = (nEvent == SalEvent::Move) ? SalEvent::MoveResize : SalEvent::Resize;
        mpWindow->ResizeTo(nWidth, nHeight);
    }*/

    if(nEvent != SalEvent::NONE)
        CallCallback(nEvent, nullptr);
//    if ( !(nFlags & SAL_FRAME_POSSIZE_X) )
//        aWindowRect.left = nX;
//    if ( !(nFlags & SAL_FRAME_POSSIZE_Y) )
//        aWindowRect.top = nY;
//    if ( !(nFlags & SAL_FRAME_POSSIZE_WIDTH) )
//        aWindowRect.right = aWindowRect.left + nWidth;
//    if ( !(nFlags & SAL_FRAME_POSSIZE_HEIGHT) )
//        aWindowRect.right = aWindowRect.top + nHeight;
}

SalFrame* HaikuSalFrame::GetParent() const
{
    fprintf(stderr, "HaikuSalFrame::GetParent()\n");
    return nullptr;
}

void HaikuSalFrame::GetWorkArea( Rectangle &rRect )
{
    fprintf(stderr, "HaikuSalFrame::GetWorkArea()\n");
}

void HaikuSalFrame::GetClientSize( long& rWidth, long& rHeight )
{
    rWidth = mpWindow->Bounds().Width();
    rHeight = mpWindow->Bounds().Height();
}

void HaikuSalFrame::SetWindowState( const SalFrameState* pState )
{
    fprintf(stderr, "HaikuSalFrame::SetWindowState()\n");
}

bool HaikuSalFrame::GetWindowState( SalFrameState* pState )
{
    fprintf(stderr, "HaikuSalFrame::GetWindowState()\n");
    return false;
}

void HaikuSalFrame::ShowFullScreen( bool bFullScreen, sal_Int32 nDisplay )
{
    fprintf(stderr, "HaikuSalFrame::ShowFullScreen()\n");
}

void HaikuSalFrame::StartPresentation( bool bStart )
{
    fprintf(stderr, "HaikuSalFrame::StartPresentation()\n");
}

void HaikuSalFrame::SetAlwaysOnTop( bool bOnTop )
{
    fprintf(stderr, "HaikuSalFrame::SetAlwaysOnTop()\n");
}

void HaikuSalFrame::ToTop( SalFrameToTop nFlags )
{
    fprintf(stderr, "HaikuSalFrame::ToTop()\n");
}

void HaikuSalFrame::SetPointer( PointerStyle ePointerStyle )
{
    //fprintf(stderr, "HaikuSalFrame::SetPointer()\n");
}

void HaikuSalFrame::CaptureMouse( bool bCapture )
{
    fprintf(stderr, "HaikuSalFrame::CaptureMouse()\n");
}

void HaikuSalFrame::SetPointerPos( long nX, long nY )
{
    fprintf(stderr, "HaikuSalFrame::SetPointerPos()\n");
}

void HaikuSalFrame::Flush()
{
    fprintf(stderr, "HaikuSalFrame::Flush()\n");
    mpWindow->Flush();
}

void HaikuSalFrame::SetInputContext( SalInputContext* pContext )
{
    fprintf(stderr, "HaikuSalFrame::SetInputContext()\n");
}

void HaikuSalFrame::EndExtTextInput( EndExtTextInputFlags nFlags )
{
    fprintf(stderr, "HaikuSalFrame::EndExtTextInput()\n");
}

OUString HaikuSalFrame::GetKeyName( sal_uInt16 nKeyCode )
{
    fprintf(stderr, "HaikuSalFrame::GetKeyName()\n");
    return OUString();
}

bool HaikuSalFrame::MapUnicodeToKeyCode( sal_Unicode aUnicode, LanguageType aLangType, vcl::KeyCode& rKeyCode )
{
    fprintf(stderr, "HaikuSalFrame::MapUnicodeToKeyCode()\n");
    return false;
}

LanguageType HaikuSalFrame::GetInputLanguage()
{
    fprintf(stderr, "HaikuSalFrame::GetInputLanguage()\n");
    return LANGUAGE_ENGLISH;
}

void HaikuSalFrame::UpdateSettings( AllSettings& rSettings )
{
    fprintf(stderr, "HaikuSalFrame::UpdateSettings()\n");
}

void HaikuSalFrame::Beep()
{
    fprintf(stderr, "HaikuSalFrame::Beep()\n");
}

const SystemEnvData* HaikuSalFrame::GetSystemData() const
{
    fprintf(stderr, "HaikuSalFrame::GetSystemData()\n");
    return nullptr;
}

SalFrame::SalPointerState HaikuSalFrame::GetPointerState()
{
    fprintf(stderr, "HaikuSalFrame::GetPointerState()\n");
    SalPointerState aState;
    aState.mnState = 0;
    aState.maPos = Point(0, 0);
    return aState;
}

KeyIndicatorState HaikuSalFrame::GetIndicatorState()
{
    fprintf(stderr, "HaikuSalFrame::GetIndicatorState()\n");
    KeyIndicatorState aState = KeyIndicatorState::NONE;
    return aState;
}

void HaikuSalFrame::SimulateKeyPress( sal_uInt16 nKeyCode )
{
    fprintf(stderr, "HaikuSalFrame::SimulateKeyPress()\n");
}

void HaikuSalFrame::SetParent( SalFrame* pNewParent )
{
    fprintf(stderr, "HaikuSalFrame::SetParent()\n");
}

bool HaikuSalFrame::SetPluginParent( SystemParentData* pNewParent )
{
    fprintf(stderr, "HaikuSalFrame::SetPluginParent()\n");
}

void HaikuSalFrame::SetScreenNumber( unsigned int nNewScreen )
{
    fprintf(stderr, "HaikuSalFrame::SetScreenNumber()\n");
}

void HaikuSalFrame::SetApplicationID( const OUString &rApplicationID )
{
    fprintf(stderr, "HaikuSalFrame::SetApplicationID()\n");
}

void HaikuSalFrame::ResetClipRegion()
{
    fprintf(stderr, "HaikuSalFrame::ResetClipRegion()\n");
}

void HaikuSalFrame::BeginSetClipRegion( sal_uLong nRects )
{
    fprintf(stderr, "HaikuSalFrame::BeginSetClipRegion()\n");
}

void HaikuSalFrame::UnionClipRegion( long nX, long nY, long nWidth, long nHeight )
{
    fprintf(stderr, "HaikuSalFrame::UnionClipRegion()\n");
}

void HaikuSalFrame::EndSetClipRegion()
{
    fprintf(stderr, "HaikuSalFrame::EndSetClipRegion()\n");
}

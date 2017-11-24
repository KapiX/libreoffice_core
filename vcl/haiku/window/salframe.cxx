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
    fprintf(stderr, "draw\n");
    //updateRect.PrintToStream();
    SetViewColor(B_TRANSPARENT_COLOR);
    DrawBitmap(mpFrame->mpPrivate->mpBmp, updateRect, updateRect); // updateRect?
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
    mpFrame->UpdateFrameGeometry();
}

void HaikuWindow::MessageReceived(BMessage* message)
{
    switch(message->what) {
        default: {
            BWindow::MessageReceived(message);
        } break;
    }
}

HaikuSalFrame::HaikuSalFrame(HaikuSalFrame *pParent, SalFrameStyleFlags nStyle)
{
    fprintf(stderr, "HaikuSalFrame::HaikuSalFrame()\n");
    mpPrivate = new HaikuSalFramePrivate;
    mpPrivate->mpParent = pParent;
    uint32 flags = 0;
    if(nStyle & SalFrameStyleFlags::TOOLTIP || nStyle & SalFrameStyleFlags::FLOAT) {
        flags |= B_AVOID_FOCUS;
    }
    if(!(nStyle & SalFrameStyleFlags::MOVEABLE))
        flags |= B_NOT_MOVABLE;
    if(!(nStyle & SalFrameStyleFlags::SIZEABLE))
        flags |= B_NOT_RESIZABLE;
    if(!(nStyle & SalFrameStyleFlags::CLOSEABLE))
        flags |= B_NOT_CLOSABLE;
    mpPrivate->mpWindow = new HaikuWindow(this, flags);
    if(nStyle & SalFrameStyleFlags::FLOAT ||
       nStyle & SalFrameStyleFlags::OWNERDRAWDECORATION) {
        mpPrivate->mpWindow->SetLook(B_NO_BORDER_WINDOW_LOOK);
    }
    if(nStyle & SalFrameStyleFlags::TOOLTIP || nStyle & SalFrameStyleFlags::FLOAT) {
        //mpPrivate->mpWindow->SetMouseEventMask(B_POINTER_EVENTS);
    }
    BRect bounds = BScreen().Frame();
    bounds.right++;
    bounds.bottom++;
    HaikuView* defView = new HaikuView(mpPrivate->mpWindow->Bounds(), this);
    mpPrivate->mpBmp = new BBitmap(bounds, B_RGB32, true);
    mpPrivate->mpWindow->AddChild(defView);
}

HaikuSalFrame::~HaikuSalFrame()
{
    fprintf(stderr, "HaikuSalFrame::~HaikuSalFrame()\n");
    if(mpPrivate->mpWindow->LockLooper()) {
        mpPrivate->mpWindow->Quit();
    }
    delete mpPrivate->mpBmp;
    delete mpPrivate;
}

SalGraphics* HaikuSalFrame::AcquireGraphics()
{
    fprintf(stderr, "HaikuSalFrame::AcquireGraphics()\n");
    BRect bounds = BScreen().Frame();
    bounds.right++;
    bounds.bottom++;
    BView* view = new BView(bounds, "drawing", B_FOLLOW_NONE, B_WILL_DRAW);
    mpPrivate->mpBmp->AddChild(view);
    return new HaikuSalGraphics(view, this);
}

void HaikuSalFrame::ReleaseGraphics( SalGraphics* pGraphics )
{
    fprintf(stderr, "HaikuSalFrame::ReleaseGraphics()\n");

    HaikuSalGraphics *graphics = static_cast<HaikuSalGraphics*>(pGraphics);
    BWindow* window = graphics->getView()->Window();
    if(window->LockLooper()) {
        graphics->getView()->RemoveSelf();
        window->UnlockLooper();
    }
    delete graphics->getView();
    delete graphics;
}

bool HaikuSalFrame::PostEvent(ImplSVEvent* pData)
{
    GetSalData()->mpFirstInstance->PostUserEvent( this, SalEvent::UserEvent, pData );
    return true;
}

void HaikuSalFrame::SetTitle( const OUString& rTitle )
{
    OString aTitle(OUStringToOString(rTitle, osl_getThreadTextEncoding()));
    mpPrivate->mpWindow->SetTitle(aTitle.getStr());
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
    mpPrivate->mpWindow->Show();
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
    fprintf(stderr, "HaikuSalFrame::SetPosSize\n");
    SalEvent nEvent = SalEvent::NONE;

    if(!mpPrivate->mpWindow->LockLooper())
        return;

    BRect aWindowRect = mpPrivate->mpWindow->Frame();
    BRect aParentRect = BScreen().Frame();
    if(mpPrivate->mpParent) {
        if(mpPrivate->mpParent->mpPrivate->mpWindow->LockLooper()) {
            aParentRect = mpPrivate->mpParent->mpPrivate->mpWindow->Frame();
            mpPrivate->mpParent->mpPrivate->mpWindow->UnlockLooper();
        }
    }

    if(nFlags & (SAL_FRAME_POSSIZE_X | SAL_FRAME_POSSIZE_Y)) {
        nEvent = SalEvent::Move;
    }
    if(nFlags & (SAL_FRAME_POSSIZE_WIDTH | SAL_FRAME_POSSIZE_HEIGHT)) {
        nEvent = (nEvent == SalEvent::Move) ? SalEvent::MoveResize : SalEvent::Resize;
    }

    if(nEvent != SalEvent::NONE)
        CallCallback(nEvent, nullptr);
    if ( (nFlags & SAL_FRAME_POSSIZE_X) )
        aWindowRect.left = aParentRect.left + nX;
    if ( (nFlags & SAL_FRAME_POSSIZE_Y) )
        aWindowRect.top = aParentRect.top + nY;
    if ( (nFlags & SAL_FRAME_POSSIZE_WIDTH) )
        aWindowRect.right = aWindowRect.left + nWidth;
    if ( (nFlags & SAL_FRAME_POSSIZE_HEIGHT) )
        aWindowRect.bottom = aWindowRect.top + nHeight;

    if(nEvent == SalEvent::Move || nEvent == SalEvent::MoveResize) {
        mpPrivate->mpWindow->MoveTo(aWindowRect.left, aWindowRect.top);
    }
    if(nEvent == SalEvent::Resize || nEvent == SalEvent::MoveResize) {
        mpPrivate->mpWindow->ResizeTo(aWindowRect.Width(), aWindowRect.Height());
    }

    UpdateFrameGeometry();

    mpPrivate->mpWindow->UnlockLooper();

}

SalFrame* HaikuSalFrame::GetParent() const
{
    return mpPrivate->mpParent;
}

void HaikuSalFrame::GetWorkArea( Rectangle &rRect )
{
    // FIXME multiscreen support
    BRect rect = BScreen().Frame();
    rRect.Left() = static_cast<long>(rect.left);
    rRect.Top() = static_cast<long>(rect.top);
    rRect.Right() = static_cast<long>(rect.right);
    rRect.Bottom() = static_cast<long>(rect.bottom);
}

void HaikuSalFrame::GetClientSize( long& rWidth, long& rHeight )
{
    rWidth = mpPrivate->mpWindow->Bounds().Width();
    rHeight = mpPrivate->mpWindow->Bounds().Height();
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
    mpPrivate->mpWindow->Sync();
    BRect rect(mpPrivate->mpWindow->Bounds());
    Invalidate(rect);
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
    //fprintf(stderr, "HaikuSalFrame::GetKeyName()\n");
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
    mpPrivate->mpParent = static_cast<HaikuSalFrame*>(pNewParent);
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

void HaikuSalFrame::UpdateFrameGeometry()
{
    if (mpPrivate == NULL || mpPrivate->mpWindow == NULL)
        return;

    // FIXME multiscreen support
    BRect aWindowRect = mpPrivate->mpWindow->Bounds();

    maGeometry.nX = static_cast<int>(aWindowRect.left);
    maGeometry.nY = static_cast<int>(aWindowRect.top);

    /*maGeometry.nLeftDecoration = static_cast<unsigned int>(aContentRect.origin.x - aFrameRect.origin.x);
    maGeometry.nRightDecoration = static_cast<unsigned int>((aFrameRect.origin.x + aFrameRect.size.width) -
                                  (aContentRect.origin.x + aContentRect.size.width));

    maGeometry.nTopDecoration = static_cast<unsigned int>(aContentRect.origin.y - aFrameRect.origin.y);
    maGeometry.nBottomDecoration = static_cast<unsigned int>((aFrameRect.origin.y + aFrameRect.size.height) -
                                   (aContentRect.origin.y + aContentRect.size.height));*/
    maGeometry.nLeftDecoration = -4;
    maGeometry.nRightDecoration = -4;
    maGeometry.nTopDecoration = -12;
    maGeometry.nBottomDecoration = -4;

    maGeometry.nWidth = static_cast<unsigned int>(aWindowRect.Width());
    maGeometry.nHeight = static_cast<unsigned int>(aWindowRect.Height());
}

void HaikuSalFrame::Invalidate(BRect updateRect)
{
    // FIXME this is smelly
    if(mpPrivate->mpWindow->LockLooper()) {
        mpPrivate->mpWindow->ChildAt(0)->Invalidate(updateRect);
        mpPrivate->mpWindow->UnlockLooper();
    }
}

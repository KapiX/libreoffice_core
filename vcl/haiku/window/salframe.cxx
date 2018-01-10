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

static sal_uInt16 ImplGetModifierMask( unsigned int nMask )
{
    sal_uInt16 nRet = 0;
    if( (nMask & B_SHIFT_KEY) != 0 )
        nRet |= KEY_SHIFT;
    if( (nMask & B_CONTROL_KEY) != 0 )
        nRet |= KEY_MOD3;
    if( (nMask & B_OPTION_KEY) != 0 )
        nRet |= KEY_MOD2;
    if( (nMask & B_COMMAND_KEY) != 0 )
        nRet |= KEY_MOD1;
    return nRet;
}

static sal_uInt16 ImplMapCharCode( sal_Unicode aCode )
{
    static sal_uInt16 aKeyCodeMap[ 128 ] =
    {
                    0,                0,                0,                0,                0,                0,                0,                0,
        KEY_BACKSPACE,          KEY_TAB,       KEY_RETURN,                0,                0,       KEY_RETURN,                0,                0,
                    0,                0,                0,                0,                0,                0,                0,                0,
                    0,          KEY_TAB,                0,       KEY_ESCAPE,                0,                0,                0,                0,
            KEY_SPACE,                0,                0,                0,                0,                0,                0,                0,
                    0,                0,     KEY_MULTIPLY,          KEY_ADD,        KEY_COMMA,     KEY_SUBTRACT,        KEY_POINT,       KEY_DIVIDE,
                KEY_0,            KEY_1,            KEY_2,            KEY_3,            KEY_4,            KEY_5,            KEY_6,            KEY_7,
                KEY_8,            KEY_9,                0,                0,         KEY_LESS,        KEY_EQUAL,      KEY_GREATER,                0,
                    0,            KEY_A,            KEY_B,            KEY_C,            KEY_D,            KEY_E,            KEY_F,            KEY_G,
                KEY_H,            KEY_I,            KEY_J,            KEY_K,            KEY_L,            KEY_M,            KEY_N,            KEY_O,
                KEY_P,            KEY_Q,            KEY_R,            KEY_S,            KEY_T,            KEY_U,            KEY_V,            KEY_W,
                KEY_X,            KEY_Y,            KEY_Z,                0,                0,                0,                0,                0,
        KEY_QUOTELEFT,            KEY_A,            KEY_B,            KEY_C,            KEY_D,            KEY_E,            KEY_F,            KEY_G,
                KEY_H,            KEY_I,            KEY_J,            KEY_K,            KEY_L,            KEY_M,            KEY_N,            KEY_O,
                KEY_P,            KEY_Q,            KEY_R,            KEY_S,            KEY_T,            KEY_U,            KEY_V,            KEY_W,
                KEY_X,            KEY_Y,            KEY_Z,                0,                0,                0,        KEY_TILDE,    KEY_BACKSPACE
    };

    // Note: the mapping 0x7f should by rights be KEY_DELETE
    // however if you press "backspace" 0x7f is reported
    // whereas for "delete" 0xf728 gets reported

    // Note: the mapping of 0x19 to KEY_TAB is because for unknown reasons
    // tab alone is reported as 0x09 (as expected) but shift-tab is
    // reported as 0x19 (end of medium)

    static sal_uInt16 aFunctionKeyCodeMap[ 128 ] =
    {
            KEY_UP,         KEY_DOWN,         KEY_LEFT,        KEY_RIGHT,           KEY_F1,           KEY_F2,           KEY_F3,           KEY_F4,
            KEY_F5,           KEY_F6,           KEY_F7,           KEY_F8,           KEY_F9,          KEY_F10,          KEY_F11,          KEY_F12,
           KEY_F13,          KEY_F14,          KEY_F15,          KEY_F16,          KEY_F17,          KEY_F18,          KEY_F19,          KEY_F20,
           KEY_F21,          KEY_F22,          KEY_F23,          KEY_F24,          KEY_F25,          KEY_F26,                0,                0,
                 0,                0,                0,                0,                0,                0,                0,       KEY_INSERT,
        KEY_DELETE,         KEY_HOME,                0,          KEY_END,        KEY_PAGEUP,    KEY_PAGEDOWN,                0,                0,
                 0,                0,                0,                0,                 0,        KEY_MENU,                0,                0,
                 0,                0,                0,                0,                 0,               0,                0,                0,
                 0,                0,                0,         KEY_UNDO,        KEY_REPEAT,        KEY_FIND,         KEY_HELP,                0,
                 0,                0,                0,                0,                 0,               0,                0,                0,
                 0,                0,                0,                0,                 0,               0,                0,                0,
                 0,                0,                0,                0,                 0,               0,                0,                0,
                 0,                0,                0,                0,                 0,               0,                0,                0,
                 0,                0,                0,                0,                 0,               0,                0,                0,
                 0,                0,                0,                0,                 0,               0,                0,                0,
                 0,                0,                0,                0,                 0,               0,                0,                0
    };

    sal_uInt16 nKeyCode = 0;
    if( aCode < SAL_N_ELEMENTS( aKeyCodeMap)  )
        nKeyCode = aKeyCodeMap[ aCode ];
    else if( aCode >= 0xf700 && aCode < 0xf780 )
        nKeyCode = aFunctionKeyCodeMap[ aCode - 0xf700 ];
    return nKeyCode;
}

static sal_uInt16 ImplMapKeyCode(sal_uInt16 nKeyCode)
{
    /*
      http://stackoverflow.com/questions/2080312/where-can-i-find-a-list-of-key-codes-for-use-with-cocoas-nsevent-class/2080324#2080324
      /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/HIToolbox.framework/Versions/A/Headers/Events.h
     */

    static sal_uInt16 aKeyCodeMap[ 0x80 ] =
    {
            KEY_A,            KEY_S,            KEY_D,            KEY_F,            KEY_H,            KEY_G,            KEY_Z,            KEY_X,
            KEY_C,            KEY_V,                0,            KEY_B,            KEY_Q,            KEY_W,            KEY_E,            KEY_R,
            KEY_Y,            KEY_T,            KEY_1,            KEY_2,            KEY_3,            KEY_4,            KEY_6,            KEY_5,
        KEY_EQUAL,            KEY_9,            KEY_7,     KEY_SUBTRACT,            KEY_8,            KEY_0, KEY_BRACKETRIGHT,            KEY_0,
            KEY_U,  KEY_BRACKETLEFT,            KEY_I,            KEY_P,       KEY_RETURN,            KEY_L,            KEY_J,   KEY_QUOTERIGHT,
            KEY_K,    KEY_SEMICOLON,                0,        KEY_COMMA,       KEY_DIVIDE,            KEY_N,            KEY_M,        KEY_POINT,
          KEY_TAB,        KEY_SPACE,    KEY_QUOTELEFT,       KEY_DELETE,                0,       KEY_ESCAPE,                0,                0,
                0,     KEY_CAPSLOCK,                0,                0,                0,                0,                0,                0,
          KEY_F17,      KEY_DECIMAL,                0,     KEY_MULTIPLY,                0,          KEY_ADD,                0,                0,
                0,                0,                0,       KEY_DIVIDE,       KEY_RETURN,                0,     KEY_SUBTRACT,          KEY_F18,
          KEY_F19,        KEY_EQUAL,                0,                0,                0,                0,                0,                0,
                0,                0,          KEY_F20,                0,                0,                0,                0,                0,
           KEY_F5,           KEY_F6,           KEY_F7,           KEY_F3,           KEY_F8,           KEY_F9,                0,          KEY_F11,
                0,          KEY_F13,          KEY_F16,          KEY_F14,                0,          KEY_F10,                0,          KEY_F12,
                0,          KEY_F15,         KEY_HELP,         KEY_HOME,       KEY_PAGEUP,       KEY_DELETE,           KEY_F4,          KEY_END,
           KEY_F2,     KEY_PAGEDOWN,           KEY_F1,         KEY_LEFT,        KEY_RIGHT,         KEY_DOWN,           KEY_UP,                0
    };

    if (nKeyCode < SAL_N_ELEMENTS(aKeyCodeMap))
        return aKeyCodeMap[nKeyCode];
    return 0;
}


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

void HaikuView::KeyDown(const char* bytes, int32 numBytes)
{
    fprintf(stderr, "HaikuSalFrame::KeyDown()\n");
    sal_Unicode keyChar = bytes[0];
    sal_uInt16 nKeyCode = ImplMapCharCode( keyChar );
    /*if (nKeyCode == 0)
    {
        sal_uInt16 nOtherKeyCode = [pEvent keyCode];
        nKeyCode = ImplMapKeyCode(nOtherKeyCode);
    }*/
    if( nKeyCode != 0 )
    {
        // don't send unicodes in the private use area
        if( keyChar >= 0xf700 && keyChar < 0xf780 )
            keyChar = 0;
    }
    SalKeyEvent aEvent;
//    aEvent.mnTime           = mpFrame->mnLastEventTime;
    aEvent.mnCode           = nKeyCode | ImplGetModifierMask( modifiers() );
//    aEvent.mnCharCode       = aChar;
    aEvent.mnCharCode       = keyChar;
    aEvent.mnRepeat         = FALSE;
    GetSalData()->mpFirstInstance->PostUserEvent(mpFrame, SalEvent::KeyInput, &aEvent);
}

void HaikuView::KeyUp(const char* bytes, int32 numBytes)
{
    sal_Unicode keyChar = bytes[0];
    sal_uInt16 nKeyCode = ImplMapCharCode( keyChar );
    /*if (nKeyCode == 0)
    {
        sal_uInt16 nOtherKeyCode = [pEvent keyCode];
        nKeyCode = ImplMapKeyCode(nOtherKeyCode);
    }*/
    if( nKeyCode != 0 )
    {
        // don't send unicodes in the private use area
        if( keyChar >= 0xf700 && keyChar < 0xf780 )
            keyChar = 0;
    }
    SalKeyEvent aEvent;
//    aEvent.mnTime           = mpFrame->mnLastEventTime;
    aEvent.mnCode           = nKeyCode | ImplGetModifierMask( modifiers() );
//    aEvent.mnCharCode       = aChar;
    aEvent.mnCharCode       = keyChar;
    aEvent.mnRepeat         = FALSE;
    GetSalData()->mpFirstInstance->PostUserEvent(mpFrame, SalEvent::KeyUp, &aEvent);
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
    /*if(!(nStyle & SalFrameStyleFlags::SIZEABLE))
        flags |= B_NOT_RESIZABLE;*/
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
    BRect rect(mpPrivate->mpWindow->Bounds());
    SalPaintEvent *aPEvt = new SalPaintEvent(rect.left, rect.top, rect.Width(), rect.Height());
    aPEvt->mbImmediateUpdate = false;
    GetSalData()->mpFirstInstance->PostUserEvent(this, SalEvent::Paint, aPEvt);
    //mpPrivate->mpWindow->Sync();
    //Invalidate(rect);
    // FIXME which child should be focused
    if (nFlags & SalFrameToTop::GrabFocus) {
        if (mpPrivate->mpWindow->LockLooper()) {
            mpPrivate->mpWindow->Activate(true);
            BView* view = mpPrivate->mpWindow->ChildAt(0);
            if (view) {
                view->MakeFocus(true);
            }
            mpPrivate->mpWindow->UnlockLooper();
        }
    } else if (nFlags & SalFrameToTop::GrabFocusOnly) {
        if (mpPrivate->mpWindow->LockLooper()) {
            BView* view = mpPrivate->mpWindow->ChildAt(0);
            if (view) {
                view->MakeFocus(true);
            }
            mpPrivate->mpWindow->UnlockLooper();
        }
    }
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

    BRect bounds = aWindowRect;
    SalPaintEvent aPEvt(bounds.left, bounds.top, bounds.Width(), bounds.Height());
    aPEvt.mbImmediateUpdate = false;
    CallCallback(SalEvent::Paint, &aPEvt);
}

void HaikuSalFrame::Invalidate(BRect updateRect)
{
    // FIXME this is smelly
    if(mpPrivate->mpWindow->LockLooper()) {
        mpPrivate->mpWindow->ChildAt(0)->Invalidate(updateRect);
        mpPrivate->mpWindow->UnlockLooper();
    }
}

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

#include <haiku/salframe.hxx>

#include <cstdio>

HaikuSalFrame::HaikuSalFrame()
{
    fprintf(stderr, "HaikuSalFrame::HaikuSalFrame()\n");
    mpWindow = new HaikuWindow();
}

HaikuSalFrame::~HaikuSalFrame()
{
    fprintf(stderr, "HaikuSalFrame::~HaikuSalFrame()\n");
    mpWindow->Quit();
}

SalGraphics* HaikuSalFrame::AcquireGraphics()
{
    fprintf(stderr, "HaikuSalFrame::AcquireGraphics()\n");
    return nullptr;
}

void HaikuSalFrame::ReleaseGraphics( SalGraphics* pGraphics )
{
    fprintf(stderr, "HaikuSalFrame::ReleaseGraphics()\n");
}

bool HaikuSalFrame::PostEvent(ImplSVEvent* pData)
{
    fprintf(stderr, "HaikuSalFrame::PostEvent()\n");
}

void HaikuSalFrame::SetTitle( const OUString& rTitle )
{
    fprintf(stderr, "HaikuSalFrame::SetTitle()\n");
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
    fprintf(stderr, "HaikuSalFrame::Show()\n");
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
    fprintf(stderr, "HaikuSalFrame::SetPosSize()\n");
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
    fprintf(stderr, "HaikuSalFrame::GetClientSize()\n");
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
    fprintf(stderr, "HaikuSalFrame::SetPointer()\n");
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

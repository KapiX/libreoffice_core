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

#include "osl/module.h"
#include "osl/file.h"
#include "osl/thread.h"

#include <haiku/saldata.hxx>
#include <haiku/salinst.hxx>
#include <haiku/salframe.hxx>
#include <haiku/sallayout.hxx>
#include <haiku/salgdi.hxx>
#include <outdev.h>

#include "sallayout.hxx"
#include "CommonSalLayout.hxx"

#define TRACE printf("Trace:%s:%d - %s\n", __FILE__, __LINE__, __FUNCTION__);


HaikuSalLayout::HaikuSalLayout()
    :
    mnGlyphCount( 0 ),
    mnCharCount( 0 ),
    mpOutGlyphs( nullptr ),
    mpGlyphAdvances( nullptr ),
    mpGlyphOrigAdvs( nullptr ),
    mpCharWidths( nullptr ),
    mnWidth( 0 )
{
    //TRACE
}

HaikuSalLayout::~HaikuSalLayout()
{
    delete[] mpOutGlyphs;
    delete[] mpGlyphAdvances;
    delete[] mpGlyphOrigAdvs;
    delete[] mpCharWidths;
}


bool HaikuSalLayout::LayoutText( ImplLayoutArgs& rArgs )
{
    //TRACE
    OString str(OUStringToOString(rArgs.mrStr, osl_getThreadTextEncoding()));
    mnWidth = be_plain_font->StringWidth(str.getStr());
    mnCharCount = rArgs.mnEndCharPos - rArgs.mnMinCharPos;
    mnGlyphCount = mnCharCount;
    mpOutGlyphs = new char[ mnGlyphCount + 1];
    mpGlyphAdvances = new int[ mnGlyphCount ];

    for( int i = 0; i < mnGlyphCount; ++i )
    {
        mpOutGlyphs[ i ] = str[ i ];
    }
    mpOutGlyphs[mnGlyphCount] = 0;
    return true;
}

void HaikuSalLayout::DrawText( SalGraphics& rGraphics ) const
{
//    TRACE
    HaikuSalGraphics& rHaikuGraphics = static_cast<HaikuSalGraphics&>(rGraphics);
    BView* view = rHaikuGraphics.getView();
    Point pt = GetDrawPosition();
    BPoint p(pt.X(), pt.Y());
    if(view->Window()->LockLooper()) {
        view->SetFont(be_plain_font);
        view->SetHighColor(rHaikuGraphics.mTextColor);
        view->DrawString(mpOutGlyphs, p);
        view->Sync();
        view->Window()->UnlockLooper();
    }
    // FIXME refactor to HaikuSalGraphics::RepaintRequest
    if(rHaikuGraphics.mpFrame) {
        SalPaintEvent *aPEvt = new SalPaintEvent(pt.X(), pt.Y(), 1000, 40); // FIXME
        aPEvt->mbImmediateUpdate = false;
        GetSalData()->mpFirstInstance->PostUserEvent(rHaikuGraphics.mpFrame, SalEvent::Paint, aPEvt);
    }
}

sal_Int32 HaikuSalLayout::GetTextBreak(DeviceCoordinate nMaxWidth, DeviceCoordinate nCharExtra, int nFactor) const
{
    return GenericSalLayout::GetTextBreak( nMaxWidth, nCharExtra, nFactor );
}

DeviceCoordinate HaikuSalLayout::FillDXArray( DeviceCoordinate* pDXArray ) const
{
    return mnWidth;
}

void    HaikuSalLayout::GetCaretPositions( int nArraySize, long* pCaretXArray ) const
{
    GenericSalLayout::GetCaretPositions( nArraySize, pCaretXArray );
}

int     HaikuSalLayout::GetNextGlyphs( int nLen, sal_GlyphId* pGlyphIdAry, Point& rPos, int& nStart,
                               DeviceCoordinate* pGlyphAdvAry, int* pCharPosAry,
                               const PhysicalFontFace** pFallbackFonts) const
{
    return GenericSalLayout::GetNextGlyphs( nLen, pGlyphIdAry, rPos, nStart, pGlyphAdvAry, pCharPosAry );
}


void    HaikuSalLayout::MoveGlyph( int nStart, long nNewXPos )
{
    GenericSalLayout::MoveGlyph( nStart, nNewXPos );
}

void    HaikuSalLayout::DropGlyph( int nStart )
{
    GenericSalLayout::DropGlyph( nStart );
}

void    HaikuSalLayout::Simplify( bool bIsBase )
{
    GenericSalLayout::Simplify( bIsBase );
}


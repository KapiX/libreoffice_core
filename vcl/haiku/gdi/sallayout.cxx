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

#include <haiku/sallayout.hxx>
#include <haiku/salgdi.hxx>
#include <outdev.h>

#include "sallayout.hxx"
#include "CommonSalLayout.hxx"

#define TRACE printf("Trace:%s:%d - %s\n", __FILE__, __LINE__, __FUNCTION__);


HaikuSalLayout::HaikuSalLayout()
{
    TRACE
}

HaikuSalLayout::~HaikuSalLayout()
{
    TRACE
}


bool HaikuSalLayout::LayoutText( ImplLayoutArgs& )
{
    TRACE
    return true;
}

void HaikuSalLayout::DrawText( SalGraphics& ) const
{
    TRACE
}

sal_Int32 HaikuSalLayout::GetTextBreak(DeviceCoordinate nMaxWidth, DeviceCoordinate nCharExtra, int nFactor) const
{
    TRACE
    return 1;
}

DeviceCoordinate HaikuSalLayout::FillDXArray( DeviceCoordinate* pDXArray ) const
{
    TRACE
    return 1;
}

void    HaikuSalLayout::GetCaretPositions( int nArraySize, long* pCaretXArray ) const
{
    TRACE
}

int     HaikuSalLayout::GetNextGlyphs( int nLen, sal_GlyphId* pGlyphIdAry, Point& rPos, int&,
                               DeviceCoordinate* pGlyphAdvAry, int* pCharPosAry,
                               const PhysicalFontFace** pFallbackFonts) const
{
    TRACE
    return 1;
}


void    HaikuSalLayout::MoveGlyph( int nStart, long nNewXPos )
{
    TRACE
}

void    HaikuSalLayout::DropGlyph( int nStart )
{
    TRACE
}

void    HaikuSalLayout::Simplify( bool bIsBase )
{
    TRACE
}


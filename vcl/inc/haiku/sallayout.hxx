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

#ifndef INCLUDED_VCL_INC_HAIKU_SALLAYOUT_HXX
#define INCLUDED_VCL_INC_HAIKU_SALLAYOUT_HXX

#include <rtl/ustring.hxx>

#include <sallayout.hxx>
#include <svsys.h>
#include <haiku/salgdi.hxx>


class HaikuSalLayout : public GenericSalLayout {
public:
    HaikuSalLayout();
    ~HaikuSalLayout();

    bool LayoutText( ImplLayoutArgs& );
    void DrawText( SalGraphics& ) const;
    sal_Int32 GetTextBreak(DeviceCoordinate nMaxWidth, DeviceCoordinate nCharExtra, int nFactor) const;
    DeviceCoordinate FillDXArray( DeviceCoordinate* pDXArray ) const;
    void    GetCaretPositions( int nArraySize, long* pCaretXArray ) const;
    int     GetNextGlyphs( int nLen, sal_GlyphId* pGlyphIdAry, Point& rPos, int&,
                                   DeviceCoordinate* pGlyphAdvAry, int* pCharPosAry,
                                   const PhysicalFontFace** pFallbackFonts) const;

    void    MoveGlyph( int nStart, long nNewXPos );
    void    DropGlyph( int nStart );
    void    Simplify( bool bIsBase );
private:
    int             mnGlyphCount;
    int             mnCharCount;
    char*           mpOutGlyphs;
    int*            mpGlyphAdvances;    // if possible this is shared with mpGlyphAdvances[]
    int*            mpGlyphOrigAdvs;
    int*            mpCharWidths;       // map rel char pos to char width
    mutable long    mnWidth;
};


#endif // _H

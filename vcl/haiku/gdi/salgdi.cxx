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

#include <string.h>
#include <svsys.h>
#include <haiku/salbmp.hxx>
#include <haiku/salgdi.hxx>
#include <haiku/salframe.hxx>
#include <haiku/sallayout.hxx>
#include <sallayout.hxx>
#include <vcl/sysdata.hxx>
#include <vcl/fontcharmap.hxx>


#include "unx/printergfx.hxx"
#include "unx/genpspgraphics.h"
#include "unx/geninst.h"
#include "unx/glyphcache.hxx"
#include "unx/fc_fontoptions.hxx"
#include "PhysicalFontFace.hxx"
#include "impfont.hxx"
#include "impfontmetricdata.hxx"

#include "gdiimpl.hxx"

#include <memory>

#define TRACE printf("Trace:%s:%d - %s\n", __FILE__, __LINE__, __FUNCTION__);


HaikuSalGraphics::HaikuSalGraphics(BView* view)
{
    mpGlyphCache.reset(new GlyphCache);
    mpView = view;
}

HaikuSalGraphics::~HaikuSalGraphics()
{
    TRACE
}

SalGraphicsImpl* HaikuSalGraphics::GetImpl() const
{
    TRACE
    //return new HaikuSalGraphicsImpl();
    return nullptr;
}

void HaikuSalGraphics::GetResolution( sal_Int32& rDPIX, sal_Int32& rDPIY )
{
    rDPIX = 96;
    rDPIY = 96;
}


sal_uInt16 HaikuSalGraphics::GetBitCount() const
{
    TRACE
    return 32;
}

long HaikuSalGraphics::GetGraphicsWidth() const
{
    return static_cast<long>(mpView->Frame().Width());
}

void HaikuSalGraphics::ResetClipRegion()
{
    TRACE
}


void HaikuSalGraphics::SetLineColor()
{
    if(!mpView->Window()) return;
    if(mpView->Window()->LockLooper()) {
        mpView->SetHighColor(0, 0, 0, 0);
        mpView->Window()->UnlockLooper();
    }
}


void HaikuSalGraphics::SetLineColor( SalColor nSalColor )
{
    if(!mpView->Window()) return;
    sal_uInt8 red = SALCOLOR_RED(nSalColor);
    sal_uInt8 green = SALCOLOR_GREEN(nSalColor);
    sal_uInt8 blue = SALCOLOR_BLUE(nSalColor);
    if(mpView->Window()->LockLooper()) {
        mpView->SetHighColor(red, green, blue);
        mpView->Window()->UnlockLooper();
    }
}


void HaikuSalGraphics::SetFillColor()
{
    if(!mpView->Window()) return;
    if(mpView->Window()->LockLooper()) {
        mpView->SetLowColor(0, 0, 0, 0);
        mpView->Window()->UnlockLooper();
    }
}


void HaikuSalGraphics::SetFillColor( SalColor nSalColor )
{
    if(!mpView->Window()) return;
    sal_uInt8 red = SALCOLOR_RED(nSalColor);
    sal_uInt8 green = SALCOLOR_GREEN(nSalColor);
    sal_uInt8 blue = SALCOLOR_BLUE(nSalColor);
    if(mpView->Window()->LockLooper()) {
        mpView->SetLowColor(red, green, blue);
        mpView->Window()->UnlockLooper();
    }
}

void HaikuSalGraphics::SetXORMode( bool bSet )
{
    if(!mpView->Window()) return;
    if(mpView->Window()->LockLooper()) {
        mpView->SetDrawingMode(bSet ? B_OP_INVERT : B_OP_COPY);
        mpView->Window()->UnlockLooper();
    }
}

void HaikuSalGraphics::SetROPLineColor( SalROPColor nROPColor )
{
    TRACE
}

void HaikuSalGraphics::SetROPFillColor( SalROPColor nROPColor )
{
    TRACE
}

void HaikuSalGraphics::SetTextColor( SalColor nSalColor )
{
    TRACE
    sal_uInt8 red = SALCOLOR_RED(nSalColor);
    sal_uInt8 green = SALCOLOR_GREEN(nSalColor);
    sal_uInt8 blue = SALCOLOR_BLUE(nSalColor);
    mTextColor = (rgb_color) { red, green, blue, 255 };
}

void HaikuSalGraphics::SetFont( FontSelectPattern*, int nFallbackLevel )
{
    //TRACE
}


void HaikuSalGraphics::GetFontMetric( ImplFontMetricDataRef& rxFontMetric, int nFallbackLevel )
{
    TRACE
    font_height fh;
    be_plain_font->GetHeight(&fh);
    rxFontMetric->SetAscent(static_cast<int>(fh.ascent));
    rxFontMetric->SetDescent(static_cast<int>(fh.descent));
}

const FontCharMapRef HaikuSalGraphics::GetFontCharMap() const
{
    TRACE
    FontCharMapRef xDefFontCharMap( new FontCharMap() );
    return xDefFontCharMap;
}

bool HaikuSalGraphics::GetFontCapabilities(vcl::FontCapabilities &rGetFontCapabilities) const
{
    TRACE
    return false;
}

void HaikuSalGraphics::GetDevFontList( PhysicalFontCollection* pFontCollection )
{
    TRACE
    GlyphCache& rGC = *mpGlyphCache.get();

    psp::PrintFontManager& rMgr = psp::PrintFontManager::get();
    ::std::list< psp::fontID > aList;
    ::std::list< psp::fontID >::iterator it;
    psp::FastPrintFontInfo aInfo;
    rMgr.getFontList( aList );
    for( it = aList.begin(); it != aList.end(); ++it )
    {
        if( !rMgr.getFontFastInfo( *it, aInfo ) )
            continue;

        // normalize face number to the GlyphCache
        int nFaceNum = rMgr.getFontFaceNumber( aInfo.m_nID );

        // inform GlyphCache about this font provided by the PsPrint subsystem
        FontAttributes aDFA = GenPspGraphics::Info2FontAttributes( aInfo );
        aDFA.IncreaseQualityBy( 4096 );
        const OString& rFileName = rMgr.getFontFileSysPath( aInfo.m_nID );
        rGC.AddFontFile( rFileName, nFaceNum, aInfo.m_nID, aDFA );
   }

    // announce glyphcache fonts
    rGC.AnnounceFonts( pFontCollection );
}


void HaikuSalGraphics::ClearDevFontCache()
{
    TRACE
}

bool HaikuSalGraphics::AddTempDevFont( PhysicalFontCollection*, const OUString& rFileURL, const OUString& rFontName )
{
    TRACE
    return true;
}


bool HaikuSalGraphics::CreateFontSubset( const OUString& rToFile,
          const PhysicalFontFace*,
          const sal_GlyphId* pGlyphIDs,
          const sal_uInt8* pEncoding,
          sal_Int32* pWidths,
          int nGlyphs,
          FontSubsetInfo& rInfo // out parameter
          )
{
    TRACE
    return true;
}


const Ucs2SIntMap* HaikuSalGraphics::GetFontEncodingVector( const PhysicalFontFace*, const Ucs2OStrMap** ppNonEncoded, std::set<sal_Unicode> const** )
{
    TRACE
    return nullptr;
}

const void* HaikuSalGraphics::GetEmbedFontData( const PhysicalFontFace*,
       const sal_Ucs* pUnicodes,
       sal_Int32* pWidths,
       size_t nLen,
       FontSubsetInfo& rInfo,
       long* pDataLen )
{
    TRACE
    return nullptr;
}

void HaikuSalGraphics::FreeEmbedFontData( const void* pData, long nDataLen )
{
    TRACE
}

void HaikuSalGraphics::GetGlyphWidths( const PhysicalFontFace*,
    bool bVertical,
    std::vector< sal_Int32 >& rWidths,
    Ucs2UIntMap& rUnicodeEnc )
{
    TRACE
}


bool HaikuSalGraphics::GetGlyphBoundRect( sal_GlyphId, Rectangle& )
{
//    TRACE
    return true;
}

bool HaikuSalGraphics::GetGlyphOutline( sal_GlyphId, basegfx::B2DPolyPolygon& )
{
    TRACE
    return true;
}

SalLayout* HaikuSalGraphics::GetTextLayout( ImplLayoutArgs&, int nFallbackLevel )
{
    //TRACE
    return new HaikuSalLayout();
}
void HaikuSalGraphics::DrawSalLayout( const CommonSalLayout& )
{
    TRACE
}

bool HaikuSalGraphics::supportsOperation( OutDevSupportType ) const
{
    TRACE
    return false;
}
// Query the platform layer for control support
bool HaikuSalGraphics::IsNativeControlSupported( ControlType nType, ControlPart nPart )
{
//    TRACE
    return false;
}

SystemGraphicsData HaikuSalGraphics::GetGraphicsData() const
{
    TRACE
    return SystemGraphicsData();
}

/// Update settings based on the platform values
void HaikuSalGraphics::updateSettingsNative( AllSettings& rSettings )
{
    TRACE
}

bool HaikuSalGraphics::setClipRegion( const vcl::Region& region )
{
    if(!mpView->Window()) return false;
    Rectangle rect = region.GetBoundRect();
    BRegion r(BRect(rect.Left(), rect.Top(), rect.Right() + 1, rect.Bottom() + 1));
    if(mpView->Window()->LockLooper()) {
        mpView->ConstrainClippingRegion(&r);
        mpView->Sync();
        mpView->Window()->UnlockLooper();
        return true;
    }
    return false;
}

void HaikuSalGraphics::drawPixel( long nX, long nY )
{
    if(!mpView->Window()) return;
    BPoint start(nX, nY);
    BPoint end(nX, nY);
    if(mpView->Window()->LockLooper()) {
        mpView->StrokeLine(start, end, B_SOLID_HIGH);
        mpView->Sync();
        mpView->Window()->UnlockLooper();
    }
}

void HaikuSalGraphics::drawPixel( long nX, long nY, SalColor nSalColor )
{
    TRACE
}

void HaikuSalGraphics::drawLine( long nX1, long nY1, long nX2, long nY2 )
{
    if(!mpView->Window()) return;
    BPoint start(nX1, nY1);
    BPoint end(nX2, nY2);
    if(mpView->Window()->LockLooper()) {
        mpView->StrokeLine(start, end, B_SOLID_HIGH);
        mpView->Sync();
        mpView->Window()->UnlockLooper();
    }
}

void HaikuSalGraphics::drawRect( long nX, long nY, long nWidth, long nHeight )
{
    TRACE
    if(!mpView->Window()) return;
    BRect rect(nX, nY, nX + nWidth - 1, nY + nHeight - 1);
    if(mpView->Window()->LockLooper()) {
        mpView->StrokeRect(rect, B_SOLID_HIGH);
        mpView->FillRect(rect, B_SOLID_LOW);
        mpView->Sync();
        mpView->Window()->UnlockLooper();
    }
}

void HaikuSalGraphics::drawPolyLine( sal_uInt32 nPoints, const SalPoint* pPtAry )
{
    if(!mpView->Window()) return;
    if(mpView->Window()->LockLooper()) {
        mpView->BeginLineArray(nPoints);
        for(sal_uInt32 i = 0; i < nPoints - 1; i++) {
            BPoint pt1 = BPoint(pPtAry[i].mnX, pPtAry[i].mnY);
            BPoint pt2 = BPoint(pPtAry[i + 1].mnX, pPtAry[i + 1].mnY);
            mpView->AddLine(pt1, pt2, mpView->HighColor());
        }
        mpView->EndLineArray();
        mpView->Sync();
        mpView->Window()->UnlockLooper();
    }
}

void HaikuSalGraphics::drawPolygon( sal_uInt32 nPoints, const SalPoint* pPtAry )
{
    if(!mpView->Window()) return;
    std::vector<BPoint> points;
    for(sal_uInt32 i = 0; i < nPoints; i++) {
        points.push_back(BPoint(pPtAry[i].mnX, pPtAry[i].mnY));
    }
    if(mpView->Window()->LockLooper()) {
        mpView->StrokePolygon(points.data(), nPoints, false, B_SOLID_HIGH);
        mpView->FillPolygon(points.data(), nPoints, B_SOLID_LOW);
        mpView->Sync();
        mpView->Window()->UnlockLooper();
    }
}

void HaikuSalGraphics::drawPolyPolygon( sal_uInt32 nPoly, const sal_uInt32* pPoints, PCONSTSALPOINT* pPtAry )
{
    TRACE
}

bool HaikuSalGraphics::drawPolyPolygon( const basegfx::B2DPolyPolygon&, double fTransparency )
{
    TRACE
    return true;
}

bool HaikuSalGraphics::drawPolyLine(
 const basegfx::B2DPolygon&,
 double fTransparency,
 const basegfx::B2DVector& rLineWidth,
 basegfx::B2DLineJoin,
 css::drawing::LineCap,
 double fMiterMinimumAngle)
{
    TRACE
    return true;
}

bool HaikuSalGraphics::drawPolyLineBezier( sal_uInt32 nPoints, const SalPoint* pPtAry, const sal_uInt8* pFlgAry )
{
    TRACE
    return true;
}

bool HaikuSalGraphics::drawPolygonBezier( sal_uInt32 nPoints, const SalPoint* pPtAry, const sal_uInt8* pFlgAry )
{
    TRACE
    return true;
}

bool HaikuSalGraphics::drawPolyPolygonBezier( sal_uInt32 nPoly, const sal_uInt32* pPoints, const SalPoint* const* pPtAry, const sal_uInt8* const* pFlgAry )
{
    TRACE
    return true;
}

    // CopyArea --> No RasterOp, but ClipRegion
void HaikuSalGraphics::copyArea( long nDestX, long nDestY, long nSrcX, long nSrcY, long nSrcWidth,
      long nSrcHeight, bool bWindowInvalidate )
{
    TRACE
}


    // CopyBits and DrawBitmap --> RasterOp and ClipRegion
    // CopyBits() --> pSrcGraphics == NULL, then CopyBits on same Graphics
void HaikuSalGraphics::copyBits( const SalTwoRect& rPosAry, SalGraphics* pSrcGraphics )
{
    TRACE
    //drawRect(rPosAry.mnDestX, rPosAry.mnDestY, rPosAry.mnDestWidth, rPosAry.mnDestHeight);
}

void HaikuSalGraphics::drawBitmap( const SalTwoRect& rPosAry, const SalBitmap& rSalBitmap )
{
    TRACE
    //drawRect(rPosAry.mnDestX, rPosAry.mnDestY, rPosAry.mnDestWidth, rPosAry.mnDestHeight);
}

void HaikuSalGraphics::drawBitmap( const SalTwoRect& rPosAry,
        const SalBitmap& rSalBitmap,
        const SalBitmap& rTransparentBitmap )
{
    TRACE
    //drawRect(rPosAry.mnDestX, rPosAry.mnDestY, rPosAry.mnDestWidth, rPosAry.mnDestHeight);
}

void HaikuSalGraphics::drawMask( const SalTwoRect& rPosAry,
      const SalBitmap& rSalBitmap,
      SalColor nMaskColor )
{
    TRACE
    //drawRect(rPosAry.mnDestX, rPosAry.mnDestY, rPosAry.mnDestWidth, rPosAry.mnDestHeight);
}

SalBitmap*  HaikuSalGraphics::getBitmap( long nX, long nY, long nWidth, long nHeight )
{
    TRACE
    return new HaikuSalBitmap();
}

SalColor    HaikuSalGraphics::getPixel( long nX, long nY )
{
    TRACE
    return SalColor();
}


    // invert --> ClipRegion (only Windows or VirDevs)
void HaikuSalGraphics::invert( long nX, long nY, long nWidth, long nHeight, SalInvert nFlags)
{
    TRACE
}

void HaikuSalGraphics::invert( sal_uInt32 nPoints, const SalPoint* pPtAry, SalInvert nFlags )
{
    TRACE
}


bool HaikuSalGraphics::drawEPS( long nX, long nY, long nWidth, long nHeight, void* pPtr, sal_uIntPtr nSize )
{
    TRACE
    return true;
}


    // native widget rendering methods that require mirroring
bool HaikuSalGraphics::hitTestNativeControl( ControlType nType, ControlPart nPart, const Rectangle& rControlRegion,
           const Point& aPos, bool& rIsInside )
{
    TRACE
    return false;
}

bool HaikuSalGraphics::drawNativeControl( ControlType nType, ControlPart nPart, const Rectangle& rControlRegion,
        ControlState nState, const ImplControlValue& aValue,
        const OUString& aCaption )
{
    TRACE
    return false;
}

bool HaikuSalGraphics::getNativeControlRegion( ControlType nType, ControlPart nPart, const Rectangle& rControlRegion, ControlState nState,
      const ImplControlValue& aValue, const OUString& aCaption,
      Rectangle &rNativeBoundingRegion, Rectangle &rNativeContentRegion )
{
    TRACE
    return false;
}


bool HaikuSalGraphics::blendBitmap( const SalTwoRect&,
         const SalBitmap& rBitmap )
{
    TRACE
    return true;
}


bool HaikuSalGraphics::blendAlphaBitmap( const SalTwoRect&,
       const SalBitmap& rSrcBitmap,
       const SalBitmap& rMaskBitmap,
       const SalBitmap& rAlphaBitmap )
{
    TRACE
    return true;
}


bool HaikuSalGraphics::drawAlphaBitmap( const SalTwoRect& rTR,
      const SalBitmap& rSourceBitmap,
      const SalBitmap& rAlphaBitmap )
{
    //if(!mpView->Window()) return false;
    TRACE
    BRect src(rTR.mnSrcX, rTR.mnSrcY, rTR.mnSrcX + rTR.mnSrcWidth - 1, rTR.mnSrcY + rTR.mnSrcHeight - 1);
    BRect dest(rTR.mnDestX, rTR.mnDestY, rTR.mnDestX + rTR.mnDestWidth - 1, rTR.mnDestY + rTR.mnDestHeight - 1);
    const HaikuSalBitmap& rSalBitmap = static_cast< const HaikuSalBitmap& >(rSourceBitmap);
    const HaikuSalBitmap& rABitmap = static_cast< const HaikuSalBitmap& >(rAlphaBitmap);
    std::shared_ptr< BBitmap > aARGB(rSalBitmap.GetBitmap(rABitmap));
    if(aARGB.get()) {
        if(mpView->Window()->LockLooper()) {
            mpView->SetDrawingMode(B_OP_ALPHA);
            mpView->DrawBitmap(aARGB.get(), src, dest);
            mpView->SetDrawingMode(B_OP_COPY);
            mpView->Sync();
            mpView->Window()->UnlockLooper();
        }
        return true;
    }
    return false;
}

bool HaikuSalGraphics::drawTransformedBitmap(
      const basegfx::B2DPoint& rNull,
      const basegfx::B2DPoint& rX,
      const basegfx::B2DPoint& rY,
      const SalBitmap& rSourceBitmap,
      const SalBitmap* pAlphaBitmap)
{
    TRACE
    return true;
}

bool HaikuSalGraphics::drawAlphaRect( long nX, long nY, long nWidth, long nHeight, sal_uInt8 nTransparency )
{
    TRACE
    return true;
}



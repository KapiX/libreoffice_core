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

#ifndef INCLUDED_VCL_HAIKU_GDIIMPL_HXX
#define INCLUDED_VCL_HAIKU_GDIIMPL_HXX

#include <rtl/ustring.hxx>

#include <tools/solar.h>

#include <vcl/salgtype.hxx>
#include <vcl/region.hxx>
#include <vcl/salnativewidgets.hxx>

#include <com/sun/star/drawing/LineCap.hpp>

class SalGraphics;
class SalBitmap;
class SalFrame;
class Gradient;
class OpenGLContext;
class SalVirtualDevice;

class HaikuSalGraphicsImpl : public SalGraphicsImpl
{
public:
    HaikuSalGraphicsImpl();
    virtual ~HaikuSalGraphicsImpl();

    virtual void Init() override;

    virtual void DeInit() {}

    virtual void freeResources() override;

    virtual bool setClipRegion( const vcl::Region& ) override;
    //
    // get the depth of the device
    virtual sal_uInt16 GetBitCount() const override;

    // get the width of the device
    virtual long GetGraphicsWidth() const override;

    // set the clip region to empty
    virtual void ResetClipRegion() override;

    // set the line color to transparent (= don't draw lines)

    virtual void SetLineColor() override;

    // set the line color to a specific color
    virtual void SetLineColor( SalColor nSalColor ) override;

    // set the fill color to transparent (= don't fill)
    virtual void SetFillColor() override;

    // set the fill color to a specific color, shapes will be
    // filled accordingly
    virtual void SetFillColor( SalColor nSalColor ) override;

    // enable/disable XOR drawing
    virtual void SetXORMode( bool bSet ) override;

    // set line color for raster operations
    virtual void SetROPLineColor( SalROPColor nROPColor ) override;

    // set fill color for raster operations
    virtual void SetROPFillColor( SalROPColor nROPColor ) override;

    // draw --> LineColor and FillColor and RasterOp and ClipRegion
    virtual void drawPixel( long nX, long nY ) override;
    virtual void drawPixel( long nX, long nY, SalColor nSalColor ) override;

    virtual void drawLine( long nX1, long nY1, long nX2, long nY2 ) override;

    virtual void drawRect( long nX, long nY, long nWidth, long nHeight ) override;

    virtual void drawPolyLine( sal_uInt32 nPoints, const SalPoint* pPtAry ) override;

    virtual void drawPolygon( sal_uInt32 nPoints, const SalPoint* pPtAry ) override;

    virtual void drawPolyPolygon( sal_uInt32 nPoly, const sal_uInt32* pPoints, PCONSTSALPOINT* pPtAry ) override;
    virtual bool drawPolyPolygon( const basegfx::B2DPolyPolygon&, double fTransparency ) override;

    virtual bool drawPolyLine(
                const basegfx::B2DPolygon&,
                double fTransparency,
                const basegfx::B2DVector& rLineWidths,
                basegfx::B2DLineJoin,
                css::drawing::LineCap,
                double fMiterMinimumAngle) override;

    virtual bool drawPolyLineBezier(
                sal_uInt32 nPoints,
                const SalPoint* pPtAry,
                const sal_uInt8* pFlgAry ) override;

    virtual bool drawPolygonBezier(
                sal_uInt32 nPoints,
                const SalPoint* pPtAry,
                const sal_uInt8* pFlgAry ) override;

    virtual bool drawPolyPolygonBezier(
                sal_uInt32 nPoly,
                const sal_uInt32* pPoints,
                const SalPoint* const* pPtAry,
                const sal_uInt8* const* pFlgAry ) override;

    // CopyArea --> No RasterOp, but ClipRegion
    virtual void copyArea(
                long nDestX, long nDestY,
                long nSrcX, long nSrcY,
                long nSrcWidth, long nSrcHeight,
                bool bWindowInvalidate ) override;

    // CopyBits and DrawBitmap --> RasterOp and ClipRegion
    // CopyBits() --> pSrcGraphics == NULL, then CopyBits on same Graphics
    virtual void copyBits( const SalTwoRect& rPosAry, SalGraphics* pSrcGraphics ) override;

    virtual void drawBitmap( const SalTwoRect& rPosAry, const SalBitmap& rSalBitmap ) override;

    virtual void drawBitmap(
                const SalTwoRect& rPosAry,
                const SalBitmap& rSalBitmap,
                const SalBitmap& rMaskBitmap ) override;

    virtual void drawMask(
                const SalTwoRect& rPosAry,
                const SalBitmap& rSalBitmap,
                SalColor nMaskColor ) override;

    virtual SalBitmap* getBitmap( long nX, long nY, long nWidth, long nHeight ) override;

    virtual SalColor getPixel( long nX, long nY ) override;

    // invert --> ClipRegion (only Windows or VirDevs)
    virtual void invert(
                long nX, long nY,
                long nWidth, long nHeight,
                SalInvert nFlags) override;

    virtual void invert( sal_uInt32 nPoints, const SalPoint* pPtAry, SalInvert nFlags ) override;

    virtual bool drawEPS(
                long nX, long nY,
                long nWidth, long nHeight,
                void* pPtr,
                sal_uLong nSize ) override;

    virtual bool blendBitmap(
                const SalTwoRect&,
                const SalBitmap& rBitmap ) override;

    virtual bool blendAlphaBitmap(
                const SalTwoRect&,
                const SalBitmap& rSrcBitmap,
                const SalBitmap& rMaskBitmap,
                const SalBitmap& rAlphaBitmap ) override;

    virtual bool drawAlphaBitmap(
                const SalTwoRect&,
                const SalBitmap& rSourceBitmap,
                const SalBitmap& rAlphaBitmap ) override;

    /** draw transformed bitmap (maybe with alpha) where Null, X, Y define the coordinate system */
    virtual bool drawTransformedBitmap(
                const basegfx::B2DPoint& rNull,
                const basegfx::B2DPoint& rX,
                const basegfx::B2DPoint& rY,
                const SalBitmap& rSourceBitmap,
                const SalBitmap* pAlphaBitmap) override;

    virtual bool drawAlphaRect(
                    long nX, long nY,
                    long nWidth, long nHeight,
                    sal_uInt8 nTransparency ) override;

    virtual bool drawGradient(const tools::PolyPolygon& rPolygon, const Gradient& rGradient) override;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

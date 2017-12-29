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

#ifndef INCLUDED_VCL_INC_HAIKU_SALGDI_HXX
#define INCLUDED_VCL_INC_HAIKU_SALGDI_HXX

#include <salgdi.hxx>

#include <cstdio>

class GlyphCache;
class HaikuView;
class HaikuSalFrame;
class HaikuSalVirtualDevice;

class HaikuSalGraphics : public SalGraphics
{
    friend class HaikuSalLayout; // FIXME
public:
    HaikuSalGraphics(BView* view);
    HaikuSalGraphics(BView* view, HaikuSalFrame* frame);
    HaikuSalGraphics(BView* view, HaikuSalVirtualDevice* vd);
    virtual ~HaikuSalGraphics() override;

    SalGraphicsImpl* GetImpl() const override;
    rgb_color mTextColor;

public:
    // public SalGraphics methods, the interface to the independent vcl part

    BView* getView() { return mpView; }
    // get device resolution
    virtual void            GetResolution( sal_Int32& rDPIX, sal_Int32& rDPIY ) override;
    // get the depth of the device
    virtual sal_uInt16          GetBitCount() const override;
    // get the width of the device
    virtual long            GetGraphicsWidth() const override;

    // set the clip region to empty
    virtual void            ResetClipRegion() override;

    // set the line color to transparent (= don't draw lines)
    virtual void            SetLineColor() override;
    // set the line color to a specific color
    virtual void            SetLineColor( SalColor nSalColor ) override;
    // set the fill color to transparent (= don't fill)
    virtual void            SetFillColor() override;
    // set the fill color to a specific color, shapes will be
    // filled accordingly
    virtual void            SetFillColor( SalColor nSalColor ) override;
    // enable/disable XOR drawing
    virtual void            SetXORMode( bool bSet ) override;
    // set line color for raster operations
    virtual void            SetROPLineColor( SalROPColor nROPColor ) override;
    // set fill color for raster operations
    virtual void            SetROPFillColor( SalROPColor nROPColor ) override;
    // set the text color to a specific color
    virtual void            SetTextColor( SalColor nSalColor ) override;
    // set the font
    virtual void            SetFont( FontSelectPattern*, int nFallbackLevel ) override;
    // get the current font's metrics
    virtual void            GetFontMetric( ImplFontMetricDataRef&, int nFallbackLevel ) override;
    // get the repertoire of the current font
    virtual const FontCharMapRef GetFontCharMap() const override;
    // get the layout capabilities of the current font
    virtual bool GetFontCapabilities(vcl::FontCapabilities &rGetFontCapabilities) const override;
    // graphics must fill supplied font list
    virtual void            GetDevFontList( PhysicalFontCollection* ) override;
    // graphics must drop any cached font info
    virtual void            ClearDevFontCache() override;
    virtual bool            AddTempDevFont( PhysicalFontCollection*, const OUString& rFileURL, const OUString& rFontName ) override;
    // CreateFontSubset: a method to get a subset of glyhps of a font
    // inside a new valid font file
    // returns TRUE if creation of subset was successful
    // parameters: rToFile: contains a osl file URL to write the subset to
    //             pFont: describes from which font to create a subset
    //             pGlyphIDs: the glyph ids to be extracted
    //             pEncoding: the character code corresponding to each glyph
    //             pWidths: the advance widths of the correspoding glyphs (in PS font units)
    //             nGlyphs: the number of glyphs
    //             rInfo: additional outgoing information
    // implementation note: encoding 0 with glyph id 0 should be added implicitly
    // as "undefined character"
    virtual bool            CreateFontSubset( const OUString& rToFile,
                                              const PhysicalFontFace*,
                                              const sal_GlyphId* pGlyphIDs,
                                              const sal_uInt8* pEncoding,
                                              sal_Int32* pWidths,
                                              int nGlyphs,
                                              FontSubsetInfo& rInfo // out parameter
                                              ) override;

    // GetFontEncodingVector: a method to get the encoding map Unicode
    // to font encoded character; this is only used for type1 fonts and
    // may return NULL in case of unknown encoding vector
    // if ppNonEncoded is set and non encoded characters (that is type1
    // glyphs with only a name) exist it is set to the corresponding
    // map for non encoded glyphs; the encoding vector contains -1
    // as encoding for these cases
    virtual const Ucs2SIntMap* GetFontEncodingVector( const PhysicalFontFace*, const Ucs2OStrMap** ppNonEncoded, std::set<sal_Unicode> const** ) override;

    // GetEmbedFontData: gets the font data for a font marked
    // embeddable by GetDevFontList or NULL in case of error
    // parameters: pFont: describes the font in question
    //             pWidths: the widths of all glyphs from char code 0 to 255
    //                      pWidths MUST support at least 256 members;
    //             rInfo: additional outgoing information
    //             pDataLen: out parameter, contains the byte length of the returned buffer
    virtual const void* GetEmbedFontData( const PhysicalFontFace*,
                                          const sal_Ucs* pUnicodes,
                                          sal_Int32* pWidths,
                                          size_t nLen,
                                          FontSubsetInfo& rInfo,
                                          long* pDataLen ) override;
    // frees the font data again
    virtual void            FreeEmbedFontData( const void* pData, long nDataLen ) override;
    virtual void            GetGlyphWidths( const PhysicalFontFace*,
                                            bool bVertical,
                                            std::vector< sal_Int32 >& rWidths,
                                            Ucs2UIntMap& rUnicodeEnc ) override;
            int             GetMinKashidaWidth();

    virtual bool            GetGlyphBoundRect( sal_GlyphId, Rectangle& ) override;
    virtual bool            GetGlyphOutline( sal_GlyphId, basegfx::B2DPolyPolygon& ) override;

    virtual SalLayout*      GetTextLayout( ImplLayoutArgs&, int nFallbackLevel ) override;
    virtual void            DrawSalLayout( const CommonSalLayout& ) override;
    virtual void            DrawServerFontLayout( const GenericSalLayout&, const FreetypeFont& ) override {};

    virtual bool            supportsOperation( OutDevSupportType ) const override;
    // Query the platform layer for control support
    virtual bool            IsNativeControlSupported( ControlType nType, ControlPart nPart ) override;

    virtual SystemGraphicsData GetGraphicsData() const override;

    /// Update settings based on the platform values
    static void updateSettingsNative( AllSettings& rSettings );

protected:
    virtual bool        setClipRegion( const vcl::Region& ) override;
    // draw --> LineColor and FillColor and RasterOp and ClipRegion
    virtual void        drawPixel( long nX, long nY ) override;
    virtual void        drawPixel( long nX, long nY, SalColor nSalColor ) override;
    virtual void        drawLine( long nX1, long nY1, long nX2, long nY2 ) override;
    virtual void        drawRect( long nX, long nY, long nWidth, long nHeight ) override;
    virtual void        drawPolyLine( sal_uInt32 nPoints, const SalPoint* pPtAry ) override;
    virtual void        drawPolygon( sal_uInt32 nPoints, const SalPoint* pPtAry ) override;
    virtual void        drawPolyPolygon( sal_uInt32 nPoly, const sal_uInt32* pPoints, PCONSTSALPOINT* pPtAry ) override;
    virtual bool        drawPolyPolygon( const basegfx::B2DPolyPolygon&, double fTransparency ) override;
    virtual bool        drawPolyLine(
        const basegfx::B2DPolygon&,
        double fTransparency,
        const basegfx::B2DVector& rLineWidth,
        basegfx::B2DLineJoin,
        css::drawing::LineCap,
        double fMiterMinimumAngle) override;
    virtual bool        drawPolyLineBezier( sal_uInt32 nPoints, const SalPoint* pPtAry, const sal_uInt8* pFlgAry ) override;
    virtual bool        drawPolygonBezier( sal_uInt32 nPoints, const SalPoint* pPtAry, const sal_uInt8* pFlgAry ) override;
    virtual bool        drawPolyPolygonBezier( sal_uInt32 nPoly, const sal_uInt32* pPoints, const SalPoint* const* pPtAry, const sal_uInt8* const* pFlgAry ) override;
    virtual bool        drawGradient( const tools::PolyPolygon&, const Gradient& ) override { return false; };

    // CopyArea --> No RasterOp, but ClipRegion
    virtual void        copyArea( long nDestX, long nDestY, long nSrcX, long nSrcY, long nSrcWidth,
                                  long nSrcHeight, bool bWindowInvalidate ) override;

    // CopyBits and DrawBitmap --> RasterOp and ClipRegion
    // CopyBits() --> pSrcGraphics == NULL, then CopyBits on same Graphics
    virtual void        copyBits( const SalTwoRect& rPosAry, SalGraphics* pSrcGraphics ) override;
    virtual void        drawBitmap( const SalTwoRect& rPosAry, const SalBitmap& rSalBitmap ) override;
    virtual void        drawBitmap( const SalTwoRect& rPosAry,
                                    const SalBitmap& rSalBitmap,
                                    const SalBitmap& rTransparentBitmap ) override;
    virtual void        drawMask( const SalTwoRect& rPosAry,
                                  const SalBitmap& rSalBitmap,
                                  SalColor nMaskColor ) override;

    virtual SalBitmap*  getBitmap( long nX, long nY, long nWidth, long nHeight ) override;
    virtual SalColor    getPixel( long nX, long nY ) override;

    // invert --> ClipRegion (only Windows or VirDevs)
    virtual void        invert( long nX, long nY, long nWidth, long nHeight, SalInvert nFlags) override;
    virtual void        invert( sal_uInt32 nPoints, const SalPoint* pPtAry, SalInvert nFlags ) override;

    virtual bool        drawEPS( long nX, long nY, long nWidth, long nHeight, void* pPtr, sal_uIntPtr nSize ) override;

    // native widget rendering methods that require mirroring
    virtual bool        hitTestNativeControl( ControlType nType, ControlPart nPart, const Rectangle& rControlRegion,
                                              const Point& aPos, bool& rIsInside ) override;
    virtual bool        drawNativeControl( ControlType nType, ControlPart nPart, const Rectangle& rControlRegion,
                                           ControlState nState, const ImplControlValue& aValue,
                                           const OUString& aCaption ) override;
    virtual bool        getNativeControlRegion( ControlType nType, ControlPart nPart, const Rectangle& rControlRegion, ControlState nState,
                                                const ImplControlValue& aValue, const OUString& aCaption,
                                                Rectangle &rNativeBoundingRegion, Rectangle &rNativeContentRegion ) override;

    virtual bool        blendBitmap( const SalTwoRect&,
                                     const SalBitmap& rBitmap ) override;

    virtual bool        blendAlphaBitmap( const SalTwoRect&,
                                          const SalBitmap& rSrcBitmap,
                                          const SalBitmap& rMaskBitmap,
                                          const SalBitmap& rAlphaBitmap ) override;

    virtual bool        drawAlphaBitmap( const SalTwoRect&,
                                         const SalBitmap& rSourceBitmap,
                                         const SalBitmap& rAlphaBitmap ) override;
    virtual bool       drawTransformedBitmap(
                           const basegfx::B2DPoint& rNull,
                           const basegfx::B2DPoint& rX,
                           const basegfx::B2DPoint& rY,
                           const SalBitmap& rSourceBitmap,
                           const SalBitmap* pAlphaBitmap) override;
    virtual bool       drawAlphaRect( long nX, long nY, long nWidth, long nHeight, sal_uInt8 nTransparency ) override;
private:
    std::unique_ptr<GlyphCache> mpGlyphCache;
    BView* mpView;
    HaikuSalFrame* mpFrame;
    HaikuSalVirtualDevice* mpVirDev;
    SalColor mpLineColor;
    SalColor mpFillColor;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

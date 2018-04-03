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

#include <svsys.h>
#include <vcl/bitmap.hxx>
#include <haiku/salbmp.hxx>
#include <cppuhelper/basemutex.hxx>

#define TRACE printf("Trace:%s:%d - %s\n", __FILE__, __LINE__, __FUNCTION__);


BitmapBuffer* ImplCreateDIB(
    const Size& rSize,
    sal_uInt16 nBitCount,
    const BitmapPalette& rPal)
{
    assert(
          (nBitCount ==  0
        || nBitCount ==  1
        || nBitCount ==  4
        || nBitCount ==  8
        || nBitCount == 16
        || nBitCount == 24
        || nBitCount == 32)
        && "Unsupported BitCount!");

    BitmapBuffer* pDIB = nullptr;

    if( rSize.Width() && rSize.Height() )
    {
        try
        {
            pDIB = new BitmapBuffer;
        }
        catch (const std::bad_alloc&)
        {
            pDIB = nullptr;
        }

        if( pDIB )
        {
            const sal_uInt16 nColors = ( nBitCount <= 8 ) ? ( 1 << nBitCount ) : 0;

            switch (nBitCount)
            {
                case 1:
                    pDIB->mnFormat = ScanlineFormat::N1BitLsbPal;
                    break;
                case 4:
                    pDIB->mnFormat = ScanlineFormat::N4BitMsnPal;
                    break;
                case 8:
                    pDIB->mnFormat = ScanlineFormat::N8BitPal;
                    break;
                case 16:
                {
#ifdef OSL_BIGENDIAN
                    pDIB->mnFormat= ScanlineFormat::N16BitTcMsbMask;
#else
                    pDIB->mnFormat= ScanlineFormat::N16BitTcLsbMask;
#endif
                    ColorMaskElement aRedMask(0xf800);
                    aRedMask.CalcMaskShift();
                    ColorMaskElement aGreenMask(0x07e0);
                    aGreenMask.CalcMaskShift();
                    ColorMaskElement aBlueMask(0x001f);
                    aBlueMask.CalcMaskShift();
                    pDIB->maColorMask = ColorMask(aRedMask, aGreenMask, aBlueMask);
                    break;
                }
                default:
                    nBitCount = 32;
                    SAL_FALLTHROUGH;
                case 32:
                {
                    pDIB->mnFormat = (ScanlineFormat::N32BitTcRgba | ScanlineFormat::TopDown);
                    break;
                }
            }

            pDIB->mnFormat |= ScanlineFormat::TopDown;
            pDIB->mnWidth = rSize.Width();
            pDIB->mnHeight = rSize.Height();
            pDIB->mnScanlineSize = AlignedWidth4Bytes( pDIB->mnWidth * nBitCount );
            pDIB->mnBitCount = nBitCount;

            if( nColors )
            {
                pDIB->maPalette = rPal;
                pDIB->maPalette.SetEntryCount( nColors );
            }

            try
            {
                size_t size = pDIB->mnScanlineSize * pDIB->mnHeight;
                pDIB->mpBits = new sal_uInt8[size];
#ifdef __SANITIZE_ADDRESS__
                if (!pDIB->mpBits)
                {   // can only happen with ASAN allocator_may_return_null=1
                    delete pDIB;
                    pDIB = nullptr;
                }
                else
#endif
                {
                    std::memset(pDIB->mpBits, 0, size);
                }
            }
            catch (const std::bad_alloc&)
            {
                delete pDIB;
                pDIB = nullptr;
            }
        }
    }
    else
        pDIB = nullptr;

    return pDIB;
}


HaikuSalBitmap::HaikuSalBitmap()
    : mpDIB(nullptr), mpBitmap(nullptr)
{
//    TRACE
}


HaikuSalBitmap::~HaikuSalBitmap()
{
//    TRACE
    Destroy();
}


bool HaikuSalBitmap::Create( const Size& rSize,
                             sal_uInt16 nBitCount,
                             const BitmapPalette& rPal )
{
//    TRACE
    Destroy();
    mpDIB = ImplCreateDIB( rSize, nBitCount, rPal );
    return mpDIB != nullptr;
}


bool HaikuSalBitmap::Create( const SalBitmap& rSalBmp )
{
    Destroy();

    const HaikuSalBitmap& rBmp = static_cast<const HaikuSalBitmap&>(rSalBmp);

    if (rBmp.mpDIB)
    {
        // TODO: reference counting...
        mpDIB = new BitmapBuffer( *rBmp.mpDIB );
        // TODO: get rid of this when BitmapBuffer gets copy constructor
        try
        {
            size_t size = mpDIB->mnScanlineSize * mpDIB->mnHeight;
            mpDIB->mpBits = new sal_uInt8[size];
            std::memcpy(mpDIB->mpBits, rBmp.mpDIB->mpBits, size);
        }
        catch (const std::bad_alloc&)
        {
            delete mpDIB;
            mpDIB = nullptr;
        }
    }

    return !rBmp.mpDIB || (mpDIB != nullptr);
}


bool HaikuSalBitmap::Create( const SalBitmap& rSalBmp, SalGraphics* pGraphics )
{
    TRACE
    return false;
}


bool HaikuSalBitmap::Create( const SalBitmap& rSalBmp, sal_uInt16 nNewBitCount )
{
    TRACE
    return false;
}


bool HaikuSalBitmap::Create( const css::uno::Reference< css::rendering::XBitmapCanvas >& rBitmapCanvas,
                                    Size& rSize, bool bMask )
{
    TRACE
    return false;
}

void HaikuSalBitmap::Destroy()
{
//    TRACE
    if (mpDIB)
    {
        delete[] mpDIB->mpBits;
        delete mpDIB;
        mpDIB = nullptr;
    }
}


Size HaikuSalBitmap::GetSize() const
{
//    TRACE
    Size aSize;

    if (mpDIB)
    {
        aSize.Width() = mpDIB->mnWidth;
        aSize.Height() = mpDIB->mnHeight;
    }

    return aSize;
}

sal_uInt16 HaikuSalBitmap::GetBitCount() const
{
//    TRACE
    sal_uInt16 nBitCount;

    if (mpDIB)
        nBitCount = mpDIB->mnBitCount;
    else
        nBitCount = 0;

    return nBitCount;
}

BitmapBuffer* HaikuSalBitmap::AcquireBuffer( BitmapAccessMode nMode )
{
//    TRACE
    return mpDIB;
}

void HaikuSalBitmap::ReleaseBuffer( BitmapBuffer* pBuffer, BitmapAccessMode nMode )
{
//    TRACE
}

bool HaikuSalBitmap::GetSystemData( BitmapSystemData& rData )
{
    TRACE
    return false;
}


bool HaikuSalBitmap::Scale( const double& rScaleX, const double& rScaleY, BmpScaleFlag nScaleFlag )
{
    TRACE
    return false;
}

bool HaikuSalBitmap::Replace( const Color& rSearchColor, const Color& rReplaceColor, sal_uInt8 nTol )
{
    TRACE
    return false;
}

BBitmap* HaikuSalBitmap::GetBitmap() const
{
    BBitmap* pRetval(nullptr);
    HaikuSalBitmap* pSalRGB = const_cast< HaikuSalBitmap* >(this);

    BitmapBuffer* pRGB = pSalRGB->AcquireBuffer(BitmapAccessMode::Read);
    BitmapBuffer* pExtraRGB = nullptr;

    if(pRGB && ScanlineFormat::N32BitTcBgra != (pRGB->mnFormat & ~ScanlineFormat::TopDown)) {
        SalTwoRect aSalTwoRect(0, 0, pRGB->mnWidth, pRGB->mnHeight, 0, 0, pRGB->mnWidth, pRGB->mnHeight);
        pExtraRGB = StretchAndConvert(
            *pRGB,
            aSalTwoRect,
            ScanlineFormat::N32BitTcBgra);

        pSalRGB->ReleaseBuffer(pRGB, BitmapAccessMode::Write);
        pRGB = pExtraRGB;
    }

    if(pRGB
        && pRGB->mnWidth > 0
        && pRGB->mnHeight > 0
        && ScanlineFormat::N32BitTcBgra == (pRGB->mnFormat & ~ScanlineFormat::TopDown))
    {
        const sal_uInt32 nW(pRGB->mnWidth);
        const sal_uInt32 nH(pRGB->mnHeight);

        pRetval = new BBitmap(BRect(0, 0, nW, nH), B_RGB32);

        if(pRetval) {
            sal_uInt8* pSrcRGB(pRGB->mpBits);
            const sal_uInt32 nExtraRGB(pRGB->mnScanlineSize - (nW * 4));
            const bool bTopDown(pRGB->mnFormat & ScanlineFormat::TopDown);

            for(sal_uInt32 y(0); y < nH; y++)
            {
                const sal_uInt32 nYInsert(bTopDown ? y : nH - y - 1);
                sal_uInt8* targetPixels = static_cast<sal_uInt8*>(pRetval->Bits()) + (nYInsert * pRetval->BytesPerRow());

                memcpy(targetPixels, pSrcRGB, nW * 4);
                pSrcRGB += nW * 4 + nExtraRGB;
            }
        } else {
            delete pRetval;
            pRetval = nullptr;
        }
    }

    if(pExtraRGB) {
        delete [] pExtraRGB->mpBits;
        delete pExtraRGB;
    } else {
        pSalRGB->ReleaseBuffer(pRGB, BitmapAccessMode::Read);
    }

    return pRetval;
}

BBitmap* HaikuSalBitmap::GetBitmap(const HaikuSalBitmap& rAlphaSource) const
{
    BBitmap* pRetval(nullptr);
    HaikuSalBitmap* pSalRGB = const_cast< HaikuSalBitmap* >(this);

    BitmapBuffer* pRGB = pSalRGB->AcquireBuffer(BitmapAccessMode::Read);
    BitmapBuffer* pExtraRGB = nullptr;

    if(pRGB && ScanlineFormat::N24BitTcBgr != (pRGB->mnFormat & ~ScanlineFormat::TopDown)) {
        SalTwoRect aSalTwoRect(0, 0, pRGB->mnWidth, pRGB->mnHeight, 0, 0, pRGB->mnWidth, pRGB->mnHeight);
        pExtraRGB = StretchAndConvert(
            *pRGB,
            aSalTwoRect,
            ScanlineFormat::N24BitTcBgr);

        pSalRGB->ReleaseBuffer(pRGB, BitmapAccessMode::Write);
        pRGB = pExtraRGB;
    }

    HaikuSalBitmap* pSalA = const_cast< HaikuSalBitmap* >(&rAlphaSource);

    BitmapBuffer* pA = pSalA->AcquireBuffer(BitmapAccessMode::Read);
    BitmapBuffer* pExtraA = nullptr;

    if(pA && ScanlineFormat::N8BitPal != (pA->mnFormat & ~ScanlineFormat::TopDown))
    {
        // convert alpha bitmap to ScanlineFormat::N8BitPal format if not yet in that format
        SalTwoRect aSalTwoRect(0, 0, pA->mnWidth, pA->mnHeight, 0, 0, pA->mnWidth, pA->mnHeight);
        const BitmapPalette& rTargetPalette = Bitmap::GetGreyPalette(256);

        pExtraA = StretchAndConvert(
            *pA,
            aSalTwoRect,
            ScanlineFormat::N8BitPal,
            &rTargetPalette);

        pSalA->ReleaseBuffer(pA, BitmapAccessMode::Read);
        pA = pExtraA;
    }

    if(pRGB
        && pA
        && pRGB->mnWidth > 0
        && pRGB->mnHeight > 0
        && pRGB->mnWidth == pA->mnWidth
        && pRGB->mnHeight == pA->mnHeight
        && ScanlineFormat::N24BitTcBgr == (pRGB->mnFormat & ~ScanlineFormat::TopDown)
        && ScanlineFormat::N8BitPal == (pA->mnFormat & ~ScanlineFormat::TopDown))
    {
        const sal_uInt32 nW(pRGB->mnWidth);
        const sal_uInt32 nH(pRGB->mnHeight);

        pRetval = new BBitmap(BRect(0, 0, nW, nH), B_RGBA32);

        if(pRetval) {
            sal_uInt8* pSrcRGB(pRGB->mpBits);
            sal_uInt8* pSrcA(pA->mpBits);
            const sal_uInt32 nExtraRGB(pRGB->mnScanlineSize - (nW * 3));
            const sal_uInt32 nExtraA(pA->mnScanlineSize - nW);
            const bool bTopDown(pRGB->mnFormat & ScanlineFormat::TopDown);
            const bool bATopDown(pA->mnFormat & ScanlineFormat::TopDown);

            for(sal_uInt32 y(0); y < nH; y++)
            {
                const sal_uInt32 nYInsert(bTopDown ? y : nH - y - 1);
                const sal_uInt32 nYA(bATopDown ? nH - y - 1 : y);
                sal_uInt8* targetPixels = static_cast<sal_uInt8*>(pRetval->Bits()) + (nYInsert * pRetval->BytesPerRow());
                sal_uInt8* pSrcAInv = pSrcA + nYA * pA->mnScanlineSize;

                for(sal_uInt32 x(0); x < nW; x++)
                {
                    *targetPixels++ = *pSrcRGB++;
                    *targetPixels++ = *pSrcRGB++;
                    *targetPixels++ = *pSrcRGB++;
                    *targetPixels++ = 0xff - *pSrcAInv++;
                }

                pSrcRGB += nExtraRGB;
                pSrcAInv += nExtraA;
            }
        } else {
            delete pRetval;
            pRetval = nullptr;
        }
    }

    if(pExtraA)
    {
        // #i123478# shockingly, BitmapBuffer does not free the memory it is controlling
        // in its destructor, this *has to be done handish*. Doing it here now
        delete[] pExtraA->mpBits;
        delete pExtraA;
    }
    else
    {
        pSalA->ReleaseBuffer(pA, BitmapAccessMode::Read);
    }

    if(pExtraRGB) {
        delete [] pExtraRGB->mpBits;
        delete pExtraRGB;
    } else {
        pSalRGB->ReleaseBuffer(pRGB, BitmapAccessMode::Read);
    }

    return pRetval;
}

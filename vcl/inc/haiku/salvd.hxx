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

#ifndef INCLUDED_VCL_INC_HAIKU_SALVD_HXX
#define INCLUDED_VCL_INC_HAIKU_SALVD_HXX

#include <salvd.hxx>

class HaikuSalGraphics;

class HaikuSalVirtualDevice : public SalVirtualDevice
{
    friend class HaikuSalGraphics; // FIXME HACK to get to BBitmap
        // to copy the pixels
private:
    BView *mpView;
    BBitmap *mpBmp;
    bool mbForeign;
    HaikuSalGraphics *mpGraphics;
    sal_uInt16 mnBitCount;
    bool mbGraphics;
    long mnWidth;
    long mnHeight;

public:
    HaikuSalVirtualDevice(HaikuSalGraphics *pGraphics);
    virtual ~HaikuSalVirtualDevice() override;

    // SalGraphics or NULL, but two Graphics for all SalVirtualDevices
    // must be returned
    SalGraphics*    AcquireGraphics() override;
    void            ReleaseGraphics( SalGraphics* pGraphics ) override;

    // Set new size, without saving the old contents
    bool            SetSize( long nNewDX, long nNewDY ) override;

    long int        GetWidth() const { return mnWidth; }
    long int        GetHeight() const { return mnHeight; }
};

#endif // INCLUDED_VCL_INC_SALVD_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

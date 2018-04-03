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
#include <headless/svpgdi.hxx>

#include <cstdio>

class BView;
class GlyphCache;
class HaikuView;
class HaikuSalFrame;
class HaikuSalVirtualDevice;

class HaikuSalGraphics : public SvpSalGraphics
{
    friend class HaikuSalLayout; // FIXME
public:
    HaikuSalGraphics(BView* view);
    HaikuSalGraphics(BView* view, HaikuSalFrame* frame);
    HaikuSalGraphics(BView* view, HaikuSalVirtualDevice* vd);
    virtual ~HaikuSalGraphics() override;

public:
    // public SalGraphics methods, the interface to the independent vcl part

    BView* getView() { return mpView; }
    // get device resolution
    virtual void            GetResolution( sal_Int32& rDPIX, sal_Int32& rDPIY ) override;
    // get the depth of the device
    virtual bool        drawNativeControl( ControlType nType, ControlPart nPart,
                                               const tools::Rectangle& rControlRegion,
                                               ControlState nState, const ImplControlValue& aValue,
                                               const OUString& rCaption ) override;
    virtual bool        IsNativeControlSupported( ControlType nType, ControlPart nPart ) override;
    virtual bool        getNativeControlRegion( ControlType nType, ControlPart nPart,
                                                    const tools::Rectangle& rControlRegion,
                                                    ControlState nState,
                                                    const ImplControlValue& aValue,
                                                    const OUString& rCaption,
                                                    tools::Rectangle &rNativeBoundingRegion,
                                                    tools::Rectangle &rNativeContentRegion ) override;
#if ENABLE_CAIRO_CANVAS

    virtual bool        SupportsCairo() const override;
    virtual cairo::SurfaceSharedPtr CreateSurface(const cairo::CairoSurfaceSharedPtr& rSurface) const override;
    virtual cairo::SurfaceSharedPtr CreateSurface(const OutputDevice& rRefDevice, int x, int y, int width, int height) const override;

#endif
private:
    std::unique_ptr<GlyphCache> mpGlyphCache;
    BView* mpView;
    HaikuSalFrame* mpFrame;
    HaikuSalVirtualDevice* mpVirDev;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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
#include <haiku/saldata.hxx>
#include <haiku/salinst.hxx>
#include <haiku/salbmp.hxx>
#include <haiku/salgdi.hxx>
#include <haiku/salframe.hxx>
#include <haiku/salvd.hxx>
#include <sallayout.hxx>
#include <vcl/sysdata.hxx>
#include <vcl/fontcharmap.hxx>


#include "unx/printergfx.hxx"
#include "unx/genpspgraphics.h"
#include "unx/glyphcache.hxx"
#include "unx/fc_fontoptions.hxx"
#include "PhysicalFontFace.hxx"
#include "impfont.hxx"
#include "impfontmetricdata.hxx"
#include "cairo_haiku_cairo.hxx"

#include <memory>

#define TRACE printf("Trace:%s:%d - %s\n", __FILE__, __LINE__, __FUNCTION__);


HaikuSalGraphics::HaikuSalGraphics(BView* view)
    : SvpSalGraphics()
{
    mpGlyphCache.reset(new GlyphCache);
    mpView = view;
    mpFrame = nullptr;
    mpVirDev = nullptr;
}

HaikuSalGraphics::HaikuSalGraphics(BView* view, HaikuSalFrame* frame)
    : SvpSalGraphics()
{
    mpGlyphCache.reset(new GlyphCache);
    mpView = view;
    mpFrame = frame;
    mpVirDev = nullptr;
}

HaikuSalGraphics::HaikuSalGraphics(BView* view, HaikuSalVirtualDevice* vd)
    : SvpSalGraphics()
{
    mpGlyphCache.reset(new GlyphCache);
    mpView = view;
    mpFrame = nullptr;
    mpVirDev = vd;
}

HaikuSalGraphics::~HaikuSalGraphics()
{
    TRACE
}

void HaikuSalGraphics::GetResolution( sal_Int32& rDPIX, sal_Int32& rDPIY )
{
    rDPIX = 96;
    rDPIY = 96;
}

bool HaikuSalGraphics::drawNativeControl( ControlType nType, ControlPart nPart,
                                               const tools::Rectangle& rControlRegion,
                                               ControlState nState, const ImplControlValue& aValue,
                                               const OUString& rCaption )
{
    return false;
}

bool HaikuSalGraphics::IsNativeControlSupported( ControlType nType, ControlPart nPart )
{
    return false;
}

bool HaikuSalGraphics::getNativeControlRegion( ControlType nType, ControlPart nPart,
                                                    const tools::Rectangle& rControlRegion,
                                                    ControlState nState,
                                                    const ImplControlValue& aValue,
                                                    const OUString& rCaption,
                                                    tools::Rectangle &rNativeBoundingRegion,
                                                    tools::Rectangle &rNativeContentRegion )
{
    return false;
}

#if ENABLE_CAIRO_CANVAS

bool HaikuSalGraphics::SupportsCairo() const
{
    return true;
}

cairo::SurfaceSharedPtr HaikuSalGraphics::CreateSurface(const cairo::CairoSurfaceSharedPtr& rSurface) const
{
    return cairo::SurfaceSharedPtr(new cairo::HaikuSurface(rSurface));
}

cairo::SurfaceSharedPtr HaikuSalGraphics::CreateSurface(const OutputDevice& /*rRefDevice*/, int x, int y, int width, int height) const
{
    return cairo::SurfaceSharedPtr(new cairo::HaikuSurface(this, x, y, width, height));
}

#endif


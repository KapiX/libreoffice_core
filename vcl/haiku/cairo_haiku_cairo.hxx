/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_CANVAS_SOURCE_CAIRO_CAIRO_HAIKU_CAIRO_HXX
#define INCLUDED_CANVAS_SOURCE_CAIRO_CAIRO_HAIKU_CAIRO_HXX

#include <sal/config.h>

#include <sal/types.h>

#include <vcl/cairo.hxx>

class HaikuSalGraphics;
class OutputDevice;

namespace cairo {

    class HaikuSurface : public Surface
    {
        const HaikuSalGraphics* mpGraphics;
        cairo_t* cr;
        CairoSurfaceSharedPtr mpSurface;
    public:
        /// takes over ownership of passed cairo_surface
        explicit HaikuSurface(const CairoSurfaceSharedPtr& pSurface);
        /// create surface on subarea of given drawable
        explicit HaikuSurface(const HaikuSalGraphics* pGraphics, int x, int y, int width, int height);

        // Surface interface
        virtual CairoSharedPtr getCairo() const override;
        virtual CairoSurfaceSharedPtr getCairoSurface() const override { return mpSurface; }
        virtual SurfaceSharedPtr getSimilar(int nContentType, int width, int height) const override;

        virtual VclPtr<VirtualDevice> createVirtualDevice() const override;

        virtual void flush() const override;

        virtual ~HaikuSurface() override;
    };

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

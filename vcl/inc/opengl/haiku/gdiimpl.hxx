/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_VCL_INC_OPENGL_HAIKU_GDIIMPL_HXX
#define INCLUDED_VCL_INC_OPENGL_HAIKU_GDIIMPL_HXX

#include <vcl/dllapi.h>

#include "openglgdiimpl.hxx"
#include <vcl/opengl/OpenGLContext.hxx>

class VCL_DLLPUBLIC HaikuOpenGLSalGraphicsImpl : public OpenGLSalGraphicsImpl
{
public:
    HaikuOpenGLSalGraphicsImpl(SalGraphics& rParent, SalGeometryProvider *pProvider);
    virtual ~HaikuOpenGLSalGraphicsImpl() override;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

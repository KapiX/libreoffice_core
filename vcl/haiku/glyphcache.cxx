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

#include "rtl/ustring.hxx"
#include "osl/module.h"
#include "osl/thread.h"

#include "haiku/salgdi.hxx"

#include "glyphcache.hxx"

HaikuGlyphCache::~HaikuGlyphCache()
{
    /*if( !ImplGetSVData() )
        return;*/

    /*//Why do this here, move into dtor/shutdown of display?
    SalDisplay* pSalDisp = vcl_sal::getSalDisplay(GetGenericData());
    Display* const pX11Disp = pSalDisp->GetDisplay();
    int nMaxScreens = pSalDisp->GetXScreenCount();
    XRenderPeer& rRenderPeer = XRenderPeer::GetInstance();

    for( int i = 0; i < nMaxScreens; i++ )
    {
        SalDisplay::RenderEntryMap& rMap = pSalDisp->GetRenderEntries( SalX11Screen (i) );
        for( SalDisplay::RenderEntryMap::iterator it = rMap.begin(); it != rMap.end(); ++it )
        {
            if( it->second.m_aPixmap )
                ::XFreePixmap( pX11Disp, it->second.m_aPixmap );
            if( it->second.m_aPicture )
                rRenderPeer.FreePicture( it->second.m_aPicture );
        }
        rMap.clear();
    }*/
}

HaikuGlyphCache::HaikuGlyphCache()
{
}

namespace
{
    struct GlyphCacheHolder
    {
    private:
        HaikuGlyphCache* m_pHaikuGlyphCache;

        GlyphCacheHolder(const GlyphCacheHolder&) = delete;
        GlyphCacheHolder& operator=(const GlyphCacheHolder&) = delete;

    public:
        GlyphCacheHolder()
        {
            m_pHaikuGlyphCache = new HaikuGlyphCache;
        }
        void release()
        {
            delete m_pHaikuGlyphCache;
            m_pHaikuGlyphCache = nullptr;
        }
        HaikuGlyphCache& getGlyphCache()
        {
            return *m_pHaikuGlyphCache;
        }
        ~GlyphCacheHolder()
        {
            release();
        }
    };

    struct theGlyphCacheHolder :
        public rtl::Static<GlyphCacheHolder, theGlyphCacheHolder>
    {};
}

HaikuGlyphCache& HaikuGlyphCache::GetInstance()
{
    return theGlyphCacheHolder::get().getGlyphCache();
}

void HaikuGlyphCache::KillInstance()
{
    return theGlyphCacheHolder::get().release();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

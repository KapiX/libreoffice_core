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

#ifndef INCLUDED_VCL_INC_HAIKU_SALFRAME_HXX
#define INCLUDED_VCL_INC_HAIKU_SALFRAME_HXX

#include <salframe.hxx>
#include <svsys.h>


const int SAL_MSG_USEREVENT = 'smue';

class HaikuSalFrame;

class HaikuView : public BView
{
private:
    HaikuSalFrame* mpFrame;
public:
    HaikuView(BRect rect, HaikuSalFrame* pFrame);
    virtual ~HaikuView();

    void Draw(BRect updateRect);
    void MouseMoved(BPoint point, uint32 transit, const BMessage* message);
    void MouseDown(BPoint point);
    void MouseUp(BPoint point);
    void KeyDown(const char* bytes, int32 numBytes);
    void KeyUp(const char* bytes, int32 numBytes);
    void FrameResized(float width, float height);

    HaikuSalFrame* getFrame() { return mpFrame; }
};

class HaikuWindow : public BWindow
{
private:
    HaikuSalFrame* mpFrame;
public:
    HaikuWindow(HaikuSalFrame* frame, uint32 flags = 0)
        : BWindow(BRect(50, 50, 500, 500), "VCLWindow", B_DOCUMENT_WINDOW, flags),
          mpFrame(frame)
        {}
    ~HaikuWindow() {}
    void MessageReceived(BMessage* message);
};


// proxy structure
// FIXME Haiku does weird things when the data is in the class
// when I added mpParent I started getting memory corruptions
struct HaikuSalFramePrivate {
    HaikuWindow *mpWindow;
    HaikuSalFrame *mpParent;
    BBitmap* mpBmp;
    int padding[22];
};


class HaikuSalFrame : public SalFrame
{
    friend class HaikuView;
    friend class HaikuSalGraphics; // FIXME HACK to get to BBitmap
        // to copy the pixels
public:
    HaikuSalFrame(HaikuSalFrame* pParent, SalFrameStyleFlags nStyle);
    virtual ~HaikuSalFrame() override;

    virtual SalGraphics*        AcquireGraphics() override;
    virtual void                ReleaseGraphics( SalGraphics* pGraphics ) override;
    virtual bool                PostEvent(ImplSVEvent* pData) override;
    virtual void                SetTitle( const OUString& rTitle ) override;
    virtual void                SetIcon( sal_uInt16 nIcon ) override;
    virtual void                SetMenu( SalMenu* pSalMenu ) override;
    virtual void                DrawMenuBar() override;
    virtual void                SetExtendedFrameStyle( SalExtStyle nExtStyle ) override;
    virtual void                Show( bool bVisible, bool bNoActivate = false ) override;
    virtual void                SetMinClientSize( long nWidth, long nHeight ) override;
    virtual void                SetMaxClientSize( long nWidth, long nHeight ) override;
    virtual void                SetPosSize( long nX, long nY, long nWidth, long nHeight, sal_uInt16 nFlags ) override;
    virtual void                GetClientSize( long& rWidth, long& rHeight ) override;
    virtual void                GetWorkArea( Rectangle& rRect ) override;
    virtual SalFrame*           GetParent() const override;
    virtual void                SetWindowState( const SalFrameState* pState ) override;
    virtual bool                GetWindowState( SalFrameState* pState ) override;
    virtual void                ShowFullScreen( bool bFullScreen, sal_Int32 nDisplay ) override;
    virtual void                StartPresentation( bool bStart ) override;
    virtual void                SetAlwaysOnTop( bool bOnTop ) override;
    virtual void                ToTop( SalFrameToTop nFlags ) override;
    virtual void                SetPointer( PointerStyle ePointerStyle ) override;
    virtual void                CaptureMouse( bool bMouse ) override;
    virtual void                SetPointerPos( long nX, long nY ) override;
    using SalFrame::Flush;
    virtual void                Flush() override;
    virtual void                SetInputContext( SalInputContext* pContext ) override;
    virtual void                EndExtTextInput( EndExtTextInputFlags nFlags ) override;
    virtual OUString            GetKeyName( sal_uInt16 nKeyCode ) override;
    virtual bool                MapUnicodeToKeyCode( sal_Unicode aUnicode, LanguageType aLangType, vcl::KeyCode& rKeyCode ) override;
    virtual LanguageType        GetInputLanguage() override;
    virtual void                UpdateSettings( AllSettings& rSettings ) override;
    virtual void                Beep() override;
    virtual const SystemEnvData*    GetSystemData() const override;
    virtual SalPointerState     GetPointerState() override;
    virtual KeyIndicatorState   GetIndicatorState() override;
    virtual void                SimulateKeyPress( sal_uInt16 nKeyCode ) override;
    virtual void                SetParent( SalFrame* pNewParent ) override;
    virtual bool                SetPluginParent( SystemParentData* pNewParent ) override;
    virtual void                SetScreenNumber( unsigned int ) override;
    virtual void                SetApplicationID( const OUString &rApplicationID ) override;
    virtual void                ResetClipRegion() override;
    virtual void                BeginSetClipRegion( sal_uIntPtr nRects ) override;
    virtual void                UnionClipRegion( long nX, long nY, long nWidth, long nHeight ) override;
    virtual void                EndSetClipRegion() override;

    void UpdateFrameGeometry();
    void Invalidate(BRect updateRect);
private:
    HaikuSalFramePrivate*       mpPrivate;
};


#endif // INCLUDED_VCL_INC_HAIKU_SALFRAME_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

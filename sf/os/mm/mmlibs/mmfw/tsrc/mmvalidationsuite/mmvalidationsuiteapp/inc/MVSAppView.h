// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Part of the MVS Application for TechView
//


#ifndef MVSAPPVIEW_H
#define MVSAPPVIEW_H

#include <gulutil.h>
#include <techview/eikprogi.h>
#include <barsread.h>
#include <mvs/videoplayagent.h>

#include "MVSApp.h"
#include "MVSAppUI.h"

#include "mvsvideocontrol.h"

//
//

class CMVSFileControl : public CCoeControl
    {
public:
    static CMVSFileControl* NewL(const CCoeControl& aContainer, 
                                  const TRect& aRect, 
                                  const TDesC& aText);
	~CMVSFileControl();
   
    void SetTextL(const TDesC& aText); //Sets the text
    void Draw(const TRect&) const;
    void DrawOtherWindows(CGraphicsContext& aGc, const TRect& aDeviceRect, 
                          const CFont* aFont) const;
    TRect& Window();

private:
    void ConstructL(const CCoeControl& aContainer, const TRect& aRect, 
                    const TDesC& aText);
    CMVSFileControl();

private:
	HBufC* iText; //The text to draw    
    TRect iWindow; //The drawing window
    };

//
//

class CMVSStateControl : public CCoeControl
	{
public:
    // Construction / Destruction //
	static CMVSStateControl* NewL(const CCoeControl& aContainer, 
                                  const TRect& aRect, 
                                  const TDesC& aText);
	~CMVSStateControl();
	void SetTextL(const TDesC& aText); //Sets the text
    void Draw(const TRect&) const;
    void DrawOtherWindows(CGraphicsContext& aGc, const TRect& aDeviceRect, 
                          const CFont* aFont) const;
    TRect& Window();

private:
    void ConstructL(const CCoeControl& aContainer, const TRect& aRect, 
                    const TDesC& aText);
    CMVSStateControl();
     
private:
	HBufC* iText; //The text to draw
    TRect iWindow; //The drawing window
    };

//
//

class CMVSTimeControl : public CCoeControl
    {
public:
    static CMVSTimeControl* NewL(const CCoeControl& aContainer, 
                                  const TRect& aRect, 
                                   const TDesC& aText);
	~CMVSTimeControl();
	void SetTextL(const TDesC& aText); //Sets the text
	void SetText2L(const TDesC& aText); //Sets the text
	void ResetText2L();
    void Draw(const TRect&) const;
    void DrawOtherWindows(CGraphicsContext& aGc, const TRect& aDeviceRect, 
                          const CFont* aFont) const;
    TRect& Window();

private:
    // Private Construction //
	void ConstructL(const CCoeControl& aContainer, const TRect& aRect, 
                    const TDesC& aText);
    CMVSTimeControl();
     
private:
	HBufC* iText; //The text to draw
	HBufC* iText2;
    TRect iWindow; //The drawing window
    };


//
//

class CMVSInfoControl : public CCoeControl
    {
public:
    // Construction / Destruction //
	static CMVSInfoControl* NewL(const CCoeControl& aContainer, 
                                  const TRect& aRect, 
                                  const TDesC& aText);
	~CMVSInfoControl();
	void SetTextL(const TDesC& aText); //Sets the text
	void Draw(const TRect&) const;
	TRect& Window();

private:
    // Private Construction //
	void ConstructL(const CCoeControl& aContainer, const TRect& aRect, 
                    const TDesC& aText);
    CMVSInfoControl();
    void DrawMainWindow(CGraphicsContext& aGc, const TRect& aDeviceRect, 
                        const CFont* aFont) const;
private:
	HBufC* iText; //The text to draw
    TRect iWindow; //The drawing window
    };


//
//
class CMVSAppView : public CCoeControl
	{
public:
	static CMVSAppView* NewL(const TRect& aRect, CMVSVideoPlayAgent& aVideoPlayAgent);
	~CMVSAppView();
	

	void SetClipLength(TInt& aClipLen);
	void UpdatePlayProgress(TTimeIntervalMicroSeconds& aPos);
	void ResetProgressBar();

private: // from CCoeControl
    void ConstructL(const TRect& aRect, CMVSVideoPlayAgent& aVideoPlayAgent);
    CMVSAppView();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	void HandlePointerEventL(const TPointerEvent &aPointerEvent);

public:
	CMVSVideoControl* iMainWindowControl;
    CMVSFileControl* iStatusWindowFNameControl;
    CMVSStateControl* iStatusWindowStateControl;
    CMVSTimeControl* iStatusWindowTimeControl;
    CMVSInfoControl* iInfoWindowControl;
    CEikProgressInfo* iProgress;
private:
	TInt iClipLength;    
	};

//
//#endif statements
//
#endif MVSAPPVIEW_H

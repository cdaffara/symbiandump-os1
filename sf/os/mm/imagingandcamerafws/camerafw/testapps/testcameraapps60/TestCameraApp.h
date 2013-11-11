// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __TESTCAMERAAPP_H__
#define __TESTCAMERAAPP_H__

#include <coecntrl.h>
#include <aknappui.h>
#include <aknapp.h>
#include <akndoc.h>

#include <ECam.h>

const TUid KUidTestCameraApp={ 268450181 };

// class CTCamAppView

class CTCamAppView : public CCoeControl
    {
public:
	CTCamAppView();
	~CTCamAppView();
    void ConstructL(const TRect& aRect);
	void DrawImage(CFbsBitmap* aImage) const;
	TPoint DrawBorders(const TSize& aSize) const;
private:
	// from CCoeControl
	void Draw(const TRect& /*aRect*/) const;
    };

// CTCamAppUi

class CTCamAppUi : public CAknAppUi, public MCameraObserver
    {
public:
    void ConstructL();
	~CTCamAppUi();
private:
	// From CEikAppUi
	virtual void HandleCommandL(TInt aCommand);
	// New functions
	void ViewFinderL();
	void CaptureImageL();
	void CaptureVideoL();
	// From MCameraObserver
	virtual void ReserveComplete(TInt aError);
	virtual void PowerOnComplete(TInt aError);
	virtual void ViewFinderFrameReady(CFbsBitmap& aFrame);
	virtual void ImageReady(CFbsBitmap* aBitmap,HBufC8* aData,TInt aError);
	virtual void FrameBufferReady(MFrameBuffer* aFrameBuffer,TInt aError);
private:
	CCamera* iCamera;
    CTCamAppView* iAppView;
    };

// CTCamDocument

class CTCamDocument : public CAknDocument
	{
public:
	CTCamDocument(CEikApplication& aApp);
private:
	 // from CEikDocument
	CEikAppUi* CreateAppUiL();
	};

// CTCamApp

class CTCamApp : public CAknApplication
	{
private:
	// from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};

#endif


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
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>

#include <ecam.h>
#include <ecamadvsettings.h>
#include <ecam/camerasnapshot.h>

const TUid KUidTestCameraApp={ 0x101F7D31 };

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

class CTCamAppUi : public CEikAppUi, public MCameraObserver, public MCameraObserver2
    {
public:
    void ConstructL();
	~CTCamAppUi();
private:
	// From CEikAppUi
	virtual void HandleCommandL(TInt aCommand);
	// New functions
	void ViewFinderL();
	void BurstModeL();
	void SingleShotL();
	void CaptureImageL();
	void CaptureVideoL();
	void IncImgProcAdjustmentL(TUid aTransformation);
	void DecImgProcAdjustmentL(TUid aTransformation);
	void EnableSnapshotL();
	void DisableSnapshot();
	// From MCameraObserver
	virtual void ReserveComplete(TInt aError);
	virtual void PowerOnComplete(TInt aError);
	virtual void ViewFinderFrameReady(CFbsBitmap& aFrame);
	virtual void ImageReady(CFbsBitmap* aBitmap,HBufC8* aData,TInt aError);
	virtual void FrameBufferReady(MFrameBuffer* aFrameBuffer,TInt aError);
	//From MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	
    void CreateDependencyFileL();
    void RemoveDependencyFile();
    
    void SnapshotDataDisplayL();
    
    void HandleEvent1(const TECAMEvent& aEvent);
    void HandleEvent2(const TECAMEvent2& aEvent2);
    
private:
	CCamera* iCamera;
	CCamera::CCameraAdvancedSettings* iCameraAdvSet;
	CCamera::CCameraPresets* iCameraPresets;
	CCamera::CCameraSnapshot* iCameraSnapshot;
	CCamera::CCameraImageProcessing* iCameraImgProc;
    CTCamAppView* iAppView;
    RFs ifsSession;
    };

// CTCamDocument

class CTCamDocument : public CEikDocument
	{
public:
	CTCamDocument(CEikApplication& aApp);
private:
	 // from CEikDocument
	CEikAppUi* CreateAppUiL();
	};

// CTCamApp

class CTCamApp : public CEikApplication
	{
private:
	// from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	};

#endif


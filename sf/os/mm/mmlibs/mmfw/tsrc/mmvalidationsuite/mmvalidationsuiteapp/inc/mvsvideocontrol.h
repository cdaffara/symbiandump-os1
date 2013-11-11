// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MVSVIDEOCONTROL_H
#define MVSVIDEOCONTROL_H

#include <coecntrl.h>
#include <surfaceeventhandler.h>

class CMVSPipControl :
	public CCoeControl,
	public MMMFSurfaceEventHandler
	{
public:
	enum TRenderType
		{
		EUseMmf,
		EUseCrp
		};
		
public:
	static CMVSPipControl* NewL(RWindowTreeNode& aParent);
	~CMVSPipControl();

	// MMMFSurfaceEventHandler
	void MmsehSurfaceCreated(TInt aDisplayId, const TSurfaceId& aId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	void MmsehSurfaceParametersChanged(const TSurfaceId& aId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	void MmsehRemoveSurface(const TSurfaceId& aId);

	void SetRenderType(TRenderType aRenderType);
	void Clear();
	RWindow& ControlWindow() const;

private:
	CMVSPipControl();
	void ConstructL(RWindowTreeNode& aParent);

	void Draw(const TRect& aRect) const;

private:
	TRenderType iRenderType;
	TInt iDisplayId;
	TRect iCropRect;
	};


class CMVSVideoControl : public CCoeControl
	{
public:
	static CMVSVideoControl* NewLC(CMVSVideoPlayAgent& aVideoPlayAgent, TInt aDisplay, RWindowTreeNode& aParent);
	static CMVSVideoControl* NewL(CMVSVideoPlayAgent& aVideoPlayAgent, TInt aDisplay, RWindowTreeNode& aParent);

	~CMVSVideoControl();

	RWindow& ControlWindow() const;
	TInt ScreenNumber() const;
	
	CMVSPipControl& Pip() const;
	void EnablePip(CMVSPipControl::TRenderType aRenderType);
	void DisablePip();

	void SetOverlayTextL(const TDesC& aOverlayText);
	void ClearOverlayText();

private:
	CMVSVideoControl(CMVSVideoPlayAgent& aVideoPlayAgent, TInt aDisplay);
	
	void ConstructL(RWindowTreeNode& aParent);
	void Draw(const TRect& aRect) const;

	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;

private:
	CMVSVideoPlayAgent& iVideoPlayAgent;
	TInt iDisplay;
	CMVSPipControl* iPip;
	
	RBuf    iOverlayText;   // Text to be drawn on top of video
	};

#endif // MVSVIDEOCONTROL_H


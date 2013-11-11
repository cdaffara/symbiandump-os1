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

#ifndef TESTSTEPVCLNT2CRP_H
#define TESTSTEPVCLNT2CRP_H

#include <surfaceeventhandler.h>
#include <graphics/surface.h>

#include "testvideoplayer2.h"

class RTestStepVclnt2CRP : 
	public RTestVclnt2PlayFile,
	public MMMFSurfaceEventHandler
	{
public:
	static RTestStepVclnt2CRP* NewL(const TDesC& aName);
	~RTestStepVclnt2CRP();

	// MMMFSurfaceEventHandler
	void MmsehSurfaceCreated(TInt aDisplayId, const TSurfaceId& aId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	void MmsehSurfaceParametersChanged(const TSurfaceId& aId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	void MmsehRemoveSurface(const TSurfaceId& aId);

protected:
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();

	virtual void HandleOpenCompleteL();

private:
	RTestStepVclnt2CRP(const TDesC& aName);

	void FailTest(const TDesC& aMessage);

private:
	TBool iUseCRP;
	TBool iAddDisplay;
	TBool iAddWindow;
	TSurfaceId iSurfaceId;
	TRect iCropRect;
	RWindow* iSurfaceWindow;
	};

#endif // TESTSTEPVCLNT2CRP_H

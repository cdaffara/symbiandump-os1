
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
//

#ifndef ECAMOBSERVERTEST_H
#define ECAMOBSERVERTEST_H

#include <testframework.h>
#include <ecam.h>
#include <ecamadvsettings.h>

class RECamObserverTest : public RTestStep, public MCameraObserver2
	{
public:
	//MCameraObserver2
	void HandleEvent(const TECAMEvent& aEvent);
	void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);	
	void CheckNotification(const TUid& aEventUid, TVerdict& aResult);	
	void CheckNotificationTwo(const TUid& aEventUid, TVerdict& aResult);	
	void CheckNotificationNeg(const TUid& aEventUid, TVerdict& aResult);
	void CheckNotificationImgProc(const TUid& aEventUid, TInt aParam, TVerdict& aResult);	

protected:
	// from RTestStep;
	TVerdict DoTestStepL()=0;

protected:
	TUid iEventUid;
	TUid iEventUidTwo;
	TInt iError;
	TInt iParam; //for TECAMEvent2 events
	};


class RECamContinousZoomObserverTest : public RTestStep, public MContinuousZoomObserver
	{
public:
	//MContinuousZoomObserver
	void ContinuousZoomProgress(CCamera::CCameraContinuousZoom& aContinuousZoomHandle, TInt aZoomFactor, TInt aError);
	void ContinuousZoomCompleted(CCamera::CCameraContinuousZoom& aContinuousZoomHandle, TInt aFinalZoomFactor, TInt aError);
	TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface);

protected:
	// from RTestStep
	TVerdict DoTestStepL()=0;
	};

#endif // ECAMOBSERVERTEST_H


// ECamObserverTest.cpp

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

#include <ecam.h>

#include "ECamObserverTest.h"


void RECamObserverTest::HandleEvent(const TECAMEvent& aEvent)
	{
	TBool eventEncapsulation = TECAMEvent2::IsEventEncapsulationValid(aEvent);
	
	if(eventEncapsulation)
		{
		TECAMEvent2 ecamEvent = static_cast<const TECAMEvent2&>(aEvent);
		
		iEventUid = ecamEvent.iEventType;
		iParam	  = ecamEvent.iParam;
		iError	= ecamEvent.iErrorCode;
		return;
		}
		
	iEventUidTwo = iEventUid; 
	iEventUid 	= aEvent.iEventType;
	iError  	= aEvent.iErrorCode;
	}
	
void RECamObserverTest::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt aError)
	{
	iError  = aError;
	}
	
void RECamObserverTest::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt aError)
	{
	iError  = aError;
	}
	
void RECamObserverTest::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt aError)
	{
	iError  = aError;
	}
	
void RECamObserverTest::CheckNotification(const TUid& aEventUid, TVerdict& aResult)
	{
    INFO_PRINTF4(_L("Expected event %x, received event %x. Error %d."), aEventUid.iUid, iEventUid.iUid, iError); 
	if (aEventUid != iEventUid || iError != KErrNone)
		{
	    aResult = EFail;
	    }	
	}
	
void RECamObserverTest::CheckNotificationTwo(const TUid& aEventUid, TVerdict& aResult)
	{
    INFO_PRINTF4(_L("Expected event %x, received event %x. Error %d."), aEventUid.iUid, iEventUid.iUid, iError); 
    if (aEventUid != iEventUidTwo || iError != KErrNone)
		{
	    aResult = EFail;
	    }	
	}
	
void RECamObserverTest::CheckNotificationNeg(const TUid& aEventUid, TVerdict& aResult)
	{
    INFO_PRINTF4(_L("Expected event %x, received event %x. Error %d."), aEventUid.iUid, iEventUid.iUid, iError); 
	if (aEventUid != iEventUid || iError == KErrNone)
		{
	    aResult = EFail;
	    }
	}
	
void RECamObserverTest::CheckNotificationImgProc(const TUid& aEventUid, TInt aParam, TVerdict& aResult)
	{
    INFO_PRINTF3(_L("Expected event %x, received event %x."), aEventUid.iUid, iEventUid.iUid); 
    INFO_PRINTF3(_L("Expected param %d, received param %d."), aParam, iParam); 
    INFO_PRINTF2(_L("Error %d."), iError); 
	if (aEventUid != iEventUid || aParam != iParam || iError != KErrNone)
		{
	    aResult = EFail;
	    }	
	}



void RECamContinousZoomObserverTest::ContinuousZoomProgress(CCamera::CCameraContinuousZoom& /*aContinuousZoomHandle*/, TInt aZoomFactor, TInt aError)
	{
	INFO_PRINTF3(_L("ContinuousZoomProgress: New zoom factor achieved = %d, error %d"), aZoomFactor, aError);
	}

void RECamContinousZoomObserverTest::ContinuousZoomCompleted(CCamera::CCameraContinuousZoom& /*aContinuousZoomHandle*/, TInt aFinalZoomFactor, TInt aError)
	{
	INFO_PRINTF3(_L("ContinuousZoomCompleted: Target zoom factor %d reached. Completed with error %d"), aFinalZoomFactor, aError);
	}

TInt RECamContinousZoomObserverTest::CustomInterface(TUid /*aInterface*/, TAny*& /*aPtrInterface*/)
	{
	return KErrNone;
	}

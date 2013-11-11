// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <graphics/surfaceupdateclient.h>
#include <graphics/surface.h>
#include <graphics/suerror.h>
#include "testgceharness.h"

inline void SurfaceUpdateAssert(TInt aCond)	{__ASSERT_ALWAYS(aCond, User::Panic(_L("surfaceupdateclient.cpp; assertion failed"), __LINE__));}

EXPORT_C RSurfaceUpdateSession::RSurfaceUpdateSession() :
	RSessionBase(), 
	iStatusAvailable(NULL), iStatusDisplayed(NULL), iStatusDisplayedXTimes(NULL)
	{
	}

EXPORT_C TInt RSurfaceUpdateSession::Connect(TInt /*aMessageSlots*/)
	{
	iStatusAvailable = NULL;
	iStatusDisplayed = NULL;
	iStatusDisplayedXTimes = NULL;

	CTestGCEHarness* gceHarness = NULL;
	TRAPD(err, gceHarness = CTestGCEHarness::RetrieveL());
	if(err != KErrNone)
		{
		return err;
		}
	return gceHarness->Connect();
	}

EXPORT_C void RSurfaceUpdateSession::Close()	
	{
	iStatusAvailable = NULL;
	iStatusDisplayed = NULL;
	iStatusDisplayedXTimes = NULL;
	
	CTestGCEHarness* gceHarness = NULL;
	TRAPD(err, gceHarness = CTestGCEHarness::RetrieveL());
	if(err != KErrNone)
		{
		return;
		}
	gceHarness->Close();
	}

EXPORT_C TInt RSurfaceUpdateSession::SubmitUpdate(TInt aScreen, const TSurfaceId& aSurfaceId, 
									TInt aBuffer, const TRegion* /*aDirtyRegion*/)
	{
	if(aScreen != KAllScreens || aBuffer < 0 || aSurfaceId.IsNull())
		{
		IssueRequestComplete(KErrArgument);
        return KErrArgument;
        }

	CTestGCEHarness* gceHarness = NULL;
	TRAPD(err, gceHarness = CTestGCEHarness::RetrieveL());
	if(err != KErrNone)
		{
		return err;
		}
	return gceHarness->SubmitUpdate(aBuffer);
	}

EXPORT_C void RSurfaceUpdateSession::NotifyWhenAvailable(TRequestStatus& aStatus)
	{
	CTestGCEHarness* gceHarness = NULL;
	TRAPD(err, gceHarness = CTestGCEHarness::RetrieveL());

	if(err != KErrNone)
		{
		return;
		}
	
	gceHarness->NotifyWhenAvailable(aStatus);
	}
	
EXPORT_C void RSurfaceUpdateSession::NotifyWhenDisplayed(TRequestStatus& aStatus, TTimeStamp& aTimeStamp)
	{
	CTestGCEHarness* gceHarness = NULL;
	TRAPD(err, gceHarness = CTestGCEHarness::RetrieveL());

	if(err != KErrNone)
		{
		return;
		}

	gceHarness->NotifyWhenDisplayed(aStatus, aTimeStamp);
	}

EXPORT_C void RSurfaceUpdateSession::NotifyWhenDisplayedXTimes(TInt aCount, TRequestStatus &aStatus)
	{
	iStatusDisplayedXTimes = &aStatus;
	//we will check aCount validity at submission time (in SubmitUpdate)
	iCount = aCount;
	}

EXPORT_C void RSurfaceUpdateSession::CancelAllUpdateNotifications()
	{
	CTestGCEHarness* gceHarness = NULL;
	TRAPD(err, gceHarness = CTestGCEHarness::RetrieveL());
	if(err != KErrNone)
		{
		return;
		}
	gceHarness->CancelAllNotifications();
	}
	
TVersion RSurfaceUpdateSession::Version() const
	{
	return (TVersion(1, 1, 1));
	}

void RSurfaceUpdateSession::IssueRequestComplete(TInt /*aErr*/)
	{
	}

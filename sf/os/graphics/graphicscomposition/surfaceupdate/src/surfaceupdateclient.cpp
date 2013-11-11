// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/

#include <graphics/surfaceupdateclient.h>
#include <e32debug.h>
#include "surfaceupdate.h"

inline void SurfaceUpdateAssert(TInt aCond)	{__ASSERT_ALWAYS(aCond, User::Panic(_L("surfaceupdateclient.cpp; assertion failed"), __LINE__));}

EXPORT_C RSurfaceUpdateSession::RSurfaceUpdateSession() :
	RSessionBase(), 
	iStatusAvailable(NULL), iStatusDisplayed(NULL), iStatusDisplayedXTimes(NULL), iTimeStamp(NULL), iCount(0)
	{
	}

EXPORT_C TInt RSurfaceUpdateSession::Connect(TInt aMessageSlots)
	{
	if(Handle())
		{
		return KErrAlreadyExists; 
		}
	iStatusAvailable = NULL;
	iStatusDisplayed = NULL;
	iStatusDisplayedXTimes = NULL;
	
	//establish connection to the server
#ifndef TEST_SURFACE_UPDATE
	return CreateSession(KSurfaceUpdateServerName, Version(), aMessageSlots);
#else
	return CreateSession(KTestSurfaceUpdateServerName, Version(), aMessageSlots);
#endif
	}

EXPORT_C void RSurfaceUpdateSession::Close()	
	{
	if(Handle())
		{
		RHandleBase::Close();
		}
	iStatusAvailable = NULL;
	iStatusDisplayed = NULL;
	iStatusDisplayedXTimes = NULL;
	}

EXPORT_C TInt RSurfaceUpdateSession::SubmitUpdate(TInt aScreen, const TSurfaceId& aSurfaceId, 
									TInt aBuffer, const TRegion* aDirtyRegion)
	{
	SurfaceUpdateAssert(Handle() != KNullHandle);

#if defined _DEBUG
	if (aScreen != KAllScreens)
		RDebug::Printf("You should use KAllScreens as the argument for the screen: (other values OK for test)");
#endif

	if(aScreen < 0 || aBuffer < 0 || aSurfaceId.IsNull())
		{
		IssueRequestComplete(KErrArgument);
		return KErrArgument;
		}
	if(iStatusAvailable)
		{
		SendReceive(EUpdateServNotifyWhenAvailable, *iStatusAvailable);
		iStatusAvailable= NULL;
		}
	
	if(iStatusDisplayed)
		{
		TIpcArgs args(iTimeStamp);
		SendReceive(EUpdateServNotifyWhenDisplayed, args, *iStatusDisplayed);
		iStatusDisplayed=NULL;
		}
		
	if(iStatusDisplayedXTimes)
		{
		if(iCount <= 0)
			{
			User::RequestComplete(iStatusDisplayedXTimes, KErrArgument);
			}
		else
			{
			TIpcArgs args(iCount);
			SendReceive(EUpdateServNotifyWhenDisplayedXTimes, 
                     args, *iStatusDisplayedXTimes);
			}	
		iStatusDisplayedXTimes=NULL;
		}

	TPckgC<TSurfaceId> pckgSurfaceId(aSurfaceId);
	TIpcArgs theArgs(aScreen, &pckgSurfaceId, aBuffer);

	if(aDirtyRegion)
		{
		const TInt regionCount = aDirtyRegion->Count();
		if(regionCount > 0)
			{
			TPtrC8 ptrRect(reinterpret_cast<const TUint8*> (aDirtyRegion->RectangleList()), regionCount * sizeof(TRect));
			theArgs.Set(3, &ptrRect);
			}
		}

	//final request for composition 
	return SendReceive(EUpdateServSubmitUpdate, theArgs);
	}

EXPORT_C void RSurfaceUpdateSession::NotifyWhenAvailable(TRequestStatus& aStatus)
	{
	SurfaceUpdateAssert(Handle() != KNullHandle);
	iStatusAvailable = &aStatus;
	}
	
EXPORT_C void RSurfaceUpdateSession::NotifyWhenDisplayed(TRequestStatus& aStatus, TTimeStamp& aTimeStamp)
	{
	SurfaceUpdateAssert(Handle() != KNullHandle);
	iStatusDisplayed = &aStatus;
	iTimeStamp = &aTimeStamp;
	}

EXPORT_C void RSurfaceUpdateSession::NotifyWhenDisplayedXTimes(TInt aCount, TRequestStatus &aStatus)
	{
	SurfaceUpdateAssert(Handle() != KNullHandle);
	iStatusDisplayedXTimes = &aStatus;
	//we will check aCount validity at submission time (in SubmitUpdate)
	iCount = aCount;
	}

EXPORT_C void RSurfaceUpdateSession::CancelAllUpdateNotifications()
	{
	SurfaceUpdateAssert(Handle() != KNullHandle);
	SendReceive(EUpdateServCancelAllNotifications);
	//Signal calling client's TRequestStatus object if server didn't do that (SubmitUpdate is not called)
	//if SubmitUpdate is called, server will take care of signaling status object and following line does nothing
	IssueRequestComplete(KErrCancel);
	}
	
TVersion RSurfaceUpdateSession::Version() const
	{
	return (TVersion(KSurfaceUpdateServMajorVersionNumber, KSurfaceUpdateServMinorVersionNumber, KSurfaceUpdateServBuildVersionNumber));
	}

/*
 Helper function to complete all outstanding requests. 
 
 param - aErr Error code for completion.
 */
void RSurfaceUpdateSession::IssueRequestComplete(TInt aErr)
	{
	if(iStatusAvailable)
		{
		User::RequestComplete(iStatusAvailable, aErr);
		iStatusAvailable= NULL;
		}
	
	if(iStatusDisplayed)
		{
		User::RequestComplete(iStatusDisplayed, aErr);
		iStatusDisplayed=NULL;
		}
		
	if(iStatusDisplayedXTimes)
		{
		User::RequestComplete(iStatusDisplayedXTimes, aErr);
		iStatusDisplayedXTimes=NULL;
		}
	}

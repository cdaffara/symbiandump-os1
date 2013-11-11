// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent
*/

#include <libc/limits.h> //UINT_MAX
#include "t_fpsappeng.h"
#include "t_pseudoapputils.h"

const TInt KSurfaceWidth = 640;
const TInt KSurfaceHeight = 240;
const TUidPixelFormat KSurfaceFormat = EUidPixelFormatXRGB_8888;
const TInt KBytesPerPixel = 4;	// Four bytes per pixel for the format above.

CFpsAppEng::CFpsAppEng(RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow)
: CTimer(CActive::EPriorityStandard),
	iClient(aClient),
	iWindow(aWindow),
	iScreenDevice(aScreenDevice)
	{
	}

CFpsAppEng* CFpsAppEng::NewL(RWsSession& aClient, CWsScreenDevice& aScreenDevice, RWindow& aWindow)
    {
    CFpsAppEng* self = new (ELeave) CFpsAppEng(aClient, aScreenDevice, aWindow);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
    }

CFpsAppEng::~CFpsAppEng()
	{
    if(IsActive())
    	{
	    Cancel();
    	}
	
	User::Free(iBufPtr1);
	DestroySurface();
	    	
	//Pause to allow the test step to kill the app
	User::After(1000000);
	}

void CFpsAppEng::ConstructL()
	{
	CTimer::ConstructL();

	iNoOfPixels = KSurfaceHeight*KSurfaceWidth;
	iNoOfBytes = iNoOfPixels*KBytesPerPixel;

	// Allocate a memory block
	iBufPtr1 = reinterpret_cast<TUint32*>(User::AllocL(iNoOfBytes));
	
	RDebug::Print(_L("Creating Surface manager..."));
	CreateSurfaceManager();
	RDebug::Print(_L("Creating Surface..."));
	CreateSurface(iSurfaceId);
	RDebug::Print(_L("Create Surface update session..."));
	CreateSurfaceUpdateSessionL();
	
	CActiveScheduler::Add(this);
	}

void CFpsAppEng::StartDrawing()
	{	
	//A value of 0 will provoke a E32User-Cbase 46 panic
	After(TTimeIntervalMicroSeconds32(100000));
	}

void CFpsAppEng::CreateSurfaceManager()
	{
	TInt ret = iSurfaceManager.Open();
	if(ret==KErrNone)
		{
		RDebug::Print(_L("Creating surface manager OK"));
		}
	}


void CFpsAppEng::CreateSurface(TSurfaceId& aSurfaceId)
	{
	RSurfaceManager::TSurfaceCreationAttributesBuf attribs;
	RSurfaceManager::TSurfaceCreationAttributes& surfaceCreationAtribs=attribs();
	surfaceCreationAtribs.iSize.iWidth = KSurfaceWidth;
	surfaceCreationAtribs.iSize.iHeight = KSurfaceHeight;
	surfaceCreationAtribs.iBuffers = 2;
	surfaceCreationAtribs.iPixelFormat = KSurfaceFormat;
	surfaceCreationAtribs.iStride = KBytesPerPixel*KSurfaceWidth;
	surfaceCreationAtribs.iOffsetToFirstBuffer = 0;
	surfaceCreationAtribs.iAlignment = 4;
	surfaceCreationAtribs.iContiguous = EFalse;
	surfaceCreationAtribs.iMappable = ETrue;
	
	TInt err = iSurfaceManager.CreateSurface(attribs, aSurfaceId);
	if (err == KErrNone)
		{
		err = iSurfaceManager.MapSurface(aSurfaceId, iChunk);
		}		
	if (err == KErrNone)
		{
		RDebug::Print(_L("Surface created: OK"));
		}
		
	iSurfacePtr = reinterpret_cast<TUint32*>(iChunk.Base());		
	err=iWindow.SetBackgroundSurface(aSurfaceId);
	if ( err!=KErrNone)
		{
		RDebug::Print(_L("ERROR: %d - Setting window background to the surface failed!"), err);
		User::Panic(_L("Fps panic"), err);
		}
	}

void CFpsAppEng::CreateSurfaceUpdateSessionL()
	{
	TInt ret = iSurfaceUpdateSession.Connect();

	if (ret==KErrAlreadyExists)
		{
		RDebug::Print(_L("Device driver already loaded"));
		}
	else if (ret==KErrNone)
		{
		RDebug::Print(_L("Connected to surface update server"));
		}
	else
		{
		RDebug::Print(_L("Fatal error connecting to surface update server"));
		User::LeaveIfError(ret);
		}
	}

void CFpsAppEng::DestroySurface()
	{
	RDebug::Print(_L("Destroy Surface update session"));
	iSurfaceUpdateSession.Close();

	RDebug::Print(_L("Closing chunk"));
	iChunk.Close();

	RDebug::Print(_L("Closing surface"));
	TInt ret = iSurfaceManager.CloseSurface(iSurfaceId);
	if(ret!=KErrNone)
		{
		RDebug::Print(_L("Surface manager failed to close"));
		}

	RDebug::Print(_L("Close Surface Manager"));
	iSurfaceManager.Close();
	}


// Timer's RunL()
void CFpsAppEng::RunL()
	{
	TInt j;
	if (iCol1)
		{
		for (j=0; j<iNoOfPixels; j++)
			{		
			iBufPtr1[j]=0xFF00FF00;
			}		
		iCol1=EFalse;
		iCol2=ETrue;		
		}
	else
		{
		for (j=0; j<iNoOfPixels; j++)
			{		
			iBufPtr1[j]=0xFF0000FF;
			}		
		iCol2=EFalse;
		iCol1=ETrue;		
		}

	Mem::Move(iSurfacePtr, iBufPtr1, iNoOfBytes);
	__ASSERT_ALWAYS( KErrNone==iSurfaceUpdateSession.SubmitUpdate(KAllScreens, iSurfaceId, 0, NULL),User::Panic(_L("Fps App"), -1));
	iClient.Flush();

//	After(TTimeIntervalMicroSeconds32(10000));	// 0.002sec, 100fps	
//	After(TTimeIntervalMicroSeconds32(20000));	// 0.02sec,  50fps
//	After(TTimeIntervalMicroSeconds32(25000));	// 0.025sec, 40fps
	After(TTimeIntervalMicroSeconds32(33333));	// 0.033sec, 30fps	
//	After(TTimeIntervalMicroSeconds32(50000));  // 0.050sec, 20fps
//	After(TTimeIntervalMicroSeconds32(100000));  //0.01sec,  10fps
	}


// Timer's DoCancel()
void CFpsAppEng::DoCancel()
	{
	// Cancel timer
	CTimer::DoCancel();
	}

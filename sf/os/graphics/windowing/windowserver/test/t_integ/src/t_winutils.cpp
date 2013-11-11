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

#include <hal.h>
#include "t_winutils.h"
#include "t_pseudoapputils.h"

EXPORT_C CTestBitmap* CTestBitmap::NewLC(const TSize& aSizeInPixels, TDisplayMode aDispMode)
	{
	return NewLC(0, aSizeInPixels, aDispMode);
	}

EXPORT_C CTestBitmap* CTestBitmap::NewL(const TSize& aSizeInPixels, TDisplayMode aDispMode)
	{
	CTestBitmap* self = NewLC(0, aSizeInPixels, aDispMode);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CTestBitmap* CTestBitmap::NewL(TInt aHandle)
	{
	CTestBitmap* self = NewLC(aHandle, TSize(), ENone);
	CleanupStack::Pop(self);
	return self;
	}

CTestBitmap* CTestBitmap::NewLC(TInt aHandle, const TSize& aSizeInPixels, TDisplayMode aDispMode)
	{
	CTestBitmap* self = new(ELeave) CTestBitmap();
	CleanupStack::PushL(self);
	self->ConstructL(aHandle, aSizeInPixels, aDispMode);
	return self;
	}

void CTestBitmap::ConstructL(TInt aHandle, const TSize& aSizeInPixels, TDisplayMode aDispMode)
	{
	iBitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(iBitmap);

	if (aHandle==0)
		{
		User::LeaveIfError(iBitmap->Create(aSizeInPixels, aDispMode));
		}
	else
		{
		User::LeaveIfError(iBitmap->Duplicate(aHandle));
		}
	iDevice = CFbsBitmapDevice::NewL(iBitmap);
	CleanupStack::PushL(iDevice);

	User::LeaveIfError(iDevice->CreateContext(iGc));

	CleanupStack::Pop(2);
	}

CTestBitmap::~CTestBitmap()
	{
	delete iGc;
	delete iDevice;
	delete iBitmap;
	}

EXPORT_C void TImportPseudoAppConfig::ImportPseudoAppConfigL(TInt aScreenNo, TInt& aFrameDuration, TInt& aFrameDelay,
                                                             TDisplayMode& aMode, TSize& aScreenSize, CIniData* aUtils)
	{
	TInt screenMode;
	TInt numberOfScreens;
	TPtrC screenAspectRatio;
	TInt screenNumber;

	READ_INI1(frame_duration, aFrameDuration, aUtils);
	READ_INI1(screen_aspect_ratio, screenAspectRatio, aUtils);
	READ_INI1(number_of_screens, numberOfScreens, aUtils);
	READ_INI1(frame_delay, aFrameDelay, aUtils);
	READ_INI2(screen_mode, EColor16MA, EColor64K, screenMode, EColor16MA, EColor64K, aUtils);

	aMode = static_cast<TDisplayMode>(screenMode);

	//Determine the number of screens from the hardware and compare with the value from the ini file
	HAL::Get(HALData::EDisplayNumberOfScreens, screenNumber);
	if(numberOfScreens != screenNumber)
		{
		RDebug::Print(_L("Error: A %d screen test has been indicated, but %d screen(s) exist\n"), numberOfScreens, screenNumber);
		}

	//Determine the screen dimensions
	RWsSession session;
	User::LeaveIfError(session.Connect());

	CWsScreenDevice* screenDevice = new(ELeave) CWsScreenDevice(session);
	CleanupStack::PushL(screenDevice);

	User::LeaveIfError(screenDevice->Construct(aScreenNo));

	aScreenSize = screenDevice->SizeInPixels();

	//Check that the aspect ratio matches that of the screen
	TSize aSpectRatio;

	if(TImportScreenConfig::ExtractAspectRatio(screenAspectRatio, aSpectRatio) == KErrNone)
		{
		if(aScreenSize.iWidth*aSpectRatio.iHeight != aScreenSize.iHeight*aSpectRatio.iWidth)
			{
			RDebug::Print(_L("Aspect ratio from string does not match screen dimensions\n"));
			RDebug::Print(_L("Read in %d:%d, Actual dimensions %d:%d\n"), aSpectRatio.iWidth, aSpectRatio.iHeight, aScreenSize.iWidth, aScreenSize.iHeight);
			}
		}
	else
		{
		RDebug::Print(_L("Cannot determine aspect ratio from string, quitting: %s\n"), screenAspectRatio.Ptr());
		User::Exit(0);
		}

	CleanupStack::PopAndDestroy(screenDevice);
	session.Close();
	}

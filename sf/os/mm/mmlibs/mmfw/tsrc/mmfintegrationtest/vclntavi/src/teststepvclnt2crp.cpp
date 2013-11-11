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

#include "teststepvclnt2crp.h"
#include <graphics/surfaceconfiguration.h>

const TInt KScreenNumber = 0;

_LIT(KUseCRP, "UseCRP");
_LIT(KAddDisplay, "AddDisplay");
_LIT(KAddWindow, "AddWindow");
_LIT(Kfilename, "filename");

RTestStepVclnt2CRP* RTestStepVclnt2CRP::NewL(const TDesC& aName)
	{
	RTestStepVclnt2CRP* self = new (ELeave) RTestStepVclnt2CRP(aName);
	return self;
	}

RTestStepVclnt2CRP::RTestStepVclnt2CRP(const TDesC& aName) :
	RTestVclnt2PlayFile(aName, aName, Kfilename, KErrNone)
	{
	}

RTestStepVclnt2CRP::~RTestStepVclnt2CRP()
	{	
	if (iSurfaceWindow)
		{	
		iSurfaceWindow->Close();
		}

	delete iSurfaceWindow;
	}

TVerdict RTestStepVclnt2CRP::DoTestStepPreambleL()
	{
	iTestStepResult = RTestVclnt2PlayFile::DoTestStepPreambleL();

	if (iTestStepResult == EPass)
		{
		iTestStepResult = EFail;
		// Assume if values are not set to revert to defaults
		// e.g. EFalse default
		GetBoolFromConfig(iTestStepName, KUseCRP, iUseCRP);
		// EFalse default
		GetBoolFromConfig(iTestStepName, KAddDisplay, iAddDisplay);
		// EFalse default
		GetBoolFromConfig(iTestStepName, KAddWindow, iAddWindow);

		iSurfaceWindow = new (ELeave) RWindow(iWs);
		// Don't use this but instead something 'unique' since this is used in the base class to
		// construct a window
		User::LeaveIfError(iSurfaceWindow->Construct(*iWindow, reinterpret_cast<TInt>(this + 3431)));
		iTestStepResult = EPass;
		}

	return iTestStepResult;	
	}

TVerdict RTestStepVclnt2CRP::DoTestStepPostambleL()
	{
	if (iSurfaceWindow)
		{
		iSurfaceWindow->Close();
		iSurfaceWindow = NULL;
		}

	return RTestVclnt2PlayFile::DoTestStepPostambleL();	
	}

void RTestStepVclnt2CRP::HandleOpenCompleteL()
	{
	if (!iAddWindow)
		{
		iVideoPlayer2->RemoveDisplayWindow(*iWindow);
		}
	
	if (iAddDisplay)
		{
		iVideoPlayer2->AddDisplayL(iWs, KScreenNumber, *this);
		}

	RTestVclnt2PlayFile::HandleOpenCompleteL();
	}

void RTestStepVclnt2CRP::MmsehSurfaceCreated(TInt aDisplayId, const TSurfaceId& aId, const TRect& aCropRect, TVideoAspectRatio /*aAspectRatio*/)
	{
	if (aDisplayId != KScreenNumber)
		{
		FailTest(_L("Mismatching display id provided to MmsehSurfaceCreated"));
		return;
		}

	iSurfaceId = aId;
	iCropRect = aCropRect;	

	const TRect windowSize(iSurfaceWindow->Size());

	TSurfaceConfiguration config;
	config.SetSurfaceId(iSurfaceId);
	config.SetExtent(windowSize);
	config.SetViewport(iCropRect);

	iSurfaceWindow->SetVisible(ETrue);
	iSurfaceWindow->SetBackgroundColor(TRgb(0, 0));

	TRAP_IGNORE(iVideoPlayer2->SetRotationL(*iWindow, EVideoRotationClockwise90));
	TRAP_IGNORE(iVideoPlayer2->RotationL(*iWindow));
	TRAP_IGNORE(iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleBestFit));

	iSurfaceWindow->SetBackgroundSurface(config, KScreenNumber);
	iSurfaceWindow->BeginRedraw();
	iSurfaceWindow->EndRedraw();
	}

void RTestStepVclnt2CRP::MmsehSurfaceParametersChanged(const TSurfaceId& /*aId*/, const TRect& /*aCropRect*/, TVideoAspectRatio /*aAspectRatio*/)
	{
	FailTest(_L("Unexpected call to MmsehSurfaceParametersChanged"));
	}

void RTestStepVclnt2CRP::MmsehRemoveSurface(const TSurfaceId& /*aId*/)
	{
	FailTest(_L("Unexpected call to MmsehRemoveSurface"));
	}

void RTestStepVclnt2CRP::FailTest(const TDesC& aMessage)
	{
	ERR_PRINTF2(_L("%S"), &aMessage);
	iTestStepResult = EFail;
	iActiveScheduler->Stop();
	}


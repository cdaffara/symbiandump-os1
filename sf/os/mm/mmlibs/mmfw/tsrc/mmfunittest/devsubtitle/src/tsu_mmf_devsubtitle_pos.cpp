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

#include "tsu_mmf_devsubtitle_pos.h"
#include "subtitlecommonutils.h"

const TInt KWindowClipRectTlX(50);
const TInt KWindowClipRectTlY(50);
const TInt KWindowClipRectBrX(200);
const TInt KWindowClipRectBrY(200);
const TInt KVideoStart= 0;
const TInt KWindowIdValid1 = 0;
const TInt KWindowIdValid2 = 1;
_LIT(KSrtDecoderMime, "srtdecoder");

const TInt KOneSecond(1000000);
const TInt KTwoSeconds(2000000);
const TInt KThreeSeconds(3000000);
const TInt KFourSeconds(4000000);
const TInt KEightSeconds(8000000);

/* 
MM-MMF-DEVSUBTITLE-U-0001-HP
Test adding subtitle configuration to DevSubtitle
*/
RDevSubtitleTestStepPos0001* RDevSubtitleTestStepPos0001::NewL(const TDesC& aStepName)
	{
	RDevSubtitleTestStepPos0001* self = new (ELeave) RDevSubtitleTestStepPos0001(aStepName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

RDevSubtitleTestStepPos0001::RDevSubtitleTestStepPos0001(const TDesC& aStepName)
	:RDevSubtitleTestStep(aStepName)
	{

	}
	
void RDevSubtitleTestStepPos0001::ConstructL()
	{
	
	}

RDevSubtitleTestStepPos0001::~RDevSubtitleTestStepPos0001()
	{
	
	}

TVerdict RDevSubtitleTestStepPos0001::DoTestStepPreambleL()
	{
	if ( RDevSubtitleTestStep::DoTestStepPreambleL() == EFail )
		{
		return EFail;
		}
	
	return EPass;
	}


TVerdict RDevSubtitleTestStepPos0001::DoTestStepPostambleL()
	{
	if ( RDevSubtitleTestStep::DoTestStepPostambleL() == EFail )
		{
		return EFail;
		}
		
	return EPass;
	}

TVerdict RDevSubtitleTestStepPos0001::DoTestStepL()
	{
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid1;
	
	DevSubtitle()->AddSubtitleConfigL(config);
	
	// Check that configuration has been set
	TWsGraphicId crpId(TWsGraphicId::EUninitialized);
	TRect region;
	DevSubtitle()->GetCrpParametersL(KWindowIdValid1, crpId, region);
	
	return EPass;
	}

/*
MM-MMF-DEVSUBTITLE-U-0002-HP
Test requesting subtitle configuration information from DevSubtitle
*/
RDevSubtitleTestStepPos0002* RDevSubtitleTestStepPos0002::NewL(const TDesC& aStepName)
	{
	RDevSubtitleTestStepPos0002* self = new (ELeave) RDevSubtitleTestStepPos0002(aStepName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

RDevSubtitleTestStepPos0002::RDevSubtitleTestStepPos0002(const TDesC& aStepName)
	:RDevSubtitleTestStep(aStepName)
	{

	}
	
void RDevSubtitleTestStepPos0002::ConstructL()
	{
	
	}

RDevSubtitleTestStepPos0002::~RDevSubtitleTestStepPos0002()
	{
	
	}

TVerdict RDevSubtitleTestStepPos0002::DoTestStepPreambleL()
	{
	if ( RDevSubtitleTestStep::DoTestStepPreambleL() == EFail )
		{
		return EFail;
		}
	
	return EPass;
	}
TVerdict RDevSubtitleTestStepPos0002::DoTestStepPostambleL()
	{
	if ( RDevSubtitleTestStep::DoTestStepPostambleL() == EFail )
		{
		return EFail;
		}
		
	return EPass;
	}

TVerdict RDevSubtitleTestStepPos0002::DoTestStepL()
	{
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid1;
	
	DevSubtitle()->AddSubtitleConfigL(config);
	
	// Check that configuration has been set
	TWsGraphicId crpId(TWsGraphicId::EUninitialized);
	TRect region;
	DevSubtitle()->GetCrpParametersL(KWindowIdValid1, crpId, region);
	
	// Check the crpId is valid.
	if ( crpId.Id() == NULL )
		{
		return EFail;
		}
		
	if ( !(crpId.IsId()) )
		{
		return EFail;
		}
		
	if ( crpId.IsUid() )
		{
		return EFail;
		}
	
	return EPass;
	}
	
	
/*
MM-MMF-DEVSUBTITLE-U-0003-HP
Update the subtitle configuration with all combinations of rotation settings
*/
RDevSubtitleTestStepPos0003* RDevSubtitleTestStepPos0003::NewL(const TDesC& aStepName)
	{
	RDevSubtitleTestStepPos0003* self = new (ELeave) RDevSubtitleTestStepPos0003(aStepName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

RDevSubtitleTestStepPos0003::RDevSubtitleTestStepPos0003(const TDesC& aStepName)
	:RDevSubtitleTestStepAsync(aStepName)
	{

	}
	
void RDevSubtitleTestStepPos0003::ConstructL()
	{
	
	}

RDevSubtitleTestStepPos0003::~RDevSubtitleTestStepPos0003()
	{
	
	}


void RDevSubtitleTestStepPos0003::KickoffTestL()
	{
	RDevSubtitleTestStepAsync::KickoffTestL();
	
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid1;
	
	DevSubtitle()->AddSubtitleConfigL(config);
	
	// Try retrieving the  subtitle config
	// Check that configuration has been set
	TWsGraphicId crpId(TWsGraphicId::EUninitialized);
	TRect region;
	DevSubtitle()->GetCrpParametersL(KWindowIdValid1, crpId, region);
	
	// Start CRP drawing
	iCommonUtils->StartCrpDrawL(KWindowIdValid1);
	
	iTestStep = CDevSubtitleTestStepPos0003Ao::NewL(DevSubtitle(), *this);
	CleanupStack::PushL(iTestStep);
	
	// Kick off test run
	iTestStep->StartL();
	CleanupStack::Pop(iTestStep);
	}

void RDevSubtitleTestStepPos0003::CloseTest()
	{
	// Cleanup test
	delete iTestStep;
	
	RDevSubtitleTestStepAsync::CloseTest();
	}

CDevSubtitleTestStepPos0003Ao* CDevSubtitleTestStepPos0003Ao::NewL(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent)
	{
	CDevSubtitleTestStepPos0003Ao* self = new (ELeave) CDevSubtitleTestStepPos0003Ao(aDevSubtitle, aParent);
	CleanupStack::PushL(self);
	
	// Construct CTimer
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
	}
	
CDevSubtitleTestStepPos0003Ao::CDevSubtitleTestStepPos0003Ao(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent):
	CDevSubtitleTestStepAoBase(aDevSubtitle, aParent)
	{
	
	}
	
CDevSubtitleTestStepPos0003Ao::~CDevSubtitleTestStepPos0003Ao()
	{
	
	}

void CDevSubtitleTestStepPos0003Ao::StartL()
	{
	// Start playback
	iDevSubtitle->SetVideoPositionL(0);
	iDevSubtitle->Start();
	
	HighRes(KEightSeconds);
	iState = ETestStep2;
	}
	
void CDevSubtitleTestStepPos0003Ao::RunL()
	{
	TTimeIntervalMicroSeconds currentPosition(KVideoStart);
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid1;
	
	TWsGraphicId crpId(TWsGraphicId::EUninitialized);
	TRect region;
		
	switch(iState)
		{
		case ETestStep2:
			// Check that the position is correct
			currentPosition = iDevSubtitle->Position();
			iParent.Test(currentPosition.Int64()==0);
			
			iDevSubtitle->Stop();
			
			// Update the configuration with a new rotation setting
			config.iRotation = EVideoRotationClockwise90;
			TRAPD(err, iDevSubtitle->UpdateSubtitleConfigL(config));
			iParent.Test(err==KErrNone);
			
			// Because we have updated the rotation settings we need to redraw the CRP with the new region
			iParent.CommonUtils()->StartCrpDrawL(KWindowIdValid1);
			
			// Reset to 0
			iDevSubtitle->SetVideoPositionL(KVideoStart);
			
			// Start playback
			iDevSubtitle->Start();
			
			// Run until the end of the subtitle stream
			HighRes(KEightSeconds);
			
			iState = ETestStep3;
			break;
		
		case ETestStep3:	
			// Check that the position is correct
			currentPosition = iDevSubtitle->Position();
			iParent.Test(currentPosition.Int64()==0);
			iDevSubtitle->Stop();
				
			// Update the configuration with a new rotation setting
			config.iRotation = EVideoRotationClockwise180;
			TRAP(err, iDevSubtitle->UpdateSubtitleConfigL(config));
			iParent.Test(err==KErrNone);
			
			// Because we have updated the rotation settings we need to redraw the CRP with the new region
			iParent.CommonUtils()->StartCrpDrawL(KWindowIdValid1);
			
			// Reset to 0
			iDevSubtitle->SetVideoPositionL(KVideoStart);
			
			// Start playback
			iDevSubtitle->Start();
			
			// Run until the end of the subtitle stream
			HighRes(KEightSeconds);
			
			iState = ETestStep4;
			break;
		
		case ETestStep4:	
			// Check that the position is correct
			currentPosition = iDevSubtitle->Position();
			iParent.Test(currentPosition.Int64()==0);
			iDevSubtitle->Stop();
			
			// Update the configuration with a new rotation setting
			config.iRotation = EVideoRotationClockwise270;
			TRAP(err, iDevSubtitle->UpdateSubtitleConfigL(config));
			iParent.Test(err==KErrNone);
			
			// Because we have updated the rotation settings we need to redraw the CRP with the new region
			iParent.CommonUtils()->StartCrpDrawL(KWindowIdValid1);
			
			// Reset to 0
			iDevSubtitle->SetVideoPositionL(KVideoStart);
			
			// Start playback
			iDevSubtitle->Start();
			
			// Run until the end of the subtitle stream
			HighRes(KEightSeconds);
			
			iState = ETestStepEnd;
			break;
			
		case ETestStepEnd:
			// Check that the position is correct
			currentPosition = iDevSubtitle->Position();
			iParent.Test(currentPosition.Int64()==0);
			iDevSubtitle->Stop();	
			iParent.StopTest();
			
			break;
						
		default:
			// Should never happen
			User::Invariant();
		}
	}


/*	
MM-MMF-DEVSUBTITLE-U-0004-HP
Request the current position from DevSubtitle
*/
RDevSubtitleTestStepPos0004* RDevSubtitleTestStepPos0004::NewL(const TDesC& aStepName)
	{
	RDevSubtitleTestStepPos0004* self = new (ELeave) RDevSubtitleTestStepPos0004(aStepName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

RDevSubtitleTestStepPos0004::RDevSubtitleTestStepPos0004(const TDesC& aStepName) :
	RDevSubtitleTestStepAsync(aStepName)
	{

	}
	
void RDevSubtitleTestStepPos0004::ConstructL()
	{
	
	}

RDevSubtitleTestStepPos0004::~RDevSubtitleTestStepPos0004()
	{
	
	}
	
// Start test
void RDevSubtitleTestStepPos0004::KickoffTestL()
	{
	RDevSubtitleTestStepAsync::KickoffTestL();
	
	DoTestStepPreambleL();
	
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid1;
	
	DevSubtitle()->AddSubtitleConfigL(config);
	
	// Start CRP drawing
	iCommonUtils->StartCrpDrawL(KWindowIdValid1);
	
	iTestStep = CDevSubtitleTestStepPos0004Ao::NewL(DevSubtitle(), *this);
	CleanupStack::PushL(iTestStep);
	
	// Kick off test run
	iTestStep->StartL();
	CleanupStack::Pop(iTestStep);
	}

void RDevSubtitleTestStepPos0004::CloseTest()
	{
	// Cleanup test
	delete iTestStep;
	
	RDevSubtitleTestStepAsync::CloseTest();
	}
	
	
CDevSubtitleTestStepPos0004Ao* CDevSubtitleTestStepPos0004Ao::NewL(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent)
	{
	CDevSubtitleTestStepPos0004Ao* self = new (ELeave) CDevSubtitleTestStepPos0004Ao(aDevSubtitle, aParent);
	CleanupStack::PushL(self);
	
	// Construct CTimer
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
	}
	
CDevSubtitleTestStepPos0004Ao::CDevSubtitleTestStepPos0004Ao(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent):
	CDevSubtitleTestStepAoBase(aDevSubtitle, aParent)
	{
	
	}
	
CDevSubtitleTestStepPos0004Ao::~CDevSubtitleTestStepPos0004Ao()
	{
	
	}

void CDevSubtitleTestStepPos0004Ao::StartL()
	{
	// Test step 1
	
	// Retrieve the current position from DevSubtitle
	TTimeIntervalMicroSeconds currentPos;
	currentPos = iDevSubtitle->Position();
	
	// Should be 0
	iParent.Test(currentPos.Int64()==0);
	
	iDevSubtitle->Start();
	
	// Wait for two seconds
	HighRes(KTwoSeconds);
	
	iState = ETestStep2;
	}
	
void CDevSubtitleTestStepPos0004Ao::RunL()
	{
	TTimeIntervalMicroSeconds currentPos(KVideoStart);
	
	switch(iState)
		{
		case ETestStep2:
			currentPos = iDevSubtitle->Position();
			iParent.Test(currentPos.Int64()>KTwoSeconds);
			
			// Wait for another 2 seconds
			HighRes(KTwoSeconds);
			iState = ETestStep3;
			break;

		case ETestStep3:
			currentPos = iDevSubtitle->Position();
			iParent.Test(currentPos.Int64()>KTwoSeconds*2);
			
			// Wait for SRT file to finish
			HighRes(KFourSeconds);
			
			iState = ETestStepEnd;
			break;
			
		case ETestStepEnd:
			currentPos = iDevSubtitle->Position();
			iParent.Test(currentPos.Int64()==0);

			currentPos = iDevSubtitle->Position();
			iParent.Test(currentPos.Int64()==0);		

			iParent.StopTest();
			break;
						
		default:
			// Should never happen
			User::Invariant();
		}
	}
	
/*
MM-MMF-DEVSUBTITLE-U-0005-HP
Request that DevSubtitle removes the current configuration.  
*/
RDevSubtitleTestStepPos0005* RDevSubtitleTestStepPos0005::NewL(const TDesC& aStepName)
	{
	RDevSubtitleTestStepPos0005* self = new (ELeave) RDevSubtitleTestStepPos0005(aStepName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

RDevSubtitleTestStepPos0005::RDevSubtitleTestStepPos0005(const TDesC& aStepName)
	:RDevSubtitleTestStep(aStepName)
	{

	}
	
void RDevSubtitleTestStepPos0005::ConstructL()
	{
	
	}

RDevSubtitleTestStepPos0005::~RDevSubtitleTestStepPos0005()
	{
	
	}

TVerdict RDevSubtitleTestStepPos0005::DoTestStepPreambleL()
	{
	if ( RDevSubtitleTestStep::DoTestStepPreambleL() == EFail )
		{
		return EFail;
		}
	
	return EPass;
	}


TVerdict RDevSubtitleTestStepPos0005::DoTestStepPostambleL()
	{
	if ( RDevSubtitleTestStep::DoTestStepPostambleL() == EFail )
		{
		return EFail;
		}
		
	return EPass;
	}

TVerdict RDevSubtitleTestStepPos0005::DoTestStepL()
	{
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid1;
	
	DevSubtitle()->AddSubtitleConfigL(config);
	DevSubtitle()->RemoveSubtitleConfigL(KWindowIdValid1);
	
	return EPass;
	}
	
/*
MM-MMF-DEVSUBTITLE-U-0007-HP 
Request that DevSubtitle sets the current video position.  
Test various valid video positions - 0, 50% and 75% of test SRT files length
*/
RDevSubtitleTestStepPos0007* RDevSubtitleTestStepPos0007::NewL(const TDesC& aStepName)
	{
	RDevSubtitleTestStepPos0007* self = new (ELeave) RDevSubtitleTestStepPos0007(aStepName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

RDevSubtitleTestStepPos0007::RDevSubtitleTestStepPos0007(const TDesC& aStepName):
	RDevSubtitleTestStepAsync(aStepName)
	{

	}
	
void RDevSubtitleTestStepPos0007::ConstructL()
	{
	
	}

RDevSubtitleTestStepPos0007::~RDevSubtitleTestStepPos0007()
	{
	
	}

// Start test
void RDevSubtitleTestStepPos0007::KickoffTestL()
	{
	RDevSubtitleTestStepAsync::KickoffTestL();
	
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid1;
	
	DevSubtitle()->AddSubtitleConfigL(config);
	
	// Try retrieving the  subtitle config
	// Check that configuration has been set
	TWsGraphicId crpId(TWsGraphicId::EUninitialized);
	TRect region;
	DevSubtitle()->GetCrpParametersL(KWindowIdValid1, crpId, region);
	
	// Start CRP drawing
	iCommonUtils->StartCrpDrawL(KWindowIdValid1);
	
	iTestStep = CDevSubtitleTestStepPos0007Ao::NewL(DevSubtitle(), *this);
	CleanupStack::PushL(iTestStep);
	
	// Kick off test run
	iTestStep->StartL();
	CleanupStack::Pop(iTestStep);
	}

void RDevSubtitleTestStepPos0007::CloseTest()
	{
	// Cleanup test
	delete iTestStep;
	
	RDevSubtitleTestStepAsync::CloseTest();
	}
	
CDevSubtitleTestStepPos0007Ao* CDevSubtitleTestStepPos0007Ao::NewL(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent)
	{
	CDevSubtitleTestStepPos0007Ao* self = new (ELeave) CDevSubtitleTestStepPos0007Ao(aDevSubtitle, aParent);
	CleanupStack::PushL(self);
	
	// Construct CTimer
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
	}
	
CDevSubtitleTestStepPos0007Ao::CDevSubtitleTestStepPos0007Ao(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent):
	CDevSubtitleTestStepAoBase(aDevSubtitle, aParent)
	{
	
	}
	
CDevSubtitleTestStepPos0007Ao::~CDevSubtitleTestStepPos0007Ao()
	{
	
	}

void CDevSubtitleTestStepPos0007Ao::StartL()
	{
	// SRT file is 6.5 seconds in length
	iDevSubtitle->SetVideoPositionL(KVideoStart);
	iDevSubtitle->Start();
	
	// Start timer
	HighRes(KOneSecond);
	iState = ETestStep2;		
	}
	
void CDevSubtitleTestStepPos0007Ao::RunL()
	{
	TTimeIntervalMicroSeconds currentPosition(KVideoStart);
		
	switch(iState)
		{
		case ETestStep2:
			// Check that the video position is over 1 second
			currentPosition = iDevSubtitle->Position();
			iParent.Test(currentPosition.Int64()>=KOneSecond);
			iDevSubtitle->Stop();
				
			// Set video position half way through the subtitle stream
			iDevSubtitle->SetVideoPositionL(KThreeSeconds);
			iDevSubtitle->Start();
			HighRes(KOneSecond);

			iState = ETestStep3;
			break;

		case ETestStep3:
			// Check that the video position is over four seconds
			currentPosition = iDevSubtitle->Position();
			iParent.Test(currentPosition.Int64()>=KFourSeconds);
			iDevSubtitle->Stop();
			
			// Set video position to 75%
			iDevSubtitle->SetVideoPositionL(KThreeSeconds);
			iDevSubtitle->Start();
			HighRes(KOneSecond);
				
			iState = ETestStep4;
			break;
			
		case ETestStep4:
			// Check that the video position is over five seconds
			currentPosition = iDevSubtitle->Position();
			iParent.Test(currentPosition.Int64()>=KFourSeconds);
			
			// Wait for the subtitle stream to finish
			HighRes(KThreeSeconds);
				
			// End of test
			iState = ETestStepEnd;
			break;
			
		case ETestStepEnd:
			// Check that the video position has returned to 0
			currentPosition = iDevSubtitle->Position();
			iParent.Test(currentPosition.Int64()==0);	
		
			iParent.StopTest();
			break;
						
		default:
			// Should never happen
			User::Invariant();
		}
	}

/*	
MM-MMF-DEVSUBTITLE-U-0009-HP
Request DevSubtitle stops playback in various different use-cases
*/
RDevSubtitleTestStepPos0009* RDevSubtitleTestStepPos0009::NewL(const TDesC& aStepName)
	{
	RDevSubtitleTestStepPos0009* self = new (ELeave) RDevSubtitleTestStepPos0009(aStepName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

RDevSubtitleTestStepPos0009::RDevSubtitleTestStepPos0009(const TDesC& aStepName)
	:RDevSubtitleTestStepAsync(aStepName)
	{

	}
	
void RDevSubtitleTestStepPos0009::ConstructL()
	{
	
	}

RDevSubtitleTestStepPos0009::~RDevSubtitleTestStepPos0009()
	{
	
	}
	
// Start test
void RDevSubtitleTestStepPos0009::KickoffTestL()
	{
	RDevSubtitleTestStepAsync::KickoffTestL();
	
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid1;
	
	DevSubtitle()->AddSubtitleConfigL(config);
	
	// Try retrieving the  subtitle config
	// Check that configuration has been set
	TWsGraphicId crpId(TWsGraphicId::EUninitialized);
	TRect region;
	DevSubtitle()->GetCrpParametersL(KWindowIdValid1, crpId, region);
	
	// Start CRP drawing
	iCommonUtils->StartCrpDrawL(KWindowIdValid1);
	
	iTestStep = CDevSubtitleTestStepPos0009Ao::NewL(DevSubtitle(), *this);
	CleanupStack::PushL(iTestStep);
	
	// Kick off test run
	iTestStep->StartL();
	CleanupStack::Pop(iTestStep);
	}

void RDevSubtitleTestStepPos0009::CloseTest()
	{
	// Cleanup test
	delete iTestStep;
	
	RDevSubtitleTestStepAsync::CloseTest();
	}
	
CDevSubtitleTestStepPos0009Ao* CDevSubtitleTestStepPos0009Ao::NewL(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent)
	{
	CDevSubtitleTestStepPos0009Ao* self = new (ELeave) CDevSubtitleTestStepPos0009Ao(aDevSubtitle, aParent);
	CleanupStack::PushL(self);
	
	// Construct CTimer
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
	}
	
CDevSubtitleTestStepPos0009Ao::CDevSubtitleTestStepPos0009Ao(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent):
	CDevSubtitleTestStepAoBase(aDevSubtitle, aParent)
	{
	
	}
	
CDevSubtitleTestStepPos0009Ao::~CDevSubtitleTestStepPos0009Ao()
	{
	
	}

void CDevSubtitleTestStepPos0009Ao::StartL()
	{
	iDevSubtitle->SetVideoPositionL(KVideoStart);
	iDevSubtitle->Start();

	// Wait for the middle of the SRT stream
	HighRes(KFourSeconds);
	iState = ETestStep2;	
	}
	
void CDevSubtitleTestStepPos0009Ao::RunL()
	{
	TTimeIntervalMicroSeconds currentPosition(KVideoStart);
		
	switch(iState)
		{
		case ETestStep2:
			currentPosition = iDevSubtitle->Position();
			iParent.Test(currentPosition.Int64()>=KFourSeconds);
			
			// Stop the stream
			iDevSubtitle->Stop();
			
			// Ensure that the video position has returned to 0
			currentPosition = iDevSubtitle->Position();
			iParent.Test(currentPosition.Int64()==0);
			
			//Restart playback
			iDevSubtitle->Start();	
					
			HighRes(KEightSeconds);
			iState = ETestStepEnd;
			break;

		case ETestStepEnd:
			currentPosition = iDevSubtitle->Position();
			// Ensure that the video position has returned to 0
			iParent.Test(currentPosition.Int64()==0);
			
			// Ensure that the video position has returned to 0
			currentPosition = iDevSubtitle->Position();
			iParent.Test(currentPosition.Int64()==0);
			iParent.StopTest();
			break;
						
		default:
			// Should never happen
			User::Invariant();
		}
	}


/*
MM-MMF-DEVSUBTITLE-U-0010-HP
Update the subtitle configuration
*/
RDevSubtitleTestStepPos0010* RDevSubtitleTestStepPos0010::NewL(const TDesC& aStepName)
	{
	RDevSubtitleTestStepPos0010* self = new (ELeave) RDevSubtitleTestStepPos0010(aStepName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

RDevSubtitleTestStepPos0010::RDevSubtitleTestStepPos0010(const TDesC& aStepName)
	:RDevSubtitleTestStep(aStepName)
	{

	}
	
void RDevSubtitleTestStepPos0010::ConstructL()
	{
	
	}

RDevSubtitleTestStepPos0010::~RDevSubtitleTestStepPos0010()
	{
	
	}

TVerdict RDevSubtitleTestStepPos0010::DoTestStepPreambleL()
	{
	if ( RDevSubtitleTestStep::DoTestStepPreambleL() == EFail )
		{
		return EFail;
		}
	
	return EPass;
	}


TVerdict RDevSubtitleTestStepPos0010::DoTestStepPostambleL()
	{
	if ( RDevSubtitleTestStep::DoTestStepPostambleL() == EFail )
		{
		return EFail;
		}
		
	return EPass;
	}

TVerdict RDevSubtitleTestStepPos0010::DoTestStepL()
	{
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid1;
	
	DevSubtitle()->AddSubtitleConfigL(config);
	
	// Try retrieving a subtitle config
	IMPORT_C void GetCrpParametersL(TInt aWindowId, TWsGraphicId& aCrpId, TRect& aRegion);
	
	// Check that configuration has been set
	TWsGraphicId crpId(TWsGraphicId::EUninitialized);
	TRect region;
	DevSubtitle()->GetCrpParametersL(KWindowIdValid1, crpId, region);
	
	// Update configuration with a new window ID
	config.iWindowId = KWindowIdValid2;
	TRAPD(err, DevSubtitle()->UpdateSubtitleConfigL(config));
		
	// Check that configuration has been set
	TRAP(err, DevSubtitle()->GetCrpParametersL(KWindowIdValid1, crpId, region));
	
	// above call should fail as we have updated with a new windowId
	TEST(err == KErrArgument);
	
	// Try calling again with the correct window ID
	TRAP(err, DevSubtitle()->GetCrpParametersL(KWindowIdValid2, crpId, region));
	TEST(err==KErrNone);
	
	return EPass;
	}
	

/*
MM-MMF-DEVSUBTITLE-U-0011-HP
Update the subtitle configuration during playback
*/
RDevSubtitleTestStepPos0011* RDevSubtitleTestStepPos0011::NewL(const TDesC& aStepName)
	{
	RDevSubtitleTestStepPos0011* self = new (ELeave) RDevSubtitleTestStepPos0011(aStepName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

RDevSubtitleTestStepPos0011::RDevSubtitleTestStepPos0011(const TDesC& aStepName)
	:RDevSubtitleTestStepAsync(aStepName)
	{

	}
	
void RDevSubtitleTestStepPos0011::ConstructL()
	{
	
	}

RDevSubtitleTestStepPos0011::~RDevSubtitleTestStepPos0011()
	{
	
	}

void RDevSubtitleTestStepPos0011::KickoffTestL()
	{
	RDevSubtitleTestStepAsync::KickoffTestL();
	
	DevSubtitle()->SelectDecoderL(KSrtDecoderMime);
	
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid1;
	
	DevSubtitle()->AddSubtitleConfigL(config);
	
	// Try retrieving the  subtitle config
	// Check that configuration has been set
	TWsGraphicId crpId(TWsGraphicId::EUninitialized);
	TRect region;
	DevSubtitle()->GetCrpParametersL(KWindowIdValid1, crpId, region);
	
	// Start CRP drawing
	iCommonUtils->StartCrpDrawL(KWindowIdValid1);
	
	iTestStep = CDevSubtitleTestStepPos0011Ao::NewL(DevSubtitle(), *this);
	CleanupStack::PushL(iTestStep);
	
	// Kick off test run
	iTestStep->StartL();
	CleanupStack::Pop(iTestStep);
	}

void RDevSubtitleTestStepPos0011::CloseTest()
	{
	// Cleanup test
	delete iTestStep;
	
	RDevSubtitleTestStepAsync::CloseTest();
	}	
	
CDevSubtitleTestStepPos0011Ao* CDevSubtitleTestStepPos0011Ao::NewL(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent)
	{
	CDevSubtitleTestStepPos0011Ao* self = new (ELeave) CDevSubtitleTestStepPos0011Ao(aDevSubtitle, aParent);
	CleanupStack::PushL(self);
	
	// Construct CTimer
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
	}
	
CDevSubtitleTestStepPos0011Ao::CDevSubtitleTestStepPos0011Ao(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent):
	CDevSubtitleTestStepAoBase(aDevSubtitle, aParent)
	{
	
	}
	
CDevSubtitleTestStepPos0011Ao::~CDevSubtitleTestStepPos0011Ao()
	{
	
	}

void CDevSubtitleTestStepPos0011Ao::StartL()
	{
	// Start playback
	iDevSubtitle->Start();
	
	HighRes(KTwoSeconds);
	iState = ETestStep2;
	}
	
void CDevSubtitleTestStepPos0011Ao::RunL()
	{
	TTimeIntervalMicroSeconds currentPosition(KVideoStart);
	TMMFSubtitleWindowConfig config;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = TRect(KWindowClipRectTlX, KWindowClipRectTlY, KWindowClipRectBrX, KWindowClipRectBrY);
	config.iWindowId = KWindowIdValid1;
	
	TWsGraphicId crpId(TWsGraphicId::EUninitialized);
	TRect region;
		
	switch(iState)
		{
		case ETestStep2:
			{
			TTimeIntervalMicroSeconds position = iDevSubtitle->Position();
			iDevSubtitle->Stop();
			
			// Remove current configuration and replace with a new window ID.  This should cause 
			// DevSubtitle to recreate and continue displaying subtitles from the current position
			TRAPD(err, iDevSubtitle->RemoveSubtitleConfigL(KWindowIdValid1));
			iParent.Test(err==KErrNone);
			config.iWindowId = KWindowIdValid2;
			TRAP(err, iDevSubtitle->AddSubtitleConfigL(config));
			iParent.Test(err==KErrNone);
			
			TRAP(err, iDevSubtitle->SetVideoPositionL(position));
			position = iDevSubtitle->Position();
			iParent.Test(err==KErrNone);
			
			iDevSubtitle->Start();
			
			// Try calling with the old window ID
			TRAP(err, iDevSubtitle->GetCrpParametersL(KWindowIdValid1, crpId, region));
			iParent.Test(err==KErrNotFound);
			
			// Try calling again with the correct window ID
			TRAP(err, iDevSubtitle->GetCrpParametersL(KWindowIdValid2, crpId, region));
			iParent.Test(err==KErrNone);
			
			// Wait for the end of the subtitle stream
			HighRes(KEightSeconds);
			
			iState = ETestStepEnd;
			break;
			}
			
		case ETestStepEnd:
			// Check that the position has returned to 0
			currentPosition = iDevSubtitle->Position();
			iParent.Test(currentPosition.Int64()==0);
			iParent.StopTest();
			break;
						
		default:
			// Should never happen
			User::Invariant();
		}
	}

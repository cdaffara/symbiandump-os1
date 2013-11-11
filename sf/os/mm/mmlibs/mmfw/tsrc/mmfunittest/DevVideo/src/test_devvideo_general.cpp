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
// This program is designed to test DevVideo.
// 
//

/**
 @file test_devvideo_general.cpp
*/
#include "test_devvideo_general.h"

//this test checks for fatal errors.
RDevVideoInfoTest* RDevVideoInfoTest::NewL(const TDesC& aStepName)
	{
	RDevVideoInfoTest* self = new(ELeave)RDevVideoInfoTest(aStepName);
	return self;
	}

RDevVideoInfoTest::RDevVideoInfoTest(const TDesC& aStepName):iDevVideoPlay(NULL), iDevVideoRecord(NULL)
	{
	iTestStepName = aStepName;
	}

void RDevVideoInfoTest::KickoffTestL()
	{
	INFO_PRINTF1(_L("INC103980:[Future OS Product]When answering incoming video call an error note application closed: PV6461604"));
	iDevVideoPlay = CMMFDevVideoPlay::NewL(*this);
	iDevVideoRecord = CMMFDevVideoRecord::NewL(*this);
	TInt error;
	//check for fatal errors for decoder info
	TRAP(error, TestDecoderInfoL());
	if(error != KErrNone)
		{
		StopTest(EInconclusive);
		return;	
		}
	//check for fatal errors for encoder info
	TRAP(error, TestEncoderInfoL());	
	if(error != KErrNone)
		{
		StopTest(EInconclusive);			
		}
	else
		{
		StopTest(EPass);			
		}
	}

void RDevVideoInfoTest::CloseTest()
	{
	delete 	iDevVideoPlay;
	iDevVideoPlay = NULL;
	delete iDevVideoRecord;
	iDevVideoRecord = NULL;
	}

void RDevVideoInfoTest::TestDecoderInfoL()
	{
	TUint32 requestedPostProcType = 0;
	RArray<TUid> foundDecoders;
	CleanupClosePushL(foundDecoders);
	_LIT8(KMimeType, "video/mp4v-es");
	iDevVideoPlay->FindDecodersL(KMimeType(), requestedPostProcType, foundDecoders, EFalse);
	TInt decodersCnt = foundDecoders.Count();
	for(TInt i = 0; i < decodersCnt; i++)
		{
		TUid theDecoderUid = foundDecoders[i];
		CVideoDecoderInfo* decoderInfo = iDevVideoPlay->VideoDecoderInfoLC(theDecoderUid);
		CleanupStack::PopAndDestroy(decoderInfo);
		}
	CleanupStack::PopAndDestroy(&foundDecoders);
	}

void RDevVideoInfoTest::TestEncoderInfoL()
	{
	TUint32 requestedPostProcType = 0;
	RArray<TUid> foundEncoders;
	CleanupClosePushL(foundEncoders);
	_LIT8(KMimeType, "video/mp4v-es");
	iDevVideoRecord->FindEncodersL(KMimeType(), requestedPostProcType, foundEncoders, EFalse);
	TInt encodersCnt = foundEncoders.Count();
	for(TInt i = 0; i < encodersCnt; i++)
		{
		TUid theEncoderUid = foundEncoders[i];
		CVideoEncoderInfo* encoderInfo = iDevVideoRecord->VideoEncoderInfoLC(theEncoderUid);
		CleanupStack::PopAndDestroy(encoderInfo);
		}
	CleanupStack::PopAndDestroy(&foundEncoders);	
	}

void RDevVideoInfoTest::MdvpoNewBuffers()
	{	
	}
	
void RDevVideoInfoTest::MdvpoReturnPicture(TVideoPicture* /*aPicture*/)
	{		
	}

void RDevVideoInfoTest::MdvpoSupplementalInformation(const TDesC8& /*aData*/, 
								  const TTimeIntervalMicroSeconds& /*aTimestamp*/, 
								  const TPictureId& /*aPictureId*/)
	{	
	}

void RDevVideoInfoTest::MdvpoPictureLoss()
	{	
	}

void RDevVideoInfoTest::MdvpoPictureLoss(const TArray<TPictureId>& /*aPictures*/)
	{	
	}

void RDevVideoInfoTest::MdvpoSliceLoss(TUint /*aFirstMacroblock*/, TUint /*aNumMacroblocks*/, const TPictureId& /*aPicture*/)
	{	
	}

void RDevVideoInfoTest::MdvpoReferencePictureSelection(const TDesC8& /*aSelectionData*/)
	{	
	}

void RDevVideoInfoTest::MdvpoTimedSnapshotComplete(TInt /*aError*/, 
													  TPictureData* /*aPictureData*/, 
													  const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/, 
													  const TPictureId& /*aPictureId*/)
	{	
	}

void RDevVideoInfoTest::MdvpoNewPictures()
	{	
	}

void RDevVideoInfoTest::MdvpoFatalError(TInt /*aError*/)
	{	
	}

void RDevVideoInfoTest::MdvpoInitComplete(TInt /*aError*/)
	{	
	}

void RDevVideoInfoTest::MdvpoStreamEnd()
	{	
	}

void RDevVideoInfoTest::MdvroReturnPicture(TVideoPicture* /*aPicture*/)
	{	
	}

void RDevVideoInfoTest::MdvroSupplementalInfoSent()
	{	
	}

void RDevVideoInfoTest::MdvroNewBuffers()
	{	
	}

void RDevVideoInfoTest::MdvroFatalError(TInt /*aError*/)
	{	
	}

void RDevVideoInfoTest::MdvroInitializeComplete(TInt /*aError*/)
	{	
	}

void RDevVideoInfoTest::MdvroStreamEnd()
	{	
	}

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
#include <testframework.h>
#include <mmf/devvideo/devvideoplay.h>
#include <mmf/devvideo/devvideorecord.h>

/**
 INC103980:[Future OS Product]When answering incoming video call an error note "application closed: PV6461604" 	
 Test checks for fatal errors on calling VideoEncoderInfoLC() and VideoDecoderInfoLC().
 */
NONSHARABLE_CLASS (RDevVideoInfoTest) : public RAsyncTestStep,
							public MMMFDevVideoPlayObserver,
							public MMMFDevVideoRecordObserver 
	{
public:
	static RDevVideoInfoTest* NewL(const TDesC& aStepName);
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	// MMMFDevVideoPlayObserver
    virtual void MdvpoNewBuffers();
    virtual void MdvpoReturnPicture(TVideoPicture* aPicture);
    virtual void MdvpoSupplementalInformation(const TDesC8& aData, 
											  const TTimeIntervalMicroSeconds& aTimestamp, 
											  const TPictureId& aPictureId);
	virtual void MdvpoPictureLoss();
	virtual void MdvpoPictureLoss(const TArray<TPictureId>& aPictures);
	virtual void MdvpoSliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture);
	virtual void MdvpoReferencePictureSelection(const TDesC8& aSelectionData);
    virtual void MdvpoTimedSnapshotComplete(TInt aError, 
											TPictureData* aPictureData, 
											const TTimeIntervalMicroSeconds& aPresentationTimestamp, 
											const TPictureId& aPictureId);
    virtual void MdvpoNewPictures();
    virtual void MdvpoFatalError(TInt aError);
    virtual void MdvpoInitComplete(TInt aError);
    virtual void MdvpoStreamEnd();		
	//MMMFDevVideoRecordObserver
	virtual void MdvroReturnPicture(TVideoPicture* aPicture);
    virtual void MdvroSupplementalInfoSent();
    virtual void MdvroNewBuffers();
    virtual void MdvroFatalError(TInt aError);
    virtual void MdvroInitializeComplete(TInt aError);
    virtual void MdvroStreamEnd();	
private:
	//calls VideoDecoderInfoLC() on decoder hwdevices
	void TestDecoderInfoL();
	//calls VideoEncoderInfoLC() on encoder hwdevices
	void TestEncoderInfoL();
	RDevVideoInfoTest(const TDesC& aStepName);
	CMMFDevVideoPlay* iDevVideoPlay;
	CMMFDevVideoRecord* iDevVideoRecord;
	};

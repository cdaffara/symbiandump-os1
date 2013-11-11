
// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestDevVideoRecord.cpp
// 
//

#include "TestDevVideoRecord.h"
#include "TestDevVideoPlayTestData.h"
#include "TestDevVideoPlugins/encoder.h"
#include "TestDevVideoPlugins/preproc.h"
#include <e32math.h>

const TUid KUidMDFVideoEncoderHwDeviceAdapter = { 0x102737EB };

// DevVideoRecord base class

CTestDevVideoRecordStep::CTestDevVideoRecordStep(const TDesC& aTestName, TTestType aTestType)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;		
	iTestType = aTestType;
	}
	
CTestDevVideoRecordStep::~CTestDevVideoRecordStep() 
	{
	delete iDevVideoRecord;
	}

TVerdict CTestDevVideoRecordStep::DoTestStepL()
	{
	// construct DevVideoRecord object
	__UHEAP_MARK;
	TRAPD(err, iDevVideoRecord = CMMFDevVideoRecord::NewL( *this ));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Could not create a CMMFDevVideoRecord class!"));
		return EInconclusive;
		}
	
	TVerdict ret = EFail;

	// call the test step
	ret = DoTestL(*iDevVideoRecord);
		
	INFO_PRINTF1(_L("CMMFDevVideoRecord: Destroying"));
	delete iDevVideoRecord;
	iDevVideoRecord = NULL;
	__MM_HEAP_MARKEND;
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CMMFDevVideoRecord failed with error %d"), iError );
	
	return	ret;
	}


// MMMFDevVideoRecordObserver
void CTestDevVideoRecordStep::MdvroNewBuffers()
	{
	INFO_PRINTF1(_L("CTestDevVideoRecordStep::MdvroNewBuffers()"));
	}

void CTestDevVideoRecordStep::MdvroReturnPicture(TVideoPicture* /*aPicture*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoRecordStep::MdvroReturnPicture()"));
	}

void CTestDevVideoRecordStep::MdvroSupplementalInfoSent()
	{
	INFO_PRINTF1(_L("CTestDevVideoRecordStep::MdvroSupplementalInfoSent()"));
	}

void CTestDevVideoRecordStep::MdvroFatalError(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("CTestDevVideoRecordStep::MdvroFatalError()\nError = %d"), aError);
	}

void CTestDevVideoRecordStep::MdvroInitializeComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("CTestDevVideoRecordStep::MdvroInitializeComplete():  Error = %d"), aError);
	}

void CTestDevVideoRecordStep::MdvroStreamEnd()
	{
	INFO_PRINTF1(_L("CTestDevVideoRecordStep::MdvroStreamEnd()"));
	}

//------------------------------------------------------------------

//------------------------------------------------------------------

CTestDevVideoRecordFindEncoders::CTestDevVideoRecordFindEncoders(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordFindEncoders* CTestDevVideoRecordFindEncoders::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordFindEncoders* self = new(ELeave) CTestDevVideoRecordFindEncoders(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordFindEncoders::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("CMMFDevVideoRecord:  FindEncoders"));

	RArray<TUid> foundEncoders;
	CleanupClosePushL(foundEncoders);
	RArray<TUid> expectedEncoders;
	CleanupClosePushL(expectedEncoders);
	RArray<TUid> unExpectedEncoders;
	CleanupClosePushL(unExpectedEncoders);

	TInt error = KErrNone;
	TUint32 requestedPreProcType = 0;
	
	// Unsupported mime type
	if (!error)
		{
		requestedPreProcType = 0;
		_LIT8(KMimeType1, "VIDEO/mpeg21");
		foundEncoders.Reset();
		expectedEncoders.Reset();
		unExpectedEncoders.Reset();
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice1));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice2));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice3));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice5));

		TRAP(error, aDevVideoRecord.FindEncodersL(KMimeType1(), requestedPreProcType, foundEncoders, EFalse));

		if (!error)
			error = CheckEncoders(foundEncoders, expectedEncoders, unExpectedEncoders);
		}

	// Unsupported pre proc type
	if (!error)
		{
		requestedPreProcType = EPpOutputCrop|EPpInputCrop;
		foundEncoders.Reset();
		_LIT8(KMimeType2, "video/dummy");
		expectedEncoders.Reset();
		unExpectedEncoders.Reset();
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice1));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice2));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice3));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice4));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice5));

		TRAP(error, aDevVideoRecord.FindEncodersL(KMimeType2(), requestedPreProcType, foundEncoders, EFalse));
		if (error == KErrNotFound)//we actually expect KErrNotFound here
			error = KErrNone;
		else
			error = KErrGeneral;

		if (!error)
			error = CheckEncoders(foundEncoders, expectedEncoders, unExpectedEncoders);
		}

	// Unsupported mime type (by exact match)
	if (!error)
		{
		_LIT8(KMimeType3, "video/H263-2000; profile=45;level=40");
		requestedPreProcType = 0;
		foundEncoders.Reset();
		expectedEncoders.Reset();
		unExpectedEncoders.Reset();
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice1));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice2));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice3));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice4));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice5));

		TRAP(error, aDevVideoRecord.FindEncodersL(KMimeType3(), requestedPreProcType, foundEncoders, ETrue));
		if (error == KErrNotFound)//we actually expect KErrNotFound here
			error = KErrNone;
		else
			error = KErrGeneral;

		if (!error)
			error = CheckEncoders(foundEncoders, expectedEncoders, unExpectedEncoders);
		}
	
	// Supported mime type (by unexact match)
	if (!error)
		{
		_LIT8(KMimeType4, "video/H263-2000; profile=1234;level=20");
		requestedPreProcType = 0;
		foundEncoders.Reset();
		expectedEncoders.Reset();
		unExpectedEncoders.Reset();
		User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice1));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice2));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice3));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice4));
		User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice5));

		TRAP(error, aDevVideoRecord.FindEncodersL(KMimeType4(), requestedPreProcType, foundEncoders, EFalse));

		if (!error)
			error = CheckEncoders(foundEncoders, expectedEncoders, unExpectedEncoders);
		}
	
	// Supported mime type and pre proc type (by exact match)
	if (!error)
		{
		_LIT8(KMimeType5, "video/zippyvideo");
		requestedPreProcType = EPpMirror|EPpInputCrop;
		foundEncoders.Reset();
		expectedEncoders.Reset();
		unExpectedEncoders.Reset();
		User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice1));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice2));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice3));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice4));
		User::LeaveIfError(unExpectedEncoders.Append(KUidDevVideoTestEncodeHwDevice5));

		TRAP(error, aDevVideoRecord.FindEncodersL(KMimeType5(), requestedPreProcType, foundEncoders, ETrue));

		if (!error)
			error = CheckEncoders(foundEncoders, expectedEncoders, unExpectedEncoders);
		}
	
	CleanupStack::PopAndDestroy(3); //foundEncoders, expectedEncoders, unExpectedEncoders

	if (!error)
		ret = EPass;

	return ret;
	}

TInt CTestDevVideoRecordFindEncoders::CheckEncoders(const RArray<TUid>& aFoundEncoders, 
													const RArray<TUid>& aExpectedEncoders, 
													const RArray<TUid>& aUnExpectedEncoders)
	{
	TInt ret = KErrGeneral;

	TBool foundUnExpected = EFalse;
	TInt numberToBeFoundInExpectedEncoders = aExpectedEncoders.Count();

	for (TInt i=0; i<aFoundEncoders.Count(); i++)
		{
		TUid uidToLookFor = aFoundEncoders[i];
		for (TInt j=0; j<aExpectedEncoders.Count(); j++)
			{
			if (aExpectedEncoders[j] == uidToLookFor)
				{
				numberToBeFoundInExpectedEncoders--;
				break;//j
				}
			}
		for (TInt k=0; k<aUnExpectedEncoders.Count(); k++)
			{
			if (aUnExpectedEncoders[k] == uidToLookFor)
				{
				foundUnExpected = ETrue;
				break;//k
				}
			}
		}

	if ((!foundUnExpected) && (numberToBeFoundInExpectedEncoders==0))
		ret = KErrNone;

	return ret;
	}

//------------------------------------------------------------------

CTestDevVideoRecordFindPreProc::CTestDevVideoRecordFindPreProc(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordFindPreProc* CTestDevVideoRecordFindPreProc::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordFindPreProc* self = new(ELeave) CTestDevVideoRecordFindPreProc(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordFindPreProc::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EFail;

	INFO_PRINTF1(_L("CMMFDevVideoRecord:  FindPreProc"));

	RArray<TUid> foundPreProx;
	CleanupClosePushL(foundPreProx);
	RArray<TUid> expectedPreProx;
	CleanupClosePushL(expectedPreProx);
	RArray<TUid> unExpectedPreProx;
	CleanupClosePushL(unExpectedPreProx);

	TInt error = KErrNone;
	TUint32 requestedPreProcType = 0;	

	// First ask for a pre process type that isn't supported
	if (!error)
		{
		requestedPreProcType = EPpInputCrop|EPpMirror|EPpScale|EPpOutputPad|EPpNoiseFilter;
		foundPreProx.Reset();
		expectedPreProx.Reset();
		unExpectedPreProx.Reset();
		User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice1));
		User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice2));
		User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice3));
		User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice4));
		User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice5));
		
		TRAP(error, aDevVideoRecord.FindPreProcessorsL(requestedPreProcType, foundPreProx));
		if (error == KErrNotFound)//we actually expect KErrNotFound here
			error = KErrNone;
		else
			error = KErrGeneral;

		if (!error)
			error = CheckPreProx(foundPreProx, expectedPreProx, unExpectedPreProx);
		}

	// Next ask for all pre processors
	if (!error)
		{
		requestedPreProcType = 0;
		foundPreProx.Reset();
		expectedPreProx.Reset();
		unExpectedPreProx.Reset();
		User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice1));
		User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice2));
		User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice3));
		User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice4));
		User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice5));

		TRAP(error, aDevVideoRecord.FindPreProcessorsL(requestedPreProcType, foundPreProx));
		if (!error)
			error = CheckPreProx(foundPreProx, expectedPreProx, unExpectedPreProx);
		}

	// Next ask for a specific subset of pre processors
	if (!error)
		{
		requestedPreProcType = EPpMirror;
		foundPreProx.Reset();
		expectedPreProx.Reset();
		unExpectedPreProx.Reset();
		User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice1));
		User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice2));
		User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice3));
		User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice4));
		User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice5));

		TRAP(error, aDevVideoRecord.FindPreProcessorsL(requestedPreProcType, foundPreProx));
		if (!error)
			error = CheckPreProx(foundPreProx, expectedPreProx, unExpectedPreProx);
		}
	
	// Next ask for another specific subset of pre processors
	if (!error)
		{
		requestedPreProcType = EPpMirror|EPpInputCrop;
		foundPreProx.Reset();
		expectedPreProx.Reset();
		User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice1));
		User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice2));
		User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice3));
		User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice4));
		User::LeaveIfError(unExpectedPreProx.Append(KUidDevVideoTestPreProcHwDevice5));

		TRAP(error, aDevVideoRecord.FindPreProcessorsL(requestedPreProcType, foundPreProx));
		if (!error)
			error = CheckPreProx(foundPreProx, expectedPreProx, unExpectedPreProx);
		}

	CleanupStack::PopAndDestroy(3);//foundPreProx, expectedPreProx, unExpectedPreProx
	
	if (!error)
		ret = EPass;
		
	return ret;
	}

TInt CTestDevVideoRecordFindPreProc::CheckPreProx(const RArray<TUid>& aFoundPreProx, 
												  const RArray<TUid>& aExpectedPreProx, 
												  const RArray<TUid>& aUnExpectedPreProx)
	{
	TInt ret = KErrGeneral;

	TBool foundUnExpected = EFalse;
	TInt numberToBeFoundInExpectedPreProx = aExpectedPreProx.Count();

	for (TInt i=0; i<aFoundPreProx.Count(); i++)
		{
		TUid uidToLookFor = aFoundPreProx[i];
		for (TInt j=0; j<aExpectedPreProx.Count(); j++)
			{
			if (aExpectedPreProx[j] == uidToLookFor)
				{
				numberToBeFoundInExpectedPreProx--;
				break;//j
				}
			}
		for (TInt k=0; k<aUnExpectedPreProx.Count(); k++)
			{
			if (aUnExpectedPreProx[k] == uidToLookFor)
				{
				foundUnExpected = ETrue;
				break;//k
				}
			}
		}

	if ((!foundUnExpected) && (numberToBeFoundInExpectedPreProx==0))
		ret = KErrNone;

	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordListEncoders::CTestDevVideoRecordListEncoders(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordListEncoders* CTestDevVideoRecordListEncoders::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordListEncoders* self = new(ELeave) CTestDevVideoRecordListEncoders(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordListEncoders::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  ListEncoders"));
		
	RArray<TUid> foundEncoders;
	CleanupClosePushL(foundEncoders);
	RArray<TUid> expectedEncoders;
	CleanupClosePushL(expectedEncoders);

	User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice1));
	User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice2));
	User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice3));
	User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice4));
	User::LeaveIfError(expectedEncoders.Append(KUidDevVideoTestEncodeHwDevice5));

	TRAPD(err, aDevVideoRecord.GetEncoderListL(foundEncoders));

	if (!err)
		{
		// Make sure that the video hardware device adapter is NOT in the array
		TBool foundAdapter = EFalse;
		for (TInt j = 0; j < foundEncoders.Count(); j++)
			{
			if (foundEncoders[j] == KUidMDFVideoEncoderHwDeviceAdapter)
				{
				foundAdapter = ETrue;
				break;
				}
			}

		// Make sure that all the UIDs of the test plugins are in the array..
		TInt numberToBeFound = expectedEncoders.Count();
		for (TInt i=0; i<expectedEncoders.Count(); i++)
			{
			TUid uidToFind = expectedEncoders[i];
			for (TInt j=0; j<foundEncoders.Count(); j++)
				{
				if (foundEncoders[j] == uidToFind)
					{
					numberToBeFound--;
					break;//j
					}
				}
			}
		if ((numberToBeFound == 0) && (!foundAdapter))
			{
			ret = EPass;
			}
		}

	CleanupStack::PopAndDestroy(2);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordListPreProc::CTestDevVideoRecordListPreProc(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordListPreProc* CTestDevVideoRecordListPreProc::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordListPreProc* self = new(ELeave) CTestDevVideoRecordListPreProc(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordListPreProc::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  ListPreProc"));
		
	RArray<TUid> foundPreProx;
	CleanupClosePushL(foundPreProx);
	RArray<TUid> expectedPreProx;
	CleanupClosePushL(expectedPreProx);

	User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice1));
	User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice2));
	User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice3));
	User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice4));
	User::LeaveIfError(expectedPreProx.Append(KUidDevVideoTestPreProcHwDevice5));

	TRAPD(err, aDevVideoRecord.GetPreProcessorListL(foundPreProx));

	if (!err)
		{
		// Make sure that all the UIDs of the test plugins are in the array..
		TInt numberToBeFound = expectedPreProx.Count();
		for (TInt i=0; i<expectedPreProx.Count(); i++)
			{
			TUid uidToFind = expectedPreProx[i];
			for (TInt j=0; j<foundPreProx.Count(); j++)
				{
				if (foundPreProx[j] == uidToFind)
					{
					numberToBeFound--;
					break;//j
					}
				}
			}
		if (numberToBeFound == 0)
			ret = EPass;
		}

	CleanupStack::PopAndDestroy(2);

	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordGetEncoderInfo::CTestDevVideoRecordGetEncoderInfo(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordGetEncoderInfo* CTestDevVideoRecordGetEncoderInfo::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordGetEncoderInfo* self = new(ELeave) CTestDevVideoRecordGetEncoderInfo(aTestName, aTestType);
	return self;
	}

TBool CTestDevVideoRecordGetEncoderInfo::ValidEncoderInfo(CVideoEncoderInfo* aInfo)
	{
	// do null pointer check
	if (!aInfo) 
		{
		ERR_PRINTF1(_L("Error - Encoder Info is NULL!"));
		return EFalse;
		}
	
	// check the encoder against test data
	// need to check every call so as to keep code coverage
	// and prevent a situation where if the first test fails
	// then subsequent tests will not be called
	TBool ret = ETrue;

	// UID check
	if (aInfo->Uid() != KUidDevVideoTestEncodeHwDevice)
		{
		ERR_PRINTF1(_L("Error - Encoder UID is incorrect!"));
		ret = EFalse;
		}
	// Manufacturer check
	if (!(aInfo->Manufacturer() == KTestEncoderInfoManufacturer))
		{
		ERR_PRINTF1(_L("Error - Manufacturer is incorrect!"));
		ret = EFalse;
		}
	// Identifier check
	if (!(aInfo->Identifier() == KTestEncoderInfoIdentifier))
		{
		ERR_PRINTF1(_L("Error - Identifier is incorrect!"));
		ret = EFalse;
		}
	// Version check
	TVersion decVer = aInfo->Version();
	if ( (decVer.iMajor != KTestEncoderInfoVersionMaj) ||
		 (decVer.iMinor != KTestEncoderInfoVersionMin) ||
		 (decVer.iBuild != KTestEncoderInfoVersionBuild) )
		{
		ERR_PRINTF1(_L("Error - Version is incorrect!"));
		ret = EFalse;
		}
	// Accelerated check
	if (!aInfo->Accelerated())
		{
		ERR_PRINTF1(_L("Error - Accelerated = EFalse!"));
		ret = EFalse;
		}
	// Direct display check
	if (!aInfo->SupportsDirectCapture())
		{
		ERR_PRINTF1(_L("Error - Supports Direct Capture = EFalse!"));
		ret = EFalse;
		}
	// Max picture size
	if (!(aInfo->MaxPictureSize() == TSize(KTestEncoderInfoMaxSizeX, KTestEncoderInfoMaxSizeY)))
		{
		ERR_PRINTF1(_L("Error - Max picture size is incorrect!"));
		ret = EFalse;
		}
	// Supported data unit types
	if (aInfo->SupportedDataUnitTypes() != KTestUnitType1)
		{
		ERR_PRINTF1(_L("Error - Supported data unit types is incorrect!"));
		ret = EFalse;
		}
	// Supported data unit encapsulation
	if (aInfo->SupportedDataUnitEncapsulations() != KTestEncapType1)
		{
		ERR_PRINTF1(_L("Error - Supported data unit encaps is incorrect!"));
		ret = EFalse;
		}
	// Max bitrate layers
	if (aInfo->MaxBitrateLayers() != KTestNumBitrateLayers)
		{
		ERR_PRINTF1(_L("Error - Max bitrate layers is incorrect!"));
		ret = EFalse;
		}
	// Supports supplemental enhancement info
	if (!aInfo->SupportsSupplementalEnhancementInfo())
		{
		ERR_PRINTF1(_L("Error - Supports Enhancement Info = EFalse!"));
		ret = EFalse;
		}
	// Max unequal error protection levels
	if (aInfo->MaxUnequalErrorProtectionLevels() != KTestEncoderInfoMaxUEPLevels)
		{
		ERR_PRINTF1(_L("Error - Max UEP levels is incorrect!"));
		ret = EFalse;
		}
	// Max bitrate
	if (aInfo->MaxBitrate() != KTestEncoderInfoMaxBitrate)
		{
		ERR_PRINTF1(_L("Error - Max bitrate is incorrect!"));
		ret = EFalse;
		}
	// Max in layer scalability steps
	if (aInfo->MaxInLayerScalabilitySteps() != KTestEncoderInfoMaxILSSteps)
		{
		ERR_PRINTF1(_L("Error - Max ILS steps is incorrect!"));
		ret = EFalse;
		}
	// Supported picture options
	if (aInfo->SupportedPictureOptions() != KTestEncoderInfoPictureOptions)
		{
		ERR_PRINTF1(_L("Error - Picture options is incorrect!"));
		ret = EFalse;
		}

	// Supports Picture Loss
	if (aInfo->SupportsPictureLoss())
		{
		ERR_PRINTF1(_L("Error - Supports Picture Loss = ETrue!"));
		ret = EFalse;
		}

	// Supports Slice Loss
	if (!aInfo->SupportsSliceLoss())
		{
		ERR_PRINTF1(_L("Error - Supports Slice Loss = EFalse!"));
		ret = EFalse;
		}

	// Coding standard info
	if (!(aInfo->CodingStandardSpecificInfo() == KTestEncoderInfoCSInfo))
		{
		ERR_PRINTF1(_L("Error - Coding standard specific info is incorrect!"));
		ret = EFalse;
		}
	// Implementation info
	if (!(aInfo->ImplementationSpecificInfo() == KTestEncoderInfoISInfo))
		{
		ERR_PRINTF1(_L("Error - Implementation specific info is incorrect!"));
		ret = EFalse;
		}

	// get some test compressed video formats
	CCompressedVideoFormat* testCVFormat = NULL;
	TInt err = KErrNone;

	// get output formats supported
	const RPointerArray<CCompressedVideoFormat>& vidFormatsCV = aInfo->SupportedOutputFormats();

	// test output formats supported
	for (TUint i = 0; i < KTestEncoderInfoCount; i++)
		{
		TPtrC8 mimeType = KTestEncoderInfoMimeArray[i];
		TRAP(err, testCVFormat = GetTestCVFormatL(mimeType));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Error - Failed to construct CCompressedVideoFormat: %d!"), i);
			ret = EFalse;
			break;
			}

		if (!(*testCVFormat == *(vidFormatsCV[i])) || !(aInfo->SupportsOutputFormat(*testCVFormat)))
			{
			ERR_PRINTF2(_L("Error - Video Format %d is corrupt!"), i);
			ret = EFalse;
			}

		delete testCVFormat; 
		testCVFormat = NULL;
		}

	// finally check an output format which isn't supposed to be supported
	TRAP(err, testCVFormat = GetTestCVFormatL(_L8("Frederick Bloggs and his amazing coloured dog")));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - Failed to construct final CCompressedVideoFormat!"));
		ret = EFalse;
		}
	else 
		{
		if (aInfo->SupportsOutputFormat(*testCVFormat))
			{
			ERR_PRINTF1(_L("Error - Failed negative SupportsOutputFormat() check!"));
			ret = EFalse;
			}
		}

	delete testCVFormat;

	// get some test uncompressed video formats
	TUncompressedVideoFormat testUCFormat;

	// get supported formats
	const RArray<TUncompressedVideoFormat>& vidFormatsUC = aInfo->SupportedInputFormats();
	
	// get supported max picture rates
	TPictureRateAndSize testRate;
	const RArray<TPictureRateAndSize>& maxRates = aInfo->MaxPictureRates();

	// test input formats supported
	for (TUint j = 0; j < KTestPostProcInfoCount; j++)
		{
		testUCFormat = KTestPostProcInfoFormatArray[j];
		if (!(testUCFormat == (vidFormatsUC[j])) || !(aInfo->SupportsInputFormat(testUCFormat)))
			{
			ERR_PRINTF2(_L("Error - Video Format %d is corrupt!"), j);
			ret = EFalse;
			}
	
		GetTestEncoderInfoRate(j, testRate);
		if (!(testRate.iPictureSize == maxRates[j].iPictureSize) || 
			!(testRate.iPictureRate == maxRates[j].iPictureRate) )
			{
			ERR_PRINTF2(_L("Error - picture rate %d is corrupt!"), j);
			ret = EFalse;
			}
		}
	
	// check an input format that isn't supposed to be supported
	testUCFormat = KTestPostProcInfoNegativeFormat;
	testUCFormat.iYuvFormat = KTestYuvFormat1;
	if (aInfo->SupportsInputFormat(testUCFormat))
		{
		ERR_PRINTF1(_L("Error - Failed negative SupportsInputFormat() check!"));
		ret = EFalse;
		}

	return ret;
	}

TVerdict CTestDevVideoRecordGetEncoderInfo::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  GetEncoderInfo"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	TInt err = KErrNone;
	CVideoEncoderInfo *encInfo = NULL;
	TBool valid = EFalse;
	
	// get encoder information from test encoder plugin
	TRAP(err, 
		encInfo = aDevVideoRecord.VideoEncoderInfoLC(KUidDevVideoTestEncodeHwDevice); 
		valid = ValidEncoderInfo(encInfo);
		CleanupStack::PopAndDestroy(encInfo); );

	if (err != expErr)
		{
		ERR_PRINTF3(_L("VideoEncoderInfoLC() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	// check the Video Encoder Info class is valid
	else if (!valid)
		{
		ERR_PRINTF1(_L("CVideoEncoderInfo class is corrupt!"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("CVideoEncoderInfo passed check successfully"));
		INFO_PRINTF3(_L("VideoEncoderInfoLC(), %d = %d"), err, expErr);
		}
		
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordGetPreProcInfo::CTestDevVideoRecordGetPreProcInfo(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordGetPreProcInfo* CTestDevVideoRecordGetPreProcInfo::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordGetPreProcInfo* self = new(ELeave) CTestDevVideoRecordGetPreProcInfo(aTestName, aTestType);
	return self;
	}

TBool CTestDevVideoRecordGetPreProcInfo::ValidPreProcInfo(CPreProcessorInfo* aInfo)
	{
	// do null pointer check
	if (!aInfo) 
		{
		ERR_PRINTF1(_L("Error - Post Processor Info is NULL!"));
		return EFalse;
		}
	
	// check the post processor against test data
	// need to check every call so as to keep code coverage
	// and prevent a situation where if the first test fails
	// then subsequent tests will not be called
	TBool ret = ETrue;

	// UID check
	if (aInfo->Uid() != KUidDevVideoTestPreProcHwDevice)
		{
		ERR_PRINTF1(_L("Error - Post Processor UID is incorrect!"));
		ret = EFalse;
		}
	// Manufacturer check
	if (!(aInfo->Manufacturer() == KTestPreProcInfoManufacturer))
		{
		ERR_PRINTF1(_L("Error - Manufacturer is incorrect!"));
		ret = EFalse;
		}
	// Identifier check
	if (!(aInfo->Identifier() == KTestPreProcInfoIdentifier))
		{
		ERR_PRINTF1(_L("Error - Identifier is incorrect!"));
		ret = EFalse;
		}
	// Version check
	TVersion posVer = aInfo->Version();
	if ( (posVer.iMajor != KTestPreProcInfoVersionMaj) ||
		 (posVer.iMinor != KTestPreProcInfoVersionMin) ||
		 (posVer.iBuild != KTestPreProcInfoVersionBuild) )
		{
		ERR_PRINTF1(_L("Error - Version is incorrect!"));
		ret = EFalse;
		}
	// Accelerated check
	if (!aInfo->Accelerated())
		{
		ERR_PRINTF1(_L("Error - Accelerated = EFalse!"));
		ret = EFalse;
		}
	// Direct display check
	if (!aInfo->SupportsDirectCapture())
		{
		ERR_PRINTF1(_L("Error - Supports Direct Capture = EFalse!"));
		ret = EFalse;
		}
	// YUVToYUV Capabilities
	if (!(CompareYuvYuvCapabilities(aInfo->YuvToYuvCapabilities(), KTestPreProcInfoYuvToYuvCaps)))
		{
		ERR_PRINTF1(_L("Error - YUV To YUV capabilities is incorrect!"));
		ret = EFalse;
		}

	// Supported rotations
	if (aInfo->SupportedRotations() != KTestPreProcInfoRotations)
		{
		ERR_PRINTF1(_L("Error - Supported Rotations is incorrect!"));
		ret = EFalse;
		}
	// Supported RGB ranges
	if (aInfo->SupportedRgbRanges() != KTestPreProcInfoRgbRanges)
		{
		ERR_PRINTF1(_L("Error - Supported RGB ranges is incorrect!"));
		ret = EFalse;
		}
	// Supports arbitrary scaling check
	if (!aInfo->SupportsAntiAliasedScaling())
		{
		ERR_PRINTF1(_L("Error - Anti-Aliased Scaling = EFalse!"));
		ret = EFalse;
		}
	// Supports anti-aliased scaling check
	if (!aInfo->SupportsArbitraryScaling())
		{
		ERR_PRINTF1(_L("Error - Supports Arbitrary Scaling = EFalse!"));
		ret = EFalse;
		}
	// Implementation info
	if (!(aInfo->ImplementationSpecificInfo() == KTestPreProcInfoISInfo))
		{
		ERR_PRINTF1(_L("Error - Implementation specific info is incorrect!"));
		ret = EFalse;
		}

	// get some test compressed video formats
	TUncompressedVideoFormat testFormat;
	TUint32 testComb = 0;
	TScaleFactor testFactor = {0, 0};

	// get supported formats
	const RArray<TUncompressedVideoFormat>& vidFormatsOutput = aInfo->SupportedOutputFormats();
	const RArray<TUncompressedVideoFormat>& vidFormatsInput = aInfo->SupportedInputFormats();
	
	// get supported combinations
	const RArray<TUint32>& suppCombs = aInfo->SupportedCombinations();

	// get supported scale factors
	const RArray<TScaleFactor>& scaleFactors = aInfo->SupportedScaleFactors();

	// test formats, combinations, and scale factors supported
	// can use test data from PostProcessor constants
	for (TUint i = 0; i < KTestPostProcInfoCount; i++)
		{
		// input formats
		testFormat = KTestPreProcInfoInputFormatArray[i];
		if (!(testFormat == (vidFormatsInput[i])) || !(aInfo->SupportsInputFormat(testFormat)))
			{
			ERR_PRINTF2(_L("Error - Input Video Format %d is corrupt!"), i);
			ret = EFalse;
			}
		// output formats
		testFormat = KTestPreProcInfoOutputFormatArray[i];
		if (!(testFormat == (vidFormatsOutput[i])) || !(aInfo->SupportsOutputFormat(testFormat)))
			{
			ERR_PRINTF2(_L("Error - Output Video Format %d is corrupt!"), i);
			ret = EFalse;
			}
		// combinations
		testComb = KTestPostProcInfoCombsArray[i];
		if (!(testComb == (suppCombs[i])) || !(aInfo->SupportsCombination(testComb)))
			{
			ERR_PRINTF2(_L("Error - Combination %d is corrupt!"), i);
			ret = EFalse;
			}
		// scale factors
		testFactor = KTestPostProcInfoScaleFactorsArray[i];
		if (!CompareScaleFactors(testFactor, scaleFactors[i]))
			{
			ERR_PRINTF2(_L("Error - Scale factor %d is corrupt!"), i);
			ret = EFalse;
			}
		}

	// check an input format that isn't supposed to be supported
	testFormat = KTestPreProcInfoNegativeFormat;
	testFormat.iYuvFormat = KTestYuvFormat1;
	if (aInfo->SupportsInputFormat(testFormat))
		{
		ERR_PRINTF1(_L("Error - Failed negative SupportsInputFormat() check!"));
		ret = EFalse;
		}
	
	// check an output format that isn't supposed to be supported
	testFormat = KTestPreProcInfoNegativeFormat;
	testFormat.iYuvFormat = KTestYuvFormat2;
	if (aInfo->SupportsOutputFormat(testFormat))
		{
		ERR_PRINTF1(_L("Error - Failed negative SupportsOutputFormat() check!"));
		ret = EFalse;
		}

	// finally check a combination that isn't supposed to be supported
	testComb = KTestPostProcInfoNegativeComb;
	if (aInfo->SupportsCombination(testComb))
		{
		ERR_PRINTF1(_L("Error - Failed negative SupportsCombination() check!"));
		ret = EFalse;
		}

	return ret;
	}

TVerdict CTestDevVideoRecordGetPreProcInfo::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  GetPreProcInfo"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	TInt err = KErrNone;
	CPreProcessorInfo *preInfo = NULL;
	TBool valid = EFalse;

	// get preproc information from test preproc plugin
	TRAP(err, 
		preInfo = aDevVideoRecord.PreProcessorInfoLC(KUidDevVideoTestPreProcHwDevice); 
		valid = ValidPreProcInfo(preInfo);
		CleanupStack::PopAndDestroy(preInfo); );

	if (err != expErr)
		{
		ERR_PRINTF3(_L("PreProcessorInfoLC() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	// check the Video PreProcessor Info class is valid
	else if (!valid)
		{
		ERR_PRINTF1(_L("CPreProcessorInfoLC class is corrupt!"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("PreProcessorInfoLC passed check successfully"));
		INFO_PRINTF3(_L("PreProcessorInfoLC(), %d = %d"), err, expErr);
		}
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSelectEncoder::CTestDevVideoRecordSelectEncoder(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSelectEncoder* CTestDevVideoRecordSelectEncoder::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSelectEncoder* self = new(ELeave) CTestDevVideoRecordSelectEncoder(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSelectEncoder::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SelectEncoder"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	iError = KErrNone;

	// this will leave if an error occurs
	TRAP( iError, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice) );

	if (iError != expErr)
		{
		ERR_PRINTF3(_L("SelectEncoderL() gave error %d (expected %d)"),iError, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SelectEncoderL(), %d = %d"), iError, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSelectPreProc::CTestDevVideoRecordSelectPreProc(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSelectPreProc* CTestDevVideoRecordSelectPreProc::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSelectPreProc* self = new(ELeave) CTestDevVideoRecordSelectPreProc(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSelectPreProc::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SelectPreProc"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	iError = KErrNone;
	
	// this will leave if an error occurs
	TRAP( iError, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice) );

	if (iError != expErr)
		{
		ERR_PRINTF3(_L("SelectPreProcessorL() gave error %d (expected %d)"),iError, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SelectPreProcessorL(), %d = %d"), iError, expErr);
	
	return ret;
	}

//------------------------------------------------------------------

CTestDevVideoRecordSetInputFormat::CTestDevVideoRecordSetInputFormat(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetInputFormat* CTestDevVideoRecordSetInputFormat::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetInputFormat* self = new(ELeave) CTestDevVideoRecordSetInputFormat(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetInputFormat::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetInputFormat"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

	THwDeviceId hwEncoder = 0;
	THwDeviceId hwPreProc = 0;

	// select Encoder
	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}
	
	TSize testSize1(KTestInputSize1X, KTestInputSize1Y);
	TSize testSize2(KTestInputSize2X, KTestInputSize2Y);

	// set input format on encoder
	TRAP(err, aDevVideoRecord.SetInputFormatL(hwEncoder, KTestVidFormat1, testSize1) );
	
	// set input format on pre-processor
	TRAP(err2, aDevVideoRecord.SetInputFormatL(hwPreProc, KTestVidFormat2, testSize2) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetInputFormatL() on Encoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetInputFormatL() on Pre-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetInputFormat(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetSourceCam::CTestDevVideoRecordSetSourceCam(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetSourceCam* CTestDevVideoRecordSetSourceCam::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetSourceCam* self = new(ELeave) CTestDevVideoRecordSetSourceCam(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetSourceCam::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	TBool selEnc = ETrue;
	TBool selPre = ETrue;
	TInt testCam = KTestCamHandlePre;

	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetSourceCam"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestBothFatal:
			expErr = KErrDied;
			testCam = KTestCamHandleFatal;
			break;
		case ETestEncoderFatal:
			expErr = KErrDied;
			testCam = KTestCamHandleFatal;
			selPre = EFalse;
			break;
		case ETestEncoderOnly:
			testCam = KTestCamHandleEnc;
			selPre = EFalse;
			break;
		case ETestPreProcFatal:
			expErr = KErrDied;
			testCam = KTestCamHandleFatal;
			// fall through to set encoder to false;
		case ETestPreProcOnly:
			selEnc = EFalse;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
	if (selEnc)
		{
//		TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
		TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select pre-processor
	if (selPre)
		{
//		TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
		TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
				return EInconclusive;
			}
		}

	// set source cam on devvideo
	TRAP(iError, aDevVideoRecord.SetSourceCameraL(testCam, KTestPictureRate) );
	
	// if performing a fatal test then iError is set to KErrDied in FatalError callback
	if (iError != expErr)
		{
		ERR_PRINTF3(_L("SetSourceCameraL() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetSourceCamera(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetSourceMem::CTestDevVideoRecordSetSourceMem(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetSourceMem* CTestDevVideoRecordSetSourceMem::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetSourceMem* self = new(ELeave) CTestDevVideoRecordSetSourceMem(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetSourceMem::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetSourceMem"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
//	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
//	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	// set source mem on devvideo
	TRAP(err, aDevVideoRecord.SetSourceMemoryL(KTestPictureRate, ETrue, EFalse) );
	
	if (err != expErr)
		{
		ERR_PRINTF3(_L("SetSourceMemoryL() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetSourceMemoryL(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetOutputFormat::CTestDevVideoRecordSetOutputFormat(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetOutputFormat* CTestDevVideoRecordSetOutputFormat::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetOutputFormat* self = new(ELeave) CTestDevVideoRecordSetOutputFormat(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetOutputFormat::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetOutputFormat"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
//	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	// get a temporary compressed video class
	CCompressedVideoFormat *tempCFormat = NULL;
	TRAP(err, tempCFormat = GetTestCVFormatL(KTestMimeType1));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize compressed video class"));
			return EInconclusive;
		}

	// this will leave if an error occurs
	TRAP(err, aDevVideoRecord.SetOutputFormatL(hwEncoder, *tempCFormat, KTestUnitType1, KTestEncapType1, ETrue) );

	// delete compressed video class
	delete tempCFormat;	
	tempCFormat = NULL;

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SetOutputFormatL() on Encoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetOutputFormat(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetOutputFormatUC::CTestDevVideoRecordSetOutputFormatUC(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetOutputFormatUC* CTestDevVideoRecordSetOutputFormatUC::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetOutputFormatUC* self = new(ELeave) CTestDevVideoRecordSetOutputFormatUC(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetOutputFormatUC::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetOutputFormatUC"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
	THwDeviceId hwPreProc = 0;

	// select Encoder
//	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	// this will leave if an error occurs in the plugin
	TRAP(err, aDevVideoRecord.SetOutputFormatL(hwPreProc, KTestVidFormat2) );

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SetOutputFormatL() on Pre-Processor gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetOutputFormatL(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetOutputRect::CTestDevVideoRecordSetOutputRect(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetOutputRect* CTestDevVideoRecordSetOutputRect::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetOutputRect* self = new(ELeave) CTestDevVideoRecordSetOutputRect(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetOutputRect::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetOutputRect"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
//	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	// set output rect on encoder
	TRect testRect1(KTestInputCropRectA, KTestInputCropRectB, KTestInputCropRectC, KTestInputCropRectD);
	TRAP(err, aDevVideoRecord.SetOutputRectL(hwEncoder, testRect1) );

	if (err != expErr)
		{
		ERR_PRINTF3(_L("SetOutputRectL() on Encoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetOutputRectL(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetClockSource::CTestDevVideoRecordSetClockSource(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetClockSource* CTestDevVideoRecordSetClockSource::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetClockSource* self = new(ELeave) CTestDevVideoRecordSetClockSource(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetClockSource::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selEnc = ETrue;
	TBool selPre = ETrue;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetClockSource"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;	// EABI warning removal
			break;
		case ETestEncoderOnly:
			selPre = EFalse;
			break;
		case ETestPreProcOnly:
			selEnc = EFalse;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
	if (selEnc)
		{
//		TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
		TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select pre-processor
	if (selPre)
		{
//		TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
		TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
				return EInconclusive;
			}
		}

	CSystemClockSource *time1 = NULL;

	// no parameters so no need to have a separate 'test clock source'
	TRAP(err, time1 = CSystemClockSource::NewL());
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't construct system clock class"));
		
		return EInconclusive;
		}
	
	// set clock source 
	aDevVideoRecord.SetClockSource(time1);
	
	// if there is no panic then test has passed

	INFO_PRINTF1(_L("SetClockSourceL() called correctly on HW Devices."));
	
	// delete clock source
	delete time1; time1 = NULL;

	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetPreProcTypes::CTestDevVideoRecordSetPreProcTypes(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetPreProcTypes* CTestDevVideoRecordSetPreProcTypes::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetPreProcTypes* self = new(ELeave) CTestDevVideoRecordSetPreProcTypes(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetPreProcTypes::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetPreProcTypes"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

	THwDeviceId hwEncoder = 0;
	THwDeviceId hwPreProc = 0;

	// select Encoder
	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	// set pre-process types on encoder
	TRAP(err, aDevVideoRecord.SetPreProcessTypesL(hwEncoder, KTestProcessType1) )
	
	// set pre-process types on pre-processor
	TRAP(err2, aDevVideoRecord.SetPreProcessTypesL(hwPreProc, KTestProcessType2) )


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetPreProcessTypesL() on Encoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetPreProcessTypesL() on Pre-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetPreProcessTypes(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------


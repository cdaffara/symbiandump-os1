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

#include <mmf/devvideo/devvideoplay.h>
#include <mmf/devvideo/devvideostandardcustominterfaces.h>
#include "testdevvideociurecord.h"
#include "testdevvideociuplugins/ciuencoder.h"
#include "testdevvideociuplugins/ciupreproc.h"
#include "testdevvideociuplugins/ciudevvideotestinterface.h"

// DevVideoCiuRecord base class

CTestDevVideoCiuRecordStep::CTestDevVideoCiuRecordStep(const TDesC& aTestName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	}
	
CTestDevVideoCiuRecordStep::~CTestDevVideoCiuRecordStep() 
	{
	}
	
TVerdict CTestDevVideoCiuRecordStep::DoTestStepPreambleL()
	{
	TVerdict ret = CTestDevVideoCiuStep::DoTestStepPreambleL();
	
	__MM_HEAP_MARK;
	TRAPD(err, iDevVideoRecord = CMMFDevVideoRecord::NewL( *this ));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Could not create a CMMFDevVideoRecord!"));
		ret = EInconclusive;
		}
	
 	return ret;
	}
	
TVerdict CTestDevVideoCiuRecordStep::DoTestStepPostambleL()
	{
	delete iDevVideoRecord;
	__MM_HEAP_MARKEND;
	
 	return EPass;
	}	

TVerdict CTestDevVideoCiuRecordStep::DoTestStepL()
	{
	TVerdict ret = EFail;
	
	iDevVideoRecord->Initialize();
	
	if(iError != KErrNone)
		{
		ERR_PRINTF2( _L("CMMFDevVideoRecord failed to initialise with error %d"), iError );		
		}
	else
		{
		// call the test step
		ret = DoTestL();
		}
	return ret;
	}

// MMMFDevVideoRecordObserver
void CTestDevVideoCiuRecordStep::MdvroNewBuffers()
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuRecordStep::MdvroNewBuffers()"));
	}

void CTestDevVideoCiuRecordStep::MdvroReturnPicture(TVideoPicture* /*aPicture*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuRecordStep::MdvroReturnPicture()"));
	}

void CTestDevVideoCiuRecordStep::MdvroSupplementalInfoSent()
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuRecordStep::MdvroSupplementalInfoSent()"));
	}

void CTestDevVideoCiuRecordStep::MdvroFatalError(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("CTestDevVideoCiuRecordStep::MdvroFatalError()\nError = %d"), aError);
	}

void CTestDevVideoCiuRecordStep::MdvroInitializeComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("CTestDevVideoCiuRecordStep::MdvroInitializeComplete():  Error = %d"), aError);
	}

void CTestDevVideoCiuRecordStep::MdvroStreamEnd()
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuRecordStep::MdvroStreamEnd()"));
	}

//------------------------------------------------------------------

CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc::CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc
														(const TDesC& aTestName) 
														: CTestDevVideoCiuRecordStep(aTestName)
	{
	}

CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc* CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc
													::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc* self = new(ELeave) 
					CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();

	// select the preprocessor
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectPreProcessorL(KUidDevVideoCiuTestPreProcHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		ERR_PRINTF1(_L("DevVideo Record Failed to select the PreProcessor."));
		}
	return ret;
	}

TVerdict CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc::DoTestL()
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("DevVideo CIU Video Media Protected Creation Failure from PreProc"));
	
	// Got this far so must have successfully initialised DevVideoRecord
	// Try to fetch the CI...
	MMmfVideoMediaProtected* interface = NULL;
	interface = static_cast<MMmfVideoMediaProtected*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																					KUidMmfVideoMediaProtected));
	if (!interface)
		{
		INFO_PRINTF1(_L("No interface retrieved"));
		}
	else
		{
		ERR_PRINTF1(_L("Error! An interface was retrieved"));
		ret = EFail;
		}

	return ret;
	}
	
//------------------------------------------------------------------
CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder::CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder
													(const TDesC& aTestName)
													: CTestDevVideoCiuRecordStep(aTestName)
	{
	}

CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder* CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder
													::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder* self = new(ELeave) 
					CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();

	// select the encoder
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectEncoderL(KUidDevVideoCiuTestEncodeHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the Encoder."));
		}
	return ret;
	}

TVerdict CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder::DoTestL()
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("DevVideo CIU Video Media Protected Creation Failure from Encoder"));
	
	// Got this far so must have successfully initialised DevVideoRecord
	// Try to fetch the CI...
	MMmfVideoMediaProtected* interface = NULL;
	interface = static_cast<MMmfVideoMediaProtected*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																					KUidMmfVideoMediaProtected));
	if (!interface)
		{
		INFO_PRINTF1(_L("No interface retrieved"));
		}
	else
		{
		ERR_PRINTF1(_L("Error! An interface was retrieved"));
		ret = EFail;
		}

	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc::CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc
													(const TDesC& aTestName)
													: CTestDevVideoCiuRecordStep(aTestName)
	{
	}

CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc* CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc* self = new(ELeave) CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();

	// select pre-processor
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectPreProcessorL(KUidDevVideoCiuTestPreProcHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		ERR_PRINTF1(_L("DevVideo Record Failed to select the pre processor."));
		}
	return ret;
	}
	
TVerdict CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc::DoTestL()
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("DevVideo CIU Video Input Snapshot Initialisation"));

	// Try to fetch the CI...
	MMmfVideoInputSnapshot* interface = NULL;
		
	interface = static_cast<MMmfVideoInputSnapshot*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																					KUidMmfVideoInputSnapshot));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Video Input Snapshot Instantiation Successfully"));
		}
		
	return ret;
	}
//------------------------------------------------------------------



CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc::CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc
														(const TDesC& aTestName)
														: CTestDevVideoCiuRecordStep(aTestName)
	{
	}

CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc* CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc* self = new(ELeave) CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();

	// select pre-processor
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectPreProcessorL(KUidDevVideoCiuTestPreProcHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		ERR_PRINTF1(_L("DevVideo Record Failed to select the pre processor."));
		}
	return ret;
	}
	
TVerdict CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc::DoTestL()
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("DevVideo CIU Video Input Snapshot Set Observer"));
	
	// Try to fetch the CI...
	MMmfVideoInputSnapshot* interface = NULL;
		
	interface = static_cast<MMmfVideoInputSnapshot*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																					KUidMmfVideoInputSnapshot));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EInconclusive;
		}
	else
		{
		MMmfVideoInputSnapshotObserver* obs = this;
		interface->MmvisSetObserver(obs);
		
		// Test interface to retrieve the observer
		MDevVideoCiuTestInterfaceInputSnapshot* testInterface = static_cast<MDevVideoCiuTestInterfaceInputSnapshot*>
															(iDevVideoRecord->CustomInterface(iHwDeviceId,
																				KUidDevVideoCiuTestInterfaceInputSnapshot));
		// DEF111127 
		SnapshotComplete(KErrNone);
																				
		if (testInterface)
			{
			if (testInterface->MmvisSnapshotObserver() != obs)
				{
				ERR_PRINTF1(_L("Error - different observer pointer returned!"));
				ret = EFail;
				}
			}
		}

	return ret;
	}

void CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc::SnapshotComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("Snapshot Complete callback received, error: "), iError);
	}
	
//------------------------------------------------------------------

CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc::CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc
															(const TDesC& aTestName)
															: CTestDevVideoCiuRecordStep(aTestName)
	{
	}

CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc* CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc* self = new(ELeave) CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();

	// select pre-processor
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectPreProcessorL(KUidDevVideoCiuTestPreProcHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the pre processor."));
		}
	return ret;
	}
	
TVerdict CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc::DoTestL()
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("DevVideo CIU Video Input Snapshot Get Snapshot"));
	
	// Try to fetch the CI...
	MMmfVideoInputSnapshot* interface = NULL;
		
	interface = static_cast<MMmfVideoInputSnapshot*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																					KUidMmfVideoInputSnapshot));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EInconclusive;
		}
	else
		{		
		// Test interface to set the snapshot
		MDevVideoCiuTestInterfaceInputSnapshot* testInterface = static_cast<MDevVideoCiuTestInterfaceInputSnapshot*>
															(iDevVideoRecord->CustomInterface(iHwDeviceId,
																				KUidDevVideoCiuTestInterfaceInputSnapshot));
																				
		if (testInterface)
			{
			// Set the value to retrieve
			TPictureData testPictureData;
			testPictureData.iDataFormat = EYuvRawData;
			TSize testSize(888, 999);
			testPictureData.iDataSize = testSize;
			TUncompressedVideoFormat testFormat;
			testFormat.iDataFormat = ERgbRawData;
			testFormat.iRgbFormat = EFbsBitmapColor16MU;
			testInterface->MmvisSetSnapshot(testPictureData, testFormat);

			// Retrieve the snapshot
			TPictureData pictureData;
			TRAPD(err, interface->MmvisGetSnapshotL(pictureData, testFormat));
	
			// Verify snapshot is as it should be
			if ((err == KErrNone) && 
				(pictureData.iDataSize == testPictureData.iDataSize) && 
				(pictureData.iDataFormat == testPictureData.iDataFormat))
				{
				INFO_PRINTF1(_L("Successfully retrieved the snapshot"));					
				}
			else
				{
				// uh-oh
				ERR_PRINTF2(_L("Error, %d. Different snapshot returned!"), err);
				ERR_PRINTF5(_L("Test size w=%d, h=%d. Returned size w=%d, h=%d"),testSize.iWidth
																				,testSize.iHeight
																				,pictureData.iDataSize.iWidth
																				,pictureData.iDataSize.iHeight);
				ERR_PRINTF3(_L("Test data format = %d, returned data format = %d"), testPictureData.iDataFormat,
																					pictureData.iDataFormat);

				ret = EFail;
				}				
			}
		else
			{
			ERR_PRINTF1(_L("Error - no test interface retrieved"));
			ret = EInconclusive;			
			}
		}
		
	return ret;
	}

//------------------------------------------------------------------
CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc::CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc
															(const TDesC& aTestName)
															: CTestDevVideoCiuRecordStep(aTestName)
	{
	}

CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc* CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc* self = new(ELeave) CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();

	// select pre-processor
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectPreProcessorL(KUidDevVideoCiuTestPreProcHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the pre processor."));
		}
	return ret;
	}
	
TVerdict CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc::DoTestL()
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("DevVideo CIU Video Input Snapshot Cancel Snapshot"));
	
	// Try to fetch the CI...
	MMmfVideoInputSnapshot* interface = NULL;
		
	interface = static_cast<MMmfVideoInputSnapshot*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																						KUidMmfVideoInputSnapshot));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EInconclusive;
		}
	else
		{		
		interface->MmvisCancelSnapshot();
	
		// Test interface to set the snapshot
		MDevVideoCiuTestInterfaceInputSnapshot* testInterface = static_cast<
																MDevVideoCiuTestInterfaceInputSnapshot*>(
																iDevVideoRecord->CustomInterface(
																				iHwDeviceId,
																				KUidDevVideoCiuTestInterfaceInputSnapshot
																				));																				
		if (testInterface)
			{
			if (testInterface->MmvisIsCancelSnapshot())
				{
				INFO_PRINTF1(_L("Successfully cancelled the snapshot"));
				}
			else
				{
				// uh-oh
				ERR_PRINTF1(_L("Error - failed to cancel the snapshot!"));
				ret = EFail;
				}			
				
			}
		}
		
	return ret;
	}

//------------------------------------------------------------------
CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc::CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc
															(const TDesC& aTestName) :
															CTestDevVideoCiuRecordStep(aTestName)
	{
	}

CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc* CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc* self = new(ELeave) CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();

	// select pre-processor
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectPreProcessorL(KUidDevVideoCiuTestPreProcHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the pre processor."));
		}
	return ret;
	}
	
TVerdict CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc::DoTestL()
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("DevVideo CIU Video Input Snapshot Cancel Snapshot"));
	
	// Try to fetch the CI...
	MMmfVideoInputSnapshot* interface = NULL;
		
	interface = static_cast<MMmfVideoInputSnapshot*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																					KUidMmfVideoInputSnapshot));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EInconclusive;
		}
	else
		{
		// Set the observer to callback
		MMmfVideoInputSnapshotObserver* obs = this;
		interface->MmvisSetObserver(obs);
		
		// Test interface to request the snapshot callback
		MDevVideoCiuTestInterfaceInputSnapshot* testInterface = static_cast<MDevVideoCiuTestInterfaceInputSnapshot*>
															(iDevVideoRecord->CustomInterface(iHwDeviceId,
																				KUidDevVideoCiuTestInterfaceInputSnapshot));
		if (testInterface)
			{
			// Prepare the error value so we can be sure it's changed
			iError = KErrNotSupported;
			testInterface->MmvisRequestSnapshotCompleteCallback();
			INFO_PRINTF1(_L("Requested the snapshot complete callback"));
			
			if (iError == KErrNone)
				{
				INFO_PRINTF1(_L("The snapshot complete callback was successful"));
				}
			else
				{
				// uh-oh
				ERR_PRINTF2(_L("Unsuccessful callback from snapshot complete, error: !"), iError);
				ret = EFail;
				}
			}
		else
			{
			// uh-oh
			ERR_PRINTF1(_L("Failed to get the test interface"));
			ret = EInconclusive;
			}
		}
		
	return ret;
	}

void CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc::SnapshotComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("Snapshot Complete callback received, error: "), iError);
	}

//
// Buffer Management Tests
//
CTestDevVideoCiuRecCreateVideoBufferManagement* CTestDevVideoCiuRecCreateVideoBufferManagement::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecCreateVideoBufferManagement* self = new (ELeave) CTestDevVideoCiuRecCreateVideoBufferManagement(aTestName);
	return self;
	}
	
TVerdict CTestDevVideoCiuRecCreateVideoBufferManagement::DoTestL()
	{
	TVerdict ret = EFail;
	
	MMmfVideoBufferManagement* interface = NULL;
	interface = static_cast<MMmfVideoBufferManagement*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																						KUidMmfVideoBufferManagement));
	
	if (!interface)
		{
		ERR_PRINTF1(_L("Failed to create the interface."));
		}
	else
		{
		INFO_PRINTF1(_L("Interface successfully created"));
		ret = EPass;
		}
		
	return ret;
	}
	
TVerdict CTestDevVideoCiuRecCreateVideoBufferManagement::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();
	
	// select the encoder
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectEncoderL(KUidDevVideoCiuTestEncodeHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the Encoder."));
		}
	
	return ret;
	}
	
CTestDevVideoCiuRecCreateVideoBufferManagement::CTestDevVideoCiuRecCreateVideoBufferManagement
												(const TDesC& aTestName)
												: CTestDevVideoCiuRecordStep(aTestName)
	{
	}
	
//	
//
// SetObserver
CTestDevVideoCiuRecSetObserverVideoBufferManagement* CTestDevVideoCiuRecSetObserverVideoBufferManagement
													::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecSetObserverVideoBufferManagement* self = new (ELeave) CTestDevVideoCiuRecSetObserverVideoBufferManagement(aTestName);
	return self;	
	}
	
TVerdict CTestDevVideoCiuRecSetObserverVideoBufferManagement::DoTestL()
	{
	TVerdict ret = EFail;
	
	MMmfVideoBufferManagement* interface = NULL;
	interface = static_cast<MMmfVideoBufferManagement*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																						KUidMmfVideoBufferManagement));
	
	if (!interface)
		{
		ERR_PRINTF1(_L("Failed to create the interface."));
		}
	else
		{
		INFO_PRINTF1(_L("Interface successfully created"));
		// Use the interface
		MMmfVideoBufferManagementObserver* obs = this;
		interface->MmvbmSetObserver(obs);
		INFO_PRINTF1(_L("SetObserver called"));
		
		// Verify that the observer was set
		MDevVideoCiuTestInterfaceBufferManagement* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceBufferManagement*>(iDevVideoRecord->
																				CustomInterface(iHwDeviceId,
																								KUidDevVideoCiuTestInterfaceBufferManagement));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			}
		else
			{
			if (obs != testInterface->MdvbmObserver())
				{
				ERR_PRINTF1(_L("Failed to set the observer."));
				}
			else
				{			
				INFO_PRINTF1(_L("SetObserver successfully called"));
				ret = EPass;
				}
			}
		}
		// DEF111127
		MmvbmoNewBuffers();
		MmvbmoReleaseBuffers();
		
	return ret;	
	}

TVerdict CTestDevVideoCiuRecSetObserverVideoBufferManagement::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();
	
	// select the encoder
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectEncoderL(KUidDevVideoCiuTestEncodeHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the Encoder."));
		}
	
	return ret;	
	}


// from MMmfVideoBufferManagementObserver
void CTestDevVideoCiuRecSetObserverVideoBufferManagement::MmvbmoNewBuffers()
	{
	}
	
void CTestDevVideoCiuRecSetObserverVideoBufferManagement::MmvbmoReleaseBuffers()
	{
	}

	
CTestDevVideoCiuRecSetObserverVideoBufferManagement::CTestDevVideoCiuRecSetObserverVideoBufferManagement
													(const TDesC& aTestName)
													: CTestDevVideoCiuRecordStep(aTestName)
	{
	}
//
//
// Enable
CTestDevVideoCiuRecEnableVideoBufferManagement* CTestDevVideoCiuRecEnableVideoBufferManagement::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecEnableVideoBufferManagement* self = new (ELeave) CTestDevVideoCiuRecEnableVideoBufferManagement(aTestName);
	return self;
	}
	
TVerdict CTestDevVideoCiuRecEnableVideoBufferManagement::DoTestL()
	{
	TVerdict ret = EFail;
	MMmfVideoBufferManagement* interface = NULL;
	interface = static_cast<MMmfVideoBufferManagement*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																						KUidMmfVideoBufferManagement));
	
	if (!interface)
		{
		ERR_PRINTF1(_L("Failed to create the interface."));
		}
	else
		{
		INFO_PRINTF1(_L("Interface successfully created"));
		// Use the interface
		interface->MmvbmEnable(ETrue);
		INFO_PRINTF1(_L("Enable called"));
		
		// Verify that the observer was set
		MDevVideoCiuTestInterfaceBufferManagement* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceBufferManagement*>(iDevVideoRecord->
																				CustomInterface(iHwDeviceId,
																								KUidDevVideoCiuTestInterfaceBufferManagement));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			}
		else
			{
			if (!testInterface->MdvbmIsEnabled())
				{
				ERR_PRINTF1(_L("Failed to set enable.."));
				}
			else
				{			
				INFO_PRINTF1(_L("Successfully enabled"));
				ret = EPass;
				}
			}
		}

	return ret;	
	}

TVerdict CTestDevVideoCiuRecEnableVideoBufferManagement::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();
		
	// select the encoder
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectEncoderL(KUidDevVideoCiuTestEncodeHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the Encoder."));
		}

	return ret;	
	}

CTestDevVideoCiuRecEnableVideoBufferManagement::CTestDevVideoCiuRecEnableVideoBufferManagement
												(const TDesC& aTestName)
												: CTestDevVideoCiuRecordStep(aTestName)
	{
	}
	
//
//
// SetBufferOptions
CTestDevVideoCiuRecSetOptionsVideoBufferManagement* CTestDevVideoCiuRecSetOptionsVideoBufferManagement::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecSetOptionsVideoBufferManagement* self = new (ELeave) CTestDevVideoCiuRecSetOptionsVideoBufferManagement(aTestName);
	return self;	
	}
	
TVerdict CTestDevVideoCiuRecSetOptionsVideoBufferManagement::DoTestL()
	{
	TVerdict ret = EFail;

	MMmfVideoBufferManagement* interface = NULL;
	interface = static_cast<MMmfVideoBufferManagement*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																						KUidMmfVideoBufferManagement));
	
	if (!interface)
		{
		ERR_PRINTF1(_L("Failed to create the interface."));
		}
	else
		{
		INFO_PRINTF1(_L("Interface successfully created"));

		MMmfVideoBufferManagement::TBufferOptions opts;
		opts.iNumInputBuffers = 999;
		opts.iBufferSize.iWidth = 888;
		opts.iBufferSize.iHeight = 777;

		TRAPD(err, interface->MmvbmSetBufferOptionsL(opts));
		
		if (err == KErrNone)
			{
			INFO_PRINTF1(_L("SetBufferOptions called"));
		
			// Verify that the options were set
			MMmfVideoBufferManagement::TBufferOptions testOpts;
			interface->MmvbmGetBufferOptions(testOpts);
			if ((testOpts.iNumInputBuffers != opts.iNumInputBuffers) ||
				(testOpts.iBufferSize.iWidth != opts.iBufferSize.iWidth) ||
				(testOpts.iBufferSize.iHeight != opts.iBufferSize.iHeight))
				{
				ERR_PRINTF1(_L("Failed to set the options."));
				}
			else
				{			
				INFO_PRINTF1(_L("SetBufferOptions successfully called"));
				ret = EPass;
				}
			}
		else
			{
			ERR_PRINTF2(_L("Failed to set the options."),err);
			}
		}
	return ret;	
	}

TVerdict CTestDevVideoCiuRecSetOptionsVideoBufferManagement::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();
	
	// select the encoder
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectEncoderL(KUidDevVideoCiuTestEncodeHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the Encoder."));
		}

	return ret;	
	}

CTestDevVideoCiuRecSetOptionsVideoBufferManagement::CTestDevVideoCiuRecSetOptionsVideoBufferManagement
													(const TDesC& aTestName)
													: CTestDevVideoCiuRecordStep(aTestName)
	{
	}
	
//
//
// GetBufferOptions
CTestDevVideoCiuRecGetOptionsVideoBufferManagement* CTestDevVideoCiuRecGetOptionsVideoBufferManagement::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecGetOptionsVideoBufferManagement* self = new (ELeave) CTestDevVideoCiuRecGetOptionsVideoBufferManagement(aTestName);
	return self;	
	}
	
TVerdict CTestDevVideoCiuRecGetOptionsVideoBufferManagement::DoTestL()
	{
	TVerdict ret = EFail;

	MMmfVideoBufferManagement* interface = NULL;
	interface = static_cast<MMmfVideoBufferManagement*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																						KUidMmfVideoBufferManagement));
	
	if (!interface)
		{
		ERR_PRINTF1(_L("Failed to create the interface."));
		}
	else
		{
		INFO_PRINTF1(_L("Interface successfully created"));
		// Use the interface
		MMmfVideoBufferManagement::TBufferOptions opts;
		opts.iNumInputBuffers = 999;
		opts.iBufferSize.iWidth = 888;
		opts.iBufferSize.iHeight = 777;
		
		TRAPD(err, interface->MmvbmSetBufferOptionsL(opts));
		
		if (err == KErrNone)
			{
			// Verify that the options were set
			MMmfVideoBufferManagement::TBufferOptions testOpts;
			interface->MmvbmGetBufferOptions(testOpts);
			if ((testOpts.iNumInputBuffers != opts.iNumInputBuffers) ||
				(testOpts.iBufferSize.iWidth != opts.iBufferSize.iWidth) ||
				(testOpts.iBufferSize.iHeight != opts.iBufferSize.iHeight))
				{
				ERR_PRINTF1(_L("Failed to get the options."));
				}
			else
				{			
				INFO_PRINTF1(_L("Successfully called GetBufferOptions"));
				ret = EPass;
				}
			}
		else
			{
			ERR_PRINTF2(_L("Failed to get the options."),err);
			}
		}
	return ret;	
	}

TVerdict CTestDevVideoCiuRecGetOptionsVideoBufferManagement::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();
	
	// select the encoder
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectEncoderL(KUidDevVideoCiuTestEncodeHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the Encoder."));
		}

	return ret;	
	}

CTestDevVideoCiuRecGetOptionsVideoBufferManagement::CTestDevVideoCiuRecGetOptionsVideoBufferManagement
													(const TDesC& aTestName)
													: CTestDevVideoCiuRecordStep(aTestName)
	{
	}
	
//
//
// GetBuffer
CTestDevVideoCiuRecGetBufferVideoBufferManagement* CTestDevVideoCiuRecGetBufferVideoBufferManagement::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecGetBufferVideoBufferManagement* self = new (ELeave) CTestDevVideoCiuRecGetBufferVideoBufferManagement(aTestName);
	return self;	
	}

TVerdict CTestDevVideoCiuRecGetBufferVideoBufferManagement::DoTestL()
	{
	TVerdict ret = EFail;

	MMmfVideoBufferManagement* interface = NULL;
	interface = static_cast<MMmfVideoBufferManagement*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																						KUidMmfVideoBufferManagement));
	
	if (!interface)
		{
		ERR_PRINTF1(_L("Failed to create the interface."));
		}
	else
		{
		MDevVideoCiuTestInterfaceBufferManagement* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceBufferManagement*>
						(iDevVideoRecord->CustomInterface(iHwDeviceId, 
														KUidDevVideoCiuTestInterfaceBufferManagement));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			}
		else
			{
			// Setup some test data
			TVideoPicture testBuffer;
			testBuffer.iOptions = 123;
			TSize testSize(10, 10);
			testInterface->MdvbmSetBuffer(testSize, testBuffer);
			
			TVideoPicture* buffer = interface->MmvbmGetBufferL(testSize);
			if (!buffer || (buffer->iOptions != testBuffer.iOptions))
				{	
				ERR_PRINTF1(_L("Error retrieving the buffer."));
				}
			else
				{
				INFO_PRINTF1(_L("Successfully retrieved the buffer"));
				ret = EPass;
				}
			}
		}
		
	return ret;	
	}

TVerdict CTestDevVideoCiuRecGetBufferVideoBufferManagement::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();

	// select the encoder
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectEncoderL(KUidDevVideoCiuTestEncodeHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the Encoder."));
		}

	return ret;	
	}

CTestDevVideoCiuRecGetBufferVideoBufferManagement::CTestDevVideoCiuRecGetBufferVideoBufferManagement
													(const TDesC& aTestName)
													: CTestDevVideoCiuRecordStep(aTestName)
	{
	}
	
//
//
// ReleaseBuffer
CTestDevVideoCiuRecReleaseBufferVideoBufferManagement* CTestDevVideoCiuRecReleaseBufferVideoBufferManagement::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecReleaseBufferVideoBufferManagement* self = new (ELeave) CTestDevVideoCiuRecReleaseBufferVideoBufferManagement(aTestName);
	return self;	
	}
	
TVerdict CTestDevVideoCiuRecReleaseBufferVideoBufferManagement::DoTestL()
	{
	TVerdict ret = EFail;
	
	MMmfVideoBufferManagement* interface = NULL;
	interface = static_cast<MMmfVideoBufferManagement*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																						KUidMmfVideoBufferManagement));
	
	if (!interface)
		{
		ERR_PRINTF1(_L("Failed to create the interface."));
		}
	else
		{
		TVideoPicture testBuffer;
		testBuffer.iOptions = 123;
		TSize testSize(10, 10);
		// The test implementation stores the buffer...
		interface->MmvbmReleaseBuffer(&testBuffer);
		// ...so that we can test that the call works using:
		TVideoPicture* buffer = interface->MmvbmGetBufferL(testSize);
		if (!buffer || (buffer->iOptions != testBuffer.iOptions))
			{	
			ERR_PRINTF1(_L("Error releasing the buffer."));
			}
		else
			{
			INFO_PRINTF1(_L("Successfully released the buffer"));
			ret = EPass;
			}
		}

	return ret;	
	}

TVerdict CTestDevVideoCiuRecReleaseBufferVideoBufferManagement::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();

	// select the encoder
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectEncoderL(KUidDevVideoCiuTestEncodeHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the Encoder."));
		}

	return ret;	
	}

CTestDevVideoCiuRecReleaseBufferVideoBufferManagement::CTestDevVideoCiuRecReleaseBufferVideoBufferManagement
														(const TDesC& aTestName)
														: CTestDevVideoCiuRecordStep(aTestName)
	{
	}
	
//
//	
// Observer NewBuffers callback
CTestDevVideoCiuRecNewBuffersVideo* CTestDevVideoCiuRecNewBuffersVideo::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecNewBuffersVideo* self = new (ELeave) CTestDevVideoCiuRecNewBuffersVideo(aTestName);
	return self;	
	}
	
TVerdict CTestDevVideoCiuRecNewBuffersVideo::DoTestL()
	{
	TVerdict ret = EFail;

	MMmfVideoBufferManagement* interface = NULL;
	interface = static_cast<MMmfVideoBufferManagement*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																						KUidMmfVideoBufferManagement));
	
	if (!interface)
		{
		ERR_PRINTF1(_L("Failed to create the interface."));
		}
	else
		{
		MDevVideoCiuTestInterfaceBufferManagement* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceBufferManagement*>(iDevVideoRecord->
																				CustomInterface(iHwDeviceId,
																								KUidDevVideoCiuTestInterfaceBufferManagement));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			}
		else
			{
			// Set the observer to callback!
			MMmfVideoBufferManagementObserver* obs = this;
			interface->MmvbmSetObserver(obs);

			INFO_PRINTF1(_L("Requesting the NewBuffers callback..."));
			testInterface->MdvbmRequestNewBuffersCallback();

			if ((iError != KErrNone) || (!iCallbackReceived))
				{	
				ERR_PRINTF1(_L("Error receiving the callback."));
				}
			else
				{
				INFO_PRINTF1(_L("Successfully retrieved the callback"));
				ret = EPass;
				}
			}
		}
	
	// DEF111127
	MmvbmoNewBuffers();
	MmvbmoReleaseBuffers();
		
	return ret;	
	}

TVerdict CTestDevVideoCiuRecNewBuffersVideo::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();

	// select the encoder
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectEncoderL(KUidDevVideoCiuTestEncodeHwDevice));

	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the Encoder."));
		}
	return ret;	
	}
	
// from MMmfVideoBufferManagementObserver
void CTestDevVideoCiuRecNewBuffersVideo::MmvbmoNewBuffers()
	{
	INFO_PRINTF1(_L("NewBuffers callback received."));
	iCallbackReceived = ETrue;
	iError = KErrNone;
	}
	
void CTestDevVideoCiuRecNewBuffersVideo::MmvbmoReleaseBuffers()
	{
	iError = KErrNotSupported;
	}

CTestDevVideoCiuRecNewBuffersVideo::CTestDevVideoCiuRecNewBuffersVideo(const TDesC& aTestName)
									: CTestDevVideoCiuRecordStep(aTestName)
	{
	}
	
//
//
// Observer ReleaseBuffers callback
CTestDevVideoCiuRecReleaseBuffersVideo* CTestDevVideoCiuRecReleaseBuffersVideo::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecReleaseBuffersVideo* self = new (ELeave) CTestDevVideoCiuRecReleaseBuffersVideo(aTestName);
	return self;	
	}
	
TVerdict CTestDevVideoCiuRecReleaseBuffersVideo::DoTestL()
	{
	TVerdict ret = EFail;

	MMmfVideoBufferManagement* interface = NULL;
	interface = static_cast<MMmfVideoBufferManagement*>(iDevVideoRecord->CustomInterface(iHwDeviceId,
																						KUidMmfVideoBufferManagement));
	if (!interface)
		{
		ERR_PRINTF1(_L("Failed to create the interface."));
		}
	else
		{
		MDevVideoCiuTestInterfaceBufferManagement* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceBufferManagement*>(iDevVideoRecord->
																				CustomInterface(iHwDeviceId,
																								KUidDevVideoCiuTestInterfaceBufferManagement));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			}
		else
			{
			// Set the observer to callback!
			MMmfVideoBufferManagementObserver* obs = this;
			interface->MmvbmSetObserver(obs);

			INFO_PRINTF1(_L("Requesting the ReleaseBuffers callback..."));
			testInterface->MdvbmRequestReleaseBuffersCallback();

			if ((iError != KErrNone) || (!iCallbackReceived))
				{	
				ERR_PRINTF1(_L("Error receiving the callback."));
				}
			else
				{
				INFO_PRINTF1(_L("Successfully retrieved the callback"));
				ret = EPass;
				}
			}		
		}
		
	// DEF111127
	MmvbmoNewBuffers();
	MmvbmoReleaseBuffers();

	return ret;	
	}

TVerdict CTestDevVideoCiuRecReleaseBuffersVideo::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();
	
	// select the encoder
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectEncoderL(KUidDevVideoCiuTestEncodeHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		INFO_PRINTF1(_L("DevVideo Record Failed to select the Encoder."));
		}

	return ret;	
	}

// from MMmfVideoBufferManagementObserver
void CTestDevVideoCiuRecReleaseBuffersVideo::MmvbmoNewBuffers()
	{
	iError = KErrNotSupported;	
	}
	
void CTestDevVideoCiuRecReleaseBuffersVideo::MmvbmoReleaseBuffers()
	{
	INFO_PRINTF1(_L("ReleaseBuffers callback received."));
	iCallbackReceived = ETrue;
	iError = KErrNone;
	}

CTestDevVideoCiuRecReleaseBuffersVideo::CTestDevVideoCiuRecReleaseBuffersVideo(const TDesC& aTestName)
										: CTestDevVideoCiuRecordStep(aTestName)
	{
	}


//------------------------------------------------------------------
// Instantiation

CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder::
CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder(const TDesC& aTestName)
:	CTestDevVideoCiuRecordStep(aTestName)
	{
	}

CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder* 
CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder* self = 
		new(ELeave) CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder(aTestName);
		
	return self;
	}

TVerdict CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();

	// select encoder
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectEncoderL(KUidDevVideoCiuTestEncodeHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		ERR_PRINTF1(_L("DevVideo Record Failed to select the encoder."));
		}
	return ret;
	}
	
TVerdict CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder::DoTestL()
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("DevVideo CIU Video Input Snapshot Initialisation"));

	// Try to fetch the CI...
	MMmfVideoH324AnnexKMode* interface = NULL;
		
	interface = static_cast<MMmfVideoH324AnnexKMode*>
				(iDevVideoRecord->CustomInterface(iHwDeviceId, KUidMmfVideoH324AnnexKMode));
				
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU 'Video H324 AnnexK Mode' Instantiation Successfull"));
		}
		
	return ret;
	}
	
//------------------------------------------------------------------
// Set Mode

CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder::
CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder(const TDesC& aTestName)
:	CTestDevVideoCiuRecordStep(aTestName)
	{
	}

CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder* 
CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder* self = 
		new(ELeave) CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder(aTestName);
		
	return self;
	}

TVerdict CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuRecordStep::DoTestStepPreambleL();

	// select encoder
	TRAPD(err, iHwDeviceId = iDevVideoRecord->SelectEncoderL(KUidDevVideoCiuTestEncodeHwDevice));
	
	if (err != KErrNone)
		{
		ret = EInconclusive;
		ERR_PRINTF1(_L("DevVideo Record Failed to select the encoder."));
		}
	return ret;
	}
	
TVerdict CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder::DoTestL()
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("DevVideo CIU 'Video H324 AnnexK Mode'"));
	
	// Try to fetch the CI...
	MMmfVideoH324AnnexKMode* interface = NULL;
		
	interface = static_cast<MMmfVideoH324AnnexKMode*>
				(iDevVideoRecord->CustomInterface(iHwDeviceId, KUidMmfVideoH324AnnexKMode));
				
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EInconclusive;
		}
	else
		{
		// Default is EFalse
		interface->MmvhakmSetH324MAnnexKModeL(ETrue);

		// Test interface to retrieve the mode
		MDevVideoCiuTestInterfaceH324AnnexKMode* testInterface = 
			static_cast<MDevVideoCiuTestInterfaceH324AnnexKMode*>
			(iDevVideoRecord->CustomInterface(iHwDeviceId, KUidDevVideoCiuTestInterfaceH324AnnexKMode));
		
		if(!testInterface)
			{
			ERR_PRINTF1(_L("Error - no test interface retrieved"));
			ret = EInconclusive;
			}
		else
		if (testInterface->MmvhakmH324MAnnexKModeL() == EFalse)
			{
			ERR_PRINTF1(_L("Error - different Mode value returned to that set!"));
			ret = EFail;
			}
		}
		
	return ret;
	}		
	
//------------------------------------------------------------------

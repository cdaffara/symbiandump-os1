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

#ifdef SYMBIAN_BUILD_GCE
#include <mmf/common/mmfvideosurfacecustomcommands.h>
#include <mmf/devvideo/devvideoclientbuffersupport.h>
#include "videoframebuffer.h"
#endif // SYMBIAN_BUILD_GCE

#include "testdevvideociuplay.h"
#include "testdevvideociuplugins/ciudecoder.h"
#include "testdevvideociuplugins/ciupostproc.h"
#include "testdevvideociuplugins/ciudevvideotestinterface.h"

// DevVideoCiu base class

CTestDevVideoCiuPlayStep::CTestDevVideoCiuPlayStep(const TDesC& aTestName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	}
	
CTestDevVideoCiuPlayStep::~CTestDevVideoCiuPlayStep() 
	{
	}

TVerdict CTestDevVideoCiuPlayStep::DoTestStepPreambleL()
	{
	TVerdict ret = CTestDevVideoCiuStep::DoTestStepPreambleL();

	__UHEAP_MARK;
	TRAPD(err, iDevVideoPlay = CMMFDevVideoPlay::NewL( *this ));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Could not create a CMMFDevVideoPlay!"));
		ret = EInconclusive;
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayStep::DoTestStepPostambleL()
	{
	delete iDevVideoPlay;	
	__MM_HEAP_MARKEND;
	
	return EPass;
	}

TVerdict CTestDevVideoCiuPlayStep::DoTestStepL()
	{
	TVerdict ret = EFail;

	iDevVideoPlay->Initialize();

	if (iError == KErrNone)
		{
		INFO_PRINTF1(_L("DevVideo Play initialisation was successful."));
		
		// call the test step
		ret = DoTestL();
		}
	else
		{
		ret = EInconclusive;
		ERR_PRINTF1(_L("Failed to initialise the DevVideo Play"));
		}
		
	return ret;
	}

// MMMFDevVideoCiuObserver
void CTestDevVideoCiuPlayStep::MdvpoNewBuffers()
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuPlayStep::MdvpoNewBuffers()"));
	}

void CTestDevVideoCiuPlayStep::MdvpoReturnPicture(TVideoPicture* /*aPicture*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuPlayStep::MdvpoReturnPicture()"));
	}

void CTestDevVideoCiuPlayStep::MdvpoSupplementalInformation(const TDesC8& /*aData*/, 
								  const TTimeIntervalMicroSeconds& /*aTimestamp*/, 
								  const TPictureId& /*aPictureId*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuPlayStep::MdvpoSupplementalInformation()"));
	}

void CTestDevVideoCiuPlayStep::MdvpoPictureLoss()
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuPlayStep::MdvpoPictureLoss()"));
	}

void CTestDevVideoCiuPlayStep::MdvpoPictureLoss(const TArray<TPictureId>& /*aPictures*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuPlayStep::MdvpoPictureLoss(TArray)"));
	}

void CTestDevVideoCiuPlayStep::MdvpoSliceLoss(TUint /*aFirstMacroblock*/, TUint /*aNumMacroblocks*/, const TPictureId& /*aPicture*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuPlayStep::MdvpoSliceLoss()"));
	}

void CTestDevVideoCiuPlayStep::MdvpoReferencePictureSelection(const TDesC8& /*aSelectionData*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuPlayStep::MdvpoReferencePictureSelection()"));
	}

void CTestDevVideoCiuPlayStep::MdvpoTimedSnapshotComplete(TInt /*aError*/, 
								TPictureData* /*aPictureData*/, 
								const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/, 
								const TPictureId& /*aPictureId*/)
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuPlayStep::MdvpoTimedSnapshotComplete()"));
	}

void CTestDevVideoCiuPlayStep::MdvpoNewPictures()
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuPlayStep::MdvpoNewPictures()"));
	}

void CTestDevVideoCiuPlayStep::MdvpoFatalError(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("CTestDevVideoCiuPlayStep::MdvpoFatalError():  Error = %d"), aError);
	}

void CTestDevVideoCiuPlayStep::MdvpoInitComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("CTestDevVideoCiuPlayStep::MdvpoInitComplete():  Error = %d"), aError);
	}

void CTestDevVideoCiuPlayStep::MdvpoStreamEnd()
	{
	INFO_PRINTF1(_L("CTestDevVideoCiuPlayStep::MdvpoStreamEnd()"));
	}

//
//	Video Window Control Tests
//
CTestDevVideoCiuPlayGetAndSetLogicalWindow::CTestDevVideoCiuPlayGetAndSetLogicalWindow(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayGetAndSetLogicalWindow* CTestDevVideoCiuPlayGetAndSetLogicalWindow::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayGetAndSetLogicalWindow* self = new(ELeave) CTestDevVideoCiuPlayGetAndSetLogicalWindow(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetLogicalWindow::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetLogicalWindow::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Window Control Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoWindowControl* interface = NULL;
		
	interface = static_cast<MMmfVideoWindowControl*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMmfVideoWindowControl));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Window Control Interface Instantiated"));
		
		TRect logicalWindow(TPoint(0,0), TPoint(300,300));
	
		interface->MmvwcSetLogicalVideoWindow(logicalWindow);
		
		MDevVideoCiuTestInterfaceVideoWindowControl* testInterface = NULL;

		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoWindowControl*>
										(iDevVideoPlay->CustomInterface(iHwDeviceId, KUidDevVideoCiuTestInterfaceVideoWindowControl));
		if (testInterface)
			{
			TRect logicalWindowResult = testInterface->MmvwcLogicalVideoWindow();
			if (logicalWindow == logicalWindowResult)
				{
				ret = EPass;
				INFO_PRINTF1(_L("Set Logical Window method call performed as expected"));
				}
			else
				{
				ret = EFail;					
				ERR_PRINTF1(_L("Set Logical Window method call failed to set the value correctly"));
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

CTestDevVideoCiuPlayGetAndSetContentOffset::CTestDevVideoCiuPlayGetAndSetContentOffset(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayGetAndSetContentOffset* CTestDevVideoCiuPlayGetAndSetContentOffset::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayGetAndSetContentOffset* self = new(ELeave) CTestDevVideoCiuPlayGetAndSetContentOffset(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetContentOffset::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetContentOffset::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Window Control Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoWindowControl* interface = NULL;
		
	interface = static_cast<MMmfVideoWindowControl*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMmfVideoWindowControl));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Window Control Interface Instantiated"));
		
		TPoint contentOffsetInput(100, 100);
	
		interface->MmvwcSetContentOffsetL(contentOffsetInput);
		
		MDevVideoCiuTestInterfaceVideoWindowControl* testInterface = NULL;

		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoWindowControl*>
										(iDevVideoPlay->CustomInterface(iHwDeviceId, KUidDevVideoCiuTestInterfaceVideoWindowControl));
		if (testInterface)
			{
			TPoint contentOffsetOutput = testInterface->MmvwcContentOffset();
			if (contentOffsetInput == contentOffsetOutput)
				{
				ret = EPass;
				INFO_PRINTF1(_L("Set Content Offset method call performed as expected"));
				}
			else
				{
				ret = EFail;					
				ERR_PRINTF1(_L("Set Content Offset method call failed to set the value correctly"));
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

CTestDevVideoCiuPlayGetAndSetContentAlignment::CTestDevVideoCiuPlayGetAndSetContentAlignment(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayGetAndSetContentAlignment* CTestDevVideoCiuPlayGetAndSetContentAlignment::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayGetAndSetContentAlignment* self = new(ELeave) CTestDevVideoCiuPlayGetAndSetContentAlignment(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetContentAlignment::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetContentAlignment::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Window Control Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoWindowControl* interface = NULL;
		
	interface = static_cast<MMmfVideoWindowControl*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMmfVideoWindowControl));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Window Control Interface Instantiated"));
		
		MDevVideoCiuTestInterfaceVideoWindowControl::TAlignment alignmentInput = {MMmfVideoWindowControl::EHorizAlignCenter, MMmfVideoWindowControl::EVertAlignTop};
	
		interface->MmvwcSetContentAlignment(alignmentInput.iHorizAlign,alignmentInput.iVertAlign);
		
		MDevVideoCiuTestInterfaceVideoWindowControl* testInterface = NULL;

		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoWindowControl*>
										(iDevVideoPlay->CustomInterface(iHwDeviceId, KUidDevVideoCiuTestInterfaceVideoWindowControl));
		if (testInterface)
			{
			MDevVideoCiuTestInterfaceVideoWindowControl::TAlignment alignmentOutput = testInterface->MmvwcContentAlignment();
			if ((alignmentInput.iHorizAlign == alignmentOutput.iHorizAlign) && (alignmentInput.iVertAlign == alignmentOutput.iVertAlign) )
				{
				ret = EPass;
				INFO_PRINTF1(_L("Set Content Alignment method call performed as expected"));
				}
			else
				{
				ret = EFail;					
				ERR_PRINTF1(_L("Set Content Alignment method call failed to set the value correctly"));
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

//
//	Video Resource Management Tests
//

CTestDevVideoCiuPlayGetAndSetResourceObserver::CTestDevVideoCiuPlayGetAndSetResourceObserver(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayGetAndSetResourceObserver* CTestDevVideoCiuPlayGetAndSetResourceObserver::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayGetAndSetResourceObserver* self = new(ELeave) CTestDevVideoCiuPlayGetAndSetResourceObserver(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetResourceObserver::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetResourceObserver::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Resource Management Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoResourceHandler* interface = NULL;
		
	interface = static_cast<MMmfVideoResourceHandler*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMmfVideoResourceManagement));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Resource Management Interface Instantiated"));
	
		MMmfVideoResourceObserver* obs = this;
		interface->MmvrhSetObserver(obs);
		INFO_PRINTF1(_L("SetObserver called"));
		
		// Verify that the observer was set
		MDevVideoCiuTestInterfaceResourceManagement* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceResourceManagement*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
																												 KUidDevVideoCiuTestInterfaceResourceManagement));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			ret = EFail;
			}
		else
			{
			if (obs != testInterface->MdvrmObserver())
				{
				ERR_PRINTF1(_L("Failed to set the observer."));
				ret = EFail;
				}
			else
				{			
				INFO_PRINTF1(_L("SetObserver successfully called"));
				ret = EPass;
				}
			}		
		}
	return ret;
	}

//from MMmfVideoResourceObserver
void CTestDevVideoCiuPlayGetAndSetResourceObserver::MmvroResourcesLost(TUid /*aMediaDevice*/)
	{}
		
void CTestDevVideoCiuPlayGetAndSetResourceObserver::MmvroResourcesRestored(TUid /*aMediaDevice*/)
	{}

//------------------------------------------------------------------

CTestDevVideoCiuPlayResourceLost::CTestDevVideoCiuPlayResourceLost(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName), iCallbackReceived(EFalse)
	{
	}

CTestDevVideoCiuPlayResourceLost* CTestDevVideoCiuPlayResourceLost::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayResourceLost* self = new(ELeave) CTestDevVideoCiuPlayResourceLost(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayResourceLost::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayResourceLost::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Resource Management Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoResourceHandler* interface = NULL;
		
	interface = static_cast<MMmfVideoResourceHandler*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMmfVideoResourceManagement));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Resource Management Interface Instantiated"));

		MDevVideoCiuTestInterfaceResourceManagement* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceResourceManagement*>(iDevVideoPlay->
																				CustomInterface(iHwDeviceId,
																								KUidDevVideoCiuTestInterfaceResourceManagement));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			ret = EFail;
			}
		else
			{
			// Set the observer to callback!
			MMmfVideoResourceObserver* obs = this;
			interface->MmvrhSetObserver(obs);

			INFO_PRINTF1(_L("Requesting the Resource Lost Callback..."));
			testInterface->MdvrmRequestResourceLostCallback();

			if ((iError != KErrNone) || (!iCallbackReceived))
				{	
				ERR_PRINTF1(_L("Error receiving the Resource Lost Callback."));
				ret = EFail;
				}
			else
				{
				INFO_PRINTF1(_L("Successfully retrieved the Resource Lost Callback"));
				ret = EPass;
				}
			}
		}
	return ret;
	}

//from MMmfVideoResourceObserver
void CTestDevVideoCiuPlayResourceLost::MmvroResourcesLost(TUid /*aMediaDevice*/)
	{
	INFO_PRINTF1(_L("Resource Lost callback received."));
	iCallbackReceived = ETrue;
	iError = KErrNone;
	}
		
void CTestDevVideoCiuPlayResourceLost::MmvroResourcesRestored(TUid /*aMediaDevice*/)
	{
	iError = KErrNotSupported;
	}

//------------------------------------------------------------------

CTestDevVideoCiuPlayResourceRestored::CTestDevVideoCiuPlayResourceRestored(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName), iCallbackReceived(EFalse)
	{
	}

CTestDevVideoCiuPlayResourceRestored* CTestDevVideoCiuPlayResourceRestored::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayResourceRestored* self = new(ELeave) CTestDevVideoCiuPlayResourceRestored(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayResourceRestored::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayResourceRestored::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Resource Management Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoResourceHandler* interface = NULL;
		
	interface = static_cast<MMmfVideoResourceHandler*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMmfVideoResourceManagement));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Resource Management Interface Instantiated"));

		MDevVideoCiuTestInterfaceResourceManagement* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceResourceManagement*>(iDevVideoPlay->
																				CustomInterface(iHwDeviceId,
																								KUidDevVideoCiuTestInterfaceResourceManagement));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			ret = EFail;
			}
		else
			{
			// Set the observer to callback!
			MMmfVideoResourceObserver* obs = this;
			interface->MmvrhSetObserver(obs);

			INFO_PRINTF1(_L("Requesting the Resource Restored Callback..."));
			testInterface->MdvrmRequestResourceRestoredCallback();

			if ((iError != KErrNone) || (!iCallbackReceived))
				{	
				ERR_PRINTF1(_L("Error receiving the Resource Restored Callback."));
				ret = EFail;
				}
			else
				{
				INFO_PRINTF1(_L("Successfully retrieved the Resource Restored Callback"));
				ret = EPass;
				}
			}
		}
	return ret;
	}

//from MMmfVideoResourceObserver
void CTestDevVideoCiuPlayResourceRestored::MmvroResourcesLost(TUid /*aMediaDevice*/)
	{
	iError = KErrNotSupported;
	}
		
void CTestDevVideoCiuPlayResourceRestored::MmvroResourcesRestored(TUid /*aMediaDevice*/)
	{
	INFO_PRINTF1(_L("Resource Restored callback received."));
	iCallbackReceived = ETrue;
	iError = KErrNone;
	}

//------------------------------------------------------------------

CTestDevVideoCiuPlayGetAndSetResourcePriority::CTestDevVideoCiuPlayGetAndSetResourcePriority(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayGetAndSetResourcePriority* CTestDevVideoCiuPlayGetAndSetResourcePriority::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayGetAndSetResourcePriority* self = new(ELeave) CTestDevVideoCiuPlayGetAndSetResourcePriority(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetResourcePriority::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetResourcePriority::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Resource Priority Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoResourcePriority* interface = NULL;
		
	interface = static_cast<MMmfVideoResourcePriority*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMmfVideoResourcePriority));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Resource Priority Interface Instantiated"));
		
		TInt inputPriority = 100;
			
		interface->MmvpSetResourcePriority(inputPriority);
		
		MDevVideoCiuTestInterfaceResourcePriority* testInterface = NULL;

		testInterface = static_cast<MDevVideoCiuTestInterfaceResourcePriority*>
										(iDevVideoPlay->CustomInterface(iHwDeviceId, KUidDevVideoCiuTestInterfaceResourcePriority));
		if (testInterface)
			{
			TInt outputPriority = testInterface->MdvrpResourcePriority();
			if (inputPriority == outputPriority)
				{
				ret = EPass;
				INFO_PRINTF1(_L("Set Resource Priority method call performed as expected"));
				}
			else
				{
				ret = EFail;					
				ERR_PRINTF1(_L("Set Resource Priority method call failed to set the value correctly"));
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


//
//	Video Properties Management Tests
//

CTestDevVideoCiuPlaySetAndGetPropertiesObserver::CTestDevVideoCiuPlaySetAndGetPropertiesObserver(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlaySetAndGetPropertiesObserver* CTestDevVideoCiuPlaySetAndGetPropertiesObserver::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlaySetAndGetPropertiesObserver* self = new(ELeave) CTestDevVideoCiuPlaySetAndGetPropertiesObserver(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlaySetAndGetPropertiesObserver::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlaySetAndGetPropertiesObserver::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Properties Management Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoPropertiesNotifier* interface = NULL;
		
	interface = static_cast<MMmfVideoPropertiesNotifier*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMmfVideoPropertiesManagement));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Properties Management Interface Instantiated"));
	
		MMmfVideoPropertiesObserver* obs = this;
		interface->MmvpnSetObserver(obs);
		INFO_PRINTF1(_L("SetObserver called"));
		
		// Verify that the observer was set
		MDevVideoCiuTestInterfacePropertiesManagement* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfacePropertiesManagement*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
																												 KUidDevVideoCiuTestInterfacePropertiesManagement));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			ret = EFail;
			}
		else
			{
			if (obs != testInterface->MmvpoObserver())
				{
				ERR_PRINTF1(_L("Failed to set the observer."));
				ret = EFail;
				}
			else
				{			
				INFO_PRINTF1(_L("SetObserver successfully called."));
				ret = EPass;
				}
			}		
		}
	return ret;
	}

//from MMmfVideoPropertiesObserver
void CTestDevVideoCiuPlaySetAndGetPropertiesObserver::MmvpoUpdateVideoProperties(const TYuvFormat& /*aYuvFormat*/, const TSize& /*aPictureSize*/)
	{}

//------------------------------------------------------------------
CTestDevVideoCiuPlayUpdateProperties::CTestDevVideoCiuPlayUpdateProperties(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName), iCallbackReceived(EFalse)
	{
	}

CTestDevVideoCiuPlayUpdateProperties* CTestDevVideoCiuPlayUpdateProperties::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayUpdateProperties* self = new(ELeave) CTestDevVideoCiuPlayUpdateProperties(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayUpdateProperties::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayUpdateProperties::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Properties Management Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoPropertiesNotifier* interface = NULL;
		
	interface = static_cast<MMmfVideoPropertiesNotifier*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMmfVideoPropertiesManagement));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Properties Management Interface Instantiated"));

		MDevVideoCiuTestInterfacePropertiesManagement* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfacePropertiesManagement*>(iDevVideoPlay->
																				CustomInterface(iHwDeviceId,
																								KUidDevVideoCiuTestInterfacePropertiesManagement));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			ret = EFail;
			}
		else
			{
			// Set the observer to callback!
			MMmfVideoPropertiesObserver* obs = this;
			interface->MmvpnSetObserver(obs);

			INFO_PRINTF1(_L("Notifying the Properties Update Callback..."));
			testInterface->MmvpoUpdateVideoPropertiesCallback();

			if ((iError != KErrNone) || (!iCallbackReceived))
				{	
				ERR_PRINTF1(_L("Error receiving the RProperties Update Callback."));
				ret = EFail;
				}
			else
				{
				INFO_PRINTF1(_L("Successfully retrieved the Properties Update Callback"));
				ret = EPass;
				}
			}
		}
	return ret;
	}

//from MMmfVideoPropertiesObserver
void CTestDevVideoCiuPlayUpdateProperties::MmvpoUpdateVideoProperties(const TYuvFormat& /*aYuvFormat*/, const TSize& /*aPictureSize*/)
	{	
	INFO_PRINTF1(_L("Properties Update Callback received."));
	iCallbackReceived = ETrue;
	iError = KErrNone;
	}
	
//------------------------------------------------------------------

CTestDevVideoCiuPlayCreateVideoMediaProtected::CTestDevVideoCiuPlayCreateVideoMediaProtected(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayCreateVideoMediaProtected* CTestDevVideoCiuPlayCreateVideoMediaProtected::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayCreateVideoMediaProtected* self = new(ELeave) CTestDevVideoCiuPlayCreateVideoMediaProtected(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayCreateVideoMediaProtected::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();

	if (ret == EPass)
		{
		// select post-processor
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectPostProcessorL(KUidDevVideoCiuTestPostProcHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the post processor."));
			}		
		}

	return ret;
	}
	
TVerdict CTestDevVideoCiuPlayCreateVideoMediaProtected::DoTestL()
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("DevVideo CIU Video Media Protected Initialisation"));

	// Try to fetch the CI...
	MMmfVideoMediaProtected* interface = NULL;
		
	interface = static_cast<MMmfVideoMediaProtected*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
																					KUidMmfVideoMediaProtected));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Video Media Protected instantiated"));
		}

	return ret;
	}
//------------------------------------------------------------------

//------------------------------------------------------------------

CTestDevVideoCiuPlaySetVideoMediaProtected::CTestDevVideoCiuPlaySetVideoMediaProtected(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlaySetVideoMediaProtected* CTestDevVideoCiuPlaySetVideoMediaProtected::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlaySetVideoMediaProtected* self = new(ELeave) CTestDevVideoCiuPlaySetVideoMediaProtected(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlaySetVideoMediaProtected::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();

	if (ret == EPass)
		{
		// select post-processor
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectPostProcessorL(KUidDevVideoCiuTestPostProcHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the post processor."));
			}
		}

	return ret;
	}

// alternate implementation that uses the test interface to check
// that the value has been set
TVerdict CTestDevVideoCiuPlaySetVideoMediaProtected::DoTestL()
	{
	TVerdict ret = EFail;

	// Got this far so must have successfully initialised DevVideoPlay
	// Try to fetch the CI...
	MMmfVideoMediaProtected* interface = NULL;
		
	interface = static_cast<MMmfVideoMediaProtected*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
																					KUidMmfVideoMediaProtected));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EInconclusive;
		}
	else
		{
		// Try to call a method on the interface
		TRAPD(err, interface->MmvsoMediaIsProtectedL(ETrue));
		// Method should have left as not supported
		if (err != KErrNotSupported)
			{
			ERR_PRINTF2(_L("Error - wrong leave code from method call"), err);
			}
		else
			{
			// Now set up the test interface...
			MDevVideoCiuTestInterfaceMediaProtected* testInterface = NULL;
			testInterface = static_cast<MDevVideoCiuTestInterfaceMediaProtected*>
										(iDevVideoPlay->CustomInterface(iHwDeviceId, KUidDevVideoCiuTestInterfaceMediaProtected));
			if (testInterface)
				{
				if (testInterface->MmvsoMediaIsProtected())
					{
					ret = EPass;
					INFO_PRINTF1(_L("Media protected method call performed as expected"));
					}
				else
					{					
					ERR_PRINTF1(_L("Media protected method call failed to set the value correctly"));
					}
				}
			else
				{
				ERR_PRINTF1(_L("Error - no test interface retrieved"));
				ret = EInconclusive;
				}
			}
		}

	return ret;
	}
//------------------------------------------------------------------


//------------------------------------------------------------------

CTestDevVideoCiuPlayCreateColourSpace::CTestDevVideoCiuPlayCreateColourSpace(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayCreateColourSpace* CTestDevVideoCiuPlayCreateColourSpace::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayCreateColourSpace* self = new(ELeave) CTestDevVideoCiuPlayCreateColourSpace(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayCreateColourSpace::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();

	if (ret == EPass)
		{
		// select post-processor
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectPostProcessorL(KUidDevVideoCiuTestPostProcHwDevice));

		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the post processor."));
			}
		}
	
	return ret;
	}

TVerdict CTestDevVideoCiuPlayCreateColourSpace::DoTestL()
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("DevVideo CIU Colour Space Header Interface Initialisation"));

	// Try to fetch the CI...
	MMmfVideoPlayHWDeviceColourInfoCustomHeader* interface = NULL;
		
	interface = static_cast<MMmfVideoPlayHWDeviceColourInfoCustomHeader*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMmfVideoPlayHWDeviceColourInfoCustomHeader));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Colour Space Header Interface Instantiated"));
		}
				
	return ret;
	}
//------------------------------------------------------------------

//------------------------------------------------------------------
CTestDevVideoCiuPlayGetColourSpace::CTestDevVideoCiuPlayGetColourSpace(const TDesC& aTestName)
	: CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayGetColourSpace* CTestDevVideoCiuPlayGetColourSpace::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayGetColourSpace* self = new(ELeave) CTestDevVideoCiuPlayGetColourSpace(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayGetColourSpace::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();

	if (ret == EPass)
		{
		// select post-processor
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectPostProcessorL(KUidDevVideoCiuTestPostProcHwDevice));

		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the post processor."));
			}
		}
		
	return ret;
	}

// Implementation that uses the test interface to set data that can be checked
// in the call to the interface's "get" method.
TVerdict CTestDevVideoCiuPlayGetColourSpace::DoTestL()
	{
	TVerdict ret = EFail;

	// Got this far so must have successfully initialised DevVideoPlay
	// Try to fetch the CI...
	MMmfVideoPlayHWDeviceColourInfoCustomHeader* interface = NULL;
		
	interface = static_cast<MMmfVideoPlayHWDeviceColourInfoCustomHeader*>
						(iDevVideoPlay->CustomInterface(iHwDeviceId, 
														KUidMmfVideoPlayHWDeviceColourInfoCustomHeader));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EInconclusive;
		}
	else
		{
		// Setup some test values...
		TVideoDataUnitType dataUnitType = EDuArbitraryStreamSection;
		TVideoDataUnitEncapsulation dataUnitEncapsulation = EDuRtpPayload;
		TYuvFormat yuvFormat;
		yuvFormat.iCoefficients = ECustomYuvMatrix;
		yuvFormat.iPattern = EYuv422Chroma2;
		yuvFormat.iDataLayout = EYuvDataSemiPlanar;
		yuvFormat.iYuv2RgbMatrix = NULL;
		yuvFormat.iRgb2YuvMatrix = NULL;
		yuvFormat.iAspectRatioNum = 666;
		yuvFormat.iAspectRatioDenom = 999;
		TVideoInputBuffer* dataUnit = new TVideoInputBuffer();

		if (!dataUnit)
			{
			ERR_PRINTF2(_L("Failed to allocate test data."), KErrNoMemory);
			}
		else
			{
			// just set one member
			dataUnit->iOptions = 444;
			// Now set up the test interface...
			MDevVideoCiuTestInterfaceColourSpace* testInterface = NULL;
			testInterface = static_cast<MDevVideoCiuTestInterfaceColourSpace*>
										(iDevVideoPlay->CustomInterface(iHwDeviceId, KUidDevVideoCiuTestInterfaceColourSpace));
			if (testInterface)
				{
				testInterface->MmvpciSetColourSpaceHeaderInfoL(dataUnitType,
																dataUnitEncapsulation,
																dataUnit,
																yuvFormat);
				INFO_PRINTF1(_L("Set the colour space header info."));
				
				TVideoDataUnitType testDataUnitType = TVideoDataUnitType(0);
				TVideoDataUnitEncapsulation testDataUnitEncapsulation = TVideoDataUnitEncapsulation(0);
				TVideoInputBuffer* testDataUnit = new TVideoInputBuffer();
				TYuvFormat testYuvFormat;
				if (!testDataUnit)
					{
					ERR_PRINTF2(_L("Failed to allocate test data."), KErrNoMemory);
					}
				else
					{				
					// Call the interface's method
					TRAPD(err, interface->MmvpciGetColourSpaceHeaderInfoL(	testDataUnitType,
																			testDataUnitEncapsulation,
																			testDataUnit,
																			testYuvFormat));
					if ((err == KErrNone) &&
						(dataUnitType == testDataUnitType) &&
						(dataUnitEncapsulation == testDataUnitEncapsulation) &&
						(dataUnit->iOptions == testDataUnit->iOptions) &&
						(yuvFormat == testYuvFormat))
						{
						INFO_PRINTF1(_L("Succesfully retrieved the colour space header info."));
						ret = EPass;
						}
					else
						{
						ERR_PRINTF1(_L("Failed to retrieve the colour space header info."));
						}
					}
				delete testDataUnit;
				}
			else
				{
				ERR_PRINTF1(_L("Error - no test interface retrieved"));
				ret = EInconclusive;
				}
			}
		delete dataUnit;
		}

	return ret;
	}

//------------------------------------------------------------------
// Resource control tests implementation
CTestDevVideoCiuPlayCreatePolicyExtension::CTestDevVideoCiuPlayCreatePolicyExtension(const TDesC& aTestName)
	: CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayCreatePolicyExtension* CTestDevVideoCiuPlayCreatePolicyExtension::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayCreatePolicyExtension* self = new(ELeave) CTestDevVideoCiuPlayCreatePolicyExtension(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayCreatePolicyExtension::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}
	
TVerdict CTestDevVideoCiuPlayCreatePolicyExtension::DoTestL()
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("DevVideo CIU Policy Extension Interface Initialisation"));
	
	// Try to fetch the CI...
	MMmfVideoHwDeviceVideoPolicyExtension* interface = NULL;
		
	interface = static_cast<MMmfVideoHwDeviceVideoPolicyExtension*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMmfVideoHwDevVideoPolicyExtension));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Policy Extension Interface Instantiated"));
		}
		
	return ret;
	}

//-------------------------------------------------------------------------
CTestDevVideoCiuPlaySetClientPolicyExtension::CTestDevVideoCiuPlaySetClientPolicyExtension(const TDesC& aTestName)
	: CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlaySetClientPolicyExtension* CTestDevVideoCiuPlaySetClientPolicyExtension::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlaySetClientPolicyExtension* self = new(ELeave) CTestDevVideoCiuPlaySetClientPolicyExtension(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlaySetClientPolicyExtension::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();

	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));

		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}

	return ret;
	}

// Implementation that uses the test interface to set data that can be checked
// in the call to the interface's "get" method.
TVerdict CTestDevVideoCiuPlaySetClientPolicyExtension::DoTestL()
	{
	TVerdict ret = EFail;

	// Got this far so must have successfully initialised DevVideoPlay
	// Try to fetch the CI...
	MMmfVideoHwDeviceVideoPolicyExtension* interface = NULL;
		
	interface = static_cast<MMmfVideoHwDeviceVideoPolicyExtension*>(iDevVideoPlay->CustomInterface(
																		iHwDeviceId, 
																		KUidMmfVideoHwDevVideoPolicyExtension));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EInconclusive;
		}
	else
		{
		MMmfVideoHwDevClientNotifier* clientNotifier = this;
		// call the interface's method
		interface->MmvhvpeSetVideoClient(clientNotifier);
		
		// Now set up the test interface...
		MDevVideoCiuTestInterfaceClientResourceNotifier* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceClientResourceNotifier*>
									(iDevVideoPlay->CustomInterface( iHwDeviceId,
																	KUidDevVideoCiuTestInterfaceClientResourceNotifier));
		if (testInterface)
			{
			if (testInterface->McrnVideoClient() == clientNotifier)
				{
				INFO_PRINTF1(_L("Succesfully set the client notifier."));
				ret = EPass;
				}
			else
				{
				ERR_PRINTF1(_L("Failed to set the client notifier."));
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

// from MMmfVideoHwDevClientNotifier
void CTestDevVideoCiuPlaySetClientPolicyExtension::MmvhcnVideoAccessRevoked()
	{
	}
// from MMmfVideoHwDevClientNotifier	
void CTestDevVideoCiuPlaySetClientPolicyExtension::MmvhcnReleaseVideoResource()
	{
	}
//----------------------------------------------------------------------------
CTestDevVideoCiuPlaySetPriorityPolicyExtension::CTestDevVideoCiuPlaySetPriorityPolicyExtension(const TDesC& aTestName)
	: CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlaySetPriorityPolicyExtension* CTestDevVideoCiuPlaySetPriorityPolicyExtension::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlaySetPriorityPolicyExtension* self = new(ELeave) CTestDevVideoCiuPlaySetPriorityPolicyExtension(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlaySetPriorityPolicyExtension::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();

	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	
	return ret;
	}

TVerdict CTestDevVideoCiuPlaySetPriorityPolicyExtension::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Policy Extension Set Priority"));

	// Try to fetch the CI...
	MMmfVideoHwDeviceVideoPolicyExtension* interface = NULL;
		
	interface = static_cast<MMmfVideoHwDeviceVideoPolicyExtension*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMmfVideoHwDevVideoPolicyExtension));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		}
	else
		{
		TInt priority = 666;
		interface->MmvhvpeSetVideoPriority(priority);
		
		// Now set up the test interface...
		MDevVideoCiuTestInterfaceClientResourceNotifier* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceClientResourceNotifier*>
									(iDevVideoPlay->CustomInterface( iHwDeviceId,
																	KUidDevVideoCiuTestInterfaceClientResourceNotifier));
		if (testInterface)
			{
			if (testInterface->McrnVideoPriority() == priority)
				{
				INFO_PRINTF1(_L("Succesfully retrieved the video priority."));
				ret = EPass;
				}
			else
				{
				ERR_PRINTF1(_L("Failed to retrieve the video priority."));
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

//-------------------------------------------------------------------------
CTestDevVideoCiuPlayClientNotifierRevoked::CTestDevVideoCiuPlayClientNotifierRevoked(const TDesC& aTestName)
	: CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayClientNotifierRevoked* CTestDevVideoCiuPlayClientNotifierRevoked::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayClientNotifierRevoked* self = new(ELeave) CTestDevVideoCiuPlayClientNotifierRevoked(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayClientNotifierRevoked::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();

	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));

		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

// Implementation that uses the test interface to set data that can be checked
// in the call to the interface's "get" method.
TVerdict CTestDevVideoCiuPlayClientNotifierRevoked::DoTestL()
	{
	TVerdict ret = EFail;

	// Got this far so must have successfully initialised DevVideoPlay
	// Try to fetch the CI...
	MMmfVideoHwDeviceVideoPolicyExtension* interface = NULL;
		
	interface = static_cast<MMmfVideoHwDeviceVideoPolicyExtension*>(iDevVideoPlay->CustomInterface(
																		iHwDeviceId, 
																		KUidMmfVideoHwDevVideoPolicyExtension));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EInconclusive;
		}
	else
		{
		MMmfVideoHwDevClientNotifier* clientNotifier = this;
		// call the interface's method
		interface->MmvhvpeSetVideoClient(clientNotifier);
		
		// Now set up the test interface...
		MDevVideoCiuTestInterfaceClientResourceNotifier* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceClientResourceNotifier*>
									(iDevVideoPlay->CustomInterface( iHwDeviceId,
																	KUidDevVideoCiuTestInterfaceClientResourceNotifier));
		if (testInterface)
			{
			INFO_PRINTF1(_L("Requesting the revoke callback."));
			testInterface->McrnRequestRevokeCallback();
			
			if (iCallback)
				{
				ret = EPass;
				}
			else
				{
				ERR_PRINTF1(_L("Didn't receive the revoke callback."));
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

// from MMmfVideoHwDevClientNotifier
void CTestDevVideoCiuPlayClientNotifierRevoked::MmvhcnVideoAccessRevoked()
	{
	INFO_PRINTF1(_L("Access revoked callback received."));
	iCallback = ETrue;
	}
	
// from MMmfVideoHwDevClientNotifier	
void CTestDevVideoCiuPlayClientNotifierRevoked::MmvhcnReleaseVideoResource()
	{
	}

//----------------------------------------------------------------------------------------

CTestDevVideoCiuPlayClientNotifierResource::CTestDevVideoCiuPlayClientNotifierResource(const TDesC& aTestName)
	: CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayClientNotifierResource* CTestDevVideoCiuPlayClientNotifierResource::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayClientNotifierResource* self = new(ELeave) CTestDevVideoCiuPlayClientNotifierResource(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayClientNotifierResource::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();

	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));

		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}

	return ret;
	}

// Implementation that uses the test interface to set data that can be checked
// in the call to the interface's "get" method.
TVerdict CTestDevVideoCiuPlayClientNotifierResource::DoTestL()
	{
	TVerdict ret = EFail;

	// Got this far so must have successfully initialised DevVideoPlay
	// Try to fetch the CI...
	MMmfVideoHwDeviceVideoPolicyExtension* interface = NULL;
		
	interface = static_cast<MMmfVideoHwDeviceVideoPolicyExtension*>(iDevVideoPlay->CustomInterface(
																		iHwDeviceId, 
																		KUidMmfVideoHwDevVideoPolicyExtension));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EInconclusive;
		}
	else
		{
		MMmfVideoHwDevClientNotifier* clientNotifier = this;
		// call the interface's method
		interface->MmvhvpeSetVideoClient(clientNotifier);
		
		// Now set up the test interface...
		MDevVideoCiuTestInterfaceClientResourceNotifier* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceClientResourceNotifier*>
									(iDevVideoPlay->CustomInterface( iHwDeviceId,
																	KUidDevVideoCiuTestInterfaceClientResourceNotifier));
		if (testInterface)
			{
			INFO_PRINTF1(_L("Requesting the resource callback."));
			testInterface->McrnRequestResourceCallback();
			
			if (iCallback)
				{
				ret = EPass;
				}
			else
				{
				ERR_PRINTF1(_L("Didn't receive the resource callback."));
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

// from MMmfVideoHwDevClientNotifier
void CTestDevVideoCiuPlayClientNotifierResource::MmvhcnVideoAccessRevoked()
	{
	}

// from MMmfVideoHwDevClientNotifier	
void CTestDevVideoCiuPlayClientNotifierResource::MmvhcnReleaseVideoResource()
	{
	INFO_PRINTF1(_L("Release video resource callback received."));
	iCallback = ETrue;
	}
//----------------------------------------------------------------------------------------

//------------------------------------------------------------------
//			Negative Tests
//------------------------------------------------------------------

CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder
	::CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder* 
CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder* self = 
									new(ELeave) CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();

	if (ret == EPass)
		{
		// select the decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the Decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder::DoTestL()
	{
	TVerdict ret = EPass;

	INFO_PRINTF1(_L("DevVideo CIU Video Media Protected Creation Failure from Decoder"));

	// Got this far so must have successfully initialised DevVideoPlay
	// Try to fetch the CI...
	MMmfVideoMediaProtected* interface = NULL;
	interface = static_cast<MMmfVideoMediaProtected*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
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


#ifdef SYMBIAN_BUILD_GCE

//
//	Video Surface Support Management Tests
//
CTestDevVideoCiuPlayGetAndSetUseSurface::CTestDevVideoCiuPlayGetAndSetUseSurface(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayGetAndSetUseSurface* CTestDevVideoCiuPlayGetAndSetUseSurface::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayGetAndSetUseSurface* self = new(ELeave) CTestDevVideoCiuPlayGetAndSetUseSurface(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetUseSurface::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetUseSurface::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Initialisation"));
	
	// Fetch the CI
	MMMFVideoSurfaceSupport* interface = NULL;
		
	interface = static_cast<MMMFVideoSurfaceSupport*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
											KUidMMFVideoSurfaceSupport));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Instantiated"));
		
		TBool useSurface = ETrue; 
		interface->MmvssUseSurfaces();
		
		MDevVideoCiuTestInterfaceVideoSurfaceSupport* testInterface = NULL;

		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoSurfaceSupport*>
										(iDevVideoPlay->CustomInterface(iHwDeviceId, 
											KUidDevVideoCiuTestInterfaceVideoSurfaceSupport));
		if (testInterface)
			{
			TBool useSurfaceResult = testInterface->MdvssUseSurface();
			if (useSurface == useSurfaceResult)
				{
				ret = EPass;
				INFO_PRINTF1(_L("Use Surface call performed as expected"));
				}
			else
				{
				ret = EFail;					
				ERR_PRINTF2(_L("Use Surface call failed. Expected: %d"),useSurface);
				ERR_PRINTF2(_L("Result: %d"),useSurfaceResult);
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
	
//-----------------------------------------------------------//
CTestDevVideoCiuPlayGetAndSetSurfaceParameters::CTestDevVideoCiuPlayGetAndSetSurfaceParameters(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayGetAndSetSurfaceParameters* CTestDevVideoCiuPlayGetAndSetSurfaceParameters::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayGetAndSetSurfaceParameters* self = new(ELeave) CTestDevVideoCiuPlayGetAndSetSurfaceParameters(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetSurfaceParameters::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetSurfaceParameters::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Initialisation"));
	
	// Fetch the CI
	MMMFVideoSurfaceSupport* interface = NULL;
		
	interface = static_cast<MMMFVideoSurfaceSupport*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
											KUidMMFVideoSurfaceSupport));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Instantiated"));
		
		TSurfaceId surfaceId;
		TRect getRect;
		TVideoAspectRatio par;
		interface->MmvssGetSurfaceParametersL(surfaceId, getRect, par);
						
		MDevVideoCiuTestInterfaceVideoSurfaceSupport* testInterface = NULL;

		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoSurfaceSupport*>
										(iDevVideoPlay->CustomInterface(iHwDeviceId, 
											KUidDevVideoCiuTestInterfaceVideoSurfaceSupport));
		if (testInterface)
			{
			TMMFVideoSurfaceConfig vsconfResult = testInterface->MdvssSurfaceParameters();
			
			if (vsconfResult.iSurfaceId == surfaceId && vsconfResult.iCropRectangle.iTl.iX == getRect.iTl.iX &&
			vsconfResult.iCropRectangle.iTl.iY == getRect.iTl.iY && vsconfResult.iCropRectangle.iBr.iX == getRect.iBr.iX && 
			vsconfResult.iCropRectangle.iBr.iY == getRect.iBr.iY &&
			vsconfResult.iPixelAspectRatio.iNumerator == par.iNumerator && 
			vsconfResult.iPixelAspectRatio.iDenominator == par.iDenominator)
				{
				ret = EPass;
				INFO_PRINTF1(_L("Get Surface Parameters method call performed as expected"));
				}
			else
				{
				ret = EFail;					
				ERR_PRINTF1(_L("Get Surface Parameters method call failed"));
				INFO_PRINTF5(_L("Expected surfaceId %d %d %d %d"),
								vsconfResult.iSurfaceId.iInternal[0], vsconfResult.iSurfaceId.iInternal[1],
								vsconfResult.iSurfaceId.iInternal[2], vsconfResult.iSurfaceId.iInternal[3]);
				INFO_PRINTF5(_L("surfaceId Available %d %d %d %d"),
								surfaceId.iInternal[0], surfaceId.iInternal[1], 
								surfaceId.iInternal[2], surfaceId.iInternal[3]);
				INFO_PRINTF2(_L("Expected Rect first %d"),vsconfResult.iCropRectangle.iTl.iX);
				INFO_PRINTF2(_L("Rect first Available %d"),getRect.iTl.iX);
				INFO_PRINTF2(_L("Expected Rect second %d"),vsconfResult.iCropRectangle.iTl.iY);
				INFO_PRINTF2(_L("Rect second Available %d"),getRect.iTl.iY);
				INFO_PRINTF2(_L("Expected Rect third %d"),vsconfResult.iCropRectangle.iBr.iX);
	
				INFO_PRINTF2(_L("Rect third Available %d"),getRect.iBr.iX);
				INFO_PRINTF2(_L("Expected Rect fourth %d"),vsconfResult.iCropRectangle.iBr.iY);
				INFO_PRINTF2(_L("Rect fourth Available %d"),getRect.iBr.iY);
				INFO_PRINTF2(_L("Expected TVideoAspectRatio iNumerator %d"),vsconfResult.iPixelAspectRatio.iNumerator);
				INFO_PRINTF2(_L("TVideoAspectRatio iNumerator Available %d"),par.iNumerator);
				INFO_PRINTF2(_L("Expected TVideoAspectRatio iDenominator %d"),vsconfResult.iPixelAspectRatio.iDenominator);
				INFO_PRINTF2(_L("TVideoAspectRatio iDenominator Available %d"),par.iDenominator);

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
	
//-----------------------------------------------------------//
CTestDevVideoCiuPlayGetAndSetSurfaceRemoved::CTestDevVideoCiuPlayGetAndSetSurfaceRemoved(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayGetAndSetSurfaceRemoved* CTestDevVideoCiuPlayGetAndSetSurfaceRemoved::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayGetAndSetSurfaceRemoved* self = new(ELeave) CTestDevVideoCiuPlayGetAndSetSurfaceRemoved(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetSurfaceRemoved::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetSurfaceRemoved::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Initialisation"));
	
	// Fetch the CI
	MMMFVideoSurfaceSupport* interface = NULL;
		
	interface = static_cast<MMMFVideoSurfaceSupport*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
											KUidMMFVideoSurfaceSupport));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Instantiated"));
		
		// these values can be anything
		TSurfaceId surfaceId;
		surfaceId.iInternal[0] = 37;
		surfaceId.iInternal[1] = 104;
		surfaceId.iInternal[2] = 118;
		surfaceId.iInternal[3] = 1045;
		
		interface->MmvssSurfaceRemovedL(surfaceId);
		
		MDevVideoCiuTestInterfaceVideoSurfaceSupport* testInterface = NULL;

		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoSurfaceSupport*>
										(iDevVideoPlay->CustomInterface(iHwDeviceId, 
											KUidDevVideoCiuTestInterfaceVideoSurfaceSupport));
		if (testInterface)
			{
			TSurfaceId surfaceIdResult = testInterface->MdvssSurfaceRemoved();
			if (surfaceId == surfaceIdResult)
				{
				ret = EPass;
				INFO_PRINTF1(_L("Surface Removed method call performed as expected"));
				}
			else
				{
				ret = EFail;					
				ERR_PRINTF1(_L("Surface Removed method call failed."));
				INFO_PRINTF5(_L("Expected surfaceId %d %d %d %d"),
							surfaceId.iInternal[0], surfaceId.iInternal[1],
							surfaceId.iInternal[2], surfaceId.iInternal[3]);
				INFO_PRINTF5(_L("surfaceId Available %d %d %d %d"),
							surfaceIdResult.iInternal[0], surfaceIdResult.iInternal[1], 
							surfaceIdResult.iInternal[2], surfaceIdResult.iInternal[3]);
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
	
//-----------------------------------------------------------//
CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver::CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver* CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver* self = new(ELeave) CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Initialisation"));
	
	// Fetch the CI
	MMMFVideoSurfaceSupport* interface = NULL;
		
	interface = static_cast<MMMFVideoSurfaceSupport*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMMFVideoSurfaceSupport));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Instantiated"));
	
		MMMFVideoSurfaceObserver* obs = this;
		interface->MmvssSetObserver(*obs);
		INFO_PRINTF1(_L("MmvssSetObserver called"));
		
		// Verify that the observer was set
		MDevVideoCiuTestInterfaceVideoSurfaceSupport* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoSurfaceSupport*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
															KUidDevVideoCiuTestInterfaceVideoSurfaceSupport));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			ret = EFail;
			}
		else
			{
			if (obs != testInterface->MvsoObserver())
				{
				ERR_PRINTF1(_L("Failed to set the observer."));
				ret = EFail;
				}
			else
				{			
				INFO_PRINTF1(_L("SetObserver successfully called"));
				ret = EPass;
				}
			}		
		}
	return ret;
	}

//from MMMFVideoSurfaceObserver
void CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver::MmvsoSurfaceCreated()
	{}
		
void CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver::MmvsoSurfaceParametersChanged()
	{}

void CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver::MmvsoRemoveSurface()
	{}
	
//------------------------------------------------------------------

CTestDevVideoCiuPlaySurfaceCreated::CTestDevVideoCiuPlaySurfaceCreated(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName), iCallbackReceived(EFalse)
	{
	}

CTestDevVideoCiuPlaySurfaceCreated* CTestDevVideoCiuPlaySurfaceCreated::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlaySurfaceCreated* self = new(ELeave) CTestDevVideoCiuPlaySurfaceCreated(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlaySurfaceCreated::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlaySurfaceCreated::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Initialisation"));
	
	// Fetch the CI
	MMMFVideoSurfaceSupport* interface = NULL;
		
	interface = static_cast<MMMFVideoSurfaceSupport*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMMFVideoSurfaceSupport));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Instantiated"));
	
		MDevVideoCiuTestInterfaceVideoSurfaceSupport* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoSurfaceSupport*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
															KUidDevVideoCiuTestInterfaceVideoSurfaceSupport));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			ret = EFail;
			}
		else
			{
			// Set the observer to callback!
			MMMFVideoSurfaceObserver* obs = this;
			interface->MmvssSetObserver(*obs);
		
			INFO_PRINTF1(_L("Requesting the surface created Callback..."));
			testInterface->MmvsoSurfaceCreatedCallback();

			if ((iError != KErrNone) || (!iCallbackReceived))
				{	
				ERR_PRINTF1(_L("Error receiving the Surface Created Callback."));
				ret = EFail;
				}
			else
				{
				INFO_PRINTF1(_L("Successfully retrieved the Surface Support Callback"));
				ret = EPass;
				}
			}
		}
	return ret;
	}

//from MMMFVideoSurfaceObserver
void CTestDevVideoCiuPlaySurfaceCreated::MmvsoSurfaceCreated()
	{
	INFO_PRINTF1(_L("Surface Support callback received."));
	iCallbackReceived = ETrue;
	iError = KErrNone;
	}
		
void CTestDevVideoCiuPlaySurfaceCreated::MmvsoSurfaceParametersChanged()
	{
	iError = KErrNotSupported;
	}
void CTestDevVideoCiuPlaySurfaceCreated::MmvsoRemoveSurface()
	{
	iError = KErrNotSupported;
	}

//------------------------------------------------------------------

CTestDevVideoCiuPlaySurfaceParametersChanged::CTestDevVideoCiuPlaySurfaceParametersChanged(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName), iCallbackReceived(EFalse)
	{
	}

CTestDevVideoCiuPlaySurfaceParametersChanged* CTestDevVideoCiuPlaySurfaceParametersChanged::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlaySurfaceParametersChanged* self = new(ELeave) CTestDevVideoCiuPlaySurfaceParametersChanged(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlaySurfaceParametersChanged::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlaySurfaceParametersChanged::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Initialisation"));
	
	// Fetch the CI
	MMMFVideoSurfaceSupport* interface = NULL;
		
	interface = static_cast<MMMFVideoSurfaceSupport*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMMFVideoSurfaceSupport));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Instantiated"));
	
		MDevVideoCiuTestInterfaceVideoSurfaceSupport* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoSurfaceSupport*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
															KUidDevVideoCiuTestInterfaceVideoSurfaceSupport));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			ret = EFail;
			}
		else
			{
			// Set the observer to callback!
			MMMFVideoSurfaceObserver* obs = this;
			interface->MmvssSetObserver(*obs);
		
			INFO_PRINTF1(_L("Requesting the Surface Parameters Changed Callback..."));
			testInterface->MmvsoSurfaceParametersChangedCallback();

			if ((iError != KErrNone) || (!iCallbackReceived))
				{	
				ERR_PRINTF1(_L("Error receiving Surface Parameters Changed Callback."));
				ret = EFail;
				}
			else
				{
				INFO_PRINTF1(_L("Successfully retrieved Surface Parameters Changed Callback"));
				ret = EPass;
				}
			}
		}
	return ret;
	}

//from MMMFVideoSurfaceObserver
void CTestDevVideoCiuPlaySurfaceParametersChanged::MmvsoSurfaceCreated()
	{
	iError = KErrNotSupported;
	}
		
void CTestDevVideoCiuPlaySurfaceParametersChanged::MmvsoSurfaceParametersChanged()
	{
	INFO_PRINTF1(_L("Surface Parameters Changed callback received."));
	iCallbackReceived = ETrue;
	iError = KErrNone;
	}
	
void CTestDevVideoCiuPlaySurfaceParametersChanged::MmvsoRemoveSurface()
	{
	iError = KErrNotSupported;
	}

//------------------------------------------------------------------

CTestDevVideoCiuRemoveSurface::CTestDevVideoCiuRemoveSurface(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName), iCallbackReceived(EFalse)
	{
	}

CTestDevVideoCiuRemoveSurface* CTestDevVideoCiuRemoveSurface::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuRemoveSurface* self = new(ELeave) CTestDevVideoCiuRemoveSurface(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuRemoveSurface::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuRemoveSurface::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Initialisation"));
	
	// Fetch the CI
	MMMFVideoSurfaceSupport* interface = NULL;
		
	interface = static_cast<MMMFVideoSurfaceSupport*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMMFVideoSurfaceSupport));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Surface Support Interface Instantiated"));
	
		MDevVideoCiuTestInterfaceVideoSurfaceSupport* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoSurfaceSupport*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
															KUidDevVideoCiuTestInterfaceVideoSurfaceSupport));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface."));
			ret = EFail;
			}
		else
			{
			// Set the observer to callback!
			MMMFVideoSurfaceObserver* obs = this;
			interface->MmvssSetObserver(*obs);
		
			INFO_PRINTF1(_L("Requesting Remove Surface Callback..."));
			testInterface->MmvsoRemoveSurfaceCallback();

			if ((iError != KErrNone) || (!iCallbackReceived))
				{	
				ERR_PRINTF1(_L("Error receiving Remove Surface Callback."));
				ret = EFail;
				}
			else
				{
				INFO_PRINTF1(_L("Successfully retrieved Remove Surface Callback"));
				ret = EPass;
				}
			}
		}
	return ret;
	}

//from MMMFVideoSurfaceObserver
void CTestDevVideoCiuRemoveSurface::MmvsoSurfaceCreated()
	{
	iError = KErrNotSupported;
	}
		
void CTestDevVideoCiuRemoveSurface::MmvsoSurfaceParametersChanged()
	{
	iError = KErrNotSupported;
	}
	
void CTestDevVideoCiuRemoveSurface::MmvsoRemoveSurface()
	{
	INFO_PRINTF1(_L("Remove Surface callback received."));
	iCallbackReceived = ETrue;
	iError = KErrNone;
	}

//----------------------------------------------------------------------------
CTestDevVideoCiuPlayUseClientBuffers::CTestDevVideoCiuPlayUseClientBuffers(const TDesC& aTestName)
	: CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayUseClientBuffers* CTestDevVideoCiuPlayUseClientBuffers::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayUseClientBuffers* self = new(ELeave) CTestDevVideoCiuPlayUseClientBuffers(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayUseClientBuffers::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();

	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	
	return ret;
	}

TVerdict CTestDevVideoCiuPlayUseClientBuffers::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU use client buffer mode test"));

	// Try to fetch the CI...
	MMMFVideoClientBufferSupport* interface = NULL;
		
	interface = static_cast<MMMFVideoClientBufferSupport*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMMFVideoClientBufferSupport));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		}
	else
		{
		TBool clientBuffers = ETrue;
		interface->MvcbsUseClientBuffers(clientBuffers);
		
		// Now set up the test interface...
		MDevVideoCiuTestInterfaceVideoClientBufferSupport* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoClientBufferSupport*>
							(iDevVideoPlay->CustomInterface( iHwDeviceId,
									KUidDevVideoCiuTestInterfaceVideoClientBufferSupport));
		if (testInterface)
			{
			if (testInterface->MdvcbsClientBuffers())
				{
				INFO_PRINTF1(_L("Succesfully retrieved the video client buffer mode"));
				ret = EPass;
				}
			else
				{
				ERR_PRINTF1(_L("Failed to retrieve the video client buffer mode."));
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

//----------------------------------------------------------------------------
CTestDevVideoCiuPlaySupplyBuffer::CTestDevVideoCiuPlaySupplyBuffer(const TDesC& aTestName)
	: CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlaySupplyBuffer* CTestDevVideoCiuPlaySupplyBuffer::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlaySupplyBuffer* self = new(ELeave) CTestDevVideoCiuPlaySupplyBuffer(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlaySupplyBuffer::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();

	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	
	return ret;
	}

TVerdict CTestDevVideoCiuPlaySupplyBuffer::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU supply buffer test"));

	// Try to fetch the CI...
	MMMFVideoClientBufferSupport* interface = NULL;
		
	interface = static_cast<MMMFVideoClientBufferSupport*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMMFVideoClientBufferSupport));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		}
	else
		{
		TUncompressedVideoFormat format;
		format.iDataFormat = ERgbRawData;
		format.iRgbFormat = ERgb32bit888;		
		RChunk aChunk;

		// Create a dummy object for testing only
		TVideoFrameBuffer buffer(format, 1, 1, aChunk, 0);
		interface->MvcbsSupplyBuffer(buffer);
		
		// Now set up the test interface...
		MDevVideoCiuTestInterfaceVideoClientBufferSupport* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoClientBufferSupport*>
							(iDevVideoPlay->CustomInterface( iHwDeviceId,
									KUidDevVideoCiuTestInterfaceVideoClientBufferSupport));
		if (testInterface)
			{
			if (testInterface->MdvcbsSupplyBuffer() == &buffer)
				{
				INFO_PRINTF1(_L("Succesfully retrieved the video supplied buffer"));
				ret = EPass;
				}
			else
				{
				ERR_PRINTF1(_L("Failed to retrieve the video supplied buffer."));
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
	
//----------------------------------------------------------------------------
CTestDevVideoCiuPlayReleaseBuffers::CTestDevVideoCiuPlayReleaseBuffers(const TDesC& aTestName)
	: CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayReleaseBuffers* CTestDevVideoCiuPlayReleaseBuffers::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayReleaseBuffers* self = new(ELeave) CTestDevVideoCiuPlayReleaseBuffers(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayReleaseBuffers::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();

	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	
	return ret;
	}

TVerdict CTestDevVideoCiuPlayReleaseBuffers::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU release buffer test"));

	// Try to fetch the CI...
	MMMFVideoClientBufferSupport* interface = NULL;
		
	interface = static_cast<MMMFVideoClientBufferSupport*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
																	KUidMMFVideoClientBufferSupport));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved"));
		}
	else
		{
		interface->MvcbsReleaseBuffers();
		
		// Now set up the test interface...
		MDevVideoCiuTestInterfaceVideoClientBufferSupport* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoClientBufferSupport*>
							(iDevVideoPlay->CustomInterface( iHwDeviceId,
									KUidDevVideoCiuTestInterfaceVideoClientBufferSupport));
		if (testInterface)
			{
			// In the test decoder, we set true when we call release buffer. Here just check it will return true.
			if (testInterface->MdvcbsReleaseBuffers())
				{
				INFO_PRINTF1(_L("Succesfully release buffer."));
				ret = EPass;
				}
			else
				{
				ERR_PRINTF1(_L("Failed to release buffer."));
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

CTestDevVideoCiuPlaySurfaceHandleControl::CTestDevVideoCiuPlaySurfaceHandleControl(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlaySurfaceHandleControl* CTestDevVideoCiuPlaySurfaceHandleControl::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlaySurfaceHandleControl* self = new(ELeave) CTestDevVideoCiuPlaySurfaceHandleControl(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlaySurfaceHandleControl::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlaySurfaceHandleControl::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Surface Handle Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoSurfaceHandleControl* interface = NULL;
	
	interface = static_cast<MMmfVideoSurfaceHandleControl*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
											KUidMMFVideoSurfaceHandleControl));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved for SurfaceHandleControl"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU SurfaceHandle Control Interface Instantiated"));
		TSurfaceId nullId = TSurfaceId::CreateNullId();
		INFO_PRINTF1(_L("DevVideo CIU SurfaceHandle Control testing both surfacehandle functions"));
		interface->MmvshcSetSurfaceHandle(nullId);

		// Fetch test interface
		MDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControl* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControl*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
												KUidDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControl));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface for Play rate control."));
			ret = EFail;
			}
		else
			{
			TSurfaceId surfaceId = testInterface->MdvshcGetPlaySurfaceHandle();
			
			if (surfaceId == nullId) 
				{
				ret = EPass;
				INFO_PRINTF1(_L("Get play surface handle method call performed as expected"));
				}
			else
				{
				ret = EFail;					
				ERR_PRINTF1(_L("Get Play surface handle method call failed"));
				}
		  	}
		}
	return ret;
	}

//------------------------------------------------------------------

CTestDevVideoCiuPlaySurfaceRedrawBuffer::CTestDevVideoCiuPlaySurfaceRedrawBuffer(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlaySurfaceRedrawBuffer* CTestDevVideoCiuPlaySurfaceRedrawBuffer::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlaySurfaceRedrawBuffer* self = new(ELeave) CTestDevVideoCiuPlaySurfaceRedrawBuffer(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlaySurfaceRedrawBuffer::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlaySurfaceRedrawBuffer::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Surface Redraw Buffer - Start"));
	
	// Fetch the CI
	MMmfVideoSurfaceHandleControl* interface = NULL;
	
	interface = static_cast<MMmfVideoSurfaceHandleControl*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
											KUidMMFVideoSurfaceHandleControl));
	if (!interface)
		{
		ERR_PRINTF1(_L("DevVideo CIU Surface Redraw Buffer - No interface"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Surface Redraw Buffer - Interface Instantiated"));
		HBufC8* redrawBuffer = HBufC8::NewLC(10);
		_LIT8(KTestString, "TestString");
		*redrawBuffer = KTestString();
		TPtrC8 bufPtr(*redrawBuffer);
		
		INFO_PRINTF1(_L("DevVideo CIU Surface Redraw Buffer - Setting redraw buffer"));
		interface->MmvshcRedrawBufferToSurface(bufPtr);
		CleanupStack::PopAndDestroy(redrawBuffer);
		redrawBuffer = NULL;

		// Fetch test interface
		MDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControl* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControl*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
												KUidDevVideoCiuTestInterfaceVideoPlaySurfaceHandleControl));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("DevVideo CIU Surface Redraw Buffer - Failed to create test interface"));
			ret = EFail;
			}
		else
			{
			TDesC8& retrievedBuffer = testInterface->MdvshcGetRedrawBuffer();
			
			if (retrievedBuffer.Compare(KTestString()) == 0)
				{
				INFO_PRINTF1(_L("DevVideo CIU Surface Redraw Buffer - Retrieved correct string"));
				ret = EPass;
				}
			else
				{
				INFO_PRINTF1(_L("DevVideo CIU Surface Redraw Buffer - Retrieved incorrect string"));
				ret = EFail;					
				}
			}
		}
	return ret;
	}
	
#endif // SYMBIAN_BUILD_GCE

//
//	Video Play Rate Control Tests
//

CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities::CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities* CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities* self = new(ELeave) CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Play Rate Control Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoPlayRateControl* interface = NULL;
	
	interface = static_cast<MMmfVideoPlayRateControl*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
											KUidMmfVideoPlayRateControl));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved for Play Rate"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Play Rate Interface Instantiated"));
		
		TVideoPlayRateCapabilities videoPlayRateCapabilities;
		TRAPD(err, interface->MmvprcGetPlayRateCapabilitiesL(videoPlayRateCapabilities));
		
		if (err == KErrNone)
			{
			// Fetch test interface
			MDevVideoCiuTestInterfaceVideoPlayRateControl* testInterface = NULL;
			testInterface = static_cast<MDevVideoCiuTestInterfaceVideoPlayRateControl*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
												KUidDevVideoCiuTestInterfaceVideoPlayRateControl));
			if (!testInterface)
				{
				ERR_PRINTF1(_L("Failed to create the test interface for Play rate control."));
				ret = EFail;
				}
			else
				{
				TVideoPlayRateCapabilities playRateCapResult = testInterface->MdvproGetPlayRateCapabilities();
				
				if (playRateCapResult.iPlayBackward == videoPlayRateCapabilities.iPlayBackward &&
						playRateCapResult.iPlayForward == videoPlayRateCapabilities.iPlayForward &&
						playRateCapResult.iStepBackward == videoPlayRateCapabilities.iStepBackward &&
						playRateCapResult.iStepForward == videoPlayRateCapabilities.iStepForward)
					{
					ret = EPass;
					INFO_PRINTF1(_L("Get Play Rate Capabilities method call performed as expected"));
					}
				else
					{
					ret = EFail;					
					ERR_PRINTF1(_L("Get Play Rate Capabilities method call failed"));
					INFO_PRINTF5(_L("Expected PlayBackward: %d, PlayForward: %d, StepBackward: %d, StepForward: %d"),
							playRateCapResult.iPlayBackward, playRateCapResult.iPlayForward,
							playRateCapResult.iStepBackward, playRateCapResult.iStepForward);
					}
				}	
			}
		else 
			{
			ERR_PRINTF2(_L("Failed to get the play rate capabilities: %d."),err);
			}
		
		}
	return ret;
	}

//-----------------------------------------------------------//
CTestDevVideoCiuPlaySetAndGetPlayRate::CTestDevVideoCiuPlaySetAndGetPlayRate(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlaySetAndGetPlayRate* CTestDevVideoCiuPlaySetAndGetPlayRate::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlaySetAndGetPlayRate* self = new(ELeave) CTestDevVideoCiuPlaySetAndGetPlayRate(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlaySetAndGetPlayRate::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlaySetAndGetPlayRate::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Play Rate Control Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoPlayRateControl* interface = NULL;
	
	interface = static_cast<MMmfVideoPlayRateControl*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
											KUidMmfVideoPlayRateControl));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved for Play Rate"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Play Rate Interface Instantiated"));
		
		TInt playRate = 99; // The hardcoded value is for test purpose only
		TRAPD(err, interface->MmvprcSetPlayRateL(playRate));
		
		if (err == KErrNone)
			{
			TInt playRateResult = 0;
			TRAP(err, playRateResult = interface->MmvprcPlayRateL());
			
			if (err == KErrNone)
				{
				if (playRate == playRateResult)
					{
					ret = EPass;
					INFO_PRINTF1(_L("Set and get Play Rate method calls performed as expected"));
					}
				else
					{
					ret = EFail;					
					ERR_PRINTF3(_L("Set and get Play Rate methods call failed. Expected playRate: %d, return is: %d"), playRate, playRateResult);
					}
				}
			else
				{
				ERR_PRINTF2(_L("Failed to get the play rate: %d."), err);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Failed to set the play rate: %d."), err);
			}
		}
	return ret;
	}

//-----------------------------------------------------------//
CTestDevVideoCiuPlayGetAndSetStepFrame::CTestDevVideoCiuPlayGetAndSetStepFrame(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlayGetAndSetStepFrame* CTestDevVideoCiuPlayGetAndSetStepFrame::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayGetAndSetStepFrame* self = new(ELeave) CTestDevVideoCiuPlayGetAndSetStepFrame(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetStepFrame::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayGetAndSetStepFrame::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Play Rate Control Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoPlayRateControl* interface = NULL;
	
	interface = static_cast<MMmfVideoPlayRateControl*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
											KUidMmfVideoPlayRateControl));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved for Play Rate"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Play Rate Interface Instantiated"));
		
		TInt stepFrame = 99; // The hardcoded value is for test purpose only
		TRAPD(err, interface->MmvprcStepFrameL(stepFrame));
		
		if (err == KErrNone)
			{
			// Fetch test interface
			MDevVideoCiuTestInterfaceVideoPlayRateControl* testInterface = NULL;
			testInterface = static_cast<MDevVideoCiuTestInterfaceVideoPlayRateControl*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
												KUidDevVideoCiuTestInterfaceVideoPlayRateControl));
			if (!testInterface)
				{
				ERR_PRINTF1(_L("Failed to create the test interface for Play rate control."));
				ret = EFail;
				}
			else
				{
				if (stepFrame == testInterface->MdvproStepFrame())
					{
					ret = EPass;
					INFO_PRINTF1(_L("Step frame method call performed as expected"));
					}
				else
					{
					ret = EFail;					
					ERR_PRINTF2(_L("Step frame method call failed. Expected stepFrame: %d."), stepFrame);
					}
				}	
			}
		else
			{
			ERR_PRINTF2(_L("Failed to set the step frame: %d."), err);
			}		
		}
	return ret;
	}
	
//------------------------------------------------------------------

CTestDevVideoCiuPlaySetAndGetPlayRateObserver::CTestDevVideoCiuPlaySetAndGetPlayRateObserver(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

CTestDevVideoCiuPlaySetAndGetPlayRateObserver* CTestDevVideoCiuPlaySetAndGetPlayRateObserver::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlaySetAndGetPlayRateObserver* self = new(ELeave) CTestDevVideoCiuPlaySetAndGetPlayRateObserver(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlaySetAndGetPlayRateObserver::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlaySetAndGetPlayRateObserver::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Play Rate Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoPlayRateControl* interface = NULL;
		
	interface = static_cast<MMmfVideoPlayRateControl*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
											KUidMmfVideoPlayRateControl));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved for Play Rate"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Play Rate Interface Instantiated"));
	
		// Set the observer
		MMmfVideoPlayRateObserver* obs = this;
		interface->MmvprcSetObserver(*obs);
		INFO_PRINTF1(_L("MmvprcSetObserver called"));
		
		// Verify that the observer was set
		MDevVideoCiuTestInterfaceVideoPlayRateControl* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoPlayRateControl*>(iDevVideoPlay->CustomInterface(iHwDeviceId,
											KUidDevVideoCiuTestInterfaceVideoPlayRateControl));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface for Play rate control."));
			ret = EFail;
			}
		else
			{
			if (obs != testInterface->MdvproObserver())
				{
				ERR_PRINTF1(_L("Failed to set the observer."));
				ret = EFail;
				}
			else
				{			
				INFO_PRINTF1(_L("SetObserver successfully called"));
				ret = EPass;
				}
			}		
		}
	return ret;
	}

//from MMmfVideoPlayRateObserver
void CTestDevVideoCiuPlaySetAndGetPlayRateObserver::MmvproStepFrameComplete(TTimeIntervalMicroSeconds /*aTimestamp*/)
	{}
		
void CTestDevVideoCiuPlaySetAndGetPlayRateObserver::MmvproKeyFrameModeRequest()
	{}
	
//------------------------------------------------------------------

CTestDevVideoCiuPlayStepFrameComplete::CTestDevVideoCiuPlayStepFrameComplete(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName), iCallbackReceived(EFalse)
	{
	}

CTestDevVideoCiuPlayStepFrameComplete* CTestDevVideoCiuPlayStepFrameComplete::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayStepFrameComplete* self = new(ELeave) CTestDevVideoCiuPlayStepFrameComplete(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayStepFrameComplete::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayStepFrameComplete::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Play Rate Control Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoPlayRateControl* interface = NULL;
	
	interface = static_cast<MMmfVideoPlayRateControl*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
									KUidMmfVideoPlayRateControl));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved for Play Rate"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Play Rate Interface Instantiated"));
	
		// Fetch test interface
		MDevVideoCiuTestInterfaceVideoPlayRateControl* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoPlayRateControl*>
											(iDevVideoPlay->CustomInterface(iHwDeviceId,
											KUidDevVideoCiuTestInterfaceVideoPlayRateControl));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface for Play rate control."));
			ret = EFail;
			}
		else
			{
			// Set the observer to callback!
			MMmfVideoPlayRateObserver* obs = this;
			interface->MmvprcSetObserver(*obs);
		
			INFO_PRINTF1(_L("Requesting the step frame complete Callback..."));
			testInterface->MdvproStepFrameCompleteCallback();

			if ((iError != KErrNone) && (!iCallbackReceived))
				{	
				ERR_PRINTF1(_L("Error receiving the step frame complete Callback."));
				ret = EFail;
				}
			else
				{
				INFO_PRINTF1(_L("Successfully retrieved the step frame complete Callback"));
				ret = EPass;
				}
			}
		}
	return ret;
	}

//from MMmfVideoPlayRateObserver
void CTestDevVideoCiuPlayStepFrameComplete::MmvproStepFrameComplete(TTimeIntervalMicroSeconds aTimestamp)
	{
	INFO_PRINTF1(_L("Step frame complete callback received. "));
	// Test h/w sets the time stamp to 10000
	if (aTimestamp == 10000)
		{
		iCallbackReceived = ETrue;
		iError = KErrNone;
		}
	else
		{
		iCallbackReceived = ETrue;
		iError = KErrGeneral;
		}
	}
		
void CTestDevVideoCiuPlayStepFrameComplete::MmvproKeyFrameModeRequest()
	{
	iError = KErrNotSupported;
	}

//------------------------------------------------------------------

CTestDevVideoCiuPlayKeyFrameModeRequest::CTestDevVideoCiuPlayKeyFrameModeRequest(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName), iCallbackReceived(EFalse)
	{
	}

CTestDevVideoCiuPlayKeyFrameModeRequest* CTestDevVideoCiuPlayKeyFrameModeRequest::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuPlayKeyFrameModeRequest* self = new(ELeave) CTestDevVideoCiuPlayKeyFrameModeRequest(aTestName);
	return self;
	}

TVerdict CTestDevVideoCiuPlayKeyFrameModeRequest::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));
		
		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuPlayKeyFrameModeRequest::DoTestL()
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("DevVideo CIU Play Rate Control Interface Initialisation"));
	
	// Fetch the CI
	MMmfVideoPlayRateControl* interface = NULL;
	
	interface = static_cast<MMmfVideoPlayRateControl*>
									(iDevVideoPlay->CustomInterface(iHwDeviceId,
									KUidMmfVideoPlayRateControl));
	if (!interface)
		{
		ERR_PRINTF1(_L("Error - no interface retrieved for Play Rate"));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("DevVideo CIU Play Rate Interface Instantiated"));
	
		// Fetch test interface
		MDevVideoCiuTestInterfaceVideoPlayRateControl* testInterface = NULL;
		testInterface = static_cast<MDevVideoCiuTestInterfaceVideoPlayRateControl*>
											(iDevVideoPlay->CustomInterface(iHwDeviceId,
											KUidDevVideoCiuTestInterfaceVideoPlayRateControl));
		if (!testInterface)
			{
			ERR_PRINTF1(_L("Failed to create the test interface for Play rate control."));
			ret = EFail;
			}
		else
			{
			// Set the observer to callback!
			MMmfVideoPlayRateObserver* obs = this;
			interface->MmvprcSetObserver(*obs);
		
			INFO_PRINTF1(_L("Requesting the key frame mode request Callback..."));
			testInterface->MdvproKeyFrameModeRequestCallback();

			if ((iError != KErrNone) && (!iCallbackReceived))
				{	
				ERR_PRINTF1(_L("Error receiving the key frame mode request Callback."));
				ret = EFail;
				}
			else
				{
				INFO_PRINTF1(_L("Successfully retrieved the key frame mode request Callback"));
				ret = EPass;
				}
			}
		}
	return ret;
	}

//from MMmfVideoPlayRateObserver
void CTestDevVideoCiuPlayKeyFrameModeRequest::MmvproStepFrameComplete(TTimeIntervalMicroSeconds /*aTimestamp*/)
	{
	iError = KErrNotSupported;
	}
		
void CTestDevVideoCiuPlayKeyFrameModeRequest::MmvproKeyFrameModeRequest()
	{
	INFO_PRINTF1(_L("Key frame mode request callback received."));
	iCallbackReceived = ETrue;
	iError = KErrNone;
	}


//------------------------------------------------------------------

CTestDevVideoCiuHwDeviceTunnel* CTestDevVideoCiuHwDeviceTunnel::NewL(const TDesC& aTestName)
	{
	CTestDevVideoCiuHwDeviceTunnel* self = new(ELeave) CTestDevVideoCiuHwDeviceTunnel(aTestName);
	return self;
	}

CTestDevVideoCiuHwDeviceTunnel::CTestDevVideoCiuHwDeviceTunnel(const TDesC& aTestName)
	:CTestDevVideoCiuPlayStep(aTestName)
	{
	}

TVerdict CTestDevVideoCiuHwDeviceTunnel::DoTestStepPreambleL()
	{
	// Call the base class first
	TVerdict ret = CTestDevVideoCiuPlayStep::DoTestStepPreambleL();
	if (ret == EPass)
		{
		// select decoder
		TRAPD(err, iHwDeviceId = iDevVideoPlay->SelectDecoderL(KUidDevVideoCiuTestDecodeHwDevice));

		if (err != KErrNone)
			{
			ret = EInconclusive;
			ERR_PRINTF1(_L("DevVideo Play Failed to select the decoder."));
			}
		}
	return ret;
	}

TVerdict CTestDevVideoCiuHwDeviceTunnel::DoTestL()
	{
	INFO_PRINTF1(_L("DevVideo CIU HwDeviceTunnel Initialisation"));

	// Fetch the CI
	MMmfVideoHwDeviceTunnel* interface = static_cast<MMmfVideoHwDeviceTunnel*>
	  (iDevVideoPlay->CustomInterface(iHwDeviceId, KUidMmfVideoHwDeviceTunnel));

	if (!interface)
		{
		ERR_PRINTF1(_L("Failed to create interface for HwDevice Tunnel"));
		return EFail;
		}

	INFO_PRINTF1(_L("Created interface for HwDevice Tunnel"));

	// Fetch test interface
	MDevVideoCiuTestInterfaceHwDeviceTunnel* testInterface =
	  static_cast<MDevVideoCiuTestInterfaceHwDeviceTunnel*>
	    (iDevVideoPlay->CustomInterface(iHwDeviceId,
	                                    KUidDevVideoCiuTestInterfaceHwDeviceTunnel));
	if (!testInterface)
		{
		ERR_PRINTF1(_L("Failed to create test interface for HwDevice Tunnel"));
		return EFail;
		}

	INFO_PRINTF1(_L("Created test interface for HwDevice Tunnel"));

	CMMFVideoHwDevice* hwDevice = interface->MmvhdtHwDevice();
	if (hwDevice != reinterpret_cast<CMMFVideoHwDevice*>(0x11111111))
		{
		INFO_PRINTF1(_L("MmvhdtHwDevice() failed"));
		return EFail;
		}

	INFO_PRINTF1(_L("MmvhdtHwDevice() succeeded"));

	interface->MmvhdtSetupTunnelL(reinterpret_cast<CMMFVideoHwDevice*>(0x22222222));

	hwDevice = testInterface->MhdtHwDevice();
	if (hwDevice != reinterpret_cast<CMMFVideoHwDevice*>(0x22222222))
		{
		INFO_PRINTF1(_L("MmvhdtSetupTunnelL() failed"));
		return EFail;
		}

	INFO_PRINTF1(_L("MmvhdtSetupTunnelL() succeeded"));

	return EPass;
	}

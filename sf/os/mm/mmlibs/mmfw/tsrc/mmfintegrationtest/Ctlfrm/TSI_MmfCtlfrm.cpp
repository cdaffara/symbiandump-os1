// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TSIMmfCtlfrm.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>
#include "TSI_MmfCtlfrm.h"
#include "TSI_MmfCtlfrmStep.h"
#include "TSI_MmfCtlfrmSuite.h"
#include "TSI_MmfCodes.h"
#include "TSI_MmfEventIds.h"
#include "ActrlTestUids.h"

#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>

const TUid KTestControllerUid = {KTSIMmfControllerUid};
const TUid KTestDataSourceUid = {KTSIMmfDataSourceUid};
const TUid KTestDataSinkUid   = {KTSIMmfDataSinkUid};

// audio controllers, from mmfControllerImplementationUIDs.hrh
const TUid KTestAudioControllerUid = {KMmfUidControllerAudio};

// pause length for repeat calls to GetPosition
// NB this can't be a const - global uninitialised data in ARM
#define KGetPositionIntervalLength 500000

// ---------------------------
// RTestMmfCtlfrmI0001
//
// Load each controller plugin into its own thread
//
// REQ172.3.1

RTestMmfCtlfrmI0001* RTestMmfCtlfrmI0001::NewL()
	{
	RTestMmfCtlfrmI0001* self = new(ELeave) RTestMmfCtlfrmI0001;
	return self;
	}

RTestMmfCtlfrmI0001::RTestMmfCtlfrmI0001()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0001");
	}


TVerdict RTestMmfCtlfrmI0001::DoTestStepL()
	{

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller, controller2;

	settings.iPriority = ETSIMmfPriorityLow;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a controller
	error = controller.Open(KTestControllerUid, settings);
	if (error)
		{
		ERR_PRINTF2(_L("controller1 failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// Open another controller 
	// (it is valid for this test to open two instances of the same controller)
	error = controller2.Open(KTestControllerUid, settings);
		if (error)
		{
		ERR_PRINTF2(_L("controller2 failed to open, error %d"), error);
		controller.Close();
		return iTestStepResult = EInconclusive;
		}

	TBuf8<KTextBufLen> memFunctionText;
	TUid uid = {KTSIMmfControllerUid};
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg messageDest(handleInfo);
	TBuf<KTextBufLen> memFunctionText16;
	TBuf<KTextBufLen> memFunction2Text16;

	// get thread addresses
	error = controller.CustomCommandSync(messageDest, KThreadIdFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller1 CustomCommandSync failed with error %d"), error);
		controller2.Close();
		controller.Close();
		return iTestStepResult = EInconclusive;
		}

	// memFunctionText comes back as hex string
	memFunctionText16.SetMax();
	memFunctionText16.Fill(0x0);
	memFunctionText16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Controller1 thread ID : 0x%S"), &memFunctionText16);

	error = controller2.CustomCommandSync(messageDest, KThreadIdFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller2 CustomCommandSync failed with error %d"), error);
		controller2.Close();
		controller.Close();
		return iTestStepResult = EInconclusive;
		}

	// memFunctionText comes back as hex string
	memFunction2Text16.SetMax();
	memFunction2Text16.Fill(0x0);
	memFunction2Text16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Controller2 thread ID : 0x%S"), &memFunction2Text16);

	// compare thread addresses; they must not match
	if(!memFunctionText16.Compare(memFunction2Text16))
		{
		ERR_PRINTF1(_L("Error - thread IDs match"));
		controller2.Close();
		controller.Close();
		return iTestStepResult = EFail;
		}
	
	controller2.Close();
	controller.Close();
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0002
//
// Give each controller thread its own heap
//
// REQ172.3.2

RTestMmfCtlfrmI0002* RTestMmfCtlfrmI0002::NewL()
	{
	RTestMmfCtlfrmI0002* self = new(ELeave) RTestMmfCtlfrmI0002;
	return self;
	}

RTestMmfCtlfrmI0002::RTestMmfCtlfrmI0002()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0002");
	}


TVerdict RTestMmfCtlfrmI0002::DoTestStepL()
	{
	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller, controller2;

	settings.iPriority = ETSIMmfPriorityLow;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a controller
	error = controller.Open(KTestControllerUid, settings);
	if (error)
		{
		ERR_PRINTF2(_L("controller1 failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// Open another controller 
	// (it is valid for this test to open two instances of the same controller)
	error = controller2.Open(KTestControllerUid, settings);
		if (error)
		{
		ERR_PRINTF2(_L("controller2 failed to open, error %d"), error);
		controller.Close();
		return iTestStepResult = EInconclusive;
		}

	TBuf8<KTextBufLen> memFunctionText;
	TUid uid = {KTSIMmfControllerUid};
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg messageDest(handleInfo);
	TBuf<KTextBufLen> memFunctionText16;
	TBuf<KTextBufLen> memFunction2Text16;

	// get heap addresses
	error = controller.CustomCommandSync(messageDest, KHeapAddressFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller1 CustomCommandSync failed with error %d"), error);
		controller2.Close();
		controller.Close();
		return iTestStepResult = EInconclusive;
		}

	// memFunctionText comes back as hex string
	memFunctionText16.SetMax();
	memFunctionText16.Fill(0x0);
	memFunctionText16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Controller1 heap address : 0x%S"), &memFunctionText16);

	error = controller2.CustomCommandSync(messageDest, KHeapAddressFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller2 CustomCommandSync failed with error %d"), error);
		controller2.Close();
		controller.Close();
		return iTestStepResult = EInconclusive;
		}

	// memFunctionText comes back as hex string
	memFunction2Text16.SetMax();
	memFunction2Text16.Fill(0x0);
	memFunction2Text16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Controller2 heap address : 0x%S"), &memFunction2Text16);

	// compare heap addresses; they must not match
	if(!memFunctionText16.Compare(memFunction2Text16))
		{
		ERR_PRINTF1(_L("Error - heap addresses match"));
		controller2.Close();
		controller.Close();
		return iTestStepResult = EFail;
		}

	// cleanup
	controller2.Close();
	controller.Close();

	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0003
//
// Provide inter-thread access from client to controller plugin
//
// REQ172.3.3

RTestMmfCtlfrmI0003* RTestMmfCtlfrmI0003::NewL()
	{
	RTestMmfCtlfrmI0003* self = new(ELeave) RTestMmfCtlfrmI0003;
	return self;
	}

RTestMmfCtlfrmI0003::RTestMmfCtlfrmI0003()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0003");
	}

TVerdict RTestMmfCtlfrmI0003::DoTestStepL()
	{
	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = ETSIMmfPriorityLow;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a controller
	
	error = controller.Open(KTestControllerUid, settings);
	if (error)
		{
		ERR_PRINTF2(_L("controller failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	TUid uid = {KTSIMmfControllerUid};
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg messageDest(handleInfo);

	// send dummy messages, to verify that return values are correct
	error = controller.CustomCommandSync(messageDest, KDummyFunc1, KNullDesC8, KNullDesC8);
	if(error == KDummyFunc1Return)
		error = KErrNone;
	else
		{
		ERR_PRINTF2(_L("CustomCommandSync KDummyFunc1 returned unexpected value %d"), error);
		controller.Close();
		return iTestStepResult = EFail;
		}


	error = controller.CustomCommandSync(messageDest, KDummyFunc2, KNullDesC8, KNullDesC8);
	if(error == KDummyFunc2Return)
		error = KErrNone;
	else
		{
		ERR_PRINTF2(_L("CustomCommandSync KDummyFunc2 returned unexpected value %d"), error);
		controller.Close();
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("CustomCommandSync returned expected values"));

	// cleanup
	controller.Close();
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0102
//
// Load a controller plugin by UID
//
// REQ172.5.1

RTestMmfCtlfrmI0102* RTestMmfCtlfrmI0102::NewL()
	{
	RTestMmfCtlfrmI0102* self = new(ELeave) RTestMmfCtlfrmI0102;
	return self;
	}

RTestMmfCtlfrmI0102::RTestMmfCtlfrmI0102()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0102");
	}

TVerdict RTestMmfCtlfrmI0102::DoTestStepL()
	{
	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = ETSIMmfPriorityLow;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a controller
	error = controller.Open(KTestControllerUid, settings);
	if (error)
		{
		ERR_PRINTF2(_L("controller failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	TUid uid = {KTSIMmfControllerUid};
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg messageDest(handleInfo);
	TBuf8<KTextBufLen> memFunctionText;
	TBuf<KTextBufLen> memFunctionText16;

	// verify that we've loaded the plugin requested
	// the return we expect to be the string KPluginUIDSubstitute
	// we'd like to use the UID itself, but it's private in CMMFController, so this will have to do
	error = controller.CustomCommandSync(messageDest, KPluginUidFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if(error)
		{
		// could be we haven't got this function in the loaded plugin 
		ERR_PRINTF2(_L("CustomCommandSync KPluginUidFunction failed, error code %d"), error);
		controller.Close();
		return iTestStepResult = EFail;
		}

	// this string must match that specified in TSIMmfController.cpp
	_LIT8(KPluginUIDSubstitute, "TSI_MMFController");

	// memFunctionText comes back as hex string
	memFunctionText16.SetMax();
	memFunctionText16.Fill(0x0);
	memFunctionText16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Controller returned : %S"), &memFunctionText16);

	// check return string is expected value
	if(memFunctionText.Compare(KPluginUIDSubstitute))	// returns 0 if equal
		{
		ERR_PRINTF1(_L("Error - string does not match expected value"));
		controller.Close();
		return iTestStepResult = EFail;
		}
	
	// cleanup
	controller.Close();
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0105
//
// Load a controller plugin by filename / extension
//
// REQ172.5.3, REQ172.5.4

RTestMmfCtlfrmI0105* RTestMmfCtlfrmI0105::NewL()
	{
	RTestMmfCtlfrmI0105* self = new(ELeave) RTestMmfCtlfrmI0105;
	return self;
	}

RTestMmfCtlfrmI0105::RTestMmfCtlfrmI0105()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0105");
	}

TVerdict RTestMmfCtlfrmI0105::DoTestStepL()
	{
	// open controller by filename / extension

	INFO_PRINTF1(_L("open controller by filename / extension"));

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = ETSIMmfPriorityLow;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data
	fSelect->SetMatchToFileNameL(_L("test.wav"));
	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
	TInt numControllers = controllers.Count();
	if(!numControllers)
		{
		ERR_PRINTF1(_L("Could not find any controllers"));
		CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
		return iTestStepResult = EFail;
		}
	INFO_PRINTF2(_L("Found %d controllers"), numControllers);
	CMMFControllerImplementationInformation* implInfo = controllers[0];
	TUid theControllerImplUid = implInfo->Uid();
	error = controller.Open(theControllerImplUid, settings); // instantiate the controller
	if(error)
		{
		ERR_PRINTF2(_L("Could not open controller, error %d"), error);
		CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
		return iTestStepResult = EFail;
		}

	INFO_PRINTF2(_L("Opened controller with UID 0x%8x"), theControllerImplUid);
	if(theControllerImplUid != KTestAudioControllerUid)	// check it's the right one
		{
		ERR_PRINTF1(_L("Error: this is not the Audio Controller"));
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("This is the Audio Controller"));
	controller.Close();
	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0106
//
// Load a controller plugin from a preferred supplier by filename / extension
//
// REQ172.5.3, REQ172.5.4

RTestMmfCtlfrmI0106* RTestMmfCtlfrmI0106::NewL()
	{
	RTestMmfCtlfrmI0106* self = new(ELeave) RTestMmfCtlfrmI0106;
	return self;
	}

RTestMmfCtlfrmI0106::RTestMmfCtlfrmI0106()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0106");
	}

TVerdict RTestMmfCtlfrmI0106::DoTestStepL()
	{
	// open controller by filename / extension and preferred supplier

	INFO_PRINTF1(_L("open controller by filename / extension and preferred supplier"));

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = ETSIMmfPriorityLow;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data
	fSelect->SetMatchToFileNameL(_L("test.wav"));
	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	// Set for only those plugins supplied by "Symbian"
	_LIT(KPrefSupplierSymbian, "Symbian");
	cSelect->SetPreferredSupplierL(KPrefSupplierSymbian, CMMFPluginSelectionParameters::EOnlyPreferredSupplierPluginsReturned);

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
	TInt numControllers = controllers.Count();
	if(!numControllers)
		{
		ERR_PRINTF1(_L("Could not find any controllers"));
		CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
		return iTestStepResult = EFail;
		}
	INFO_PRINTF2(_L("Found %d controllers"), numControllers);
	CMMFControllerImplementationInformation* implInfo = controllers[0];
	TUid theControllerImplUid = implInfo->Uid();
	error = controller.Open(theControllerImplUid, settings); // instantiate the controller
	if(error)
		{
		ERR_PRINTF2(_L("Could not open controller, error %d"), error);
		CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
		return iTestStepResult = EFail;
		}

	INFO_PRINTF2(_L("Opened controller with UID 0x%8x"), theControllerImplUid);
	if(theControllerImplUid != KTestAudioControllerUid)	// check it's the right one
		{
		ERR_PRINTF1(_L("Error: this is not the requested controller"));
		controller.Close();
		CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("This is the Audio Controller"));
	if(implInfo->Supplier() != KPrefSupplierSymbian)	// check supplier
		{
		ERR_PRINTF1(_L("Error: this controller is not from the requested supplier"));
		controller.Close();
		CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
		return iTestStepResult = EFail;
		}

	INFO_PRINTF1(_L("This controller is from the requested supplier"));
	controller.Close();
	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0107
//
// Add a second instance of a controller which already exists
//
// REQ172.5.1

RTestMmfCtlfrmI0107* RTestMmfCtlfrmI0107::NewL()
	{
	RTestMmfCtlfrmI0107* self = new(ELeave) RTestMmfCtlfrmI0107;
	return self;
	}

RTestMmfCtlfrmI0107::RTestMmfCtlfrmI0107()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0107");
	}

TVerdict RTestMmfCtlfrmI0107::DoTestStepL()
	{
	// add a second instance of an already loaded controller
	// essentially a duplicate of test 0001/0002 - simply to verify that we are running
	// two instances of the same controller

	INFO_PRINTF1(_L("add a second instance of an already loaded controller"));

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller, controller2;

	settings.iPriority = ETSIMmfPriorityLow;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a controller
	error = controller.Open(KTestControllerUid, settings);
	if (error)
		{
		ERR_PRINTF2(_L("controller1 failed to open, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// Open another controller 
	// (it is valid for this test to open two instances of the same controller)
	error = controller2.Open(KTestControllerUid, settings);
		if (error)
		{
		ERR_PRINTF2(_L("controller2 failed to open, error %d"), error);
		controller.Close();
		return iTestStepResult = EInconclusive;
		}

	TBuf8<KTextBufLen> memFunctionText;
	TUid uid = {KTSIMmfControllerUid};
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg messageDest(handleInfo);
	TBuf<KTextBufLen> memFunctionText16;
	TBuf<KTextBufLen> memFunction2Text16;

	// get thread addresses
	error = controller.CustomCommandSync(messageDest, KThreadIdFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller1 CustomCommandSync failed with error %d"), error);
		controller2.Close();
		controller.Close();
		return iTestStepResult = EInconclusive;
		}

	// memFunctionText comes back as hex string
	memFunctionText16.SetMax();
	memFunctionText16.Fill(0x0);
	memFunctionText16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Controller1 thread ID : 0x%S"), &memFunctionText16);

	error = controller2.CustomCommandSync(messageDest, KThreadIdFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller2 CustomCommandSync failed with error %d"), error);
		controller2.Close();
		controller.Close();
		return iTestStepResult = EInconclusive;
		}

	// memFunctionText comes back as hex string
	memFunction2Text16.SetMax();
	memFunction2Text16.Fill(0x0);
	memFunction2Text16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Controller2 thread ID : 0x%S"), &memFunction2Text16);

	// compare thread addresses; they must not match
	if(!memFunctionText16.Compare(memFunction2Text16))
		{
		ERR_PRINTF1(_L("Error - thread IDs match"));
		controller2.Close();
		controller.Close();
		return iTestStepResult = EFail;
		}

	// get heap addresses
	error = controller.CustomCommandSync(messageDest, KHeapAddressFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller1 CustomCommandSync failed with error %d"), error);
		controller2.Close();
		controller.Close();
		return iTestStepResult = EInconclusive;
		}

	// memFunctionText comes back as hex string
	memFunctionText16.SetMax();
	memFunctionText16.Fill(0x0);
	memFunctionText16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Controller1 heap address : 0x%S"), &memFunctionText16);

	error = controller2.CustomCommandSync(messageDest, KHeapAddressFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller2 CustomCommandSync failed with error %d"), error);
		controller2.Close();
		controller.Close();
		return iTestStepResult = EInconclusive;
		}

	// memFunctionText comes back as hex string
	memFunction2Text16.SetMax();
	memFunction2Text16.Fill(0x0);
	memFunction2Text16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Controller2 heap address : 0x%S"), &memFunction2Text16);

	// compare heap addresses; they must not match
	if(!memFunctionText16.Compare(memFunction2Text16))
		{
		ERR_PRINTF1(_L("Error - heap addresses match"));
		controller2.Close();
		controller.Close();
		return iTestStepResult = EFail;
		}

	// success if we've got here
	INFO_PRINTF1(_L("Two instances of the same controller loaded successfully"));

	// cleanup
	controller2.Close();
	controller.Close();

	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0108
//
// Load a controller plugin using ambiguous or insufficient information
//
// REQ172.5.3

RTestMmfCtlfrmI0108* RTestMmfCtlfrmI0108::NewL()
	{
	RTestMmfCtlfrmI0108* self = new(ELeave) RTestMmfCtlfrmI0108;
	return self;
	}

RTestMmfCtlfrmI0108::RTestMmfCtlfrmI0108()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0108");
	}

TVerdict RTestMmfCtlfrmI0108::DoTestStepL()
	{
	// load a controller with ambiguous information

	INFO_PRINTF1(_L("load a controller with insufficient information"));

	// we'll basically give no information at all. if we get a controller back at all,
	// we've passed.
	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = ETSIMmfPriorityLow;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
	TInt numControllers = controllers.Count();
	if(!numControllers)
		{
		ERR_PRINTF1(_L("Could not find any controllers"));
		CleanupStack::PopAndDestroy(2);//controllers, cSelect
		return iTestStepResult = EFail;
		}
	INFO_PRINTF2(_L("Found %d controllers"), numControllers);
	TBool controllerLoaded = EFalse;
	CMMFControllerImplementationInformation* implInfo = NULL;
	TUid theControllerImplUid = TUid::Uid(0);
	for(TInt i = 0; i < numControllers; i++)
		{
		implInfo = controllers[i];
		theControllerImplUid = implInfo->Uid();
		error = controller.Open(theControllerImplUid, settings); // instantiate the controller
		if(error)
			{
			if(error != KErrNoMemory)
				{
				ERR_PRINTF2(_L("Could not open controller, error %d"), error);
				break;
				}
			}
		else
			{
			controllerLoaded = ETrue;
			break;
			}
		}
	if(!controllerLoaded)
		{
		INFO_PRINTF1(_L("No controller loaded"));
		CleanupStack::PopAndDestroy(2);//controllers, cSelect
		return iTestStepResult = EFail;
		}

	const TDesC& theControllerImplName = implInfo->DisplayName();
	const TDesC& theControllerImplSupplier = implInfo->Supplier();
	INFO_PRINTF2(_L("Opened controller with UID 0x%8x"), theControllerImplUid);
	INFO_PRINTF3(_L("This is the %S, supplied by %S"), &theControllerImplName, &theControllerImplSupplier);
	controller.Close();
	CleanupStack::PopAndDestroy(2);//controllers, cSelect
	return iTestStepResult = EPass;
	
	}

// ---------------------------
// RTestMmfCtlfrmI0109
//
// Call overloaded constructors for parameters
//
// REQ: none

RTestMmfCtlfrmI0109* RTestMmfCtlfrmI0109::NewL()
	{
	RTestMmfCtlfrmI0109* self = new(ELeave) RTestMmfCtlfrmI0109;
	return self;
	}

RTestMmfCtlfrmI0109::RTestMmfCtlfrmI0109()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0109");
	}

TVerdict RTestMmfCtlfrmI0109::DoTestStepL()
	{
	// open controller by filename / extension

	INFO_PRINTF1(_L("call overloaded constructors for parameters"));

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewL();
	CleanupStack::PushL(cSelect);
	INFO_PRINTF1(_L("CMMFControllerPluginSelectionParameters constructed ok"));

	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewL();
	CleanupStack::PushL(fSelect);
	INFO_PRINTF1(_L("CMMFFormatSelectionParameters constructed ok"));

	CMMFFormatDecodePluginSelectionParameters* dSelect = CMMFFormatDecodePluginSelectionParameters::NewL();
	CleanupStack::PushL(dSelect);
	INFO_PRINTF1(_L("CMMFFormatDecodePluginSelectionParameters constructed ok"));

	CMMFFormatEncodePluginSelectionParameters* eSelect = CMMFFormatEncodePluginSelectionParameters::NewL();
	CleanupStack::PushL(eSelect);
	INFO_PRINTF1(_L("CMMFFormatEncodePluginSelectionParameters constructed ok"));

	// if we get here without leaving, we've passed
	CleanupStack::PopAndDestroy(4);//eSelect, dSelect, fSelect, cSelect
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0111
//
// Add a data source
//
// REQ172.5.5.1

RTestMmfCtlfrmI0111* RTestMmfCtlfrmI0111::NewL()
	{
	RTestMmfCtlfrmI0111* self = new(ELeave) RTestMmfCtlfrmI0111;
	return self;
	}

RTestMmfCtlfrmI0111::RTestMmfCtlfrmI0111()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0111");
	}

TVerdict RTestMmfCtlfrmI0111::DoTestStepL()
	{
	INFO_PRINTF1(_L("(a) add a data source"));

	TInt error = KErrNone;

	_LIT8(KInitData,"TEST");

	// first : add a data source without a handle
	error = iController.AddDataSource(KTestDataSourceUid, KInitData);
	if (error)
		{
		ERR_PRINTF2(_L("AddDataSource failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// Get log info - we expect "AddDataSourceL Called"
	_LIT8(KExpectedResult, "AddDataSourceL Called");
	TBuf8<KTextBufLen> memFunctionText;

	TUid uid = {KTSIMmfControllerUid};
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg messageDest(handleInfo);

	error = iController.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller CustomCommandSync failed with error %d"), error);
		return iTestStepResult = EInconclusive;
		}
		
	TBuf<KTextBufLen> memFunctionText16;
	memFunctionText16.SetMax();
	memFunctionText16.Fill(0x0);
	memFunctionText16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Log : %S"), &memFunctionText16);

	if (memFunctionText != KExpectedResult)
		{
		ERR_PRINTF1(_L("Return value did not match expected"));
		return iTestStepResult = EFail;
		}

	// second : add a data source with a handle
	INFO_PRINTF1(_L("(b) add a data source with handle"));
	TMMFMessageDestination* sourceHandlePtr = new (ELeave) TMMFMessageDestination();
	TMMFMessageDestination& sourceHandle = *sourceHandlePtr;

	error = iController.AddDataSource(KTestDataSourceUid, KInitData, sourceHandle);
	if (error)
		{
		ERR_PRINTF2(_L("AddDataSource failed, error %d"), error);
		delete sourceHandlePtr;
		return iTestStepResult = EInconclusive;
		}

	// Get log info - we expect "AddDataSourceL Called" again
	error = iController.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller CustomCommandSync failed with error %d"), error);
		delete sourceHandlePtr;
		return iTestStepResult = EInconclusive;
		}
		
	memFunctionText16.SetMax();
	memFunctionText16.Fill(0x0);
	memFunctionText16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Log : %S"), &memFunctionText16);

	if (memFunctionText != KExpectedResult)
		{
		ERR_PRINTF1(_L("Return value did not match expected"));
		delete sourceHandlePtr;
		return iTestStepResult = EFail;
		}

	INFO_PRINTF2(_L("Data source handle is %d"), sourceHandle.DestinationHandle());

	// call a custom source command. doesn't matter what it is, we'd expect it to return KErrNotSupported
	// anyway - the point is to ensure CMMFDataSourceHolder::HandleRequest() is called.
	// a return of 0 or -5 indicates it has been.
	error = iController.CustomCommandSync(sourceHandle, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
	INFO_PRINTF2(_L("CustomCommandSync on sourceHandle returned %d"), error);
	if (error && error != KErrNotSupported)
		{
		ERR_PRINTF1(_L("Unexpected error code returned"));
		delete sourceHandlePtr;
		return iTestStepResult = EFail;
		}
	
	// cleanup 
	delete sourceHandlePtr;
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0112
//
// Remove a data source
//
// REQ172.5.5.2

RTestMmfCtlfrmI0112* RTestMmfCtlfrmI0112::NewL()
	{
	RTestMmfCtlfrmI0112* self = new(ELeave) RTestMmfCtlfrmI0112;
	return self;
	}

RTestMmfCtlfrmI0112::RTestMmfCtlfrmI0112()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0112");
	}

TVerdict RTestMmfCtlfrmI0112::DoTestStepL()
	{
	INFO_PRINTF1(_L("remove a data source"));

	TInt error = KErrNone;

	_LIT8(KInitData,"TEST");

	// add a data source with a handle
	TMMFMessageDestination* sourceHandlePtr = new (ELeave) TMMFMessageDestination();
	TMMFMessageDestination& sourceHandle = *sourceHandlePtr;

	error = iController.AddDataSource(KTestDataSourceUid, KInitData, sourceHandle);
	if (error)
		{
		ERR_PRINTF2(_L("AddDataSource failed, error %d"), error);
		delete sourceHandlePtr;
		return iTestStepResult = EInconclusive;
		}

	// Get log info - we expect "AddDataSourceL Called"
	_LIT8(KExpectedResult, "AddDataSourceL Called");
	TBuf8<KTextBufLen> memFunctionText;

	TUid uid = {KTSIMmfControllerUid};
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg messageDest(handleInfo);

	error = iController.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller CustomCommandSync failed with error %d"), error);
		delete sourceHandlePtr;
		return iTestStepResult = EInconclusive;
		}
		
	if (memFunctionText != KExpectedResult)
		{
		ERR_PRINTF1(_L("Return value did not match expected"));
		delete sourceHandlePtr;
		return iTestStepResult = EInconclusive;
		}

	// now, delete the data source we just added
	error = iController.RemoveDataSource(sourceHandle);
	if (error)
		{
		ERR_PRINTF2(_L("RemoveDataSource failed with error %d"), error);
		delete sourceHandlePtr;
		return iTestStepResult = EInconclusive;
		}
	
	// verify that the data source is no longer there. try and remove it again, this should
	// return an error
	error = iController.RemoveDataSource(sourceHandle);
	if (!error)
		{
		ERR_PRINTF1(_L("Error : data source was not removed at first attempt"));
		delete sourceHandlePtr;
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF2(_L("Data source removed (second attempt failed with error %d)"), error);

	// cleanup 
	delete sourceHandlePtr;
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0113
//
// Add a data sink
//
// REQ172.5.5.3

RTestMmfCtlfrmI0113* RTestMmfCtlfrmI0113::NewL()
	{
	RTestMmfCtlfrmI0113* self = new(ELeave) RTestMmfCtlfrmI0113;
	return self;
	}

RTestMmfCtlfrmI0113::RTestMmfCtlfrmI0113()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0113");
	}

TVerdict RTestMmfCtlfrmI0113::DoTestStepL()
	{
	INFO_PRINTF1(_L("(a) add a data sink"));

	TInt error = KErrNone;

	_LIT8(KInitData,"TEST");

	// first : add a data sink without a handle
	error = iController.AddDataSink(KTestDataSinkUid, KInitData);
	if (error)
		{
		ERR_PRINTF2(_L("AddDataSink failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// Get log info - we expect "AddDataSinkL Called"
	_LIT8(KExpectedResult, "AddDataSinkL Called");
	TBuf8<KTextBufLen> memFunctionText;

	TUid uid = {KTSIMmfControllerUid};
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg messageDest(handleInfo);

	error = iController.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller CustomCommandSync failed with error %d"), error);
		return iTestStepResult = EInconclusive;
		}
		
	TBuf<KTextBufLen> memFunctionText16;
	memFunctionText16.SetMax();
	memFunctionText16.Fill(0x0);
	memFunctionText16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Log : %S"), &memFunctionText16);

	if (memFunctionText != KExpectedResult)
		{
		ERR_PRINTF1(_L("Return value did not match expected"));
		return iTestStepResult = EFail;
		}

	// second : add a data sink with a handle
	INFO_PRINTF1(_L("(b) add a data sink with handle"));
	TMMFMessageDestination* sinkHandlePtr = new (ELeave) TMMFMessageDestination();
	TMMFMessageDestination& sinkHandle = *sinkHandlePtr;

	error = iController.AddDataSink(KTestDataSinkUid, KInitData, sinkHandle);
	if (error)
		{
		ERR_PRINTF2(_L("AddDataSink failed, error %d"), error);
		delete sinkHandlePtr;
		return iTestStepResult = EInconclusive;
		}

	// Get log info - we expect "AddDataSinkL Called" again
	error = iController.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller CustomCommandSync failed with error %d"), error);
		delete sinkHandlePtr;
		return iTestStepResult = EInconclusive;
		}
		
	memFunctionText16.SetMax();
	memFunctionText16.Fill(0x0);
	memFunctionText16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Log : %S"), &memFunctionText16);

	if (memFunctionText != KExpectedResult)
		{
		ERR_PRINTF1(_L("Return value did not match expected"));
		delete sinkHandlePtr;
		return iTestStepResult = EFail;
		}

	INFO_PRINTF2(_L("Data sink handle is %d"), sinkHandle.DestinationHandle());

	// call a custom sink command. doesn't matter what it is, we'd expect it to return KErrNotSupported
	// anyway - the point is to ensure CMMFDataSinkHolder::HandleRequest() is called.
	// a return of 0 or -5 indicates it has been.
	error = iController.CustomCommandSync(sinkHandle, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
	INFO_PRINTF2(_L("CustomCommandSync on sinkHandle returned %d"), error);
	if (error && error != KErrNotSupported)
		{
		ERR_PRINTF1(_L("Unexpected error code returned"));
		delete sinkHandlePtr;
		return iTestStepResult = EFail;
		}
	
	// cleanup 
	delete sinkHandlePtr;
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0114
//
// Remove a data sink
//
// REQ172.5.5.4

RTestMmfCtlfrmI0114* RTestMmfCtlfrmI0114::NewL()
	{
	RTestMmfCtlfrmI0114* self = new(ELeave) RTestMmfCtlfrmI0114;
	return self;
	}

RTestMmfCtlfrmI0114::RTestMmfCtlfrmI0114()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0114");
	}

TVerdict RTestMmfCtlfrmI0114::DoTestStepL()
	{
	INFO_PRINTF1(_L("remove a data sink"));

	TInt error = KErrNone;

	_LIT8(KInitData,"TEST");

	// add a data sink with a handle
	TMMFMessageDestination* sinkHandlePtr = new (ELeave) TMMFMessageDestination();
	TMMFMessageDestination& sinkHandle = *sinkHandlePtr;

	error = iController.AddDataSink(KTestDataSinkUid, KInitData, sinkHandle);
	if (error)
		{
		ERR_PRINTF2(_L("AddDataSink failed, error %d"), error);
		delete sinkHandlePtr;
		return iTestStepResult = EInconclusive;
		}

	// Get log info - we expect "AddDataSinkL Called"
	_LIT8(KExpectedResult, "AddDataSinkL Called");
	TBuf8<KTextBufLen> memFunctionText;

	TUid uid = {KTSIMmfControllerUid};
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg messageDest(handleInfo);

	error = iController.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("controller CustomCommandSync failed with error %d"), error);
		delete sinkHandlePtr;
		return iTestStepResult = EInconclusive;
		}
		
	if (memFunctionText != KExpectedResult)
		{
		ERR_PRINTF1(_L("Return value did not match expected"));
		delete sinkHandlePtr;
		return iTestStepResult = EInconclusive;
		}

	// now, delete the data sink we just added
	error = iController.RemoveDataSink(sinkHandle);
	if (error)
		{
		ERR_PRINTF2(_L("RemoveDataSink failed with error %d"), error);
		delete sinkHandlePtr;
		return iTestStepResult = EInconclusive;
		}
	
	// verify that the data sink is no longer there. try and remove it again, this should
	// return an error
	error = iController.RemoveDataSink(sinkHandle);
	if (!error)
		{
		ERR_PRINTF1(_L("Error : data sink was not removed at first attempt"));
		delete sinkHandlePtr;
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF2(_L("Data sink removed (second attempt failed with error %d)"), error);

	// cleanup 
	delete sinkHandlePtr;
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0117
//
// Set / get source config parameters
//
// REQ: none

RTestMmfCtlfrmI0117* RTestMmfCtlfrmI0117::NewL()
	{
	RTestMmfCtlfrmI0117* self = new(ELeave) RTestMmfCtlfrmI0117;
	return self;
	}

RTestMmfCtlfrmI0117::RTestMmfCtlfrmI0117()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0117");
	}

TVerdict RTestMmfCtlfrmI0117::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("Set / get source config parameters"));

	TInt error = KErrNone;
	
	RMMFAudioControllerCustomCommands customCommands(iController);

	RArray<TUint> supportedSampleRates;
	TRAP(error, customCommands.GetSupportedSourceSampleRatesL(supportedSampleRates));
	if (error)
		{
		ERR_PRINTF2(_L("GetSupportedSourceSampleRatesL left, error %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF2(_L("GetSupportedSourceSampleRatesL returned %d entries"), supportedSampleRates.Count());

	RArray<TUint> supportedBitRates;
	// we expect this to be Not Supported
	TRAP(error, customCommands.GetSupportedSourceBitRatesL(supportedBitRates));
	if (error)
		{
		ERR_PRINTF2(_L("GetSupportedSourceBitRatesL left, error %d"), error);
		if(error != KErrNotSupported)
			return iTestStepResult = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("GetSupportedSourceBitRatesL returned %d entries"), supportedBitRates.Count());
		}

	RArray<TUint> supportedNumChannels;
	TRAP(error, customCommands.GetSupportedSourceNumChannelsL(supportedNumChannels));
	if (error)
		{
		ERR_PRINTF2(_L("GetSupportedSourceNumChannelsL left, error %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF2(_L("GetSupportedSourceNumChannelsL returned %d entries"), supportedNumChannels.Count());

	TUint bitRate = 1000; // 1 Khz
	error = customCommands.SetSourceBitRate(bitRate);
	INFO_PRINTF2(_L("SetSourceBitRate returned %d"), error);
	if (error && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("SetSourceBitRate failed, error %d"), error);
		return iTestStepResult = EFail;
		}

	TUint sampleRate = 5;
	error = customCommands.SetSourceSampleRate(sampleRate);
	INFO_PRINTF2(_L("SetSourceSampleRate returned %d"), error);
	// Setting source sample rate to a different value 
	// is tolerated (INC038043) to preserve BC with 6.1
	if (error != KErrNone)
		{
		ERR_PRINTF3(_L("SetSourceSampleRate failed, error %d, expected %d"), error, KErrAlreadyExists);
		return iTestStepResult = EFail;
		}
	
	TUint numChannels = 1;
	error = customCommands.SetSourceNumChannels(numChannels);
	INFO_PRINTF2(_L("SetSourceNumChannels returned %d"), error);
	// Setting number of channels to a different value 
	// is tolerated (INC038043) to preserve BC with 6.1
	if (error && error != KErrNone)
		{
		ERR_PRINTF2(_L("SetSourceNumChannels failed, error %d"), error);
		return iTestStepResult = EFail;
		}

	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0118
//
// Set / get sink config parameters
//
// REQ: none

RTestMmfCtlfrmI0118* RTestMmfCtlfrmI0118::NewL()
	{
	RTestMmfCtlfrmI0118* self = new(ELeave) RTestMmfCtlfrmI0118;
	return self;
	}

RTestMmfCtlfrmI0118::RTestMmfCtlfrmI0118()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0118");
	}

TVerdict RTestMmfCtlfrmI0118::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("Set / get sink config parameters"));

	TInt error = KErrNone;
	
	RMMFAudioControllerCustomCommands customCommands(iController);

	RArray<TUint> supportedSampleRates;
	TRAP(error, customCommands.GetSupportedSinkSampleRatesL(supportedSampleRates));
	if (error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("GetSupportedSinkSampleRatesL left with error %d but expected is KErrNotSupported"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF2(_L("GetSupportedSinkSampleRatesL returned %d entries"), supportedSampleRates.Count());

	RArray<TUint> supportedBitRates;
	// we expect this to be Not Supported
	TRAP(error, customCommands.GetSupportedSinkBitRatesL(supportedBitRates));
	if (error)
		{
		ERR_PRINTF2(_L("GetSupportedSinkBitRatesL left, error %d"), error);
		if(error != KErrNotSupported)
			return iTestStepResult = EFail;
		}
	else
		{
		INFO_PRINTF2(_L("GetSupportedSinkBitRatesL returned %d entries"), supportedBitRates.Count());
		}

	RArray<TUint> supportedNumChannels;
	TRAP(error, customCommands.GetSupportedSinkNumChannelsL(supportedNumChannels));
	if (error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("GetSupportedSinkNumChannelsL left with error %d but expected is KErrNotSupported"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF2(_L("GetSupportedSinkNumChannelsL returned %d entries"), supportedNumChannels.Count());

	TUint bitRate = 1000; // 1 Khz
	error = customCommands.SetSinkBitRate(bitRate);
	INFO_PRINTF2(_L("SetSinkBitRate returned %d"), error);
	if (error && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("SetSinkBitRate failed, error %d"), error);
		return iTestStepResult = EFail;
		}

	TUint sampleRate = 5;
	error = customCommands.SetSinkSampleRate(sampleRate);
	INFO_PRINTF2(_L("SetSinkSampleRate returned %d"), error);
	if (error && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("SetSinkSampleRate failed, error %d"), error);
		return iTestStepResult = EFail;
		}
	
	TUint numChannels = 1;
	error = customCommands.SetSinkNumChannels(numChannels);
	INFO_PRINTF2(_L("SetSinkNumChannels returned %d"), error);
	if (error && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("SetSinkNumChannels failed, error %d"), error);
		return iTestStepResult = EFail;
		}

	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0119
//
// Get supported source/sink video types
//
// REQ: none

RTestMmfCtlfrmI0119* RTestMmfCtlfrmI0119::NewL()
	{
	RTestMmfCtlfrmI0119* self = new(ELeave) RTestMmfCtlfrmI0119;
	return self;
	}

RTestMmfCtlfrmI0119::RTestMmfCtlfrmI0119()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0119");
	}

TVerdict RTestMmfCtlfrmI0119::DoTestStepL( void )
	{
	// NB this is implemented purely for code coverage purposes
	INFO_PRINTF1(_L("Get supported source/sink audio/video types"));

	// GetSupportedSinkAudioTypes no longer in base class
	INFO_PRINTF1(_L("GetSupportedSinkAudioTypes no longer in video controller base class"));

	// GetSupportedSinkVideoTypes no longer in base class
	INFO_PRINTF1(_L("GetSupportedSinkVideoTypes no longer in video controller base class"));

	// GetSupportedSourceAudioTypes no longer supported
	INFO_PRINTF1(_L("GetSupportedSourceAudioTypes no longer supported"));

	// GetSupportedSourceVideoTypes no longer supported
	INFO_PRINTF1(_L("GetSupportedSourceVideoTypes no longer supported"));

	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0120
//
// Call display window related methods for video play controller
//
// REQ: none

RTestMmfCtlfrmI0120* RTestMmfCtlfrmI0120::NewL()
	{
	RTestMmfCtlfrmI0120* self = new(ELeave) RTestMmfCtlfrmI0120;
	return self;
	}

RTestMmfCtlfrmI0120::RTestMmfCtlfrmI0120()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0120");
	}

TVerdict RTestMmfCtlfrmI0120::DoTestStepL( void )
	{
	// NB this is implemented purely for code coverage purposes
	INFO_PRINTF1(_L("Call display window related methods for video play controller"));

	TInt error = KErrNone;
	
	RMMFVideoPlayControllerCustomCommands playCustomCommands(iController);

	TRect r(0,0,0,0);
	error = playCustomCommands.SetDisplayWindow(r, r);
	INFO_PRINTF2(_L("SetDisplayWindow returned %d"), error);
	if (error)
		{
		if(error != KErrNotSupported)
			return iTestStepResult = EFail;
		}

	RRegion rg;
	error = playCustomCommands.UpdateDisplayRegion(rg);
	INFO_PRINTF2(_L("UpdateDisplayRegion returned %d"), error);
	if (error)
		{
		if(error != KErrNotSupported)
			return iTestStepResult = EFail;
		}

	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0121
//
// Prime (prepare data flow)
//
// REQ172.5.5.5

RTestMmfCtlfrmI0121* RTestMmfCtlfrmI0121::NewL()
	{
	RTestMmfCtlfrmI0121* self = new(ELeave) RTestMmfCtlfrmI0121;
	return self;
	}

RTestMmfCtlfrmI0121::RTestMmfCtlfrmI0121()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0121");
	}

TVerdict RTestMmfCtlfrmI0121::DoTestStepL()
	{
	INFO_PRINTF1(_L("Prime a controller"));

	TInt error = KErrNone;
	TVerdict result = EPass;
	
	error = iController.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("Prime failed, error %d"), error);
		return iTestStepResult = EFail;
		}
	
	// [ wait for and process the return event from the Custom audio controller ]
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Custom audio controller did not return a prime event"));
		return iTestStepResult = EFail;
		}
	
	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0122
//
// Play
//
// REQ172.5.5.6

RTestMmfCtlfrmI0122* RTestMmfCtlfrmI0122::NewL()
	{
	RTestMmfCtlfrmI0122* self = new(ELeave) RTestMmfCtlfrmI0122;
	return self;
	}

RTestMmfCtlfrmI0122::RTestMmfCtlfrmI0122()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0122");
	}

TVerdict RTestMmfCtlfrmI0122::DoTestStepL()
	{
	// Play a controller. Must be primed first
	INFO_PRINTF1(_L("Play a controller"));

	TInt error = KErrNone;
	TVerdict result = EPass;
	
	error = iController.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("Prime failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	
	// [ wait for and process the return event from the Custom audio controller ]
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Custom audio controller did not return a prime event"));
		return iTestStepResult = EInconclusive;
		}
	
    error = iController.Play();
	if(error)
		{
		ERR_PRINTF2(_L("Play failed, error %d"), error);
		return iTestStepResult = EFail;
		}

	// wait for an event from the Custom audio controller
	TMMFEvent playEvent( KPlayTestId, KErrNone);
	result = SearchForEvent( playEvent );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Custom audio controller did not return a play event"));
		return iTestStepResult = EFail;
		}
	
	// verify that clip is playing : get the position a couple of times
	// NB the clip must be at least 1 second long for this to work

	TTimeIntervalMicroSeconds clipPos1, clipPos2, clipPos3;
	User::After(KGetPositionIntervalLength);
	error = iController.GetPosition(clipPos1);
	User::After(KGetPositionIntervalLength);
	error += iController.GetPosition(clipPos2);
	User::After(KGetPositionIntervalLength);
	error += iController.GetPosition(clipPos3);

	if(error)
		{
		// NB it's cumulative so don't give return value
		ERR_PRINTF1(_L("Error : GetPosition failed"));	
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF4(_L("Clip positions : %ld %ld %ld"), I64LOW(clipPos1.Int64()), I64LOW(clipPos2.Int64()), I64LOW(clipPos3.Int64()));

	if((clipPos1 == clipPos2) && (clipPos2 == clipPos3))
		{
		ERR_PRINTF1(_L("Error : clip position not changing"));
		return iTestStepResult = EFail;
		}

	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0123
//
// Stop
//
// REQ172.5.5.7

RTestMmfCtlfrmI0123* RTestMmfCtlfrmI0123::NewL()
	{
	RTestMmfCtlfrmI0123* self = new(ELeave) RTestMmfCtlfrmI0123;
	return self;
	}

RTestMmfCtlfrmI0123::RTestMmfCtlfrmI0123()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0123");
	}

TVerdict RTestMmfCtlfrmI0123::DoTestStepL()
	{
	// Stop a controller. Must be playing first
	INFO_PRINTF1(_L("Stop a controller"));

	TInt error = KErrNone;
	TVerdict result = EPass;
	
	error = iController.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("Prime failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	
	// [ wait for and process the return event from the Custom audio controller ]
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Custom audio controller did not return a prime event"));
		return iTestStepResult = EInconclusive;
		}
	
    error = iController.Play();
	if(error)
		{
		ERR_PRINTF2(_L("Play failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// get an event from the controller
	TMMFEvent playEvent( KPlayTestId, KErrNone);
	result = SearchForEvent( playEvent );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Custom audio controller did not return a play event"));
		return iTestStepResult = EFail;
		}
	
	// verify that clip is playing : get the position a couple of times
	// NB the clip must be at least 1.5 seconds long for this to work

	TTimeIntervalMicroSeconds clipPos1, clipPos2, clipPos3;
	User::After(KGetPositionIntervalLength);
	error = iController.GetPosition(clipPos1);
	User::After(KGetPositionIntervalLength);
	error += iController.GetPosition(clipPos2);
	User::After(KGetPositionIntervalLength);
	error += iController.GetPosition(clipPos3);

	if(error)
		{
		// NB it's cumulative so don't give return value
		ERR_PRINTF1(_L("Error : GetPosition failed"));	
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF4(_L("Clip positions : %ld %ld %ld"), I64LOW(clipPos1.Int64()), I64LOW(clipPos2.Int64()), I64LOW(clipPos3.Int64()));

	if((clipPos1 == clipPos2) && (clipPos2 == clipPos3))
		{
		ERR_PRINTF1(_L("Error : clip position not changing"));
		return iTestStepResult = EInconclusive;
		}

	// now stop the clip. verify that the position is 0 and not changing.
	error = iController.Stop();
	if(error)
		{
		ERR_PRINTF2(_L("Stop failed, error %d"), error);
		return iTestStepResult = EFail;
		}
		
 	// verify that clip is stopped : get the position a couple of times
	// NB we have to re-prime it first for this to work. If it is still playing,
	// the prime will fail
	error = iController.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("Prime failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	
	User::After(KGetPositionIntervalLength);
	error = iController.GetPosition(clipPos1);
	User::After(KGetPositionIntervalLength);
	error += iController.GetPosition(clipPos2);
	User::After(KGetPositionIntervalLength);
	error += iController.GetPosition(clipPos3);

	if(error)
		{
		// NB it's cumulative so don't give return value
		ERR_PRINTF1(_L("Error : GetPosition failed"));	
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF4(_L("Clip positions : %ld %ld %ld"), I64LOW(clipPos1.Int64()), I64LOW(clipPos2.Int64()), I64LOW(clipPos3.Int64()));

	if((clipPos1 != clipPos2) && (clipPos2 != clipPos3))
		{
		ERR_PRINTF1(_L("Error : clip position still changing"));
		return iTestStepResult = EFail;
		}

	if(clipPos3.Int64() != 0)
		{
		ERR_PRINTF1(_L("Error : clip position not at start"));
		return iTestStepResult = EFail;
		}

	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0124
//
// Pause
//
// REQ172.5.5.8

RTestMmfCtlfrmI0124* RTestMmfCtlfrmI0124::NewL()
	{
	RTestMmfCtlfrmI0124* self = new(ELeave) RTestMmfCtlfrmI0124;
	return self;
	}

RTestMmfCtlfrmI0124::RTestMmfCtlfrmI0124()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0124");
	}

TVerdict RTestMmfCtlfrmI0124::DoTestStepL()
	{
	// Pause a controller. Must be playing first
	INFO_PRINTF1(_L("Pause a controller"));

	TInt error = KErrNone;
	TVerdict result = EPass;
	
	error = iController.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("Prime failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	
	// [ wait for and process the return event from the Custom audio controller ]
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Custom audio controller did not return a prime event"));
		return iTestStepResult = EInconclusive;
		}
	
    error = iController.Play();
	if(error)
		{
		ERR_PRINTF2(_L("Play failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// get an event from the Custom audio controller
	TMMFEvent playEvent( KPlayTestId, KErrNone);
	result = SearchForEvent( playEvent );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Custom audio controller did not return a play event"));
		return iTestStepResult = EFail;
		}
	
	// verify that clip is playing : get the position a couple of times
	// NB the clip must be at least 1 second long for this to work

	TTimeIntervalMicroSeconds clipPos1, clipPos2, clipPos3;
	User::After(KGetPositionIntervalLength);
	error = iController.GetPosition(clipPos1);
	User::After(KGetPositionIntervalLength);
	error += iController.GetPosition(clipPos2);
	User::After(KGetPositionIntervalLength);
	error += iController.GetPosition(clipPos3);

	if(error)
		{
		// NB it's cumulative so don't give return value
		ERR_PRINTF1(_L("Error : GetPosition failed"));	
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF4(_L("Clip positions : %ld %ld %ld"), I64LOW(clipPos1.Int64()), I64LOW(clipPos2.Int64()), I64LOW(clipPos3.Int64()));

	if((clipPos1 == clipPos2) || (clipPos2 == clipPos3))
		{
		ERR_PRINTF1(_L("Error : clip position not changing"));
		return iTestStepResult = EInconclusive;
		}

	// now pause the clip. verify that the position is not 0 and not changing.
	error = iController.Pause();
	if(error)
		{
		ERR_PRINTF2(_L("Pause failed, error %d"), error);
		return iTestStepResult = EFail;
		}
	
	// [ wait for and process the return event from the Custom audio controller ]
	TMMFEvent pauseEvent( KPauseTestId, KErrNone);
	result = SearchForEvent( pauseEvent );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Custom audio controller did not return a pause event"));
		return iTestStepResult = EFail;
		}
	
 	// verify that clip is paused : get the position a couple of times
	User::After(KGetPositionIntervalLength);
	error = iController.GetPosition(clipPos1);
	User::After(KGetPositionIntervalLength);
	error += iController.GetPosition(clipPos2);
	User::After(KGetPositionIntervalLength);
	error += iController.GetPosition(clipPos3);

	if(error)
		{
		// NB it's cumulative so don't give return value
		ERR_PRINTF1(_L("Error : GetPosition failed"));	
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF4(_L("Clip positions : %ld %ld %ld"), I64LOW(clipPos1.Int64()), I64LOW(clipPos2.Int64()), I64LOW(clipPos3.Int64()));

	if((clipPos1 != clipPos2) || (clipPos2 != clipPos3))
		{
		ERR_PRINTF1(_L("Error : clip position still changing"));
		return iTestStepResult = EFail;
		}

	if(clipPos2.Int64() == 0)
		{
		ERR_PRINTF1(_L("Error : clip position is zero"));
		return iTestStepResult = EFail;
		}

	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0125
//
// Assign priority to controller
//
// REQ172.5.5.9

RTestMmfCtlfrmI0125* RTestMmfCtlfrmI0125::NewL()
	{
	RTestMmfCtlfrmI0125* self = new(ELeave) RTestMmfCtlfrmI0125;
	return self;
	}

RTestMmfCtlfrmI0125::RTestMmfCtlfrmI0125()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0125");
	}

TVerdict RTestMmfCtlfrmI0125::DoTestStepL()
	{
	INFO_PRINTF1(_L("Assign priority to controller"));
	TInt error = KErrNone;

	// set priorities - 1 higher than 2
	iSettings.iPriority = 10;
	error = iController1.SetPrioritySettings(iSettings);
	if(error)
		{
		ERR_PRINTF2(_L("controller1 SetPrioritySettings failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	iSettings.iPriority = -10;
	error = iController2.SetPrioritySettings(iSettings);
	if(error)
		{
		ERR_PRINTF2(_L("controller2 SetPrioritySettings failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// prime them
	error = iController1.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("controller1 prime failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	error = iController2.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("controller2 prime failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// at last we are ready to do the test...
	// play both controllers. give the first time to start before playing the second.
	// we should get an error.
	error = iController1.Play();
	if(error)
		{
		ERR_PRINTF2(_L("iController1 play failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	// wait a while, to give it chance to play
	const TTimeIntervalMicroSeconds32 KPlayPosition32 = 500000L;
	INFO_PRINTF2(_L("Waiting %d mcs..."), KPlayPosition32.Int());
	User::After(KPlayPosition32);
	error = iController2.Play();
	if(error)
		{
		ERR_PRINTF2(_L("iController2 play failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// search for a KErrInUse from anywhere on controller 2
	// this is from Audio Policy
	TMMFEvent inUseEvent( KMMFEventCategoryPlaybackComplete, KErrInUse );
	TVerdict result;
	result = SearchForEvent( inUseEvent, 2 );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Audio Policy did not return a KErrInUse event"));
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Audio Policy returned a KErrInUse event"));

	// check that the position on controller1 is actually changing
	// and that the position on controller2 isn't

	// NB this may fail - if iController2 is actually stopped. in that case
	// we just have to check that GetPosition returns an error
	TTimeIntervalMicroSeconds clipPos1a, clipPos1b, clipPos1c;
	TTimeIntervalMicroSeconds clipPos2a, clipPos2b, clipPos2c;
	User::After(KGetPositionIntervalLength);
	error = iController1.GetPosition(clipPos1a);

	TInt ctrl2Error=KErrNone;
	ctrl2Error = iController2.GetPosition(clipPos2a);
	if(ctrl2Error)
		{
		ERR_PRINTF1(_L("Controller 2 GetPosition returned an Error"));
		return iTestStepResult = EFail;
		}
	
	User::After(KGetPositionIntervalLength);
	error += iController1.GetPosition(clipPos1b);
	ctrl2Error = iController2.GetPosition(clipPos2b);
	if(ctrl2Error)
		{
		ERR_PRINTF1(_L("Controller 2 GetPosition returned an Error"));
		return iTestStepResult = EFail;
		}

	User::After(KGetPositionIntervalLength);
	error += iController1.GetPosition(clipPos1c);
	ctrl2Error = iController2.GetPosition(clipPos2c);
	if(ctrl2Error)
		{
		ERR_PRINTF1(_L("Controller 2 GetPosition returned an Error"));
		return iTestStepResult = EFail;
		}
	if(error)
		{
		// NB it's cumulative so don't give return value
		ERR_PRINTF1(_L("Error : GetPosition failed"));	
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF4(_L("Clip positions 1 : %d %d %d"), I64LOW(clipPos1a.Int64()), I64LOW(clipPos1b.Int64()), I64LOW(clipPos1c.Int64()));
	INFO_PRINTF4(_L("Clip positions 2 : %d %d %d"), I64LOW(clipPos2a.Int64()), I64LOW(clipPos2b.Int64()), I64LOW(clipPos2c.Int64()));

	if((clipPos1a == clipPos1b) || (clipPos1b == clipPos1c))
		{
		ERR_PRINTF1(_L("Error : clip position 1 not changing"));
		return iTestStepResult = EFail;
		}
    if(clipPos2a != clipPos2b || clipPos2b != clipPos2c)
        {
        ERR_PRINTF1(_L("Controller 2 position changed"));
        return iTestStepResult = EFail;
        }
	// ok - now do the same the other way up - set controller 2 priority higher than 
	// controller 1
	iController1.Stop();
	iController2.Stop();


	iSettings.iPriority = -10;
	error = iController1.SetPrioritySettings(iSettings);
	if(error)
		{
		ERR_PRINTF2(_L("controller1 SetPrioritySettings failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	iSettings.iPriority = 10;
	error = iController2.SetPrioritySettings(iSettings);
	if(error)
		{
		ERR_PRINTF2(_L("controller2 SetPrioritySettings failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// prime them
	error = iController1.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("controller1 prime failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	error = iController2.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("controller2 prime failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// play both controllers. give the first time to start before playing the second.
	// we should get an error.
	error = iController1.Play();
	if(error)
		{
		ERR_PRINTF2(_L("iController1 play failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	// wait a while, to give it chance to play
	INFO_PRINTF2(_L("Waiting %d mcs..."), KPlayPosition32.Int());
	User::After(KPlayPosition32);
	error = iController2.Play();
	if(error)
		{
		ERR_PRINTF2(_L("iController2 play failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	// *** Test Changed - originally searched for a KErrAccessDenied
	// from anywhere on controller 1
	// BUT since controller 1 is already playing, this isn't appropriate
	// Instead we search for a KErrCancel event from controller 1;
	// this should be passed through the Custom Audio Controller and arrive
	// with uid KMMFEventCategoryPlaybackComplete


	// Search for the event caused by playing the higher priority controller
	inUseEvent.iEventType = KMMFEventCategoryPlaybackComplete;
	result = SearchForEvent( inUseEvent, 1 );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Audio Policy did not return a KErrInUse event"));
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Audio Policy returned a KErrInUse event"));


	// check that the position on controller2 is actually changing
	// and that the position on controller1 isn't
	User::After(KGetPositionIntervalLength);

	TInt ctrlError1=KErrNone;
	ctrlError1 = iController1.GetPosition(clipPos1a);
	if(ctrlError1)
		{
		ERR_PRINTF1(_L("Controller 1 GetPosition returned an Error"));
		return iTestStepResult = EFail;
		}

	error = iController2.GetPosition(clipPos2a);
	User::After(KGetPositionIntervalLength);
	ctrlError1 = iController1.GetPosition(clipPos1b);
	if(ctrlError1)
		{
		ERR_PRINTF1(_L("Controller 1 GetPosition returned an Error"));
		return iTestStepResult = EFail;
		}

	error += iController2.GetPosition(clipPos2b);
	User::After(KGetPositionIntervalLength);
	ctrlError1 = iController1.GetPosition(clipPos1c);
	if(ctrlError1)
		{
		ERR_PRINTF1(_L("Controller 1 GetPosition returned an Error"));
		return iTestStepResult = EFail;
		}

	error += iController2.GetPosition(clipPos2c);

	if(error)
		{
		// NB it's cumulative so don't give return value
		ERR_PRINTF1(_L("Error : GetPosition failed"));	
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF4(_L("Clip positions 1 : %d %d %d"), I64LOW(clipPos1a.Int64()), I64LOW(clipPos1b.Int64()), I64LOW(clipPos1c.Int64()));
	INFO_PRINTF4(_L("Clip positions 2 : %d %d %d"), I64LOW(clipPos2a.Int64()), I64LOW(clipPos2b.Int64()), I64LOW(clipPos2c.Int64()));

	if((clipPos2a == clipPos2b) || (clipPos2b == clipPos2c))
		{
		ERR_PRINTF1(_L("Error : clip position 2 not changing"));
		return iTestStepResult = EFail;
		}
    if(clipPos1a != clipPos1b || clipPos1b != clipPos1c)
        {
        ERR_PRINTF1(_L("Controller 1 position changed"));
        return iTestStepResult = EFail;
        }
	return iTestStepResult = EPass;
	}


// ---------------------------
// RTestMmfCtlfrmI0128
//
// Get position
//
// REQ172.5.5.10

RTestMmfCtlfrmI0128* RTestMmfCtlfrmI0128::NewL()
	{
	RTestMmfCtlfrmI0128* self = new(ELeave) RTestMmfCtlfrmI0128;
	return self;
	}

RTestMmfCtlfrmI0128::RTestMmfCtlfrmI0128()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0128");
	}

TVerdict RTestMmfCtlfrmI0128::DoTestStepL()
	{
 	// Get position
	INFO_PRINTF1(_L("Get Position"));

	TInt error = KErrNone;
	TVerdict result = EPass;
	
	// 1. before playing, verify that GetPosition returns start
	// 2. set to known position, verify that GetPosition returns correct position
	// 3. set to start, play for N seconds, check position is approx as expected

	error = iController.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("Prime failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	
	// [ wait for and process the return event from the Custom audio controller ]
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Custom audio controller did not return a prime event"));
		return iTestStepResult = EInconclusive;
		}
	
 	// 1. verify that clip is at position 0
	TTimeIntervalMicroSeconds clipPos;
	error = iController.GetPosition(clipPos);

	if(error)
		{
		ERR_PRINTF2(_L("Error : GetPosition failed, error %d"), error);	
		return iTestStepResult = EFail;
		}

	INFO_PRINTF2(_L("Clip position : %ld"), I64LOW(clipPos.Int64()));
	if(clipPos.Int64() != 0)
		{
		ERR_PRINTF1(_L("Error : clip position not at start"));
		return iTestStepResult = EFail;
		}

 	// 2. set position and verify that clip is there
	// NB allow for resolution of the controller itself
	const TTimeIntervalMicroSeconds KPlayPosition(2000000);
	const TTimeIntervalMicroSeconds KPlayPositionPlusError(3000000);
	const TTimeIntervalMicroSeconds KPlayPositionMinusError(1000000); // allow +/1 sec
	const TTimeIntervalMicroSeconds32 KPlayPosition32 = 2000000L; // for User::After
	error = iController.SetPosition(KPlayPosition);
	if(error)
		{
		ERR_PRINTF2(_L("SetPosition failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	error = iController.GetPosition(clipPos);
	if(error)
		{
		ERR_PRINTF2(_L("Error : GetPosition failed, error %d"), error);	
		return iTestStepResult = EFail;
		}

	INFO_PRINTF3(_L("Clip position : %ld Expected : %ld"), I64LOW(clipPos.Int64()), I64LOW(KPlayPosition.Int64()));
	if( (clipPos < (KPlayPositionMinusError)) || (clipPos > (KPlayPositionPlusError)) )
		{
		ERR_PRINTF1(_L("Error : clip position not set correctly"));
		return iTestStepResult = EFail;
		}

	// 3. set position to start, play clip for a time span, and verify that its position is approx. correct
	const TTimeIntervalMicroSeconds KStartPosition(0);
    error = iController.SetPosition(KStartPosition);
	if(error)
		{
		ERR_PRINTF2(_L("SetPosition failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}

	error = iController.GetPosition(clipPos);
	if(error)
		{
		ERR_PRINTF2(_L("Error : GetPosition failed, error %d"), error);	
		return iTestStepResult = EFail;
		}

	INFO_PRINTF2(_L("Clip position : %ld"), I64LOW(clipPos.Int64()));
	if(clipPos != KStartPosition)
		{
		ERR_PRINTF1(_L("Error : clip position not at start"));
		return iTestStepResult = EFail;
		}

	error = iController.Play();
	if(error)
		{
		ERR_PRINTF2(_L("Play failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	TMMFEvent playEvent( KPlayTestId, KErrNone);
	result = SearchForEvent( playEvent );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Custom audio controller did not return a play event"));
		return iTestStepResult = EInconclusive;
		}
	
	TTimeIntervalMicroSeconds clipPosStart;
	error = iController.GetPosition(clipPosStart);
	if(error)
		{
		ERR_PRINTF2(_L("Error : GetPosition failed, error %d"), error);	
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF2(_L("Clip position : %ld"), I64LOW(clipPosStart.Int64()));

	// wait, then get pos again
	User::After(KPlayPosition32);

	error = iController.GetPosition(clipPos);
	if(error)
		{
		ERR_PRINTF2(_L("Error : GetPosition failed, error %d"), error);	
		return iTestStepResult = EFail;
		}

	TTimeIntervalMicroSeconds clipPosPlayedFor = clipPos;
	clipPosPlayedFor = clipPosPlayedFor.Int64() - clipPosStart.Int64();

	TTimeIntervalMicroSeconds clipPosExpected = KPlayPosition;
	clipPosExpected = clipPosExpected.Int64() + clipPosStart.Int64();

	INFO_PRINTF3(_L("Clip position : %ld Expected : %ld"), I64LOW(clipPos.Int64()), I64LOW(clipPosExpected.Int64()));
	if( (clipPosPlayedFor < (KPlayPositionMinusError)) || (clipPosPlayedFor > (KPlayPositionPlusError)) )
		{
		ERR_PRINTF1(_L("Error : clip position not correct"));
		return iTestStepResult = EFail;
		}

	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0129
//
// Set position
//
// REQ172.5.5.11

RTestMmfCtlfrmI0129* RTestMmfCtlfrmI0129::NewL()
	{
	RTestMmfCtlfrmI0129* self = new(ELeave) RTestMmfCtlfrmI0129;
	return self;
	}

RTestMmfCtlfrmI0129::RTestMmfCtlfrmI0129()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0129");
	}

TVerdict RTestMmfCtlfrmI0129::DoTestStepL()
	{
	// Set position
	INFO_PRINTF1(_L("Set Position"));

	TInt error = KErrNone;
	TVerdict result = EPass;
	
	// set to known position, verify that GetPosition returns correct position

	error = iController.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("Prime failed, error %d"), error);
		return iTestStepResult = EInconclusive;
		}
	
	// [ wait for and process the return event from the Custom audio controller ]
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		ERR_PRINTF1(_L("Custom audio controller did not return a prime event"));
		return iTestStepResult = EInconclusive;
		}
	
 	// 2. set position and verify that clip is there
	// NB allow for resolution of the controller itself
	const TTimeIntervalMicroSeconds KPlayPosition(2000000);
	const TTimeIntervalMicroSeconds KPlayPositionPlusError(2500000);
	const TTimeIntervalMicroSeconds KPlayPositionMinusError(1500000); // allow +/-0.5 sec
	error = iController.SetPosition(KPlayPosition);
	if(error)
		{
		ERR_PRINTF2(_L("SetPosition failed, error %d"), error);
		return iTestStepResult = EFail;
		}

	TTimeIntervalMicroSeconds clipPos;
	error = iController.GetPosition(clipPos);
	if(error)
		{
		ERR_PRINTF2(_L("Error : GetPosition failed, error %d"), error);	
		return iTestStepResult = EInconclusive;
		}

	INFO_PRINTF3(_L("Clip position : %ld Expected : %ld"), I64LOW(clipPos.Int64()), I64LOW(KPlayPosition.Int64()));
	if( (clipPos < (KPlayPositionMinusError)) || (clipPos > (KPlayPositionPlusError)) )
		{
		ERR_PRINTF1(_L("Error : clip position not set correctly"));
		return iTestStepResult = EFail;
		}
	   
	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0131
//
// Reset controller
//
// REQ172.5.5.12

RTestMmfCtlfrmI0131* RTestMmfCtlfrmI0131::NewL()
	{
	RTestMmfCtlfrmI0131* self = new(ELeave) RTestMmfCtlfrmI0131;
	return self;
	}

RTestMmfCtlfrmI0131::RTestMmfCtlfrmI0131()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0131");
	}

TVerdict RTestMmfCtlfrmI0131::DoTestStepL()
	{
	INFO_PRINTF1(_L("Reset"));

	TInt error = KErrNone;
	
	// reset the controller; verify that source and sink are no longer present, and that
	// it is not primed.

	error = iController.Reset();
	if(error)
		{
		ERR_PRINTF2(_L("Error : Reset failed, error %d"), error);	
		return iTestStepResult = EFail;
		}

	// try and play : we should get an error KErrNotReady
	error = iController.Play();
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : play returned unexpected code %d"), error);	
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Attempt to play controller failed with KErrNotReady"));	

	// try and remove data source : we should get an error KErrNotFound
	TMMFMessageDestination& sourceHandle = *iSourceHandlePtr;
	error = iController.RemoveDataSource(sourceHandle);
	if(error != KErrNotFound)
		{
		ERR_PRINTF2(_L("Error : remove data source returned unexpected code %d"), error);	
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Attempt to remove data source failed with KErrNotFound"));	

	// try and remove data sink : we should get an error KErrNotFound
	TMMFMessageDestination& sinkHandle = *iSinkHandlePtr;
	error = iController.RemoveDataSink(sinkHandle);
	if(error != KErrNotFound)
		{
		ERR_PRINTF2(_L("Error : remove data sink returned unexpected code %d"), error);	
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Attempt to remove data sink failed with KErrNotFound"));	

	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0132
//
// Close controller
//
// REQ172.5.5.12

RTestMmfCtlfrmI0132* RTestMmfCtlfrmI0132::NewL()
	{
	RTestMmfCtlfrmI0132* self = new(ELeave) RTestMmfCtlfrmI0132;
	return self;
	}

RTestMmfCtlfrmI0132::RTestMmfCtlfrmI0132()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0132");
	}

TVerdict RTestMmfCtlfrmI0132::DoTestStepL()
	{
	// Reset
	INFO_PRINTF1(_L("Close"));

	TInt error = KErrNone;
	
	// close the controller; verify that source and sink are no longer present, and that
	// it is not primed.

	iController.Close();
	// returns no error

	// try and prime : we should get an error KErrNotReady
	error = iController.Prime();
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : prime returned unexpected code %d"), error);	
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Attempt to prime controller failed with KErrNotReady"));	
	// try and play : we should get an error KErrNotReady
	error = iController.Play();
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : play returned unexpected code %d"), error);	
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Attempt to play controller failed with KErrNotReady"));	

	// try and remove data source : we should get an error KErrNotReady
	TMMFMessageDestination& sourceHandle = *iSourceHandlePtr;
	error = iController.RemoveDataSource(sourceHandle);
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : remove data source returned unexpected code %d"), error);	
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Attempt to remove data source failed with KErrNotReady"));	

	// try and remove data sink : we should get an error KErrNotReady
	TMMFMessageDestination& sinkHandle = *iSinkHandlePtr;
	error = iController.RemoveDataSink(sinkHandle);
	if(error != KErrNotReady)
		{
		ERR_PRINTF2(_L("Error : remove data sink returned unexpected code %d"), error);	
		return iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("Attempt to remove data sink failed with KErrNotReady"));	

	// EPass if we got here
	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0141
//
// Event notification
//
// REQ172.5.5.13

RTestMmfCtlfrmI0141* RTestMmfCtlfrmI0141::NewL()
	{
	RTestMmfCtlfrmI0141* self = new(ELeave) RTestMmfCtlfrmI0141;
	return self;
	}

RTestMmfCtlfrmI0141::RTestMmfCtlfrmI0141()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0141");
	}

TVerdict RTestMmfCtlfrmI0141::DoTestStepL()
	{
	// NB : if event search is disabled, we can't run this test - but unlike other
	// tests which can pass without SearchForEvent being called, this one can't.
#ifdef EVENT_SEARCH_DISABLED
	ERR_PRINTF1(_L("SearchForEvent disabled, we can't run this test"));
	return iTestStepResult = EInconclusive;

#else
	INFO_PRINTF1(_L("Receive Events"));

	TInt error = KErrNone;
	TVerdict result = EPass;
	
	error = iController.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("Prime failed, error %d"), error);
		return iTestStepResult = EFail;
		}

	// SearchForEvent() calls ReceiveEvents() and waits until the event is received
	// or times out.
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result != EPass )
		{
		ERR_PRINTF1(_L("Event request did not find the event"));
		return iTestStepResult = result;
		}

	// we pass if we receive the event
	INFO_PRINTF1(_L("Event request succeeded"));
	
	// EPass if we got here
	return iTestStepResult = EPass;

#endif // EVENT_SEARCH_DISABLED
	}

// ---------------------------
// RTestMmfCtlfrmI0142
//
// Cancel event notification
//
// REQ172.5.5.13

RTestMmfCtlfrmI0142* RTestMmfCtlfrmI0142::NewL()
	{
	RTestMmfCtlfrmI0142* self = new(ELeave) RTestMmfCtlfrmI0142;
	return self;
	}

RTestMmfCtlfrmI0142::RTestMmfCtlfrmI0142()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0142");
	}

// SearchForEventWithCancel() is a modified version of SearchForEvent() which
// calls ReceiveEvents() but then calls CancelReceiveEvents() before waiting for the request.
// It will return EPass if, and only if, the request times out.
TVerdict RTestMmfCtlfrmI0142::SearchForEventWithCancel( TMMFEvent& aEvent )
	{
	TVerdict result = EPass;
	
	TRequestStatus timerStatus;
	TRequestStatus eventStatus;

	RTimer  myTimer;
	TInt err = myTimer.CreateLocal();
	if(err)
		{
		ERR_PRINTF1(_L("Error : could not create local timer in SearchForEventWithCancel"));
		return EFail;
		}
	
	// NOTE. This test is somewhat spurious - we cannot call ReceiveEvents() without getting
	// an event back even if we call ReceiveEvents() straight afterwards. 
	
	// Therefore we don't call ReceiveEvents() here. Instead we call
	// CancelEvents() straight away, and set eventStatus to KRequestPending to ensure we get
	// a timeout from the 'request'.

	// This is still a valid test as long as we precede it in DoTestStepL() with a call to
	// SearchForEvent() to show that the event mechanism is working.

    TMMFEventPckg receivedEvent;
	
	// event values for audio controller testing
	const TInt KDelay            = 1000000;
	const TInt KMaxRetries       = 1;

	// for KMaxRetries attempt to find the event
	for( TInt retries = 0; retries < KMaxRetries; retries++ )
		{
		eventStatus = KRequestPending;
		// now cancel receive events before we have a chance to receive any!
		CancelReceivedEvents();
		
		// start breakout timer to escape
		myTimer.After( timerStatus, KDelay );
		// wait for an event to mature
		User::WaitForRequest( eventStatus, timerStatus );
		
		// fail if we get an event. pass if we timeout
        if( IsTimeOut( eventStatus ))
			{
			CancelReceivedEvents();
			}
		else 
			{
			if( IsSoughtEvent( aEvent, receivedEvent ) )
				{
				User::Check();
				myTimer.Cancel();
				// set status to FAIL since we have found the event
				return EFail;
				}
			else
				{
				// we've received an event other than the one expected.
				// we aren't expecting to receive events at all, so this too is a fail
				myTimer.Cancel();
				return EFail;
				}
			}
		}
    return result;
	}

TVerdict RTestMmfCtlfrmI0142::DoTestStepL()
	{
	// NB : if event search is disabled, we can't run this test - but unlike other
	// tests which can pass without SearchForEvent being called, this one can't.
#ifdef EVENT_SEARCH_DISABLED
	ERR_PRINTF1(_L("SearchForEvent disabled, we can't run this test"));
	return iTestStepResult = EInconclusive;

#else
	INFO_PRINTF1(_L("Cancel receive events"));

	TInt error = KErrNone;
	TVerdict result = EPass;
	
	error = iController.Prime();
	if(error)
		{
		ERR_PRINTF2(_L("Prime failed, error %d"), error);
		return iTestStepResult = EFail;
		}
	
	// Get an event first, to ensure that the mechanism is working.
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result != EPass )
		{
		ERR_PRINTF1(_L("First event request timed out"));
		return iTestStepResult = result;
		}
	INFO_PRINTF1(_L("First event request succeeded"));

	// SearchForEventWithCancel() is an override which doesn't actually get an event at all.
	TMMFEvent primeEvent2( KPrimeTestId, KErrNone);
	result = SearchForEventWithCancel( primeEvent2 );
	if( result != EPass )
		{
		ERR_PRINTF1(_L("Second event request did NOT timeout"));
		return iTestStepResult = result;
		}

	// we pass if we don't receive an event
	INFO_PRINTF1(_L("Second event request timed out"));
	
	// EPass if we got here
	return iTestStepResult = EPass;

#endif // EVENT_SEARCH_DISABLED
	}

// ---------------------------
// RTestMmfCtlfrmI0143
//
// Custom commands (synchronous)
//
// REQ172.5.5.14

RTestMmfCtlfrmI0143* RTestMmfCtlfrmI0143::NewL()
	{
	RTestMmfCtlfrmI0143* self = new(ELeave) RTestMmfCtlfrmI0143;
	return self;
	}

RTestMmfCtlfrmI0143::RTestMmfCtlfrmI0143()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0143");
	}

TVerdict RTestMmfCtlfrmI0143::DoTestStepL()
	{
	INFO_PRINTF1(_L("Custom Command (sync)"));

	TInt error = KErrNone;

	TUid uid = {KTSIMmfControllerUid};
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg messageDest(handleInfo);

	// call CustomCommandSync
	error = iController.CustomCommandSync(messageDest, KDummyFunc1, KNullDesC8, KNullDesC8);
	if(error == KDummyFunc1Return)
		error = KErrNone;
	else
		{
		ERR_PRINTF2(_L("CustomCommandSync KDummyFunc1 returned unexpected value %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF2(_L("CustomCommandSync KDummyFunc1 returned expected value %d"), KDummyFunc1Return);

	// now call the log - to test CustomCommandSync with aDataFrom
	_LIT8(KExpectedResult, "DummyFunc1 Called");
	TBuf8<KTextBufLen> memFunctionText;

	error = iController.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("CustomCommandSync failed with error %d"), error);
		return iTestStepResult = EFail;
		}
		
	TBuf<KTextBufLen> memFunctionText16;
	memFunctionText16.SetMax();
	memFunctionText16.Fill(0x0);
	memFunctionText16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Log : %S"), &memFunctionText16);

	if (memFunctionText != KExpectedResult)
		{
		ERR_PRINTF1(_L("Return value did not match expected"));
		return iTestStepResult = EFail;
		}

	// this is simply to ensure that all variations of ReadDataNFromClient*() are called
	// SetControllerMode() in TSI_MmfController does this.
	error = iController.CustomCommandSync(messageDest, KModeFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("CustomCommandSync failed with error %d"), error);
		return iTestStepResult = EFail;
		}
		// now call the log - to test CustomCommandSync with aDataFrom
	_LIT8(KExpectedResultModePass, "SetControllerMode completed successfully");
	error = iController.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
	if (error)
		{
		ERR_PRINTF2(_L("CustomCommandSync failed with error %d"), error);
		return iTestStepResult = EFail;
		}
	memFunctionText16.SetMax();
	memFunctionText16.Fill(0x0);
	memFunctionText16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Log : %S"), &memFunctionText16);

	if (memFunctionText != KExpectedResultModePass)
		return iTestStepResult = EFail;

	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0144
//
// Custom commands (asynchronous)
//
// REQ172.5.5.1

RTestMmfCtlfrmI0144* RTestMmfCtlfrmI0144::NewL()
	{
	RTestMmfCtlfrmI0144* self = new(ELeave) RTestMmfCtlfrmI0144;
	return self;
	}

RTestMmfCtlfrmI0144::RTestMmfCtlfrmI0144()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0144");
	}

TVerdict RTestMmfCtlfrmI0144::DoTestStepL()
	{
	INFO_PRINTF1(_L("Custom Command (async)"));

	TInt error = KErrNone;

	TUid uid = {KTSIMmfControllerUid};
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg messageDest(handleInfo);

	// call CustomCommandAsync
	const TInt KGuardTimerValue = 5000000; // give it 5 sec timeout
	RTimer guardTimer;
	TRequestStatus timerStatus;
	TRequestStatus controllerStatus;
	guardTimer.CreateLocal();

	iController.CustomCommandAsync(messageDest, KDummyFunc1, KNullDesC8, KNullDesC8, controllerStatus);
	guardTimer.After(timerStatus, KGuardTimerValue);
	User::WaitForRequest(controllerStatus, timerStatus);
	guardTimer.Cancel();
	guardTimer.Close();

	if(controllerStatus == KRequestPending)
		{ 
		// request timed out
		ERR_PRINTF1(_L("CustomCommandAsync : request timed out"));
		return iTestStepResult = EFail;
		}
		
	error = controllerStatus.Int();
	if(error == KDummyFunc1Return)
		error = KErrNone;
	else
		{
		ERR_PRINTF2(_L("CustomCommandAsync KDummyFunc1 returned unexpected value %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF2(_L("CustomCommandAsync KDummyFunc1 returned expected value %d"), KDummyFunc1Return);

	// now call the log - to test CustomCommandAsync with aDataFrom
	_LIT8(KExpectedResult, "DummyFunc1 Called");
	TBuf8<KTextBufLen> memFunctionText;

	guardTimer.CreateLocal();
	iController.CustomCommandAsync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, 
								   memFunctionText, controllerStatus);
	guardTimer.After(timerStatus, KGuardTimerValue);
	User::WaitForRequest(controllerStatus, timerStatus);
	guardTimer.Cancel();
	guardTimer.Close();

	if(controllerStatus == KRequestPending)
		{ 
		// request timed out
		ERR_PRINTF1(_L("CustomCommandAsync : request timed out"));
		return iTestStepResult = EFail;
		}
		
	error = controllerStatus.Int();
	if (error)
		{
		ERR_PRINTF2(_L("CustomCommandAsync failed with error %d"), error);
		return iTestStepResult = EFail;
		}
		
	TBuf<KTextBufLen> memFunctionText16;
	memFunctionText16.SetMax();
	memFunctionText16.Fill(0x0);
	memFunctionText16.Copy(memFunctionText);
	INFO_PRINTF2(_L("Log : %S"), &memFunctionText16);

	if (memFunctionText != KExpectedResult)
		{
		ERR_PRINTF1(_L("Return value did not match expected"));
		return iTestStepResult = EFail;
		}

	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0151
//
// Get duration
//
// (no REQ)

RTestMmfCtlfrmI0151* RTestMmfCtlfrmI0151::NewL()
	{
	RTestMmfCtlfrmI0151* self = new(ELeave) RTestMmfCtlfrmI0151;
	return self;
	}

RTestMmfCtlfrmI0151::RTestMmfCtlfrmI0151()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0151");
	}

TVerdict RTestMmfCtlfrmI0151::DoTestStepL()
	{
	INFO_PRINTF1(_L("Get Duration"));

	TInt error = KErrNone;
	TTimeIntervalMicroSeconds duration;

	error = iController.GetDuration(duration);
	if (error)
		{
		ERR_PRINTF2(_L("GetDuration failed with error %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF2(_L("Clip duration: %d"), I64LOW(duration.Int64()));

	// we just happen to know that this clip is 5.33 seconds long
	// currently assuming resolution 1/100 of a sec, rounding up
	const TTimeIntervalMicroSeconds expectedDuration(5330000);
	const TInt expectedDurationMilliSeconds = 533;
	TInt64 duration64 = duration.Int64();
	TInt64 durationMod;
	I64DIVMOD(duration64, 10000, durationMod);
	if (durationMod >= 5000)
		duration64++;

	if(duration64 != expectedDurationMilliSeconds)
		{
		ERR_PRINTF2(_L("Expected duration was %d"), I64LOW(expectedDuration.Int64()));
		return iTestStepResult = EFail;
		}

	return iTestStepResult = EPass;
	}

// ---------------------------
// RTestMmfCtlfrmI0152
//
// Get metadata entries
//
// (no REQ)

RTestMmfCtlfrmI0152* RTestMmfCtlfrmI0152::NewL()
	{
	RTestMmfCtlfrmI0152* self = new(ELeave) RTestMmfCtlfrmI0152;
	return self;
	}

RTestMmfCtlfrmI0152::RTestMmfCtlfrmI0152()
	{
	iTestStepName = _L("MM-MMF-CTLFRM-I-0152");
	}

TVerdict RTestMmfCtlfrmI0152::DoTestStepL()
	{
	INFO_PRINTF1(_L("Get Metadata Entries"));

	TInt error = KErrNone;

	// expected values from our test controller
	const TInt KExpectedNumberOfMetaDataEntries = 4;
	// NB : these are 8-bit strings
	_LIT(KExpectedMetaDataEntryName1, "AudioLevel");
	_LIT(KExpectedMetaDataEntryValue1, "HIGH");
	_LIT(KExpectedMetaDataEntryName2, "AudioQuality");
	_LIT(KExpectedMetaDataEntryValue2, "LOW");

	TInt numEntries;
	error = iController.GetNumberOfMetaDataEntries(numEntries);
	if (error)
		{
		ERR_PRINTF2(_L("GetNumberOfMetaDataEntries failed, error %d"), error);
		return iTestStepResult = EFail;
		}
	INFO_PRINTF2(_L("Number of metadata entries : %d"), numEntries);
	if (numEntries != KExpectedNumberOfMetaDataEntries)
		{
		ERR_PRINTF2(_L("GetNumberOfMetaDataEntries returned unexpected value %d"), numEntries);
		return iTestStepResult = EFail;
		}

	CMMFMetaDataEntry* metaEntry1 = NULL;
	TRAPD(err1, metaEntry1 = iController.GetMetaDataEntryL(1))
	if(err1)
		{
		ERR_PRINTF2(_L("GetMetaDataEntryL left with error %d"), err1);
		return iTestStepResult = EFail;
		}
	CleanupStack::PushL(metaEntry1);
	TPtrC entry1Name = metaEntry1->Name();
	TPtrC entry1Value = metaEntry1->Value();
	INFO_PRINTF3(_L("Metadata entry 1 = %S:%S"), &entry1Name, &entry1Value);

	if((entry1Name != KExpectedMetaDataEntryName1) || (entry1Value != KExpectedMetaDataEntryValue1))
		{
		ERR_PRINTF1(_L("Metadata entries did not match expected"));
		CleanupStack::PopAndDestroy(metaEntry1);
		return iTestStepResult = EFail;
		}

	CMMFMetaDataEntry* metaEntry2 = NULL;
	TRAPD(err2, metaEntry2 = iController.GetMetaDataEntryL(2))
	if(err2)
		{
		ERR_PRINTF2(_L("GetMetaDataEntryL left with error %d"), err2);
		CleanupStack::PopAndDestroy(metaEntry1);
		return iTestStepResult = EFail;
		}
	CleanupStack::PushL(metaEntry2);
	TPtrC entry2Name = metaEntry2->Name();
	TPtrC entry2Value = metaEntry2->Value();
	INFO_PRINTF3(_L("Metadata entry 2 = %S:%S"), &entry2Name, &entry2Value);

	if((entry2Name != KExpectedMetaDataEntryName2) || (entry2Value != KExpectedMetaDataEntryValue2))
		{
		ERR_PRINTF1(_L("Metadata entries did not match expected"));
		CleanupStack::PopAndDestroy(2); // metaEntry2, metaEntry1
		return iTestStepResult = EFail;
		}

	// test the copy constructor
	CMMFMetaDataEntry* metaEntry3 = CMMFMetaDataEntry::NewL(*metaEntry2);
	CleanupStack::PushL(metaEntry3);
	TPtrC entry3Name = metaEntry3->Name();
	TPtrC entry3Value = metaEntry3->Value();
	INFO_PRINTF3(_L("Copy-constructed metadata entry = %S:%S"), &entry3Name, &entry3Value);

	if((entry3Name != KExpectedMetaDataEntryName2) || (entry3Value != KExpectedMetaDataEntryValue2))
		{
		ERR_PRINTF1(_L("Copy-constructed metadata entries did not match expected"));
		CleanupStack::PopAndDestroy(3); // metaEntry3, metaEntry2, metaEntry1
		return iTestStepResult = EFail;
		}

	// test setters
	TRAPD(err3, metaEntry3->SetNameL(KExpectedMetaDataEntryName1));
	if(err3)
		{
		ERR_PRINTF2(_L("SetNameL left with error %d"), err3);
		CleanupStack::PopAndDestroy(3); // metaEntry3, metaEntry2, metaEntry1
		return iTestStepResult = EFail;
		}

	TRAP(err3, metaEntry3->SetValueL(KExpectedMetaDataEntryValue1));
	if(err3)
		{
		ERR_PRINTF2(_L("SetValueL left with error %d"), err3);
		CleanupStack::PopAndDestroy(3); // metaEntry3, metaEntry2, metaEntry1
		return iTestStepResult = EFail;
		}

	TPtrC entry3NewName = metaEntry3->Name();
	TPtrC entry3NewValue = metaEntry3->Value();
	INFO_PRINTF3(_L("Copy-constructed metadata entry now = %S:%S"), &entry3NewName, &entry3NewValue);

	if((entry3NewName != KExpectedMetaDataEntryName1) || (entry3NewValue != KExpectedMetaDataEntryValue1))
		{
		ERR_PRINTF1(_L("Copy-constructed metadata entries did not match after Set()"));
		CleanupStack::PopAndDestroy(3); // metaEntry3, metaEntry2, metaEntry1
		return iTestStepResult = EFail;
		}


	CleanupStack::PopAndDestroy(3); // metaEntry3, metaEntry2, metaEntry1
	return iTestStepResult = EPass;
	}

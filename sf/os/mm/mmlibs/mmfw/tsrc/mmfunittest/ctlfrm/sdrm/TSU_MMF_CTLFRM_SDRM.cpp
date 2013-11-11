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
// NB: these includes are added here to pull in includes from MmfControllerFramework.h
// They are placed here as we are messing aroung with private (see below) to allow access to 
// the private member iSubThread in RMMFControllerProxy for test 152 without resorting to friend 
// classes. This was done to test INC038123.
// +++++++++  Start of playing around with private block. +++++++++++++
// 
//

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include <mmf/common/mmfcontroller.h>

#define private public
#include <mmf/common/mmfcontrollerframework.h>
#undef private
//++++++++ End of playing around with private block. +++++++++++


// Test system includes
#include <testframework.h>

#include <mmf/plugin/mmfformatimplementationuids.hrh>
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfdrmcustomcommands.h>
#include <fbs.h>

#include "CmmfTestControllerUIDs.hrh"
#include "TSU_MMF_CTLFRM_SDRM.h"
#include "TSU_MMF_CTLFRMSuite_SDRM.h"

//const TUid KMmfUidControllerAudioUid = {KMmfUidControllerAudio};	// EABI warning removal
const TUid KTstControllerUid = {KMmfTestControllerUid}; //(0x101F88D8)
const TUid KTestDataSourceUid = {KMmfTestDataSourceUid};
const TUid KTestDataSourceBUid = {KMmfTestDataSourceBUid};
const TUid KTestDataSinkUid = {KMmfTestDataSinkUid};
const TUid KTestDataSinkBUid = 	{KMmfTestDataSinkBUid};

// Expected results in Test 0140s.
// 'KMmfUidControllerAudio' is the preferred Controller used in these tests
const TUid KExpectedMediaIDUid140 = {0x101F7D8D};//{0x101F5D07};
                                    
const TUid KMmfUidFormatPAVReadUid = {KMmfUidFormatPAVRead};
const TUid KMmfUidFormatFAWReadUid = {KMmfUidFormatFAWRead};
const TUid KMmfUidFormatAIReadUid = {KMmfUidFormatAIRead};
const TUid KMmfUidFormatMBEGReadUid = {KMmfUidFormatMBEGRead};

const TUid KMmfUidFormatPAVWriteUid = {KMmfUidFormatPAVWrite};
const TUid KMmfUidFormatFAWWriteUid = {KMmfUidFormatFAWWrite};
const TUid KMmfUidFormatAIWriteUid = {KMmfUidFormatAIWrite};

//const TUid KMmfUidPluginInterfaceFormatEncodeUid = {KMmfUidPluginInterfaceFormatEncode};
//const TUid KMmfUidPluginInterfaceFormatDecodeUid = {KMmfUidPluginInterfaceFormatDecode};

_LIT(KTestControllerSupplier, "TSU_MMF_CTLFRM");

// Used as dummy data to pass to sources, sinks etc
_LIT8(KInitData,"TEST");

/**
 *
 * CTestStepSelfTest constructor
 *
 * @xxxx
 * 
 */
CTestStep_MMF_CTLFRM_SDRM::CTestStep_MMF_CTLFRM_SDRM() 
	{
	}

/**
 *
 * CTestStepSelfTest destructor
 *
 * @xxxx
 * 
 */
CTestStep_MMF_CTLFRM_SDRM::~CTestStep_MMF_CTLFRM_SDRM()
	{
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM::DoTestStepPreambleL()
	{
	TVerdict verdict = EPass;

	return verdict;
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM::DoTestStepPostambleL()
	{
	TVerdict verdict = EPass;

	return verdict;
	}


CTestStep_MMF_CTLFRM_SDRM_U_0001::CTestStep_MMF_CTLFRM_SDRM_U_0001()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0001");

	
	}


TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0001::DoTestStepL( void )
/** 
 * Call the RMMFController::OpenInSecureDRMProcess(..) with the parameters specified, 
 * varify the return value is 0. Call the RMMFController::Close() on the Custom Controller. 
 * Varify 'error' is 0.

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	INFO_PRINTF1(_L("Attempting to Open Controller"));

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//------------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0007::CTestStep_MMF_CTLFRM_SDRM_U_0007()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0007");

	
	}


TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0007::DoTestStepL( void )
/** Call the RMMFController::OpenInSecureDRMProcess(..) (with the parameters from test TSU_MMF_CTLFRM_SDRM_U_0001), 
 *  varify the return value is 0. Instantiate a Custom DataSource Object and add  it to 
 *  the Custom Controller using RMMFController::AddDataSource(..), with the specified 
 *  parameters. Call Custom Command 'KLogFunction' and varify the text value returned is 
 *  equal to the specifed text in Test Step. Call the RMMFController::Close() on the Custom 
 *  Controller. Varify 'error' is 0.

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "AddDataSourceL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TDesC8& initData = KInitData;

	// Add Source
	INFO_PRINTF1(_L("Attempting to add a Source and a Sink to Controller"));
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSource"));
		error = controller.AddDataSource(KTestDataSourceUid, initData);
		error = controller.AddDataSource(KTestDataSourceBUid, initData);
		}

	INFO_PRINTF2(_L("Controller Status: %d"), error);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);


		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//
	
	
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0008::CTestStep_MMF_CTLFRM_SDRM_U_0008()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0008");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0008::DoTestStepL( void )
/** Call the RMMFController::OpenInSecureDRMProcess(..) (with the parameters from test TSU_MMF_CTLFRM_SDRM_U_0001), 
 *  varify the return value is 0. Instantiate a Custom DataSink Object and add it to the 
 *  Custom Controller using RMMFController::AddDataSink(..), with the specified parameters. 
 *  Call Custom Command 'KLogFunction' and varify the text value returned is equal to the 
 *  specifed text in Test Step. Call the RMMFController::Close() on the Custom Controller. 
 *  Varify 'error' is 0.

 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "AddDataSinkL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TDesC8& initData = KInitData;

	// Add Sink
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSink"));
		error = controller.AddDataSink(KTestDataSinkUid, initData);
		error = controller.AddDataSink(KTestDataSinkBUid, initData);
		}

		
	INFO_PRINTF2(_L("Controller Status: %d"), error);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0009::CTestStep_MMF_CTLFRM_SDRM_U_0009()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0009");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0009::DoTestStepL( void )
/** Call the RMMFController::OpenInSecureDRMProcess(..) (with the parameters from test TSU_MMF_CTLFRM_SDRM_U_0001), 
 *  varify the return value is 0. Instantiate a Custom DataSource Object and add it to the 
 *  Custom Controller using RMMFController::AddDataSource(..), with the specified parameters.
 *  Call Custom Command 'KLogFunction' and varify the text value returned is equal to the 
 *  specifed text in Test Step. Call the RMMFController::Close() on the Custom Controller. 
 *  Varify 'error' is 0.

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "AddDataSourceL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TDesC8& initData = KInitData;

	TMMFMessageDestination sourceHandlePtr;
	TMMFMessageDestination& sourceHandle = sourceHandlePtr;

	// Add Source
	INFO_PRINTF1(_L("Attempting to add a Source and a Sink to Controller"));
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSource"));
		error = controller.AddDataSource(KTestDataSourceUid, initData, sourceHandle);
		}
		
	INFO_PRINTF2(_L("Controller Status: %d"), error);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0010::CTestStep_MMF_CTLFRM_SDRM_U_0010()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0010");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0010::DoTestStepL( void )
/** Call the RMMFController::OpenInSecureDRMProcess(..) (with the parameters from test TSU_MMF_CTLFRM_SDRM_U_0001), 
 *  varify the return value is 0. Instantiate a Custom DataSink Object and add it to the 
 *  Custom Controller using RMMFController::AddDataSink(..), with the specified parameters. 
 *  Call Custom Command 'KLogFunction' and varify the text value returned is equal to the 
 *  specifed text in Test Step. Call the RMMFController::Close() on the Custom Controller. 
 *  Varify 'error' is 0.

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "AddDataSinkL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TDesC8& initData = KInitData;
	TMMFMessageDestination sinkHandlePtr;
	TMMFMessageDestination& sinkHandle = sinkHandlePtr;

	// Add Sink
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSink"));
		error = controller.AddDataSink(KTestDataSinkUid, initData, sinkHandle);
		}

	INFO_PRINTF2(_L("Controller Status: %d"), error);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//
	
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0011::CTestStep_MMF_CTLFRM_SDRM_U_0011()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0011");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0011::DoTestStepL( void )
/** Call the RMMFController::OpenInSecureDRMProcess(..) (with the parameters from test TSU_MMF_CTLFRM_SDRM_U_0001),
 *  varify the return value is 0. Instantiate a Custom DataSource Object and add it to the 
 *  Custom Controller using RMMFController::AddDataSource(..), with the specified parameters.
 *  Call Custom Command 'KLogFunction' and varify the text value returned is equal to the 
 *  specifed text in Test Step. Call the RMMFController::Close() on the Custom Controller. 
 *  Varify 'error' is 0.

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult,"RemoveDataSourceL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TDesC8& initData = KInitData;

	TMMFMessageDestination sourceHandlePtr;
	TMMFMessageDestination& sourceHandle = sourceHandlePtr;

	// Add and Remove Source
	INFO_PRINTF1(_L("Attempting to add a Source and a Sink to Controller"));
	if (!error)
		{
		error = controller.AddDataSource(KTestDataSourceUid, initData, sourceHandle);
		INFO_PRINTF1(_L("API TEST: Calling RemoveDataSource"));
		error = controller.RemoveDataSource(sourceHandle);
		}

	INFO_PRINTF2(_L("Controller Status: %d"), error);

		//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//
	
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0012::CTestStep_MMF_CTLFRM_SDRM_U_0012()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0012");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0012::DoTestStepL( void )
/** Call the RMMFController::OpenInSecureDRMProcess(..) (with the parameters from test TSU_MMF_CTLFRM_SDRM_U_0001),
 *  varify the return value is 0. Instantiate a Custom DataSink Object and add it to the 
 *  Custom Controller using RMMFController::AddDataSink(..), with the specified parameters. 
 *  Call Custom Command 'KLogFunction' and varify the text value returned is equal to the 
 *  specifed text in Test Step. Call the RMMFController::Close() on the Custom Controller. 
 *  Varify 'error' is 0.

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "RemoveDataSinkL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TDesC8& initData = KInitData;
	TMMFMessageDestination sinkHandlePtr;
	TMMFMessageDestination& sinkHandle = sinkHandlePtr;

	// Add and Remove Sink
	if (!error)
		{
		error = controller.AddDataSink(KTestDataSinkUid, initData, sinkHandle);
		INFO_PRINTF1(_L("API TEST: Calling RemoveDataSink"));
		error = controller.RemoveDataSink(sinkHandle);
		}

	INFO_PRINTF2(_L("Controller Status: %d"), error);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0013::CTestStep_MMF_CTLFRM_SDRM_U_0013()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0013");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0013::DoTestStepL( void )
/** Call the RMMFController::OpenInSecureDRMProcess(..) (with the parameters from test TSU_MMF_CTLFRM_SDRM_U_0001),
 *  varify the return value is 0. Call RMMFController Method specified (in API Method Calls 
 *  and Parameters) and varify the 'error' is 0. Follow this by calling the  Custom Command 
 *  'KLogFunction' and varify the text value returned is equal to the specifed text in 
 *  Test Step. Call the RMMFController::Close() on the Custom Controller. Varify 'error' 
 *  is zero.

 * Use case: N/A
 * @test Req. under test No Requirement Found
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult,"ResetL Called");
 
	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	// Reset Controller (API being tested)
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling Reset"));
		error = controller.Reset();		
		}

	INFO_PRINTF2(_L("Controller Status: %d"), error);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//
	
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0014::CTestStep_MMF_CTLFRM_SDRM_U_0014()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0014");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0014::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 * ::Prime()
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "PrimeL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	// Prime Controller
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling Prime"));
		error = controller.Prime();
		}

	INFO_PRINTF2(_L("Controller Status: %d"), error);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//
		
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0015::CTestStep_MMF_CTLFRM_SDRM_U_0015()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0015");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0015::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 *  ::Play()
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "PlayL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	// Play Controller
	if (!error)
		{
		INFO_PRINTF1(_L("Calling Play"));
		error = controller.Play();
		}

	INFO_PRINTF2(_L("API TEST: Controller Status: %d"), error);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//
		
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0016::CTestStep_MMF_CTLFRM_SDRM_U_0016()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0016");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0016::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 * ::Pause()
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "PauseL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);	

	// Pause Controller
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling Pause"));
		error = controller.Pause();
		}

	INFO_PRINTF2(_L("Controller Status: %d"), error);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//
		
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0017::CTestStep_MMF_CTLFRM_SDRM_U_0017()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0017");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0017::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 * ::Stop()
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "StopL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	// Stop Controller
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling Stop"));
		error = controller.Stop();
		}

	INFO_PRINTF2(_L("Controller Status: %d"), error);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0018::CTestStep_MMF_CTLFRM_SDRM_U_0018()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0018");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0018::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 * ::GetPosition(..)
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "PositionL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TInt64 interval = 5;
	TTimeIntervalMicroSeconds position(interval);

	const TInt64 expectedInterval = 16;
	TTimeIntervalMicroSeconds expectedPosition(expectedInterval);

	// Get Position Controller
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling GetPosition"));
		error = controller.GetPosition(position);
		}

	INFO_PRINTF2(_L("Controller Status: %d"), error);
	INFO_PRINTF2(_L("Expected Position: %d"), I64LOW(expectedPosition.Int64()));
	INFO_PRINTF2(_L("Current Position: %d"), I64LOW(position.Int64()));
	

	if (expectedPosition != position)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}
		}

	INFO_PRINTF1(_L("Closing Controller"));	
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0019::CTestStep_MMF_CTLFRM_SDRM_U_0019()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0019");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0019::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 * ::SetPosition(..)
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "SetPositionL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TInt64 interval = 5;
	TTimeIntervalMicroSeconds position(interval);

	// Set Position Controller
	if (!error) 
		{
		INFO_PRINTF1(_L("API TEST: Calling SetPosition"));
		error = controller.SetPosition(position);
		}

	if (!error) 
		error = controller.GetPosition(position);


	INFO_PRINTF2(_L("Controller Status: %d"), error);
	INFO_PRINTF2(_L("Current Position: %d"), position.Int64());

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//
		
	INFO_PRINTF1(_L("Closing Controller"));	
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0020::CTestStep_MMF_CTLFRM_SDRM_U_0020()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0020");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0020::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 * ::GetDuration(..)
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TInt64 interval = 5;
	TTimeIntervalMicroSeconds duration(interval);

	const TInt64 expectedInterval = 36;
	TTimeIntervalMicroSeconds expectedDuration(expectedInterval);
	
	if (!error) 
		{
		INFO_PRINTF1(_L("API TEST: Calling GetDuration"));
		error = controller.GetDuration(duration);
		}


	INFO_PRINTF2(_L("Controller Status: %d"), error);
	INFO_PRINTF2(_L("Current Duration: %d"), I64LOW(expectedDuration.Int64()));
	INFO_PRINTF2(_L("Current Duration: %d"), I64LOW(duration.Int64()));

	if (duration != expectedDuration)
		{
		verdict = EFail;
		}
		
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0021::CTestStep_MMF_CTLFRM_SDRM_U_0021()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0021");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0021::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 * ::GetNumberOfMetaDataEntries(..)
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "GetNumberOfMetaDataEntriesL Called");

	TInt numberOfEntries = 0;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);


	// Add Source and Sink
	INFO_PRINTF1(_L("Attempting to add a Source and a Sink to Controller"));

	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling GetNumberOfMetaDataEntries"));
		error = controller.GetNumberOfMetaDataEntries(numberOfEntries);
		}

	INFO_PRINTF2(_L("Number of Meta entries is: %d"), numberOfEntries);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//
		
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0022::CTestStep_MMF_CTLFRM_SDRM_U_0022()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0022");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0022::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 * ::GetMetaDataEntry(..)
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "GetMetaDataEntryL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	if (!error)
		{
		TInt indexOfEntry = 1;
		INFO_PRINTF1(_L("API TEST: Calling GetMetaDataEntryL"));
		CMMFMetaDataEntry* metaEntry = controller.GetMetaDataEntryL(indexOfEntry);
		CleanupStack::PushL(metaEntry);
		INFO_PRINTF2(_L("AudioLevel is: %S"), &metaEntry->Value());

		CleanupStack::PopAndDestroy(1);
		}

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//
		
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0023::CTestStep_MMF_CTLFRM_SDRM_U_0023()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0023");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0023::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 * ::CustomSommandSync(..)
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "KDummyFunc1 Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	if (!error)
		{
		INFO_PRINTF1(_L("Calling CustomCommandSync"));
		TMMFMessageDestinationPckg messageDest;

		const TInt interval = 5;
		TTimeIntervalMicroSeconds randomData(interval);
		TTimeIntervalMicroSeconds randomData2(interval+1);
		TTimeIntervalMicroSeconds randomData3(interval+2);

		TPckgBuf<TTimeIntervalMicroSeconds> randomDataPckg = randomData;
		TPckgBuf<TTimeIntervalMicroSeconds> randomData2Pckg = randomData2;
		TPckgBuf<TTimeIntervalMicroSeconds> randomData3Pckg = randomData3;
		
		error = controller.CustomCommandSync(messageDest, KDummyFunc1, randomDataPckg, randomData2Pckg, randomData3Pckg);
		}

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0024::CTestStep_MMF_CTLFRM_SDRM_U_0024()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0024");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0024::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 * ::CustomCommandAsync(..)
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "KDummyFunc1 Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;


	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	if (!error)
		{
		INFO_PRINTF1(_L("Calling CustomCommandSync"));
		TMMFMessageDestinationPckg messageDest;

		const TInt interval = 5;
		TTimeIntervalMicroSeconds randomData(interval);
		TTimeIntervalMicroSeconds randomData2(interval+1);
		TTimeIntervalMicroSeconds randomData3(interval+2);

		TPckgBuf<TTimeIntervalMicroSeconds> randomDataPckg = randomData;
		TPckgBuf<TTimeIntervalMicroSeconds> randomData2Pckg = randomData2;
		TPckgBuf<TTimeIntervalMicroSeconds> randomData3Pckg = randomData3;
		
		TRequestStatus stat;
		controller.CustomCommandAsync(messageDest, KDummyFunc1, randomDataPckg, randomData2Pckg, randomData3Pckg, stat);
		User::WaitForRequest(stat);
		error = stat.Int();
		}

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0027::CTestStep_MMF_CTLFRM_SDRM_U_0027()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0027");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0027::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 * ::GetAudioQuality(..)
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "GetAudioQuality Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;


	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);
	TUid uid = {0x101f72B4};

	RMMFTestCustomCommands customCommands(controller,uid);

	TInt audioQuality;
	error = customCommands.GetAudioQuality(audioQuality);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//


	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0028::CTestStep_MMF_CTLFRM_SDRM_U_0028()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0028");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0028::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0013
 * ::SetAudioQuality(..)
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "SetAudioQuality Called");


	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;


	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);
	TUid uid = {0x101f72B4};

	RMMFTestCustomCommands customCommands(controller,uid);


	TInt audioQuality = 14;

	error = customCommands.SetAudioQuality(audioQuality);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//


	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//------------------------------------------------------------------
// This test is supposed to fail

CTestStep_MMF_CTLFRM_SDRM_U_0029::CTestStep_MMF_CTLFRM_SDRM_U_0029()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0029");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0029::DoTestStepL( void )
/** Call the RMMFController::OpenInSecureDRMProcess(..) with the parameters specified, varify the 
 *  return value is NOT 0. Call the RMMFController::Close() on the Custom Controller. 
 *  Varify 'error' is NOT zero.

 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	INFO_PRINTF1(_L("Test to Fail to Open a Custom Controller"));


	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Dummy UID to try and instantiate a controller which does not exist
	TUid uidDummy = {0x12345678}; 

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(uidDummy,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	if (!error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0030::CTestStep_MMF_CTLFRM_SDRM_U_0030()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0030");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0030::DoTestStepL( void )
/** Call the RMMFController::OpenInSecureDRMProcess(..) (with the parameters from test 001), varify 
 *  the return value is 0. Fail to Instantiate a Custom DataSource Object and add it to 
 *  the Custom Controller using RMMFController::AddDataSource(..), with the specified 
 *  parameters. Call Custom Command 'KLogFunction' and varify the text value returned is 
 *  not equal to the specifed text in Test Step. Call the RMMFController::Close() on the 
 *  Custom Controller. Varify 'error' is NOT zero.

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	INFO_PRINTF1(_L("Test to Fail to Open a Data Source"));

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TDesC8& initData = KInitData;
	TUid uidDummy = {0x12345678};

	// Add Source
	INFO_PRINTF1(_L("Attempting to add a Source to Controller"));
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSource"));
		error = controller.AddDataSource(uidDummy, initData);
		}

	INFO_PRINTF2(_L("Controller Status: %d"), error);
	
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (!error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0031::CTestStep_MMF_CTLFRM_SDRM_U_0031()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0031");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0031::DoTestStepL( void )
/** Call the RMMFController::OpenInSecureDRMProcess(..) (with the parameters from test 001), varify the 
 *  return value is 0. Instantiate a Custom DataSink Object and add it to the Custom 
 *  Controller using RMMFController::AddDataSink(..), with the specified parameters. Call 
 *  Custom Command 'KLogFunction' and varify the text value returned is equal to the 
 *  specifed text in Test Step. Call the RMMFController::Close() on the Custom Controller. 
 *  Varify 'error' is zero.

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	INFO_PRINTF1(_L("Test to Fail to Open a Data Sink"));

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TDesC8& initData = KInitData;
	TUid uidDummy = {0x12345678};

	// Add Source
	INFO_PRINTF1(_L("Attempting to add a Sink to Controller"));
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSource"));
		error = controller.AddDataSink(uidDummy, initData);
		}
	
	INFO_PRINTF2(_L("Controller Status: %d"), error);
	
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (!error)
		{
		verdict = EFail;
		}	

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//----------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0032::CTestStep_MMF_CTLFRM_SDRM_U_0032()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0032");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0032::DoTestStepL( void )
/** Verfiy CMMFDescriptorBuffer creation and deletion using default buffer size
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	//---------------------//
	// Set Controller Mode //
	//---------------------//
	if (!error)
		{
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		TPckgBuf<TUint8> value = 1; // 1 = EFailure Mode

		error = controller.CustomCommandSync(messageDest, KModeFunction, value, KNullDesC8);
		}

	const TDesC8& initData = KInitData;
	TUid uidDummy = {0x12345678};

	// Add Source
	INFO_PRINTF1(_L("Attempting to add a Source to Controller"));
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSource"));
		//error = controller.AddDataSource(KTestDataSourceUid, initData);
		error = controller.AddDataSource(uidDummy, initData);
		}
	
	INFO_PRINTF2(_L("Controller Status: %d"), error);

	if (error != KAddDataSourceFailure)
		{
		verdict = EFail;
		}

	
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
		
	return verdict;
	}

//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0033::CTestStep_MMF_CTLFRM_SDRM_U_0033()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0033");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0033::DoTestStepL( void )
/** Verfiy CMMFDescriptorBuffer creation and deletion using default buffer size
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	//---------------------//
	// Set Controller Mode //
	//---------------------//
	if (!error)
		{
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);


		TPckgBuf<TUint8> value = 1; // 1 = EFailure Mode

		error = controller.CustomCommandSync(messageDest, KModeFunction, value, KNullDesC8);
		}

	const TDesC8& initData = KInitData;

	// Add Sink
	INFO_PRINTF1(_L("Attempting to add a Sink to Controller"));
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSink"));
		error = controller.AddDataSource(KTestDataSinkUid, initData);
		}
	
	INFO_PRINTF2(_L("Controller Status: %d"), error);

	if (error != KAddDataSinkFailure)
		{
		verdict = EFail;
		}


	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
		
	return verdict;
	}

//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0034::CTestStep_MMF_CTLFRM_SDRM_U_0034()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0034");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0034::DoTestStepL( void )
/** Verfiy CMMFDescriptorBuffer creation and deletion using default buffer size
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	//---------------------//
	// Set Controller Mode //
	//---------------------//
	if (!error)
		{
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);
 
		TPckgBuf<TUint8> value = 1; // 1 = EFailure Mode

		error = controller.CustomCommandSync(messageDest, KModeFunction, value, KNullDesC8);
		}

	const TDesC8& initData = KInitData;
	TMMFMessageDestination sourceHandlePtr;
	TMMFMessageDestination& sourceHandle = sourceHandlePtr;

	// Add Source
	INFO_PRINTF1(_L("Attempting to add a Source to Controller"));
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSource"));
		error = controller.AddDataSource(KTestDataSourceUid, initData, sourceHandle);
		error = controller.RemoveDataSource(sourceHandle);
		}
		
	
	INFO_PRINTF2(_L("Controller Status: %d"), error);

	if (error != KAddDataSourceFailure)
		{
		verdict = EFail;
		}

	
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//-----------------------------------------------------------//

CTestStep_MMF_CTLFRM_SDRM_U_0035::CTestStep_MMF_CTLFRM_SDRM_U_0035()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0035");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0035::DoTestStepL( void )
/** Verfiy CMMFDescriptorBuffer creation and deletion using default buffer size
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	//---------------------//
	// Set Controller Mode //
	//---------------------//
	if (!error)
		{
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

//		TUint8 value = 1; // 1 = EFailure Mode
		TPckgBuf<TUint8> value = 1;

		error = controller.CustomCommandSync(messageDest, KModeFunction, value, KNullDesC8);
		}

	const TDesC8& initData = KInitData;
	TMMFMessageDestination sinkHandlePtr;
	TMMFMessageDestination& sinkHandle = sinkHandlePtr;

	// Add Source
	INFO_PRINTF1(_L("Attempting to add a Sink to Controller"));
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSink"));
		error = controller.AddDataSink(KTestDataSinkUid, initData, sinkHandle);
		error = controller.RemoveDataSink(sinkHandle);
		}
		
	
	INFO_PRINTF2(_L("Controller Status: %d"), error);

	if (error != KAddDataSinkFailure)
		{
		verdict = EFail;
		}
	
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//-----------------------------------------------------------//

CTestStep_MMF_CTLFRM_SDRM_U_0038::CTestStep_MMF_CTLFRM_SDRM_U_0038()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0038");
	}


TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0038::DoTestStepL( void )
/** 
 * Call the RMMFController::Open(..) with the parameters specified, 
 * varify the return value is 0. Test the accessibility of custom coammands 
 * with RMMFDRMCustomCommands and RMMFVideoDRMExtCustomCommands with the opened controller
 * Varify 'error' is 0 and .

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	INFO_PRINTF1(_L("Attempting to Open Controller"));

	// Open a Custom Controller
	error = controller.Open(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	if (error)
		{
		iTestStepResult = EInconclusive;
		}
	else
		{
		INFO_PRINTF1(_L("Testing the accessibiliy of DRM CustomCommand on a controller"));
		
		RMMFDRMCustomCommands drmCmd(controller);
		TInt err = drmCmd.EvaluateIntent(ContentAccess::EPeek);
		INFO_PRINTF2(_L("RMMFDRMCustomCommands::EvaluateIntent returns [%d]"), err);
		TEST(err == KErrNotSupported);
		
		RMMFVideoDRMExtCustomCommands videoDrmCmd(controller);
		CFbsBitmap dummyBitmap;
		TRequestStatus dummyStatus;
		videoDrmCmd.GetFrame(dummyBitmap, ContentAccess::EPeek, dummyStatus);
		INFO_PRINTF2(_L("RMMFVideoDRMExtCustomCommands::GetFrame returns [%d]"), dummyStatus.Int());
		TEST(dummyStatus.Int() == KErrNotSupported);
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return iTestStepResult;
	}
//-----------------------------------------------------------//

CTestStep_MMF_CTLFRM_SDRM_U_0039::CTestStep_MMF_CTLFRM_SDRM_U_0039()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0039");
	}


TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0039::DoTestStepL( void )
/** 
 * Call the RMMFController::OpenInSecureDRMProcess(..) with the parameters specified, 
 * varify the return value is 0. Test the accessibility of custom coammands 
 * with RMMFDRMCustomCommands and RMMFVideoDRMExtCustomCommands with the opened controller
 * Varify 'error' is 0 and .

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	INFO_PRINTF1(_L("Attempting to Open Controller"));

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	if (error)
		{
		iTestStepResult = EInconclusive;
		}
	else
		{
		INFO_PRINTF1(_L("Testing the accessibiliy of DRM CustomCommand on a controller"));
		
		RMMFDRMCustomCommands drmCmd(controller);
		TInt err = drmCmd.EvaluateIntent(ContentAccess::EPeek);
		INFO_PRINTF2(_L("RMMFDRMCustomCommands::EvaluateIntent returns [%d]"), err);
		TEST(err == KErrPermissionDenied);
		
		RMMFVideoDRMExtCustomCommands videoDrmCmd(controller);
		CFbsBitmap dummyBitmap;
		TRequestStatus dummyStatus;
		videoDrmCmd.GetFrame(dummyBitmap, ContentAccess::EPeek, dummyStatus);
		INFO_PRINTF2(_L("RMMFVideoDRMExtCustomCommands::GetFrame returns [%d]"), dummyStatus.Int());
		TEST(dummyStatus.Int() == KErrPermissionDenied);
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return iTestStepResult;
	}

//-----------------------------------------------------------//

//-----------------------------------------------------------
// Testing of the Standard Custom Commands
//-----------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0040::CTestStep_MMF_CTLFRM_SDRM_U_0040()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0040");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0040::DoTestStepL( void )
/** Call the RMMFController::OpenInSecureDRMProcess(..) (with the parameters from test 001), varify the 
 *  return value is 0. Instantiate a CustomCommands Object (specified). Call RMMFController 
 *  Method specified and varify the 'error' is 0. Follow this by calling the  Custom 
 *  Command 'KLogFunction' and varify the text value returned is equal to the specifed text 
 *  in Test Step. Call the RMMFController::Close() on the Custom Controller. Varify 'error' 
 *  is zero.

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MapdSetVolumeL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	TInt volume = 27;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioPlayDeviceCustomCommands customCommands(controller);

	error = customCommands.SetVolume(volume);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
 		INFO_PRINTF2(_L("Log:%S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0041::CTestStep_MMF_CTLFRM_SDRM_U_0041()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0041");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0041::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MapdGetMaxVolumeL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	TInt maxVolume = 0;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioPlayDeviceCustomCommands customCommands(controller);

	error = customCommands.GetMaxVolume(maxVolume);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log:%S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//


	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0042::CTestStep_MMF_CTLFRM_SDRM_U_0042()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0042");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0042::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MapdGetVolumeL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	TInt volume = 0;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioPlayDeviceCustomCommands customCommands(controller);

	error = customCommands.GetVolume(volume);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log:%S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0043::CTestStep_MMF_CTLFRM_SDRM_U_0043()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0043");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0043::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MapdSetVolumeRampL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	const TInt64 interval = 10;
	TTimeIntervalMicroSeconds rampData(interval);

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioPlayDeviceCustomCommands customCommands(controller);

	error = customCommands.SetVolumeRamp(rampData);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log:%S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0044::CTestStep_MMF_CTLFRM_SDRM_U_0044()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0044");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0044::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MapdSetBalanceL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	TInt balance = 5;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioPlayDeviceCustomCommands customCommands(controller);

	error = customCommands.SetBalance(balance);


	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log:%S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//


	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0045::CTestStep_MMF_CTLFRM_SDRM_U_0045()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0045");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0045::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MapdGetBalanceL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	TInt balance = 5;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioPlayDeviceCustomCommands customCommands(controller);

	customCommands.GetBalance(balance);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log:%S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//


	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0050::CTestStep_MMF_CTLFRM_SDRM_U_0050()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0050");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0050::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MardSetGainL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	TInt gain = 5;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioRecordDeviceCustomCommands customCommands(controller);
	error = customCommands.SetGain(gain);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log:%S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0051::CTestStep_MMF_CTLFRM_SDRM_U_0051()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0051");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0051::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MardGetMaxGainL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	TInt maxGain = 0;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioRecordDeviceCustomCommands customCommands(controller);
	error = customCommands.GetMaxGain(maxGain);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log:%S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	INFO_PRINTF2(_L("Max Gain: %d"),maxGain);


	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0052::CTestStep_MMF_CTLFRM_SDRM_U_0052()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0052");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0052::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MardGetGainL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	TInt gain = 0;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioRecordDeviceCustomCommands customCommands(controller);

	error = customCommands.GetGain(gain);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log:%S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	INFO_PRINTF2(_L("Gain: %d"),gain);


	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0053::CTestStep_MMF_CTLFRM_SDRM_U_0053()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0053");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0053::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MardSetBalanceL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	TInt balance = 5;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioRecordDeviceCustomCommands customCommands(controller);
	error = customCommands.SetBalance(balance);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log:%S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0054::CTestStep_MMF_CTLFRM_SDRM_U_0054()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0054");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0054::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MardGetBalanceL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	TInt balance = 0;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioRecordDeviceCustomCommands customCommands(controller);
	error = customCommands.GetBalance(balance);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log:%S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	INFO_PRINTF2(_L("Balance: %d"),balance);

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0060::CTestStep_MMF_CTLFRM_SDRM_U_0060()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0060");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0060::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MapdSetPlaybackWindowL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	const TInt64 interval1 = 10;
	TTimeIntervalMicroSeconds start(interval1);

	const TInt64 interval2 = 25;
	TTimeIntervalMicroSeconds end(interval2);

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioPlayControllerCustomCommands customCommands(controller);
	error = customCommands.SetPlaybackWindow(start,end);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log:%S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0061::CTestStep_MMF_CTLFRM_SDRM_U_0061()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0061");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0061::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MapdDeletePlaybackWindowL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioPlayControllerCustomCommands customCommands(controller);
	error = customCommands.DeletePlaybackWindow();

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0070::CTestStep_MMF_CTLFRM_SDRM_U_0070()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0070");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0070::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MarcGetRecordTimeAvailableL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	const TInt64 interval = 0;
	TTimeIntervalMicroSeconds recordTime(interval);

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioRecordControllerCustomCommands customCommands(controller);
	error = customCommands.GetRecordTimeAvailable(recordTime);

	INFO_PRINTF2(_L("Record Time Available %d"),recordTime.Int64());

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}

//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0071::CTestStep_MMF_CTLFRM_SDRM_U_0071()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0071");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0071::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MarcSetMaxDurationL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	const TInt64 interval = 25;
	TTimeIntervalMicroSeconds maxDuration(interval);

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioRecordControllerCustomCommands customCommands(controller);
	error = customCommands.SetMaxDuration(maxDuration);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0072::CTestStep_MMF_CTLFRM_SDRM_U_0072()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0072");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0072::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MarcSetMaxFileSizeL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	const TInt fileSize = 10;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioRecordControllerCustomCommands customCommands(controller);
	error = customCommands.SetMaxFileSize(fileSize);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//


	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0073::CTestStep_MMF_CTLFRM_SDRM_U_0073()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0073");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0073::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MarcCropL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioRecordControllerCustomCommands customCommands(controller);
	error = customCommands.Crop(ETrue);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);


		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0074::CTestStep_MMF_CTLFRM_SDRM_U_0074()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0074");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0074::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MarcAddMetaDataEntryL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	CMMFMetaDataEntry* metaEntry = CMMFMetaDataEntry::NewL();
	CleanupStack::PushL(metaEntry);

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioRecordControllerCustomCommands customCommands(controller);
	customCommands.AddMetaDataEntryL(*metaEntry);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	CleanupStack::PopAndDestroy();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0075::CTestStep_MMF_CTLFRM_SDRM_U_0075()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0075");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0075::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MarcRemoveMetaDataEntryL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	TInt index = 1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioRecordControllerCustomCommands customCommands(controller);
	error = customCommands.RemoveMetaDataEntry(index);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0076::CTestStep_MMF_CTLFRM_SDRM_U_0076()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0076");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0076::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "MarcReplaceMetaDataEntryL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	const TInt index = 1;
	CMMFMetaDataEntry* metaEntry = CMMFMetaDataEntry::NewL();
	CleanupStack::PushL(metaEntry);

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioRecordControllerCustomCommands customCommands(controller);
	customCommands.ReplaceMetaDataEntryL(index, *metaEntry);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	CleanupStack::PopAndDestroy();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0080::CTestStep_MMF_CTLFRM_SDRM_U_0080()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0080");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0080::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	const TUint sample = 1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioControllerCustomCommands customCommands(controller);
	error = customCommands.SetSourceSampleRate(sample);
	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0081::CTestStep_MMF_CTLFRM_SDRM_U_0081()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0081");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0081::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	const TUint numChannels = 1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioControllerCustomCommands customCommands(controller);
	error = customCommands.SetSourceNumChannels(numChannels);

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0082::CTestStep_MMF_CTLFRM_SDRM_U_0082()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0082");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0082::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	TUid uid = {0x12345678};

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioControllerCustomCommands customCommands(controller);
	error = customCommands.SetSourceFormat(uid);

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0083::CTestStep_MMF_CTLFRM_SDRM_U_0083()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0083");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0083::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	TUint sampleRate = 5;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioControllerCustomCommands customCommands(controller);
	error = customCommands.SetSinkSampleRate(sampleRate);

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0084::CTestStep_MMF_CTLFRM_SDRM_U_0084()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0084");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0084::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	TUint numChannels = 1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioControllerCustomCommands customCommands(controller);
	error = customCommands.SetSinkNumChannels(numChannels);

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0085::CTestStep_MMF_CTLFRM_SDRM_U_0085()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0085");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0085::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	TUid sinkUid = {0x12345679};
	CMMFMetaDataEntry* metaEntry = CMMFMetaDataEntry::NewL();
	CleanupStack::PushL(metaEntry);

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioControllerCustomCommands customCommands(controller);
	error = customCommands.SetSinkFormat(sinkUid);

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	CleanupStack::PopAndDestroy(1);

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0086::CTestStep_MMF_CTLFRM_SDRM_U_0086()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0086");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0086::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0040
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	TFourCC sourceFourCC(' ','P','1','6');
	TFourCC sinkFourCC(' ','P','1','6');

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	RMMFAudioControllerCustomCommands customCommands(controller);
	error = customCommands.SetCodec(sourceFourCC, sinkFourCC);

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();


	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------



CTestStep_MMF_CTLFRM_SDRM_U_0100::CTestStep_MMF_CTLFRM_SDRM_U_0100()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0100");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0100::DoTestStepL( void )
/** This test is to add and remove multiple Sources and Sinks and ensure this is 
 *  carried out correctly.

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	const TDesC8& initData = KInitData;

	CArrayFixFlat<TMMFMessageDestination>* sinkHandles = new (ELeave) CArrayFixFlat<TMMFMessageDestination>(6);
	CleanupStack::PushL(sinkHandles);
	CArrayFixFlat<TMMFMessageDestination>* sourceHandles = new (ELeave) CArrayFixFlat<TMMFMessageDestination>(3);
	CleanupStack::PushL(sourceHandles);


	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

		// Add Sink
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSink"));
		TMMFMessageDestination& sinkHandle = sinkHandles->ExtendL();
		error = controller.AddDataSink(KTestDataSinkUid, initData, sinkHandle);   // index = 1

		TMMFMessageDestination& sinkHandle2= sinkHandles->ExtendL();
		error = controller.AddDataSink(KTestDataSinkBUid, initData, sinkHandle2);  // index = 2
		
		TMMFMessageDestination& sinkHandle3 = sinkHandles->ExtendL();
		error = controller.AddDataSink(KTestDataSinkUid, initData, sinkHandle3);   // index = 3

		TMMFMessageDestination& sinkHandle4 = sinkHandles->ExtendL();
		error = controller.AddDataSink(KTestDataSinkBUid, initData, sinkHandle4);  // index = 4

		TMMFMessageDestination& sourceHandle = sourceHandles->ExtendL();
		error = controller.AddDataSource(KTestDataSourceUid, initData, sourceHandle);  // index = 1

		TMMFMessageDestination& sourceHandle2 = sourceHandles->ExtendL();
		error = controller.AddDataSource(KTestDataSourceUid, initData, sourceHandle2);  // index = 2

		TMMFMessageDestination& sourceHandle3 = sourceHandles->ExtendL();
		error = controller.AddDataSource(KTestDataSourceBUid, initData, sourceHandle3); // index = 3

		TMMFMessageDestination& sinkHandle5 = sinkHandles->ExtendL();
		error = controller.AddDataSink(KTestDataSinkUid, initData, sinkHandle5); // index = 5

		TMMFMessageDestination& sinkHandle6 = sinkHandles->ExtendL();
		error = controller.AddDataSink(KTestDataSinkUid, initData, sinkHandle6); // index = 6
		}

	if (!error)
		{
		// Remove two Sinks and a Source
		error = controller.RemoveDataSink((*sinkHandles)[2]);

		error = controller.RemoveDataSource((*sourceHandles)[2]);

		error = controller.RemoveDataSink((*sinkHandles)[5]);

		}

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	CleanupStack::PopAndDestroy(2);

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0110::CTestStep_MMF_CTLFRM_SDRM_U_0110()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0110");
	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0110::DoTestStepL( void )
/** Verfiy CMMFDescriptorBuffer creation and deletion using default buffer size
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	// Open a Custom Controller

	INFO_PRINTF1(_L("Controller Instantiation Fail"));
	__UHEAP_FAILNEXT(1); // Make the Controller Instantiation fail
	__MM_HEAP_MARK;
	TRAP(error, controller.OpenInSecureDRMProcess(KTstControllerUid,settings));
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	__MM_HEAP_MARKEND;

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0111::CTestStep_MMF_CTLFRM_SDRM_U_0111()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0111");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0111::DoTestStepL( void )
/** Verfiy CMMFDescriptorBuffer creation and deletion using default buffer size
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "AddDataSourceL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TDesC8& initData = KInitData;

	// Add Source
	INFO_PRINTF1(_L("Attempting to add a Source and a Sink to Controller"));
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSource"));
		__UHEAP_FAILNEXT(1);
		error = controller.AddDataSource(KTestDataSourceUid, initData);
		}

	INFO_PRINTF2(_L("Controller Status: %d"), error);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;

	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//
	
	
	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0112::CTestStep_MMF_CTLFRM_SDRM_U_0112()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0112");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0112::DoTestStepL( void )
/** Verfiy CMMFDescriptorBuffer creation and deletion using default buffer size
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedResult, "AddDataSinkL Called");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	error = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	const TDesC8& initData = KInitData;

	// Add Sink
	if (!error)
		{
		INFO_PRINTF1(_L("API TEST: Calling AddDataSink"));
		__UHEAP_FAILNEXT(1);
		error = controller.AddDataSink(KTestDataSinkUid, initData);
		}

		
	INFO_PRINTF2(_L("Controller Status: %d"), error);

	//------------------------------------//
	// Getting Log from Custom Controller //
	//------------------------------------//
	if (!error)
		{
		INFO_PRINTF1(_L("Getting Log"));

		TBuf8<64> memFunctionText;
	
		TUid uid = {KMmfTestControllerUid};
		TMMFMessageDestination handleInfo(uid);
		TMMFMessageDestinationPckg messageDest(handleInfo);

		error = controller.CustomCommandSync(messageDest, KLogFunction, KNullDesC8, KNullDesC8, memFunctionText);
		
		TBuf<64> memFunctionText16;
		memFunctionText16.Copy(memFunctionText);
		INFO_PRINTF2(_L("Log: %S"),&memFunctionText16);

		if (memFunctionText != KExpectedResult)
			{
			verdict = EFail;
			}

		}
	//------------------------------------//

	INFO_PRINTF1(_L("Closing Controller"));
	controller.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
 
	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0120::CTestStep_MMF_CTLFRM_SDRM_U_0120()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0120");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0120::DoTestStepL( void )
/** Instantiate a CMMFControllerPluginSelectionParameters and CMMFFormatSelectionParameters 
 *  set the parameters appropiately (See API Method Calls and Parameters) to obtain 
 *  controller Implementation Uid's in a CMMFControllerImplementationInformation Array to 
 *  open a controller. Open the controller, check for errors and close the controller. 
 *  (Test CMMFFormatSelectionParameters API).

 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data

	//---------------------//
	// Method being Tested //
	//---------------------//
	fSelect->SetMatchToFileNameL(_L("test.pav"));
	//---------------------//

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller
	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect

	INFO_PRINTF1(_L("Closing Controller"));
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0121::CTestStep_MMF_CTLFRM_SDRM_U_0121()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0121");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0121::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0120
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	//---------------------//
	// Method being Tested //
	//---------------------//
	_LIT8(KMimeData, "audio/mbeg");
	fSelect->SetMatchToMimeTypeL(KMimeData);
	//--------------------//

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect);

	cSelect->SetPreferredSupplierL(_L("Symbian"), CMMFPluginSelectionParameters::EPreferredSupplierPluginsFirstInList);

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers

	// Check Array Size here
	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller
	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect

	INFO_PRINTF1(_L("Closing Controller"));
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0122::CTestStep_MMF_CTLFRM_SDRM_U_0122()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0122");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0122::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0120
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	//---------------------//
	// Method being Tested //
	//---------------------//
	_LIT8(KHeaderData, "RIFF????PAVE");
	fSelect->SetMatchToHeaderDataL(KHeaderData); 
	//--------------------//

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect);

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller
	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0123::CTestStep_MMF_CTLFRM_SDRM_U_0123()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0123");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0123::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0120
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();


	_LIT8(KHeaderData, "RIFF????PAVE");
	fSelect->SetMatchToHeaderDataL(KHeaderData); 


	//---------------------//
	// Method being Tested //
	//---------------------//
	fSelect->MatchData();
	//---------------------//

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect);

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller
	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0124::CTestStep_MMF_CTLFRM_SDRM_U_0124()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0124");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0124::DoTestStepL( void )
/**As TSU_MMF_CTLFRM_SDRM_U_0120
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	_LIT8(KHeaderData, "RIFF????PAVE");
	fSelect->SetMatchToHeaderDataL(KHeaderData); 

	//---------------------//
	// Method being Tested //
	//---------------------//
//	CMMFFormatSelectionParameters::TMatchDataType dataType;	//EABI warning removal
//	dataType = fSelect->MatchDataType();
	//--------------------//

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect);

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller
	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//---------------------------------------------------------------



CTestStep_MMF_CTLFRM_SDRM_U_0130::CTestStep_MMF_CTLFRM_SDRM_U_0130()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0130");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0130::DoTestStepL( void )
/** Instantiate a CMMFControllerPluginSelectionParameters and CMMFFormatSelectionParameters 
 *  set the parameters appropiately (See API Method Calls and Parameters) to obtain 
 *  controller Implementation Uid's in a CMMFControllerImplementationInformation Array to 
 *  open a controller. Check the details obtained to open the controller are correct. Open 
 *  the controller, check for errors and close the controller. 
 *  (Test CMMFControllerPluginSelectionParameters API).

 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	//_LIT8(KHeaderData, "RIFF????PAVE");
	//fSelect->SetMatchToHeaderDataL(KHeaderData);
	fSelect->SetMatchToFileNameL(_L("test.pav"));

	//---------------------//
	// Method being Tested //
	//---------------------//
	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect);
	//--------------------//

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	TUid controllerUid;
	controllerUid = controllers[0]->Uid();
	
	if (controllerUid == KTstControllerUid)
		{
		verdict = EPass;
		}
	else
		{
		verdict = EFail;
		}

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}


//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0131::CTestStep_MMF_CTLFRM_SDRM_U_0131()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0131");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0131::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0130

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;


	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	fSelect->SetMatchToFileNameL(_L("test.ai"));

	//---------------------//
	// Method being Tested //
	//---------------------//
	// Set the controller plugin play format match data
	cSelect->SetRequiredRecordFormatSupportL(*fSelect);
	//--------------------//

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	TUid controllerUid;
	controllerUid = controllers[0]->Uid();
	
	if (controllerUid == KTstControllerUid)
		{
		verdict = EPass;
		}
	else
		{
		verdict = EFail;
		}

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}


//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0140::CTestStep_MMF_CTLFRM_SDRM_U_0140()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0140");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0140::DoTestStepL( void )
/** Instantiate a CMMFControllerPluginSelectionParameters and CMMFFormatSelectionParameters 
 *  set the parameters appropiately (See API Method Calls and Parameters) to obtain 
 *  controller Implementation Uid's in a CMMFControllerImplementationInformation Array to 
 *  open a controller. Check the details obtained to open the controller are correct. Open 
 *  the controller, check for errors and close the controller. 
 *  (Test CMMFPluginImplementationInformation API)

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data

	fSelect->SetMatchToFileNameL(_L("test.pav"));

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	//---------------------//
	// Method being Tested //
	//---------------------//
	TUid controllerUid;
	controllerUid = controllers[0]->Uid();
	
	if (controllerUid == KTstControllerUid)
		{
		verdict = EPass;
		}
	else
		{
		verdict = EFail;
		}
	//--------------------//


	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}


//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0141::CTestStep_MMF_CTLFRM_SDRM_U_0141()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0141");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0141::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0140

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	_LIT(KExpectedName,"CMMFTestController");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;


	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data

	//---------------------//
	// Method being Tested //
	//---------------------//
	fSelect->SetMatchToFileNameL(_L("test.pav"));
	//---------------------//

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	//---------------------//
	// Method being Tested //
	//---------------------//
	TPtrC controllerName;
	controllerName.Set(controllers[0]->DisplayName());
	INFO_PRINTF2(_L("Controller Name: %S"),&controllerName);

	if (controllerName == KExpectedName)
		{
		verdict = EPass;
		}
	else
		{
		verdict = EFail;
		}
	//--------------------//

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}


//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0142::CTestStep_MMF_CTLFRM_SDRM_U_0142()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0142");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0142::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0140

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	_LIT(KExpectedSupplier, "TSU_MMF_CTLFRM");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data

	fSelect->SetMatchToFileNameL(_L("test.pav"));

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	//---------------------//
	// Method being Tested //
	//---------------------//
	TPtrC supplier;

	supplier.Set(controllers[0]->Supplier());

	INFO_PRINTF2(_L("Controller Name: %S"),&supplier);
	TPtrC expectedSupplierPtrC;
	expectedSupplierPtrC.Set(KExpectedSupplier);

	if (*supplier.Ptr() == *expectedSupplierPtrC.Ptr())
		{
		verdict = EPass;
		}
	else
		{
		verdict = EFail;
		}
	//--------------------//

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0143::CTestStep_MMF_CTLFRM_SDRM_U_0143()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0143");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0143::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0140

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	// Expected result
	TInt expectVersionNo = 3;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;


	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data

	//---------------------//
	// Method being Tested //
	//---------------------//
	fSelect->SetMatchToFileNameL(_L("test.pav"));
	//---------------------//

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // instantiate the controller


	//---------------------//
	// Method being Tested //
	//---------------------//
	TInt versionNo;
	versionNo = controllers[0]->Version();
	INFO_PRINTF2(_L("Controller Name:%d"),versionNo);

	if (expectVersionNo != versionNo)
		{
		verdict = EFail;
		}
	//--------------------//


	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}


//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0144::CTestStep_MMF_CTLFRM_SDRM_U_0144()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0144");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0144::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0140

 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data

	fSelect->SetMatchToFileNameL(_L("test.pav"));

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	//---------------------//
	// Method being Tested //
	//---------------------//
	const RArray<TUid>& mediaIds = controllers[0]->SupportedMediaIds();

	TUid controllerUid = mediaIds[0];
	if (controllerUid == KExpectedMediaIDUid140)
		{
		verdict = EPass;
		}
	else
		{
		verdict = EFail;
		}
	//--------------------//

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}
		
	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0145::CTestStep_MMF_CTLFRM_SDRM_U_0145()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0145");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0145::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0140

 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data

	//---------------------//
	// Method being Tested //
	//---------------------//
	fSelect->SetMatchToFileNameL(_L("test.pav"));
	//---------------------//

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	//---------------------//
	// Method being Tested //
	//---------------------//
	TBool supplierSupported = controllers[0]->SupportsSupplier(_L("TSU_MMF_CTLFRM"));

	if (!supplierSupported)
		{
		verdict = EFail;
		}
	//--------------------//

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0146::CTestStep_MMF_CTLFRM_SDRM_U_0146()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0146");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0146::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0140
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data

	fSelect->SetMatchToFileNameL(_L("test.pav"));

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	//---------------------//
	// Method being Tested //
	//---------------------//
	TBool mediaSupported = controllers[0]->SupportsMediaId(KExpectedMediaIDUid140);
	
	if (!mediaSupported)
		{
		verdict = EFail;
		}
	//--------------------//

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0150::CTestStep_MMF_CTLFRM_SDRM_U_0150()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0150");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0150::DoTestStepL( void )
/** Instanciate a CMMFControllerPluginSelectionParameters and CMMFFormatSelectionParameters 
 *  set the parameters appropiately (See API Method Calls and Parameters) to obtain 
 *  controller Implementation Uid's in a CMMFControllerImplementationInformation Array to 
 *  open a controller. Check the details obtained to open the controller are correct. Open 
 *  the controller, check for errors and close the controller. 
 *  (Test CMMFControllerImplementationInformation API)

 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data

	fSelect->SetMatchToFileNameL(_L("test.pav"));

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers

	//---------------------//
	// Method being Tested //
	//---------------------//
	RMMFFormatImplInfoArray controllersPlayFormats = controllers[0]->PlayFormats();
	INFO_PRINTF2(_L("Number of Play Formats: %d"),controllersPlayFormats.Count());

	TUid formatUid = controllersPlayFormats[0]->Uid();

	if (formatUid == KMmfUidFormatPAVReadUid || 
		formatUid == KMmfUidFormatFAWReadUid ||
		formatUid == KMmfUidFormatAIReadUid  ||
		formatUid == KMmfUidFormatMBEGReadUid)
		{
		verdict = EPass;
		}
	else
		{
		verdict = EFail;
		}
	//--------------------//

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0151::CTestStep_MMF_CTLFRM_SDRM_U_0151()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0151");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0151::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0150
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data
	fSelect->SetMatchToFileNameL(_L("test.ai"));

	// Set the controller plugin play format match data
	cSelect->SetRequiredRecordFormatSupportL(*fSelect); 

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers); // Populates the array with all the suitable controllers

	//---------------------//
	// Method being Tested //
	//---------------------//
	RMMFFormatImplInfoArray controllersRecordFormats = controllers[0]->RecordFormats();
	INFO_PRINTF2(_L("Number of Record Formats: %d"),controllersRecordFormats.Count());

	TUid formatUid = controllersRecordFormats[0]->Uid();

	if (formatUid == KMmfUidFormatPAVWriteUid  || 
		formatUid == KMmfUidFormatFAWWriteUid ||
		formatUid == KMmfUidFormatAIWriteUid)
		{
		verdict = EPass;
		}
	else
		{
		verdict = EFail;
		}
	//--------------------//

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0197::CTestStep_MMF_CTLFRM_SDRM_U_0197()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0197");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0197::DoTestStepL( void )
/** As TSU_MMF_CTLFRM_SDRM_U_0140

 * Use case: N/A
 * @test Req.
 */
	{
	
	_LIT8(KExpectedText, "rtsp");
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data
	fSelect->SetMatchToUriSupportL(_L("rtsp://www.symbian.com/first/test/directory/test.rm"));
	
	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	//---------------------//
	// Method being Tested //
	//---------------------//
	const CDesC8Array* uriSchemes = &controllers[0]->SupportedUriSchemes();

	if (uriSchemes->Count() > 0)
    	{
		TPtrC8 ptrC;
		ptrC.Set(uriSchemes->MdcaPoint(0));
		
		if (ptrC.Compare(KExpectedText)==0)
			{
			verdict = EPass;
			}
		else
			{
			verdict = EFail;
			}
    	}

	//--------------------//

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}
		
	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}
	
		
	
//------------------------------------------------------------



//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0152::CTestStep_MMF_CTLFRM_SDRM_U_0152()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0152");
	}






TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0152::DoTestStepL( void )
/** 
 * Call the RMMFController::OpenInSecureDRMProcess(..) for a number of controllers and determine what Heap size has been allocated.
 *
 * This test needs access to the private member iSubThread in RMMFControllerProxy. To achieve this,
 * private has been #defined to public oprior to including MmfControllerFramework.h and them #undef.
 * This avoided the use of a friend declaration, but may later result in a maintenance problem.
 *
 * Test for fix to INC038123
 */
	{

	INFO_PRINTF1(_L("The test step is not available on EKA2 - Skipping test!"));
	return EPass;

	}


//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0160::CTestStep_MMF_CTLFRM_SDRM_U_0160()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0160");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0160::DoTestStepL( void )
/** 
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedText, ".ai");
	

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data
	fSelect->SetMatchToFileNameL(_L("test.ai"));


	// Set the controller plugin play format match data
	cSelect->SetRequiredRecordFormatSupportL(*fSelect); 

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers); // Populates the array with all the suitable controllers
	if (controllers.Count() > 0 )
		{
		RMMFFormatImplInfoArray controllersRecordFormats = controllers[0]->RecordFormats();
		INFO_PRINTF2(_L("Number of Record Formats: %d"),controllersRecordFormats.Count());
		for (TInt recordFormatCount = 0;recordFormatCount < controllersRecordFormats.Count();recordFormatCount++)
			{
			//---------------------//
			// Method being Tested //
			const CDesC8Array* fileExtensions = &controllersRecordFormats[recordFormatCount]->SupportedFileExtensions();
			//---------------------//
		 	if (fileExtensions->Count() > 0)
            	{
				TPtrC8 ptrC;
				ptrC.Set(fileExtensions->MdcaPoint(0));
				if (ptrC.Compare(KExpectedText)==0)
					{
					verdict = EPass;
					break;
					}
				else
					{
					verdict = EFail;
					}
            	}
			}	
		}
	else
		{
		verdict = EFail;
		}
	if (controllers.Count() >0)//if loop added so that it will enter into loop only if Controller count >0
		{
		User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller
		controller1.Close();
		}
	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	if (error)
		{
		verdict = EFail;
		}
	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
	return verdict;
	}

//---------------------------------------------------------------



CTestStep_MMF_CTLFRM_SDRM_U_0161::CTestStep_MMF_CTLFRM_SDRM_U_0161()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0161");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0161::DoTestStepL( void )
/** 
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedText,"audio/mbeg");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data
	fSelect->SetMatchToFileNameL(_L("test.pav"));

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 
	cSelect->SetPreferredSupplierL(KTestControllerSupplier, CMMFPluginSelectionParameters::EPreferredSupplierPluginsFirstInList);

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers); // Populates the array with all the suitable controllers

	if (controllers.Count() > 0)
		{
		RMMFFormatImplInfoArray controllersPlayFormats = controllers[0]->PlayFormats();
		INFO_PRINTF2(_L("Number of Play Formats: %d"),controllersPlayFormats.Count());

		if (controllersPlayFormats.Count() > 0)
			{
			//---------------------//
			// Method being Tested //
			const CDesC8Array* mimeTypes = &controllersPlayFormats[1]->SupportedMimeTypes();
			//---------------------//
	
			if (mimeTypes->Count() <= 0)
				{
				verdict = EFail;
				}
			else
				{
				TPtrC8 ptrC;
				ptrC.Set(mimeTypes->MdcaPoint(0));
				if (ptrC.Compare(KExpectedText) == 0)
					{
					verdict = EPass;
					}
				else
					{
					verdict = EFail;
					}
				}
			}
		else
			{
			verdict = EFail;
			}
		}
	else
		{
		verdict = EFail;
		}


	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0162::CTestStep_MMF_CTLFRM_SDRM_U_0162()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0162");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0162::DoTestStepL( void )
/** 
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedText, "RIFF????PAVE");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data
	fSelect->SetMatchToFileNameL(_L("test.pav"));

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 
	cSelect->SetPreferredSupplierL(KTestControllerSupplier, CMMFPluginSelectionParameters::EPreferredSupplierPluginsFirstInList);

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers); // Populates the array with all the suitable controllers

	//---------------------//
	// Method being Tested //
	//---------------------//
	RMMFFormatImplInfoArray controllersPlayFormats = controllers[0]->PlayFormats();
	INFO_PRINTF2(_L("Number of Play Formats: %d"),controllersPlayFormats.Count());

	if (controllersPlayFormats.Count() > 0)
		{
		//---------------------//
		// Method being Tested //
		const CDesC8Array* headerData = &controllersPlayFormats[0]->SupportedHeaderData();
		//---------------------//

		if (headerData->Count() <= 0)
			{
			verdict = EFail;
			}
		else
			{
			TPtrC8 ptrC;
			ptrC.Set(headerData->MdcaPoint(0));
			if (ptrC.Compare(KExpectedText)==0)
				{
				verdict = EPass;
				}
			else
				{
				verdict = EFail;
				}
			}
		}
	else
		{
		verdict = EFail;
		}

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0163::CTestStep_MMF_CTLFRM_SDRM_U_0163()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0163");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0163::DoTestStepL( void )
/** 
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedText, ".ai");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data
	fSelect->SetMatchToFileNameL(_L("test.ai"));

	// Set the controller plugin play format match data
	cSelect->SetRequiredRecordFormatSupportL(*fSelect); 
	cSelect->SetPreferredSupplierL(KTestControllerSupplier, CMMFPluginSelectionParameters::EPreferredSupplierPluginsFirstInList);

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers); // Populates the array with all the suitable controllers
	if (controllers.Count() > 0)
		{
		//---------------------//
		// Method being Tested //
		//---------------------//
		RMMFFormatImplInfoArray controllersRecordFormats = controllers[0]->RecordFormats();
		INFO_PRINTF2(_L("Number of Record Formats: %d"),controllersRecordFormats.Count());
		TBufC8<32> bufC8Expected;
		bufC8Expected = KExpectedText;
		for (TInt recordFormatCount=0;recordFormatCount<controllersRecordFormats.Count();recordFormatCount++)
			{
			TBool fileExtensionSupported = controllersRecordFormats[recordFormatCount]->SupportsFileExtension(bufC8Expected);
			if (fileExtensionSupported)
				{
				verdict = EPass;
				break;
				}
			else if (!fileExtensionSupported)
				{
				verdict = EFail;
				}
			}
		}
	else
		{
		verdict = EFail;
		}

	if (controllers.Count() >0) //if loop added so that it will enter into loop only if Controller count >0
		{
		User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller
		controller1.Close();
		}
	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//---------------------------------------------------------------



CTestStep_MMF_CTLFRM_SDRM_U_0164::CTestStep_MMF_CTLFRM_SDRM_U_0164()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0164");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0164::DoTestStepL( void )
/** 
 * Use case: N/A
 * @test Req.
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedText, "audio/mbeg");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data
	fSelect->SetMatchToFileNameL(_L("test.mp3"));

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 
	cSelect->SetPreferredSupplierL(KTestControllerSupplier, CMMFPluginSelectionParameters::EPreferredSupplierPluginsFirstInList);

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers); // Populates the array with all the suitable controllers

	//---------------------//
	// Method being Tested //
	//---------------------//
	RMMFFormatImplInfoArray controllersPlayFormats = controllers[0]->PlayFormats();
	INFO_PRINTF2(_L("Number of Play Formats: %d"),controllersPlayFormats.Count());

	if (controllersPlayFormats.Count() > 0)
		{
		TBufC8<32> bufC8Expected;
		bufC8Expected = KExpectedText;
		//---------------------//
		// Method being Tested //
		TBool mimeTypeSupported = controllersPlayFormats[1]->SupportsMimeType(bufC8Expected);
		//---------------------//

		if (!mimeTypeSupported)
			{
			verdict = EFail;
			}
		}
	else
		{
		verdict = EFail;
		}

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//---------------------------------------------------------------


CTestStep_MMF_CTLFRM_SDRM_U_0165::CTestStep_MMF_CTLFRM_SDRM_U_0165()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0165");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0165::DoTestStepL( void )
/** 
 * Use case: N/A
 * @test Req. 
 */
	{
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;

	TVerdict verdict = EPass;
	_LIT8(KExpectedText, "RIFF????PAVE");

	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller1;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Set the format match data
	fSelect->SetMatchToFileNameL(_L("test.pav"));

	// Set the controller plugin play format match data
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 
	cSelect->SetPreferredSupplierL(KTestControllerSupplier, CMMFPluginSelectionParameters::EPreferredSupplierPluginsFirstInList);

	// Get Controllers 
	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers); // Populates the array with all the suitable controllers

	//---------------------//
	// Method being Tested //
	//---------------------//
	RMMFFormatImplInfoArray controllersPlayFormats = controllers[0]->PlayFormats();
	INFO_PRINTF2(_L("Number of Play Formats: %d"),controllersPlayFormats.Count());

	if (controllersPlayFormats.Count() > 0)
		{
		TBufC8<32> bufC8Expected;
		bufC8Expected = KExpectedText;
		//---------------------//
		// Method being Tested //
		TBool headerDataSupported = controllersPlayFormats[0]->SupportsHeaderDataL(bufC8Expected);
		//---------------------//

		if (!headerDataSupported)
			{
			verdict = EFail;
			}
		}
	else
		{
		verdict = EFail;
		}

	User::LeaveIfError(error = controller1.OpenInSecureDRMProcess(controllers[0]->Uid(), settings)); // iinstantiate the controller

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect
	controller1.Close();

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;

	return verdict;
	}

//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0200::CTestStep_MMF_CTLFRM_SDRM_U_0200()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0200");

	
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0200::DoTestStepL( void )
/** 
 * Use case: N/A
 * @test Req.
 */
	{
	TVerdict verdict = EPass;
	TInt err = KErrNone;
	
	// Printing to the console and log file
	INFO_PRINTF1(_L("this is Alloc Failure test of RMMFController::OpenInSecureDRMProcess()"));

	TMMFPrioritySettings settings;
	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__MM_HEAP_MARK;
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		
		RMMFController controller;
		err = controller.OpenInSecureDRMProcess(KTstControllerUid,settings);

		completed = EFalse;
		if (err == KErrNone)
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				{
				User::Free(testAlloc);
				}
			controller.Close();
			completed = reachedEnd;
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		__UHEAP_SETFAIL(RHeap::ENone ,0);
		__MM_HEAP_MARKEND;

		if (completed)
			break; // exit loop

		failCount++;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
		verdict = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		verdict = EPass;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return verdict;
	}
	
//------------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_OpenController::CTestStep_MMF_CTLFRM_SDRM_U_OpenController(const TDesC& aTestName,
																					   TUid aControllerUid)
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iControllerUid = aControllerUid;
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_OpenController::DoTestStepL( void )
/** Opening test controller ts_cmmftestconstructcontroller
 *  called in its ConstructL()
 * Use case: N/A
 * @test Req. 
 */
	{
	TVerdict verdict = EPass;
	
	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	INFO_PRINTF1(_L("Attempting to Open Controller"));

	// Open a Custom Controller
	error = controller.Open(iControllerUid, settings);
	CleanupClosePushL(controller);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	if (error)
		{
		verdict = EFail;
		}
	
	INFO_PRINTF1(_L("Closing Controller"));
	CleanupStack::PopAndDestroy(&controller);
	return verdict;
	}
//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_OpenSecureController::CTestStep_MMF_CTLFRM_SDRM_U_OpenSecureController(const TDesC& aTestName,
																					   TUid aControllerUid)
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	//iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0210");
	iTestStepName = aTestName;
	iControllerUid = aControllerUid;
	}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_OpenSecureController::DoTestStepL( void )
/** Opening test controller ts_cmmftestconstructcontroller
 *  called in its ConstructL()
 * Use case: N/A
 * @test Req. 
 */
	{
	TVerdict verdict = EPass;
	
	TMMFPrioritySettings settings;
	TInt error = KErrNone;
	RMMFController controller;

	settings.iPriority = 1;
	settings.iPref = EMdaPriorityPreferenceTime;
	settings.iState = EMMFStateIdle;

	INFO_PRINTF1(_L("Attempting to Open Controller"));

	// Open a Custom Controller
	error = controller.OpenInSecureDRMProcess(iControllerUid, settings);
	CleanupClosePushL(controller);
	INFO_PRINTF2(_L("Controller Open: %d"), error);

	if (error)
		{
		verdict = EFail;
		}

	INFO_PRINTF1(_L("Closing Controller"));
	CleanupStack::PopAndDestroy(&controller);
	return verdict;
	}
//---------------------------------------------------------------

CTestStep_MMF_CTLFRM_SDRM_U_0300 ::CTestStep_MMF_CTLFRM_SDRM_U_0300 ()
/** Constructor
*/
{
// store the name of this test case
// this is the name that is used by the script file
// Each test step initialises it's own name
iTestStepName = _L("MM-MMF-CTLFRM-SDRM-U-0300");


}

TVerdict CTestStep_MMF_CTLFRM_SDRM_U_0300 ::DoTestStepL( void )
{
INFO_PRINTF1(_L("Setting UHEAP_MARK"));
INFO_PRINTF1(_L("CR1655 - Test to check that stack size has been set to 0x4000 on hw"));
INFO_PRINTF1(_L("On emulator 1MB should be fine, since stack size cannot be set"));
__MM_HEAP_MARK;

TVerdict verdict = EPass;


TMMFPrioritySettings settings;
TInt error = KErrNone;
RMMFController controller;

settings.iPriority = 1;
settings.iPref = EMdaPriorityPreferenceTime;
settings.iState = EMMFStateIdle;

// Open a Custom Controller
error = controller.OpenInSecureDRMProcess(KTstControllerUid, settings);
//CleanupClosePushL(controller);
INFO_PRINTF2(_L("Controller Open: %d"), error);

if (error)
	{
	verdict = EFail;
	}

TUint32 expectedStackSize;

#ifndef __WINSCW__
//on hardware we expect the stack size to be set to what is in the resource file
expectedStackSize=0x4000;
#else
//on emulator stack size is 1MB due to it being on a single process / windows thread
expectedStackSize=0x100000;
#endif

TUint32 stksz=0;
if (!error)
	{
	INFO_PRINTF1(_L("Calling CustomCommandSync"));
	TUid uid = {KMmfTestControllerUid};
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg messageDest(handleInfo);
	TPckg<TUint32> stacksize(0);
				
	error = controller.CustomCommandSync(messageDest, KFuncThreadStackSize, KNullDesC8, KNullDesC8,stacksize);
	stksz=stacksize();
	}


INFO_PRINTF2(_L("Stack Size is set to %d"),stksz);
INFO_PRINTF1(_L("Closing Controller"));

if(stksz!=expectedStackSize || error)
	{
	verdict=EFail;
	}
controller.Close();

INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
__MM_HEAP_MARKEND;

return verdict;
}


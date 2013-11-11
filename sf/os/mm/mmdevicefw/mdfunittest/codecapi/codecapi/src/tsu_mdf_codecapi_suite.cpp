// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// System includes
#include <e32base.h>

// Test system includes
#include "tsu_mdf_codecapi_suite.h"
#include "tsu_mdf_codecapi_step.h"
#include "tsu_mdf_codecapi_load.h"
#include "tsu_mdf_codecapi_load2.h"
#include "tsu_mdf_codecapi_play.h"
#include "tsu_mdf_codecapi_play2.h"
#include "tsu_mdf_codecapi_record.h"
#include "tsu_mdf_codecapi_resolver.h"
#include "tsu_mdf_codecapi_resolver2.h"
#include "tsu_mdf_codecapi_pu.h"
#include "tsu_mdf_codecapi_opaque.h"

/** 
 *
 * NewTestSuiteL
 *	NewTestSuite is exported at ordinal 1
 *	this provides the interface to allow schedule test
 *	to create instances of this test suite
 * @result CTestSuiteCodecApi*
 *
 */
EXPORT_C CTestSuiteCodecApi* NewTestSuiteL() 
	{ 
	CTestSuiteCodecApi* result = new (ELeave) CTestSuiteCodecApi;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
	}

/**
 *
 * CTestSuiteCodecApi
 *
 */
CTestSuiteCodecApi::CTestSuiteCodecApi() 
	{
	
	}


/**
 *
 * ~CTestSuiteCodecApi
 *
 */
CTestSuiteCodecApi::~CTestSuiteCodecApi() 
	{}

/**
 *
 * GetVersion
 *	Get Test Suite version
 * @result TPtrC
 *
 */
TPtrC CTestSuiteCodecApi::GetVersion( void ) const
	{
	_LIT(KTxtVersion,"1.00");
	return KTxtVersion();
	}



/**
 *
 * InitialiseL
 *	Constructor for test suite
 *	this creates all the test steps and 
 *	stores them inside CTestSuiteCodecApi
 *
 */
void CTestSuiteCodecApi::InitialiseL(void)
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MDF_CODECAPI");
	
	// load
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadAdapter());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCustomInterface());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCodec());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadInitCodec());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadPUDirect);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadPUDirectByUid);
	AddTestStepL(new(ELeave) RTestStepCodecApiTunnelSetup);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadAdapterDirect);
	
	// play a file
	AddTestStepL(new(ELeave) RTestStepCodecApiPlay());
	AddTestStepL(new(ELeave) RTestStepCodecApiPlayPartial());
	AddTestStepL(new(ELeave) RTestStepCodecApiPlayStopRestart());
	AddTestStepL(new(ELeave) RTestStepCodecApiPlayStopDelete());

	// record a file
	AddTestStepL(new(ELeave) RTestStepCodecApiRecord());

	// stop
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadStop());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCIStop());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCISetupStop());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCISetupInitStop());
	
	// pause
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadPause());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCIPause());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCISetupPause());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCISetupInitPause());
	
	// SDC
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadSDC());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCISDC());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCISetupSDC());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCISetupInitSDC());
	
	// DeleteCodec
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadDeleteCodec());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCIDeleteCodec());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCISetupDeleteCodec());
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCISetupInitDeleteCodec());

	// resolve
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadFourCC);
	AddTestStepL(new(ELeave) RTestStepCodecApiFindP8InputFourCC);
	AddTestStepL(new(ELeave) RTestStepCodecApiFindPU8OutputFourCC);
	AddTestStepL(new(ELeave) RTestStepCodecApiFindIncorrectInputFourCC);			
	AddTestStepL(new(ELeave) RTestStepCodecApiFindIncorrectImpUid);			
	AddTestStepL(new(ELeave) RTestStepCodecApiFindIncorrectInterfaceUid);				
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadVideoEncoder);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadVideoDecoder);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadVideoOpaqueData);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadVideoOpaqueDataInvalid);
	
	// direct PU
	AddTestStepL(new(ELeave) RTestStepCodecApiPuCreate);
	AddTestStepL(new(ELeave) RTestStepCodecApiPuCustomInterface);
	AddTestStepL(new(ELeave) RTestStepCodecApiPuGetPorts);
	AddTestStepL(new(ELeave) RTestStepCodecApiPuConfig);
	AddTestStepL(new(ELeave) RTestStepCodecApiPuCreateBuffers);
	AddTestStepL(new(ELeave) RTestStepCodecApiPuUseBuffers);
	AddTestStepL(new(ELeave) RTestStepCodecApiPuInit);
	AddTestStepL(new(ELeave) RTestStepCodecApiPuExecute);
	AddTestStepL(new(ELeave) RTestStepCodecApiPuExecuteTunnel);
	AddTestStepL(new(ELeave) RTestStepCodecApiPuPauseRestart);
	AddTestStepL(new(ELeave) RTestStepCodecApiPuPauseRestartTunnel);
	AddTestStepL(new(ELeave) RTestStepCodecApiPuStopRestart);
	AddTestStepL(new(ELeave) RTestStepCodecApiPuStopRestartTunnel);
	
	// invalid load PU
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadPUDirectInvalid);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadPUDirectByUidInvalid);
	AddTestStepL(new(ELeave) RTestStepCodecApiTunnelSetupInvalid);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadAdapterInvalid);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCustomInterfaceInvalid);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadCodecInvalid);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadInitCodecInvalid);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadInitCodec2Invalid);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadInitCodec3Invalid);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadNoOpenMax);
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadStartInvalid);
	
	// opaque data
	AddTestStepL(new(ELeave) RTestStepCodecApiOpaqueDataL);
	AddTestStepL(new(ELeave) RTestStepCodecApiOpaqueDataP);
	AddTestStepL(new(ELeave) RTestStepCodecApiOpaqueDataI);
	AddTestStepL(new(ELeave) RTestStepCodecApiOpaqueDataS);
	AddTestStepL(new(ELeave) RTestStepCodecApiOpaqueDataNone);
	
	// PDEF111147
	AddTestStepL(new(ELeave) RTestStepCodecApiLoadPUMemLeak);
	
	}
	
	



	






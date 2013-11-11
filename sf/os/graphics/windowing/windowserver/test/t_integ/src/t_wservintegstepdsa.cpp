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

/**
 @file
 @test
 @internalComponent
*/

#include "t_wservintegstepdsa.h"
#include "t_wservconsts.h"
#include "t_inidata.h"

/*@{*/
//	Data objects

_LIT(KWServDsaApp,              "z:\\sys\\bin\\t_dsaapp.exe");
_LIT(KWServTApp1,               "z:\\sys\\bin\\t_app1.exe");

_LIT(KDsaAppData,				"dsaapp_data%d");
_LIT(KDsaAppDataName,			"dsaapp_data%d_name");

_LIT(KDsaAppResultData,			"dsaappresult_data%d");
_LIT(KDsaAppResultDataName,		"dsaappresult_data%d_name");

_LIT(KTApp1Data,				"tapp1_data%d");
_LIT(KTApp1DataName,			"tapp1_data%d_name");

/*@}*/

/**
 CT_WServIntegStepDsa constructor.
 @return N/A
 @pre None
 @post CT_WServIntegStepDsa 1st stage constructor
*/
CT_WServIntegStepDsa::CT_WServIntegStepDsa()
	{
	SetTestStepName(KT_WServIntegStepDsa);
	}


enum TVerdict CT_WServIntegStepDsa::doTestStepPostambleL()
	{
	CleanUp();

	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
	
/**
Starts test step
@internalComponent
@return TVerdict pass / fail
@pre N/A
@post N/A
*/
enum TVerdict CT_WServIntegStepDsa::doTestStepL()
	{
	__UHEAP_MARK;

	GetConfigDataL(KDsaAppData(), KDsaAppDataName());
	PrintConfigDataL(KWServDsaAppConfigFile());
	TInt err = CreateTestProcessL(KWServDsaApp, EPriorityForeground);
	TEST(err==KErrNone);
	ClearConfigData();
	
	// wait for dsa app to create start file, to permit
	// creation of t_app1 process
	WaitForTestL(KWServDsaAppStartFile());

	GetConfigDataL(KTApp1Data(), KTApp1DataName());
	PrintConfigDataL(KWServTApp1ConfigFile());
	err = CreateTestProcessL(KWServTApp1, EPriorityForeground);
	TEST(err==KErrNone);
	ClearConfigData();
	
	// wait for dsa app to create finish file, to permit
	// results checking and clean up
	WaitForTestL(KWServDsaAppFinishFile());

	TRAP(err,CheckResultsL(KDsaAppResultData(), KDsaAppResultDataName(), KWServDsaAppResultFile()));
	if (err == KErrNone)
		{	
		PrintResultsL(KWServDsaAppResultFile());

		TInt expFrameRate = 0;
		READ_INT(KDsaAppMinFrameRate, KWServDsaAppConfigFile, expFrameRate);

		if (expFrameRate>0)
			{
			TInt actFrameRate = 0;
			READ_INT(KPerfDataFrameRate, KWServDsaAppResultFile, actFrameRate);
			
			// There is no need to test the measured frame rate for emulator
			#if defined __WINS__ || defined __WINSCW__
			INFO_PRINTF2(_L("Frame Rate: Measured: %d"), actFrameRate);
			#else
			INFO_PRINTF3(_L("Frame Rate: Target: %d, Measured: %d"), expFrameRate, actFrameRate);
			TEST(actFrameRate>expFrameRate);
			#endif
			}
		}

	return TestStepResult();
	}

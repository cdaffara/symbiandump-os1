// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "t_wservintegstepsurface.h"
#include "t_wservconsts.h"
#include <test/t_simload.h>

/**
 CT_WServIntegStepSurface constructor.
 @return N/A
 @pre None
 @post CT_WServIntegStepSurface 1st stage constructor
*/
CT_WServIntegStepSurface::CT_WServIntegStepSurface()
	{
	SetTestStepName(KT_WServIntegStepSurface);
	}

/**
Starts test step
@internalComponent
@return TVerdict pass / fail
@pre N/A
@post N/A
*/
enum TVerdict CT_WServIntegStepSurface::doTestStepL()
	{
	__UHEAP_MARK;

	PrintTestStepName();

	GetConfigDataL(KSimLoadData(), KSimLoadDataName());
	PrintConfigDataL(KSimLoadConfigFile());
	CreateSimLoadProcL();
	ClearConfigData();

	// Semaphore created here to maintain compatibility with DSA test use of t_app1
	RSemaphore dudSemaphore;
	User::LeaveIfError(dudSemaphore.CreateGlobal(KWservDsaSemaphoreName(), 0));

    GetConfigDataL(KTApp1Data(), KTApp1DataName());
	PrintConfigDataL(KWServTApp1ConfigFile());
	TInt err = CreateTestProcessL(KWServTApp1, EPriorityForeground);
	TEST(err==KErrNone);
	ClearConfigData();

	GetConfigDataL(KPseudoAppData(), KPseudoAppDataName());
	PrintConfigDataL(KWServPseudoAppConfigFile());
	CreatePseudoAppProcL();
	ClearConfigData();

	/***
	* Create a second process pseudoapp1.exe similar to pseudoapp.exe to test GCE
	* composing two independent surfaces at different frame rates
	*/
	TBool pseudoApp1=EFalse;
	if ( GetBoolFromConfig(ConfigSection(),KCreatePseudoApp1, pseudoApp1) )
		{
		if (pseudoApp1)
			{
			GetConfigDataL(KPseudoApp1Data(), KPseudoApp1DataName());
			PrintConfigDataL(KWServPseudoApp1ConfigFile());
			err=CreateTestProcessL(KWServPseudoApp1, EPriorityForeground);
			TEST(err==KErrNone);
			ClearConfigData();

			WaitForTestL(KWServPseudoApp1FinishFile());

			// retrieve results from pseudo app1
			CheckResultsL(KPseudoApp1ResultData(), KPseudoApp1ResultDataName(), KWServPseudoApp1ResultFile());
			PrintResultsL(KWServPseudoApp1ResultFile());
			}
		}

	WaitForTestL(KWServPseudoAppFinishFile());

	// retrieve results from pseudo app
	CheckResultsL(KPseudoAppResultData(), KPseudoAppResultDataName(), KWServPseudoAppResultFile());
	PrintResultsL(KWServPseudoAppResultFile());

	dudSemaphore.Close();
	CleanUp();

	__UHEAP_MARKEND;

	return TestStepResult();
	}

void CT_WServIntegStepSurface::CreateSimLoadProcL()
	{
	TInt simLoadAppErr = KErrNone;
	TInt index = 1;
	TBuf<KMaxTestExecuteCommandLength> tempStore;

	while (simLoadAppErr == KErrNone && index <= KNumSimLoadApps)
		{
		tempStore.Format(KSimLoadApp, index++);

		simLoadAppErr = CreateTestProcessL(tempStore, EPriorityHigh);
		}
	}

void CT_WServIntegStepSurface::CreatePseudoAppProcL()
	{
	TInt err;
	err = CreateTestProcessL(KWServPseudoApp, EPriorityForeground);
	TEST(err==KErrNone);
	}


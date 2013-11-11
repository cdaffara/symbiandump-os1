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

#include "ctlbshybridx3pstep.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CT_LbsHybridX3PStep::~CT_LbsHybridX3PStep()
	{
	iExpectedModuleGpsOptions.ResetAndDestroy();	
	iExpectedModuleGpsOptions.Close();		
	}


CT_LbsHybridX3PStep::CT_LbsHybridX3PStep(CT_LbsHybridX3PServer& aParent) 
	: iParent(aParent), iExpectedModuleGpsOptionsIndex(0), iAgpsModuleEventListener(NULL)
	{
	}

void CT_LbsHybridX3PStep::ConstructL()
	{
	}



/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsHybridX3PStep::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("doTestStepPreabmleL()"));

	// The expected module GPS options
	T_LbsUtils utils;
	TPtrC configFileName;
	_LIT(KUpdateOptionsFile, "agps_module_update_file");
	GetStringFromConfig(ConfigSection(), KUpdateOptionsFile, configFileName);
	utils.GetExpected_ModuleModes(configFileName, ConfigSection(), iExpectedModuleGpsOptions);
	if(iExpectedModuleGpsOptions.Count() > 0)
		{ // if there are some options then start listening for events from the module
		iAgpsModuleEventListener = CT_AgpsModuleEventListener::NewL(*this);
		}
	
	SetTestStepResult(EPass);

	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsHybridX3PStep::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("doTestStepPostabmleL()"));
	TEST(iExpectedModuleGpsOptionsIndex == iExpectedModuleGpsOptions.Count());
	iExpectedModuleGpsOptions.ResetAndDestroy();
	iExpectedModuleGpsOptionsIndex = 0;
	delete iAgpsModuleEventListener;
	iAgpsModuleEventListener = NULL;
	
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

void CT_LbsHybridX3PStep::OnSetGpsOptions(const TLbsGpsOptions& aGpsOptions)
/**
 * Compares the GPS options received by the AGPS module to the ones expected by the test
 */
	{
	
	INFO_PRINTF1(_L("CT_LbsHybridX3PStep::OnSetGpsOptions()"));
	if(iExpectedModuleGpsOptionsIndex >= iExpectedModuleGpsOptions.Count())
		{
		TEST(EFalse);
		return;
		}
	TLbsGpsOptions* expectedOptions = iExpectedModuleGpsOptions[iExpectedModuleGpsOptionsIndex];
	++iExpectedModuleGpsOptionsIndex;

	if(aGpsOptions.GpsMode() != expectedOptions->GpsMode())
		{
		INFO_PRINTF3(_L("CT_LbsHybridX3PStep::OnSetGpsOptions - FAILED because got unexpected mode. Got 0x%x expected 0x%x()"), aGpsOptions.GpsMode(), expectedOptions->GpsMode());
		TEST(EFalse);
		return;
		}
	if(aGpsOptions.ClassType() != expectedOptions->ClassType())
		{
		INFO_PRINTF3(_L("CT_LbsHybridX3PStep::OnSetGpsOptions - FAILED because got unexpected class type. Got 0x%x expected 0x%x()"), aGpsOptions.ClassType(), expectedOptions->ClassType());
		TEST(EFalse);
		return;
		}
	if(aGpsOptions.ClassType() & ELbsGpsOptionsArrayClass)
		{
		//INFO_PRINTF1(_L("CT_LbsHybridX3PStep::OnSetGpsOptions Got options array"));
		const TLbsGpsOptionsArray& optionsArr = reinterpret_cast<const TLbsGpsOptionsArray&>(aGpsOptions);
		const TLbsGpsOptionsArray& expectedOptionsArr = reinterpret_cast<const TLbsGpsOptionsArray&>(*expectedOptions);
		if(optionsArr.NumOptionItems() != expectedOptionsArr.NumOptionItems())
			{
			INFO_PRINTF3(_L("CT_LbsHybridX3PStep::OnSetGpsOptions - FAILED because got unexpected number of options in array. Got %d expected %d)"), optionsArr.NumOptionItems(), expectedOptionsArr.NumOptionItems());
			TEST(EFalse);
			return;
			}
		for(TInt index = 0; index < optionsArr.NumOptionItems(); ++index)
			{
			TLbsGpsOptionsItem item;
			TLbsGpsOptionsItem expectedItem;
			optionsArr.GetOptionItem(index, item);
			expectedOptionsArr.GetOptionItem(index, expectedItem);

			if(item.PosUpdateType() != expectedItem.PosUpdateType())
				{
				INFO_PRINTF3(_L("CT_LbsHybridX3PStep::OnSetGpsOptions - FAILED because got unexpected position update type. Got %d expected %d)"), item.PosUpdateType(), expectedItem.PosUpdateType());
				TEST(EFalse);
				return;
				}
			}
		}
	}

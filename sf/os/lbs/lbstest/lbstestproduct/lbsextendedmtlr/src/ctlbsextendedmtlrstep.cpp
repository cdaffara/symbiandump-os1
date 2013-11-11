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
// @file ctlbsextendedmtlrstep.cpp
// This is the class implementation for the LBS EXTENDED MTLR Test Step Base
// 
//

#include "ctlbsextendedmtlrstep.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CT_LbsExtendedMTLRStep::~CT_LbsExtendedMTLRStep()
	{
	iExpectedModuleGpsOptions.ResetAndDestroy();
	iExpectedModuleGpsOptions.Close();	
	}


CT_LbsExtendedMTLRStep::CT_LbsExtendedMTLRStep(CT_LbsExtendedMTLRServer& aParent) :iExpectedModuleGpsOptionsIndex(0), 
											iAgpsModuleEventListener(NULL), iParent(aParent)
	{
	}

void CT_LbsExtendedMTLRStep::ConstructL()
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
TVerdict CT_LbsExtendedMTLRStep::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("doTestStepPreabmleL()"));

	// Get the expected module GPS options
	T_LbsUtils utils;
	TPtrC configFileName;
	_LIT(KUpdateOptionsFile, "agps_module_update_file");
	GetStringFromConfig(ConfigSection(), KUpdateOptionsFile, configFileName);
	utils.GetExpected_ModuleModes(configFileName, ConfigSection(), iExpectedModuleGpsOptions);
	if(iExpectedModuleGpsOptions.Count() > 0)
		{ // Start listening for the GPS module options only if expecting any options
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
TVerdict CT_LbsExtendedMTLRStep::doTestStepPostambleL()
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

void CT_LbsExtendedMTLRStep::OnSetGpsOptions(const TLbsGpsOptions& aGpsOptions)
/**
 * Compares the GPS options received by the AGPS module to the ones expected by the test
 */
	{
	INFO_PRINTF1(_L("CT_LbsExtendedMTLRStep::OnSetGpsOptions()"));
	if(iExpectedModuleGpsOptionsIndex >= iExpectedModuleGpsOptions.Count())
		{
		INFO_PRINTF3(_L("CT_LbsExtendedX3PStep::OnSetGpsOptions - FAILED because got unexpected number of mode changes. Got %d expected %d"), iExpectedModuleGpsOptionsIndex -1, iExpectedModuleGpsOptions.Count());
		TEST(EFalse);
		return;
		}
	TLbsGpsOptions* expectedOptions = iExpectedModuleGpsOptions[iExpectedModuleGpsOptionsIndex];
	++iExpectedModuleGpsOptionsIndex;

	if(aGpsOptions.GpsMode() != expectedOptions->GpsMode())
		{
		INFO_PRINTF3(_L("CT_LbsExtendedX3PStep::OnSetGpsOptions - FAILED because got unexpected mode. Got 0x%x expected 0x%x()"), aGpsOptions.GpsMode(), expectedOptions->GpsMode());
		TEST(EFalse);
		return;
		}
	if(aGpsOptions.ClassType() != expectedOptions->ClassType())
		{
		TEST(EFalse);
		return;
		}
	if(aGpsOptions.ClassType() & ELbsGpsOptionsArrayClass)
		{
		const TLbsGpsOptionsArray& optionsArr = reinterpret_cast<const TLbsGpsOptionsArray&>(aGpsOptions);
		const TLbsGpsOptionsArray& expectedOptionsArr = reinterpret_cast<const TLbsGpsOptionsArray&>(*expectedOptions);
		if(optionsArr.NumOptionItems() != expectedOptionsArr.NumOptionItems())
			{
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
				TEST(EFalse);
				return;
				}
			}
		}
	}


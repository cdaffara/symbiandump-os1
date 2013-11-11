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
// @file ctlbshybridmultiplestep.cpp
// This is the class implementation for the Lbs Hybrid Multiple Test Step Base
// 
//

#include "ctlbshybridmultiplestep.h"

CT_LbsHybridMultipleStep::~CT_LbsHybridMultipleStep()
	{
	iSequences.ResetAndDestroy();
	iSequences.Close();
	
	iProxy.ResetAndDestroy();
	iProxy.Close();
	
	iExpectedModuleGpsOptions.ResetAndDestroy();
	iExpectedModuleGpsOptions.Close();	
	}


CT_LbsHybridMultipleStep::CT_LbsHybridMultipleStep(CT_LbsHybridMultipleServer& aParent) :iParent(aParent), iExpectedModuleGpsOptionsIndex(0), 
											iAgpsModuleEventListener(NULL)
	{
	}

void CT_LbsHybridMultipleStep::ConstructL()
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
TVerdict CT_LbsHybridMultipleStep::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("doTestStepPreabmleL()"));
	SetTestStepResult(EPass);

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
	
	//Read in the sequences from the testdata ini file
	TInt numberOfSequences = 0;
	GetIntFromConfig(ConfigSection(), _L("NumberOfSequences"), numberOfSequences);
	if(numberOfSequences <= 0)
		{
		INFO_PRINTF1(_L("ERROR - Trying to run the test with no sequences!"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	//Loop through the sequences carrying out the required configuration
	for(TInt i=0; i<numberOfSequences; ++i)
		{
		ReadTestDataConfig(i);
		}

	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsHybridMultipleStep::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("doTestStepPostabmleL()"));
	
	//Loop through all the sequences to print out their final status
	INFO_PRINTF1(_L("---------------------------------- Sequence Status ----------------------------------"));
	INFO_PRINTF1(_L("Still waiting for the following messages on the following sequences:"));
	for(TInt i=0; i<iSequences.Count(); ++i)
		{
		iSequences[i]->PrintRemainingActions();
		}
	
	TEST(iExpectedModuleGpsOptionsIndex == iExpectedModuleGpsOptions.Count());
	iExpectedModuleGpsOptions.ResetAndDestroy();
	iExpectedModuleGpsOptionsIndex = 0;
	delete iAgpsModuleEventListener;
	iAgpsModuleEventListener = NULL;
	
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

/** Reads in the details of a sequence from the test data ini file
 */
void CT_LbsHybridMultipleStep::ReadTestDataConfig(TInt aCurrentNum)
	{
	CTestSessionSequence* newSequence;
	
	//Get the name of the file being used
	TPtrC fileName;
	GetStringFromConfig(_L("FileName"), _L("sequence_data_file"), fileName);
	
	//Find out what Protocol Module this sequence is going to be run on
	TBuf<4> pmName;
	TInt pmNum = 0;
	pmName.Format(_L("PM%d"), aCurrentNum);
	GetIntFromConfig(ConfigSection(), pmName, pmNum);
	
	//Create the required protocol module Net Proxy and get the position of this PM in the array
	TInt position;
	position = CreateProtocolModuleProxy(pmNum);
	
	//Find out what type the sequence is
	TBuf<6> typeName;
	TPtrC sequenceType;
	typeName.Format(_L("Type%d"), aCurrentNum);
	GetStringFromConfig(ConfigSection(), typeName, sequenceType);
	
	//Create the specific test sequence type
	if(sequenceType.Find(_L("MOLR")) != KErrNotFound)	//MOLR
		{
		newSequence = CTestMolrSessionSequence::NewL(*iProxy[position], *this, aCurrentNum, fileName, ConfigSection());
		}
	else if(sequenceType.Find(_L("X3P")) != KErrNotFound)	//X3P
		{
		newSequence = CTestX3PSessionSequence::NewL(*iProxy[position], *this, aCurrentNum, fileName, ConfigSection());
		}
	else if((sequenceType.Find(_L("MTLR")) != KErrNotFound) || (sequenceType.Find(_L("NIMTLR")) != KErrNotFound))	//MTLR
		{
		newSequence = CTestMtlrSessionSequence::NewL(*iProxy[position], *this, aCurrentNum, fileName, ConfigSection());
		}
	else	//Unknown type, will get generic handling
		{
		newSequence = CTestSessionSequence::NewL(*iProxy[position], *this, aCurrentNum, fileName, ConfigSection());
		}
	
	//Add the new sequence to the array
	TRAPD(err, iSequences.AppendL(newSequence));
	if(err != KErrNone)
		{
		SetTestStepResult(EFail);
		}
	}


/** Creates the Protocol Module Proxy for the PM id past into the function. If the 
 *   protocol module proxy is already created, do nothing, just return the position
 *   of the proxy in the iProxy array
 */
CLbsTestNgMessageHandler* CT_LbsHybridMultipleStep::LookupProtocolModuleProxy(TInt aPmId)
    {
    for(TInt i=0; i<iProxy.Count(); ++i)
        {
        if(iProxy[i]->GetPmId() == aPmId)
            {
            return iProxy[i];
            }
        }
    
    // Didn't find a proxy, so return null
    return 0;
    }


/** Creates the Protocol Module Proxy for the PM id past into the function. If the 
 *   protocol module proxy is already created, do nothing, just return the position
 *   of the proxy in the iProxy array
 */
TInt CT_LbsHybridMultipleStep::CreateProtocolModuleProxy(TInt aPmId)
	{
	TInt position = KErrNotFound;
	
	//Check to see whether this PM has already been created for another sequence
	for(TInt i=0; i<iProxy.Count(); ++i)
		{
		if(iProxy[i]->GetPmId() == aPmId)
			{
			position = i;
			break;
			}
		}
	
	if(position == KErrNotFound)
		{
		//Create the new Protocol Proxy for this new Protocol Module
		CLbsTestNgMessageHandler* pmProxy = CLbsTestNgMessageHandler::NewL(*this, aPmId);		
		iProxy.AppendL(pmProxy);
		position = iProxy.Count() - 1;
		}
	
	return position;
	}


void CT_LbsHybridMultipleStep::OnSetGpsOptions(const TLbsGpsOptions& aGpsOptions)
/**
 * Compares the GPS options received by the AGPS module to the ones expected by the test
 */
	{
	INFO_PRINTF1(_L("CT_LbsHybridMTLRStep::OnSetGpsOptions()"));
	if(iExpectedModuleGpsOptionsIndex >= iExpectedModuleGpsOptions.Count())
		{
		INFO_PRINTF3(_L("CT_LbsHybridX3PStep::OnSetGpsOptions - FAILED because got unexpected number of mode changes. Got %d expected %d"), iExpectedModuleGpsOptionsIndex -1, iExpectedModuleGpsOptions.Count());
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


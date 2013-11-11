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
// Example CTestStep derived implementation
// 
//

/**
 @file cmiscstep1.cpp
*/

#include <lbspositioninfo.h>
#include "cconfigmanager.h"
#include "cmiscstep1.h"
#include "te_testprotocolmodulesuitedefs.h"


_LIT(KTestDataIntStart, "Uid");
const TInt KDataItemNameMaxLen = 5;

/** Destructor
*/
CMiscStep1::~CMiscStep1()
	{
	}


/** Constructor
*/
CMiscStep1::CMiscStep1()
	{
	SetTestStepName(KMiscStep1);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CMiscStep1::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CMiscStep1 test step.
This tests the loading and look-up of external configuration data
provided in an external central repository file.

@return TVerdict test result code
*/
TVerdict CMiscStep1::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMiscellaneous - external configuration data loading and lookup"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

	INFO_PRINTF1(_L("\tLoading configuration..."));
	// Create config manager - which automatically loads default external data
	CConfigManager* config = CConfigManager::NewL();

	// Check this worked
	if (NULL == config)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	INFO_PRINTF1(_L("\tLoaded"));
	CleanupStack::PushL(config);

	INFO_PRINTF1(_L("\tCheck each loaded decision value"));
	// For each row - current operation
	for (TInt row = 0; row < KMaxOperationTypes; row++)
		{
		// Create an operation object
		CConfigManager::TConflictOperation currentOp;
		switch (row)
			{
		case KMoLr:
			INFO_PRINTF1(_L("\tMO-LR as current operation..."));
			currentOp.iOperation = CConfigManager::EOpMoLr;
			currentOp.iPriority = CConfigManager::EPriorityNone;
			break;
		case KMtLr:
			INFO_PRINTF1(_L("\tMT-LR as current operation..."));
			currentOp.iOperation = CConfigManager::EOpMtLr;
			currentOp.iPriority = CConfigManager::EPriorityNone;
			break;
		case KEmergency:
			INFO_PRINTF1(_L("\tEmergency MT-LR as current operation..."));
			currentOp.iOperation = CConfigManager::EOpMtLr;
			currentOp.iPriority = CConfigManager::EPriorityEmergency;
			break;
		case KX3pLow:
			INFO_PRINTF1(_L("\tLow Priority X3P as current operation..."));
			currentOp.iOperation = CConfigManager::EOpX3p;
			currentOp.iPriority = CConfigManager::EPriorityLow;
			break;
		case KX3pMid:
			INFO_PRINTF1(_L("\tMedium Priority X3P as current operation..."));
			currentOp.iOperation = CConfigManager::EOpX3p;
			currentOp.iPriority = CConfigManager::EPriorityMedium;
			break;
		case KX3pHi:
			INFO_PRINTF1(_L("\tHigh Priority X3P as current operation..."));
			currentOp.iOperation = CConfigManager::EOpX3p;
			currentOp.iPriority = CConfigManager::EPriorityHigh;
			break;
		default:
			break;
			}

		// For each column - new operation
		for (TInt col = 0; col < KMaxOperationTypes; col++)
			{
			// Create an operation object
			CConfigManager::TConflictOperation newOp;
			switch (col)
				{
			case KMoLr:
				newOp.iOperation = CConfigManager::EOpMoLr;
				newOp.iPriority = CConfigManager::EPriorityNone;
				break;
			case KMtLr:
				newOp.iOperation = CConfigManager::EOpMtLr;
				newOp.iPriority = CConfigManager::EPriorityNone;
				break;
			case KEmergency:
				newOp.iOperation = CConfigManager::EOpMtLr;
				newOp.iPriority = CConfigManager::EPriorityEmergency;
				break;
			case KX3pLow:
				newOp.iOperation = CConfigManager::EOpX3p;
				newOp.iPriority = CConfigManager::EPriorityLow;
				break;
			case KX3pMid:
				newOp.iOperation = CConfigManager::EOpX3p;
				newOp.iPriority = CConfigManager::EPriorityMedium;
				break;
			case KX3pHi:
				newOp.iOperation = CConfigManager::EOpX3p;
				newOp.iPriority = CConfigManager::EPriorityHigh;
				break;
			default:
				break;
				}

			// Lookup conflict decision result
			CConfigManager::TConflictResult result = config->ConflictDecision(currentOp, newOp);
			// Convert to an integer value (as used in central repository)
			TInt value;
			switch (result)
				{
				case CConfigManager::EConflictRejectNew:
					value = 0;
					break;
				case CConfigManager::EConflictQueueNew:
					value = 1;
					break;
				case CConfigManager::EConflictCancelCurrent:
					value = 2;
					break;
				case CConfigManager::EConflictQueueCurrent:
					value = 3;
					break;
				case CConfigManager::EConflictNone:
				default:
					value = -1;
					break;
				};

			// Generate external data item name
			TBuf<KDataItemNameMaxLen> itemName;
			itemName.Append(KTestDataIntStart);
			itemName.AppendNum((row * KMaxOperationTypes) + col + 1);

			// Get item from config data
			TInt itemValue = -1;
			TBool found = GetIntFromConfig(ConfigSection(), itemName, itemValue);

			// Check this is correct
			if (!found || (itemValue != value))
				{
				INFO_PRINTF4(_L("\tFailed on column %d - expected %d, received %d"), col+1, itemValue, value);
				CleanupStack::PopAndDestroy(config);
				SetTestStepResult(EFail);
				return TestStepResult();
				}

			}
		INFO_PRINTF1(_L("\tPassed"));
		}

	CleanupStack::PopAndDestroy(config);

	INFO_PRINTF1(_L("- END -"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/** Perform post test actions
@return TVerdict test result code
*/
TVerdict CMiscStep1::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}

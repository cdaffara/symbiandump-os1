// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file LoadDllStep.cpp
*/
#include "LoadDllStep.h"
//#include "Te_lbssupltestmoduleSuiteDefs.h"

#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsassistancedatabase.h>
#include <lbsposition.h>

const TUid KLbsAssistanceDataSourceInterfaceUid = {0x1028225A};


CLoadDllStep::~CLoadDllStep()
/**
 * Destructor
 */
	{
	}

CLoadDllStep::CLoadDllStep(CTe_lbssupltestmoduleSuite* aOwningServer): CTe_lbssupltestmoduleSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLoadDllStep);
	}

TVerdict CLoadDllStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class CLoadDllStep"));
	// Install the active scheduler
    iActSchd = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActSchd);
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLoadDllStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		// List plugin implementations
		RImplInfoPtrArray pluginArray;
		REComSession::ListImplementationsL( KLbsAssistanceDataSourceInterfaceUid,
										pluginArray );

		// Confirm that at least one plug-in exists
		INFO_PRINTF2(_L("\t Number of plugins listed = %d"), pluginArray.Count());
		if (pluginArray.Count() <= 0)
			{
			SetTestStepResult(EFail);
			}
		else
			{
			// Confirm that one of the plug-ins is the SUPL Test Protocol Module
			TInt loop;
			TBool pluginFound(EFalse);
			for (loop = 0; (loop < pluginArray.Count()) && !pluginFound; ++loop)
				{
				CImplementationInformation* pluginImp = pluginArray[loop];
				if (pluginImp->ImplementationUid() == TUid::Uid(KPluginUid))
					{
					pluginFound = ETrue;
					INFO_PRINTF1(_L("\t Test protocol module plug-in UID found"));
					}
				}
			SetTestStepResult(pluginFound ? EPass : EFail);
			}
		}
		
	if (TestStepResult()==EPass)
		{
		//If the plug-in loaded fine, try to retrieve assistance data
		//
		TInt numberOfRequestedAssistantDataSets = 1;
		CAssistanceDataRetriever* dataRetriever = CAssistanceDataRetriever::NewL(*this,numberOfRequestedAssistantDataSets);
	

		// Request all available assistance data
		//
		TLbsAssistanceDataGroup assistanceDataFilter = 0;
		assistanceDataFilter |= EAssistanceDataReferenceLocation;
		assistanceDataFilter |= EAssistanceDataReferenceTime;
		assistanceDataFilter |= EAssistanceDataNavigationModel;
		assistanceDataFilter |= EAssistanceDataIonosphericModel;
		assistanceDataFilter |= EAssistanceDataAquisitionAssistance;
		assistanceDataFilter |= EAssistanceDataPositioningGpsUtcModel;
		assistanceDataFilter |= EAssistanceDataAlmanac;
	
		dataRetriever->GetTheAssistanceData(assistanceDataFilter);
		CActiveScheduler::Start();
		
		delete dataRetriever;
    	delete iActSchd;
		}
	
	return TestStepResult();
	}

void CLoadDllStep::notifyResult(const TInt& aResult)
{
	if (aResult != KErrNone)
	{
	   ERR_PRINTF2(_L("Could not get Assistance Data: %d"), aResult);
       SetTestStepResult(EFail);
	}
}

TVerdict CLoadDllStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}


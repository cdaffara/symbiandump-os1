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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "tconditionevaluatorstep.h"
#include "conditionevaluate.h"
#include <e32property.h>
#include <barsc2.h>
#include <tunitconditionevaluator.rsg>
#include <e32const.h>
#include <e32def.h> 
#include <featmgr/featurecontrol.h> 
#include <ssm/ssmswp.h>
//Resource file with test conditions
_LIT(KTestUnitConditionEvaluator, "z:\\resource\\sspluginstest\\conditionalevaluator\\tunitconditionevaluator.rsc");

_LIT(KTestPSConditionsL, "TestPSConditionsL");
_LIT(KTestCRConditionsL, "TestCRConditionsL");
_LIT(KTestFMConditionsL, "TestFMConditionsL");
_LIT(KTestLogicalAndConditionsL, "TestLogicalAndConditionsL");
_LIT(KTestLogicalOrConditionsL, "TestLogicalOrConditionsL");
_LIT(KTestLogicalNotConditionsL, "TestLogicalNotConditionsL");
_LIT(KTestNestedConditionsL, "TestNestedConditionsL");
_LIT(KSetTestValidCommand, "SetTestValidCommand");
_LIT(KSetTestInvalidCommand, "SetTestInvalidCommand");
_LIT(KSetTestValidSwp, "SetTestValidSwp");
_LIT(KSetTestInvalidSwp, "SetTestInvalidSwp");
_LIT(KTestSwPConditionsL, "TestSwPConditionsL");

//Command resource ID
_LIT(KCommandId, "CommandId");

//For Publish & Subscribe conditions
const TUint KTestPS1 = 0x1;
const TUint KTestSwp1 = {0x2};
const TInt KInvalidValue = 0x0101;
const TInt KValidValue = 0x1010;

//For Feature manager
const TUid KTestFMUid = {0x2001D2A6};
const TUint32 KValidFMData = 0;

CConditionEvaluatorTestWrapper::CConditionEvaluatorTestWrapper():	iConditionEvaluate(NULL)
	{
	
	}

CConditionEvaluatorTestWrapper* CConditionEvaluatorTestWrapper::NewL()
	{
	CConditionEvaluatorTestWrapper*	ret = new (ELeave) CConditionEvaluatorTestWrapper();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}	

CConditionEvaluatorTestWrapper* CConditionEvaluatorTestWrapper::NewLC()
	{
	CConditionEvaluatorTestWrapper*	ret = new (ELeave) CConditionEvaluatorTestWrapper();
	CleanupStack::PushL(ret);
	ret->Construct1L();
	return ret;
	}
	
void CConditionEvaluatorTestWrapper::ConstructL()	
	{
	_LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
	TInt ret = RProperty::Define(RProcess().SecureId(), KTestPS1, RProperty::EInt, KAllowAllPolicy, KAllowAllPolicy);
	SetError(ret);
	iConditionEvaluate = CConditionEvaluate::NewL();
	User::LeaveIfError(iFs.Connect());
	iResourceFile = CResourceFile::NewL(iFs, KTestUnitConditionEvaluator, 0, 0 );
	SetBlockResult(EPass);
	}
		
void CConditionEvaluatorTestWrapper::Construct1L()	
	{
	_LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
	TInt ret = RProperty::Define(RProcess().SecureId(), KTestPS1, RProperty::EInt, KAllowAllPolicy, KAllowAllPolicy);
	SetError(ret);
	iConditionEvaluate = CConditionEvaluate::NewLC();
	User::LeaveIfError(iFs.Connect());
	iResourceFile = CResourceFile::NewL(iFs, KTestUnitConditionEvaluator, 0, 0 );
	SetBlockResult(EPass);
	CleanupStack::Pop(iConditionEvaluate);	
	}
	
CConditionEvaluatorTestWrapper::~CConditionEvaluatorTestWrapper()
	{
	INFO_PRINTF1(_L("~CConditionEvaluatorTestWrapper"));
	delete iConditionEvaluate;
	iConditionEvaluate = NULL;	
	delete iResourceFile;
	iFs.Close();
	}
	
void CConditionEvaluatorTestWrapper::SetTestInvalidCommand()	
	{
	//Invalid case
	TInt ret = RProperty::Set(RProcess().SecureId(), KTestPS1, KInvalidValue);
	SetError(ret);	
	}
	
void CConditionEvaluatorTestWrapper::SetTestValidCommand()	
	{
	//Valid case
	TInt ret = RProperty::Set(RProcess().SecureId(), KTestPS1, KValidValue);
	SetError(ret);	
	}
	
void CConditionEvaluatorTestWrapper::SetTestValidSwp()	
	{
	//Valid case
	TSsmSwp swp(KTestSwp1, 0);
	TInt err = RProperty::Define(RProcess().SecureId(), swp.Key(), RProperty::EInt);
	err = RProperty::Set(RProcess().SecureId(), KTestSwp1, KValidValue);
	SetError(err);
	}

void CConditionEvaluatorTestWrapper::SetTestInvalidSwp()	
	{
	//Invalid case
	TInt err = RProperty::Set(RProcess().SecureId(), KTestSwp1, KInvalidValue);
	SetError(err);
	}

TInt CConditionEvaluatorTestWrapper::GetCommandIdFromConfig(const TDesC& aSection)	
	{
	TInt commandId = 0;
	if (!GetIntFromConfig(aSection, KCommandId(), commandId))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KCommandId());
		SetBlockResult(EFail);
		}
	return commandId;	
	}
//
/**
Old Test CaseID 		APPFWK-CONDEVAL-0006
New Test CaseID 		DEVSRVS-SSPLUGINS-CONDEVAL-0006
 */


void CConditionEvaluatorTestWrapper::TestPSConditionsL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TestPSConditionsL"));
	TBool result = iConditionEvaluate->EvaluateL(*iResourceFile, GetCommandIdFromConfig(aSection));
	SetError(result);
	}

//
/**
Old Test CaseID 		APPFWK-CONDEVAL-0001
New Test CaseID 		DEVSRVS-SSPLUGINS-CONDEVAL-0006
 */

void CConditionEvaluatorTestWrapper::TestCRConditionsL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TestCRConditionsL"));
	TBool result = iConditionEvaluate->EvaluateL(*iResourceFile, GetCommandIdFromConfig(aSection));
	SetError(result);	
	}

//
/**
Old Test CaseID 		APPFWK-CONDEVAL-0002
New Test CaseID 		DEVSRVS-SSPLUGINS-CONDEVAL-0006
 */

void CConditionEvaluatorTestWrapper::TestFMConditionsL()
	{
	INFO_PRINTF1(_L("TestFMConditionsL"));
	
	//Invalid case
	TBool result = iConditionEvaluate->EvaluateL(*iResourceFile, R_CND_FM_VALUE_INVALID);
	SetError(result);
	result = iConditionEvaluate->EvaluateL(*iResourceFile, R_CND_FM_DATA_VALUE_INVALID);
	SetError(result);

	RFeatureControl feature;
	User::LeaveIfError(feature.Open());
	CleanupClosePushL(feature);

	TBitFlags32 flags;
	flags.Set( EFeatureSupported);
	flags.Set( EFeaturePersisted);

	TFeatureEntry fEntry(KTestFMUid, flags, KValidFMData);
	TInt err = feature.AddFeature(fEntry);
#ifdef __WINS__
	if(KErrNone == err || KErrAlreadyExists == err)
		SetError(EFalse);
	else
		SetError(ETrue);
#endif //__WINS__

	CleanupStack::PopAndDestroy(&feature);
	
	//Valid case
	result = iConditionEvaluate->EvaluateL(*iResourceFile, R_CND_FM_VALUE);
	SetError(!result);
	result = iConditionEvaluate->EvaluateL(*iResourceFile, R_CND_FM_DATA_VALUE);
	SetError(!result);
	}

//
/**
Test CaseID 		DEVSRVS-SSPLUGINS-CONDEVAL-0011
 */
void CConditionEvaluatorTestWrapper::TestSwPConditionsL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TestSWPConditionsL"));
	TBool result = iConditionEvaluate->EvaluateL(*iResourceFile, GetCommandIdFromConfig(aSection));
	SetError(result);
	}
	
//
/**
Old Test CaseID 		APPFWK-CONDEVAL-0004
New Test CaseID 		DEVSRVS-SSPLUGINS-CONDEVAL-0006
 */

void CConditionEvaluatorTestWrapper::TestLogicalAndConditionsL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TestLogicalAndConditionsL"));
	TBool result = iConditionEvaluate->EvaluateL(*iResourceFile, GetCommandIdFromConfig(aSection));
	SetError(result);
	}

//
/**
Old Test CaseID 		APPFWK-CONDEVAL-0005
New Test CaseID 		DEVSRVS-SSPLUGINS-CONDEVAL-0006
 */

void CConditionEvaluatorTestWrapper::TestLogicalOrConditionsL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TestLogicalOrConditionsL"));
	TBool result = iConditionEvaluate->EvaluateL(*iResourceFile, GetCommandIdFromConfig(aSection));
	SetError(result);
	}

//
/**
Old Test CaseID 		APPFWK-CONDEVAL-0003
New Test CaseID 		DEVSRVS-SSPLUGINS-CONDEVAL-0006
 */

void CConditionEvaluatorTestWrapper::TestLogicalNotConditionsL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TestLogicalNotConditionsL"));	
	//Invalid case condition but logical NOT makes it valid
	TBool result = iConditionEvaluate->EvaluateL(*iResourceFile, GetCommandIdFromConfig(aSection));
	SetError(result);
	}

//
/**
Old Test CaseID 		APPFWK-CONDEVAL-00010
New Test CaseID 		DEVSRVS-SSPLUGINS-CONDEVAL-0006
 */


void CConditionEvaluatorTestWrapper::TestNestedConditionsL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TestNestedConditionsL"));
	TBool result = iConditionEvaluate->EvaluateL(*iResourceFile, GetCommandIdFromConfig(aSection));
	SetError(result);
	}
/**
Purpose: Command fuction for a wrapper class

@internalComponent
*/
TBool CConditionEvaluatorTestWrapper::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool ret = ETrue;

	// Print out the parameters for debugging
	INFO_PRINTF2( _L("<font size=2 color=990000><b>aCommand = %S</b></font>"), &aCommand );
	INFO_PRINTF2( _L("aSection = %S"), &aSection );
			
	if(KSetTestInvalidCommand() == aCommand)
		{
		SetTestInvalidCommand();
		}
	else if(KSetTestValidCommand() == aCommand)
		{
		SetTestValidCommand();
		}
	else if(KTestPSConditionsL() == aCommand)
		{
		TestPSConditionsL(aSection);
		}	
	else if(KTestCRConditionsL() == aCommand)
		{
		TestCRConditionsL(aSection);
		}
	else if(KTestFMConditionsL() == aCommand)
		{
		TestFMConditionsL();
		}
	else if(KTestLogicalAndConditionsL() == aCommand)
		{
		TestLogicalAndConditionsL(aSection);
		}
	else if(KTestLogicalOrConditionsL() == aCommand)
		{
		TestLogicalOrConditionsL(aSection);
		}
	else if(KTestLogicalNotConditionsL() == aCommand)
		{
		TestLogicalNotConditionsL(aSection);
		}
	else if(KTestNestedConditionsL() == aCommand)
		{
		TestNestedConditionsL(aSection);
		}
	else if(KSetTestValidSwp() == aCommand)
		{
		SetTestValidSwp();
		}	
	else if(KSetTestInvalidSwp() == aCommand)
		{
		SetTestInvalidSwp();
		}	
	else if(KTestSwPConditionsL() == aCommand)
		{
		TestSwPConditionsL(aSection);
		}				
	else
		{
		ret = EFalse;
		}
	
	return ret;
}



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

#include "tconditionevaluator_step_evaluate.h"
#include <ssm/ssmsystemwideproperty.h>
#include <centralrepository.h>  
#include <featmgr/featurecontrol.h> 
#include <featmgr/featmgr.h>

const TUid KPropertyCategory={0x2000D75B};	// ssmserver SID 
const TUid KServerSID = KPropertyCategory;

// Commands
_LIT(KOOMCondCmd,		"OOMCondCmd");
_LIT(KEvaluateCondCmd,	"EvaluateCondCmd");


CCondCmdTestEvaluateWrapper::CCondCmdTestEvaluateWrapper():	iObject(NULL)
	{
	}

CCondCmdTestEvaluateWrapper::~CCondCmdTestEvaluateWrapper()
	{
	iClient.Close();
	}

CCondCmdTestEvaluateWrapper* CCondCmdTestEvaluateWrapper::NewL()
	{
	CCondCmdTestEvaluateWrapper*	ret = new (ELeave) CCondCmdTestEvaluateWrapper();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}	

void CCondCmdTestEvaluateWrapper::ConstructL()
	{
	const TInt connect = iClient.Connect();
	if(KErrNone != connect)
		{
		SetError(connect);
		}	
    //For publish and subscribe condition check
	const TUint KPropertyKey = 0x0012AC;
	//PS and central repositary value 
	TInt value = 0x1010;
	DefineAndSetPropertyL(KPropertyKey,value);
	DefineCRL(value);	         
	DefineFML();
	}

/**
 Local helper function to define the property
 */
void CCondCmdTestEvaluateWrapper::DefineAndSetPropertyL(TUint aKey, TInt aValue)
	{
	TInt err = RProperty::Define(KServerSID, aKey, RProperty::EInt, EFalse);
	if(err != KErrNone && err != KErrAlreadyExists)
		{	
		INFO_PRINTF3(_L("Error when defining property 0x%x: %d"), aKey, err);
		User::Leave(err);
		}
	User::LeaveIfError(RProperty::Set(KServerSID, aKey, aValue));
	}

/**
 Local helper function to define the property
 */
void CCondCmdTestEvaluateWrapper::DefineFML()
	{
	//For Feature manager
	const TUid KTestFMUid = {0x2001D2A6};
	const TUint32 KValidFMData = 0;

	RFeatureControl feature;
	
	feature.Open();
	CleanupClosePushL(feature);
	TBitFlags32 flags;
	flags.Set( EFeatureSupported);
	flags.Set( EFeaturePersisted);

	TFeatureEntry fEntry(KTestFMUid, flags, KValidFMData);
	TInt err = feature.AddFeature(fEntry);
	#ifdef __WINS__
	if(KErrNone == err || KErrAlreadyExists == err)
		SetError(err);
	#endif //__WINS__
	CleanupStack::PopAndDestroy(&feature);
	}

/**
 Local helper function to set the central repository
 */	
void CCondCmdTestEvaluateWrapper::DefineCRL(TInt value)
	{
	TInt KCentRepKey = 0x011; 
	TUid KCentRepId = {0x2001D2A8}; 
	CRepository* repository = CRepository::NewLC(KCentRepId);
	TInt ret = repository->Set(KCentRepKey,value); 
	CleanupStack::PopAndDestroy(repository);
	}
	
/**
 Local helper function to test the property
 */
void CCondCmdTestEvaluateWrapper::TestPropertyL(TUint aKey, TInt aValue)
	{
	TInt value = 0;
	TInt err = RProperty::Get(KServerSID, aKey, value);
	if (err == KErrNotFound)
		{
		INFO_PRINTF2(_L("Testing property 0x%x did not exist, assuming false"), aKey);
		value = 0;
		err = KErrNone;
		}
	User::LeaveIfError(err);
	// Test value is the same truth value as aValue
	INFO_PRINTF4(_L("Testing Property 0x%x has value: %d, expected: %d"), aKey, value, aValue);
	if(aValue != value)
		{
		SetError(value);
		}
	}


/**
Purpose: Command fuction for a wrapper class

@internalComponent
*/
TBool CCondCmdTestEvaluateWrapper::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool ret = ETrue;

	// Print out the parameters for debugging
	INFO_PRINTF2( _L("<font size=2 color=990000><b>aCommand = %S</b></font>"), &aCommand );
	INFO_PRINTF2( _L("aSection = %S"), &aSection );
	INFO_PRINTF2( _L("aAsyncErrorIndex = %D"), aAsyncErrorIndex );

	if(KEvaluateCondCmd() == aCommand)
		{
		DoCmdEvaluateCondCmdL( );
		}
	else if(KOOMCondCmd() == aCommand)
		{
		DoCmdOOMCondCmdEvaluatorL();
		}
	else
		{
		ret = EFalse;
		}

	return ret;
	}


/**
Old Test CaseID 		APPFWK-CONDEVAL-0008
Old Test CaseID 		APPFWK-CONDEVAL-0009
New Test CaseID 		DEVSRVS-SSPLUGINS-CONDEVAL-0008
 */
void CCondCmdTestEvaluateWrapper::DoCmdEvaluateCondCmdL()
	{
	INFO_PRINTF1(_L("TestEvaluateCondCmd"));
	
	//set EPropertyCondition1 to EPropertyCondition45 keys to 0 before execution the condition
	TInt condValue = 0;
	//If the condition evaluates to TRUE then it will set the PSkeys(EPropertyCondition1 to EPropertyCondition45) to 1
	TInt resOfCond = 1;
	for(TUint pSkeyid = EPropertyCondition1; pSkeyid <= EPropertyCondition42 ; pSkeyid++ )
		{
		DefineAndSetPropertyL(pSkeyid,condValue);	
		}

	CreateAndExecuteCmd(EConditionalCommandsList);		
	
	for(TUint pSkeyid = EPropertyCondition1; pSkeyid <= EPropertyCondition36; pSkeyid++ )
		{
		TestPropertyL(pSkeyid,resOfCond);	
		}
	//conditions EPropertyCondition37 to EPropertyCondition42 are negative test cases
	for(TUint pSkeyid = EPropertyCondition37; pSkeyid <= EPropertyCondition42; pSkeyid++ )
		{
		TestPropertyL(pSkeyid,condValue);	
		}
		
	//Nested condition
	DefineAndSetPropertyL(EPropertyCondition43,condValue);
	DefineAndSetPropertyL(EPropertyCondition44,condValue);
	CreateAndExecuteCmd(ENestedConditionalCommandsList);
	TestPropertyL(EPropertyCondition43,condValue);
	TestPropertyL(EPropertyCondition44,resOfCond);
	
	//Recursive condition
	TUint pSkeyid = EPropertyCondition45;
	DefineAndSetPropertyL(pSkeyid,condValue);
	CreateAndExecuteCmd(ERecuesiveConditionalCommandsList);
	TestPropertyL(pSkeyid,resOfCond);

	RDebug::Print(_L("TestEvaluateCondCmd completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	INFO_PRINTF3(_L("TestEvaluateCondCmd completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	}	

/**
Old Test CaseID 		APPFWK-CONDEVAL-00013
New Test CaseID 		DEVSRVS-SSPLUGINS-CONDEVAL-0008
 */

void CCondCmdTestEvaluateWrapper::DoCmdOOMCondCmdEvaluatorL()
	{
	INFO_PRINTF1(_L("*** Starting TestOOMCondCmdEvaluator"));
	TInt failRate = 1;
	TInt ret;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAP(ret, EvaluateCondCmdL());
	    	    
        if((ret==KErrNone || ret==KErrNoMemory))
        	SetError(ret);
        __UHEAP_MARKEND;

	    if (ret==KErrNone)
	        {
	        break;
	        }
	    }	
	__UHEAP_RESET;
	TestPropertyL(EPropertyCondition43,0);
	TestPropertyL(EPropertyCondition44,1);
	INFO_PRINTF3(_L(" Evaluated conditional command %d times and with %d errors."),failRate,ret);        
	}

void CCondCmdTestEvaluateWrapper::EvaluateCondCmdL()
	{
	DefineAndSetPropertyL(EPropertyCondition43,0);
	DefineAndSetPropertyL(EPropertyCondition44,0);
	CreateAndExecuteCmd(ENestedConditionalCommandsList);
	}
	
void CCondCmdTestEvaluateWrapper::CreateAndExecuteCmd(TConditionalCommandsLists aCmdValue)
	{
	TInt err = RProperty::Define(KMySID, KSwpTestKey1, RProperty::EInt);
	RDebug::Print(_L(" CCondCmdTestEvaluateWrapper::CreateAndExecuteCmd RProperty::Define %d"), err);
	err = iClient.RegisterSwpMapping(KSwpTestKey1, KTestSwpCondCmdEvaluatePolicy);
	TSsmSwp swp(KSwpTestKey1,aCmdValue);

	RSsmSystemWideProperty property;
	err = property.Connect(KSwpTestKey1);
	RDebug::Print(_L("RSsmStateManager RSsmSystemWideProperty Connect %d"), err);
	iClient.RequestSwpChange(swp, iStatus);
	User::WaitForRequest(iStatus);
	User::After(5000000);		
	SetError( iStatus.Int() );
	}


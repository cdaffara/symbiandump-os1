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


#ifndef __TCONDITIONEVALUATOR_STEP_EVALUATE_H__
#define __TCONDITIONEVALUATOR_STEP_EVALUATE_H__

#include <ssm/ssmstartupproperties.h>
#include <test/testexecutestepbase.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmswp.h>
#include <e32property.h>
#include <test/datawrapper.h>
#include "tconditionevaluator_commandlist.hrh"


//Using UID of tintconditionevaluator_server.exe as the swp key to avoid conflict
//with swp keys used in other tests 
const TUint KSwpTestKey1 = 0x2001D2A8;
const TUid KMySID={0x2000D75B}; // tssm_server SID 
_LIT(KTestSwpCondCmdEvaluatePolicy, "tintconditionevaluator_swppolicy.dll");

class CCondCmdTestEvaluateWrapper: public CDataWrapper
	{
public:
	CCondCmdTestEvaluateWrapper();
	~CCondCmdTestEvaluateWrapper();
	static CCondCmdTestEvaluateWrapper* NewL();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	virtual TAny*	GetObject() { return iObject; }
	inline virtual void	SetObjectL(TAny* aObject)
		{
		DestroyData();
		iObject	= static_cast<TInt*> (aObject);
		}
		
	inline virtual void	DisownObjectL() 
		{
		iObject = NULL;
		}

	void DestroyData()
		{
		delete iObject;
		iObject=NULL;
		}
		
	inline virtual TCleanupOperation CleanupOperation()
		{
		return CleanupOperation;
		}
protected:
	void ConstructL();	
private:
	static void CleanupOperation(TAny* aAny)
		{
		TInt* number = static_cast<TInt*>(aAny);
		delete number;
		}
	void CreateAndExecuteCmd(TConditionalCommandsLists aCmdValue);
	void DefineAndSetPropertyL(TUint aKey, TInt aValue);
	void TestPropertyL(TUint aKey, TInt aValue);
	void DefineFML();
	void DefineCRL(TInt value);
	void EvaluateCondCmdL();
	
	inline void DoCmdEvaluateCondCmdL();	
	inline void DoCmdOOMCondCmdEvaluatorL();
	
private:
	TRequestStatus iStatus;
	TRequestStatus iCancelReqStatus;
	TInt* iObject;
	RSsmStateManager iClient;
	};

#endif	// __TCMD_STEP_SSMCOMMAND_H__

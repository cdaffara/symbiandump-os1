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
// tconditionevaluatorWrapper.h
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TCONDITIONEVALUATORSTEP_H__
#define __TCONDITIONEVALUATORSTEP_H__

#include <test/datawrapper.h>	
#include "conditionevaluate.h"

//_LIT(KTCConditionEvaluatorTestWrapper, "CConditionEvaluatorTestWrapper");

class CConditionEvaluatorTestWrapper: public CDataWrapper
	{
public:
	CConditionEvaluatorTestWrapper();
	~CConditionEvaluatorTestWrapper();
	static CConditionEvaluatorTestWrapper* NewL();
	static CConditionEvaluatorTestWrapper* NewLC();
	//from CDataWrapper
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	virtual TAny*	GetObject() 
		{	
		return iConditionEvaluate; 
		}
	
	inline virtual void SetObjectL(TAny* aObject)		// setter for the wrapped object
		{
		delete iConditionEvaluate;	// it makes sense to cleanup an old object if existed
		iConditionEvaluate = NULL;
		iConditionEvaluate = static_cast<CConditionEvaluate*> (aObject);
		}

	inline virtual void	DisownObjectL()
		{
		iConditionEvaluate = NULL;
		}

	inline virtual TCleanupOperation CleanupOperation()
		{
		return CleanupOperation;
		}
protected:
	void ConstructL();
	void Construct1L();
private:
	static void CleanupOperation(TAny* aAny)
		{
		CConditionEvaluate* myObject = static_cast<CConditionEvaluate*>(aAny);
		delete myObject;
		}
	TInt GetCommandIdFromConfig(const TDesC& aSection);	
	void SetTestInvalidCommand();
	void SetTestValidCommand();
	void TestPSConditionsL(const TDesC& aSection);
	void TestCRConditionsL(const TDesC& aSection);
	void TestFMConditionsL();
	void TestLogicalAndConditionsL(const TDesC& aSection);
	void TestLogicalOrConditionsL(const TDesC& aSection);
	void TestLogicalNotConditionsL(const TDesC& aSection);
	void TestNestedConditionsL(const TDesC& aSection);
	void SetTestValidSwp();
	void SetTestInvalidSwp();
	void TestSwPConditionsL(const TDesC& aSection);
private:
	CConditionEvaluate* iConditionEvaluate;	
	CResourceFile* iResourceFile; 
	RFs iFs;
	};

#endif	// __TCONDITIONEVALUATORSTEP_H__

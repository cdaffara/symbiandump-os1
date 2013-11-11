// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// System includes
#include <e32base.h>

// Test system includes
#include "testframework.h"

#include "TSU_MMF_ASRCL.h"
#include "TSU_MMF_ASRCLSuite.h"



EXPORT_C CTestSuite* NewTestSuite()
/** NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
    {
	CTestMMF_ASRCLSuite* self= new (ELeave) CTestMMF_ASRCLSuite;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

CTestMMF_ASRCLSuite::~CTestMMF_ASRCLSuite()
/** destructor
 *
 */
	{
	}

TPtrC CTestMMF_ASRCLSuite::GetVersion( void )
/** make a version string available for test system
 * @return version string
 */
	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

void CTestMMF_ASRCLSuite::AddTestStepL( RTestMMF_ASRCLStep* aPtrTestStep )
/**
 * Add a test step into the suite
 * @param test step pointer to add
 */
	{
	// test steps contain a pointer back to the suite which owns them
	aPtrTestStep->SetTestSuite(this);
	// add the step using the base class method
	CTestSuite::AddTestStepL(aPtrTestStep);
	}


//------------------------------------------------------------------

/** Constructor for test suite
 * this creates all the test steps and 
 * stores them inside RTestMMF_ASRCLSuite 
*/
void CTestMMF_ASRCLSuite::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MMF_ASRCL");
	AddTestStepL( new(ELeave) RTestASRCLNewL() ) ;
	AddTestStepL( new(ELeave) RTestASRCLNewLC() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetModelCount() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAvailableStorage() ) ;
	AddTestStepL( new(ELeave) RTestASRCLCreateModelBank() ) ;
	AddTestStepL( new(ELeave) RTestASRCLTrain() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllPronunciationIDs() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllRuleIDs() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllModelIDs() ) ;
	AddTestStepL( new(ELeave) RTestASRCLRecordUtterance() ) ;
	AddTestStepL( new(ELeave) RTestASRCLPlayUtterance() ) ;
	AddTestStepL( new(ELeave) RTestASRCLCommitChanges() ) ;
	AddTestStepL( new(ELeave) RTestASRCLCreateLexicon() ) ;
	AddTestStepL( new(ELeave) RTestASRCLAddPronunciation() ) ;
	AddTestStepL( new(ELeave) RTestASRCLCreateGrammar() ) ;
	AddTestStepL( new(ELeave) RTestASRCLAddRule() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetRuleValidity() ) ;
	AddTestStepL( new(ELeave) RTestASRCLStartRecSession() ) ;
	AddTestStepL( new(ELeave) RTestASRCLLoadModelBank() ) ;
	AddTestStepL( new(ELeave) RTestASRCLLoadLexicon() ) ;
	AddTestStepL( new(ELeave) RTestASRCLLoadGrammar() ) ;
	AddTestStepL( new(ELeave) RTestASRCLUnloadRule() ) ;
	AddTestStepL( new(ELeave) RTestASRCLRecognize() ) ;
	AddTestStepL( new(ELeave) RTestASRCLEndRecSession() ) ;
	AddTestStepL( new(ELeave) RTestASRCLCancel() ) ;
	AddTestStepL( new(ELeave) RTestASRCLRemoveRule() ) ;
	AddTestStepL( new(ELeave) RTestASRCLRemovePronunciation() ) ;
	AddTestStepL( new(ELeave) RTestASRCLRemoveGrammar() ) ;
	AddTestStepL( new(ELeave) RTestASRCLRemoveLexicon() ) ;
	AddTestStepL( new(ELeave) RTestASRCLRemoveModel() ) ;
	AddTestStepL( new(ELeave) RTestASRCLRemoveModelBank() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetUtteranceDuration() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetEngineProperties() ) ;
	AddTestStepL( new(ELeave) RTestASRCLLoadEngineParameters() ) ;
	AddTestStepL( new(ELeave) RTestASRCLSetEventHandler() ) ;
	AddTestStepL( new(ELeave) RTestASRCLNegAsyncCalls() ) ;
	
	AddTestStepL( new(ELeave) RTestASRCLGetAllGrammarIDs() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllLexiconIDs() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllModelBankIDs() ) ;

	AddTestStepL( new(ELeave) RTestASRCLGetAllClientGrammarIDs() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllClientLexiconIDs() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllClientModelBankIDs() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllPronunciationIDsAlloc() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllModelIDsAlloc() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllRuleIDsAlloc() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllGrammarIDsAlloc() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllLexiconIDsAlloc() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllModelBankIDsAlloc() ) ;

	AddTestStepL( new(ELeave) RTestASRCLGetAllClientGrammarIDsAlloc() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllClientLexiconIDsAlloc() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetAllClientModelBankIDsAlloc() ) ;
	AddTestStepL( new(ELeave) RTestASRCLRecognizeAlloc() ) ;
	AddTestStepL( new(ELeave) RTestASRCLGetEnginePropertiesAlloc() ) ;

	AddTestStepL( new(ELeave) RTestASRCLSDDataRule());
	AddTestStepL( new(ELeave) RTestASRCLSDDataPronunciation());
	AddTestStepL( new(ELeave) RTestASRCLSDDataModel());

	AddTestStepL( new(ELeave) RTestASRCLSDDataGrammar());
	AddTestStepL( new(ELeave) RTestASRCLSDDataGrammarAdd());
	AddTestStepL( new(ELeave) RTestASRCLSDDataGrammarFind());
	AddTestStepL( new(ELeave) RTestASRCLSDDataGrammarDelete());
	AddTestStepL( new(ELeave) RTestASRCLSDDataGrammarAlloc());

	AddTestStepL( new(ELeave) RTestASRCLSDDataLexicon());
	AddTestStepL( new(ELeave) RTestASRCLSDDataLexiconAdd());
	AddTestStepL( new(ELeave) RTestASRCLSDDataLexiconFind());
	AddTestStepL( new(ELeave) RTestASRCLSDDataLexiconAlloc());

	AddTestStepL( new(ELeave) RTestASRCLSDDataModelBank());
	AddTestStepL( new(ELeave) RTestASRCLSDDataModelBankAdd());
	AddTestStepL( new(ELeave) RTestASRCLSDDataModelBankFind());
	AddTestStepL( new(ELeave) RTestASRCLSDDataModelBankAlloc());

	AddTestStepL( new(ELeave) RTestASRCLSDDataResult());
	AddTestStepL( new(ELeave) RTestASRCLSDDataResultSetAdd());
	AddTestStepL( new(ELeave) RTestASRCLSDDataResultSetAlloc());
	}



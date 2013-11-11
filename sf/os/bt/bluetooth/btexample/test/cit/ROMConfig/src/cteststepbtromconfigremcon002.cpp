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
//

#include "cteststepbtromconfigremcon002.h"
#include <remconinterfaceselector.h>
#include <remconcoreapitarget.h>

CTestStepBtRomConfigRemCon002::~CTestStepBtRomConfigRemCon002()
	{
    }
	

CTestStepBtRomConfigRemCon002::CTestStepBtRomConfigRemCon002
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameRemCon002);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigRemCon002* CTestStepBtRomConfigRemCon002::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigRemCon002(aParent); 
	}


/**
See BT_ROMCONFIG_REMCON_002.script
*/
TVerdict CTestStepBtRomConfigRemCon002::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigRemCon002::doTestStepL()"));
	
	CActiveScheduler* sched = new (ELeave)CActiveScheduler;
	CActiveScheduler::Install(sched);
	CleanupStack::PushL(sched);
	
	CRemConInterfaceSelector* remconSelector = CRemConInterfaceSelector::NewL();
	CleanupStack::PushL( remconSelector );
	
	CRemConCoreApiTarget* target = CRemConCoreApiTarget::NewL(*remconSelector, *this);
	// Ownership of target passed to remconSelector
	
	const TInt expectedError = ( iBtExcluded ? KErrNotFound : KErrNone );
	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr = ( iBtExcluded ? &KErrNotFoundLit : &KErrNoneLit ); 
	
	TRAPD(err, remconSelector->OpenTargetL())
	if ( err!=expectedError )
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d when calling CRemConInterfaceSelector::OpenTargetL"),\
			expectedErrorDesPtr, expectedError, err);
		SetTestStepResult(EFail);
		}

	CleanupStack::PopAndDestroy(remconSelector);
	CleanupStack::PopAndDestroy(sched);
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigRemCon002::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

// stubs
void CTestStepBtRomConfigRemCon002::MrccatoCommand(TRemConCoreApiOperationId /*aOperationId*/, 
	TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	}
	
void CTestStepBtRomConfigRemCon002::MrccatoPlay(TRemConCoreApiPlaybackSpeed /*aSpeed*/, 
	TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	}
	
void CTestStepBtRomConfigRemCon002::MrccatoTuneFunction(TBool /*aTwoPart*/, 
	TUint /*aMajorChannel*/, TUint /*aMinorChannel*/,
	TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	}
	
void CTestStepBtRomConfigRemCon002::MrccatoSelectDiskFunction(TUint /*aDisk*/,
	TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	}
	
void CTestStepBtRomConfigRemCon002::MrccatoSelectAvInputFunction(TUint8 /*aAvInputSignalNumber*/,
	TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	}
	
void CTestStepBtRomConfigRemCon002::MrccatoSelectAudioInputFunction(TUint8 /*aAudioInputSignalNumber*/,
	TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	}
		
// EOF

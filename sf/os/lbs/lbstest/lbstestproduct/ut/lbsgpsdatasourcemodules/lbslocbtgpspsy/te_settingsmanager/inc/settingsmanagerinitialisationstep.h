/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file SettingsManagerInitialisationStep.h
 @internalTechnology
*/

#ifndef SETTINGSMANAGERINITIALISATION_STEP_H
#define SETTINGSMANAGERINITIALISATION_STEP_H

#include <test/TestExecuteStepBase.h>
#include "te_settingsmanagerstepbase.h"
#include "pnschecker.h"
#include "lbscustomer.h"

class CSettingsManagerInitialisationStep : public CTe_settingsmanagerStepBase
	{
public:
	/**
	 * Two-phased constructor.
	 */
	static CSettingsManagerInitialisationStep* NewL();
	
	/**
	 * Two-phased constructor.
	 */
	static CSettingsManagerInitialisationStep* NewLC();
	
	CSettingsManagerInitialisationStep();
	~CSettingsManagerInitialisationStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void PrintResults(TBool areAvailableKeys,TBool isBTPSYLoaded,TBool areValidKeys);
	/**
	 * default constructor for performing second stage construction
	 */
	void ConstructL();
private:
	CPNSChecker* iPNSChecker;
	CLBSCustomer* iLBSCustomer;
	};

_LIT(KSettingsManagerInitialisationStep,"settingsmanagerinitialisationstep");

#endif

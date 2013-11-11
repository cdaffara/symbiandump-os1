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
 @file settingsmanagerfunctionteststep.h
 @internalTechnology
*/

#ifndef SETTINGSMANAGERFUNCTIONTEST_STEP_H
#define SETTINGSMANAGERFUNCTIONTEST_STEP_H

#include <bt_sock.h>
#include <bttypes.h>
#include <test/TestExecuteStepBase.h>
#include "te_settingsmanagerstepbase.h"
#include "BTGPSSettingManager.h"
#include "pnschecker.h"


class CSettingsManagerFunctionTestStep : public CTe_settingsmanagerStepBase
	{
public:
	/**
	 * Two-phased constructor.
	 */
	static CSettingsManagerFunctionTestStep* NewL();
	
	/**
	 * Two-phased constructor.
	 */
	static CSettingsManagerFunctionTestStep* NewLC();
	CSettingsManagerFunctionTestStep();
	~CSettingsManagerFunctionTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	/**
	 * default constructor for performing second stage construction
	 */
	void ConstructL();

private:
	CActiveScheduler* iScheduler;
	CPNSChecker* iPNSChecker;
	};

_LIT(KSettingsManagerFunctionTestStep,"settingsmanagerfunctionteststep");

#endif

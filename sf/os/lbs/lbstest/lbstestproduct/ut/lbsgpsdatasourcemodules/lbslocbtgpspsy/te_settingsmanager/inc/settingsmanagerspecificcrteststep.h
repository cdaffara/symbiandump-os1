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
 @file settingsmanagerspecificcrteststep.h
 @internalTechnology
*/

#ifndef SETTINGSMANAGERSPECIFICCRTEST_STEP_H
#define SETTINGSMANAGERSPECIFICCRTEST_STEP_H

#include <bt_sock.h>
#include <bttypes.h>
#include <test/TestExecuteStepBase.h>
#include "te_settingsmanagerstepbase.h"
#include "BTGPSSettingManager.h"
#include "crchecker.h"


class CSettingsManagerSpecificCRTestStep : public CTe_settingsmanagerStepBase
	{
public:
	/**
	 * Two-phased constructor.
	 */
	static CSettingsManagerSpecificCRTestStep* NewL();
	
	/**
	 * Two-phased constructor.
	 */
	static CSettingsManagerSpecificCRTestStep* NewLC();
	CSettingsManagerSpecificCRTestStep();
	~CSettingsManagerSpecificCRTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	/**
	 * default constructor for performing second stage construction
	 */
	void ConstructL();

private:
	CCRChecker* iCRChecker;
	};

_LIT(KSettingsManagerSpecificCRTestStep,"settingsmanagerspecificcrteststep");

#endif

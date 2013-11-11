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
 @file settingsmanagercommoncrteststep.h
 @internalTechnology
*/

#ifndef SETTINGSMANAGERCOMMONCRTEST_STEP_H
#define SETTINGSMANAGERCOMMONCRTEST_STEP_H

#include <bt_sock.h>
#include <bttypes.h>
#include <test/TestExecuteStepBase.h>
#include "te_settingsmanagerstepbase.h"
#include "BTGPSSettingManager.h"
#include "crchecker.h"


class CSettingsManagerCommonCRTestStep : public CTe_settingsmanagerStepBase
	{
public:
	/**
	 * Two-phased constructor.
	 */
	static CSettingsManagerCommonCRTestStep* NewL();
	
	/**
	 * Two-phased constructor.
	 */
	static CSettingsManagerCommonCRTestStep* NewLC();
	CSettingsManagerCommonCRTestStep();
	~CSettingsManagerCommonCRTestStep();
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

_LIT(KSettingsManagerCommonCRTestStep,"settingsmanagercommoncrteststep");

#endif

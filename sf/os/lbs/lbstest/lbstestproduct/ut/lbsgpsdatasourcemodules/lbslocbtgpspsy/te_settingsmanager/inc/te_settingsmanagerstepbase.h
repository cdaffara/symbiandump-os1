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
 @file Te_settingsmanagerStepBase.h
 @internalTechnology
*/

#ifndef TE_SETTINGSMANAGER_STEP_BASE
#define TE_SETTINGSMANAGER_STEP_BASE
#include <test/TestExecuteStepBase.h>

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_settingsmanagerStepBase : public CTestStep
	{
public:
	virtual ~CTe_settingsmanagerStepBase();
	CTe_settingsmanagerStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	};

#endif

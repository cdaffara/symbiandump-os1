/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_integration_stltsySuiteStepBase.h
 @internalTechnology
*/

#if (!defined __TE_INTEGRATION_STLTSY_STEP_BASE__)
#define __TE_INTEGRATION_STLTSY_STEP_BASE__
#include <TestExecuteStepBase.h>
// Please add your include here if you have 

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_integration_stltsySuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_integration_stltsySuiteStepBase();
	CTe_integration_stltsySuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

//Please add your class members which will be common to all individual test steps:
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	};

#endif

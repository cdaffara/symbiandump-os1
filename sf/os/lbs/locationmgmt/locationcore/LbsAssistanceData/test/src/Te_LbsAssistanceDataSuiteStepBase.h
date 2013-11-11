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
 @file Te_LbsAssistanceDataSuiteStepBase.h
*/

#if (!defined __TE_LBSASSISTANCEDATA_STEP_BASE__)
#define __TE_LBSASSISTANCEDATA_STEP_BASE__
#include <test/testexecutestepbase.h>

class CTe_LbsAssistanceDataSuite;

/****************************************************************************
* This class keeps variable and members common to all test steps.
****************************************************************************/
class CTe_LbsAssistanceDataSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_LbsAssistanceDataSuiteStepBase();
	CTe_LbsAssistanceDataSuiteStepBase(CTe_LbsAssistanceDataSuite& aOwningServer);
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

    CTe_LbsAssistanceDataSuite& iServer; 

//Please add your class members which will be common to all individual test steps:
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	};

#endif

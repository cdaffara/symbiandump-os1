/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file NonExistedPortCheckingStep.h
 @internalTechnology
*/
#if (!defined __NONEXISTEDPORTCHECKING_STEP_H__)
#define __NONEXISTEDPORTCHECKING_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_loopbackSuiteStepBase.h"

class CNonExistedPortCheckingStep : public CTe_loopbackSuiteStepBase
	{
public:
	CNonExistedPortCheckingStep();
	~CNonExistedPortCheckingStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	RComm iAnavailablePort_1, iAnavailablePort_2;
};

_LIT(KNonExistedPortCheckingStep,"NonExistedPortCheckingStep");

#endif


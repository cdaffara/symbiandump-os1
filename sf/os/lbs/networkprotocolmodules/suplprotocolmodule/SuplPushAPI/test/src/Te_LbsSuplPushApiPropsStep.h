// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// These are the unit tests for the LbsSuplPushProps class
// 
//

#if (!defined __TE_LBSSUPLPUSHAPIPROPS_STEP_BASE__)
#define __TE_LBSSUPLPUSHAPIPROPS_STEP_BASE__

#include <test/testexecutestepbase.h>

/** The name of the test step */
_LIT(KLbsSuplPushApiPropsStep, "LbsSuplPushApiPropsStep");

/**
The unit tests with successful scenarios for the LbsSuplPushProps::InitializeL function.
*/
class CTe_LbsSuplPushApiPropsStep : public CTestStep
	{

public:
	CTe_LbsSuplPushApiPropsStep();
	virtual ~CTe_LbsSuplPushApiPropsStep();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

protected:	
	TInt CheckProps();
	
	};

#endif //__TE_LBSSUPLPUSHAPIPROPS_STEP_BASE__

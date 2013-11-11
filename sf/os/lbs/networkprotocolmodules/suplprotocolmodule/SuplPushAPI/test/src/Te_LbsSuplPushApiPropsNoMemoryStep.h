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
// These are the memory allocation failure unit tests for the LbsSuplPushProps class
// 
//

#if (!defined __TE_LBSSUPLPUSHAPIPROPSNOMEMORY_STEP_BASE__)
#define __TE_LBSSUPLPUSHAPIPROPSNOMEMORY_STEP_BASE__

#include <test/testexecutestepbase.h>
#include "Te_LbsSuplPushApiPropsStep.h"

/** The name of the test step */
_LIT(KLbsSuplPushApiPropsNoMemoryStep, "LbsSuplPushApiPropsNoMemoryStep");

/**
The test step checking memory allocation error situations while defining the SUPL Push API props.

@see CTe_LbsSuplPushApiPropsStep
*/
class CTe_LbsSuplPushApiPropsNoMemoryStep : public CTe_LbsSuplPushApiPropsStep
	{

public:
	CTe_LbsSuplPushApiPropsNoMemoryStep();
	
	virtual TVerdict doTestStepL();
	};

#endif //__TE_LBSSUPLPUSHAPIPROPSNOMEMORY_STEP_BASE__

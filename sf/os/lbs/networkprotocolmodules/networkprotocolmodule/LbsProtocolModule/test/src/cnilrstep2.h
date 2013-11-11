/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file cnilrstep2.h
*/

#if (!defined __NILR_STEP2_H__)
#define __NILR_STEP2_H__

#include <test/testexecutestepbase.h>
#include "te_testprotocolmodulesuitestepbase.h"
#include "cgatewayinterface.h"

class CNiLrStep2 : public CTe_testprotocolmoduleSuiteStepBase
	{
public:
	CNiLrStep2();
	~CNiLrStep2();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CGatewayInterface* iModule;

	};

_LIT(KNiLrStep2,"NiLrStep2");

#endif

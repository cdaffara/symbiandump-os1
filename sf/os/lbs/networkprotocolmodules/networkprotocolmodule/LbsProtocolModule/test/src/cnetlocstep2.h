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
 @file cnetlocstep2.h
*/

#if (!defined __NETLOC_STEP2_H__)
#define __NETLOC_STEP2_H__

#include <test/testexecutestepbase.h>
#include "te_testprotocolmodulesuitestepbase.h"
#include "cgatewayinterface.h"

class CNetLocStep2 : public CTe_testprotocolmoduleSuiteStepBase
	{
public:
	CNetLocStep2();
	~CNetLocStep2();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CGatewayInterface* iModule;
	
	};

_LIT(KNetLocStep2,"NetLocStep2");

#endif

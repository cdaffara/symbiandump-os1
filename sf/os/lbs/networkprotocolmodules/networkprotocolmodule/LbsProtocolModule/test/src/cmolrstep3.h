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
 @file cmolrstep3.h
*/

#if (!defined __MOLR_STEP3_H__)
#define __MOLR_STEP3_H__

#include <test/testexecutestepbase.h>
#include "te_testprotocolmodulesuitestepbase.h"
#include "cgatewayinterface.h"

class CMoLrStep3 : public CTe_testprotocolmoduleSuiteStepBase
	{
public:
	CMoLrStep3();
	~CMoLrStep3();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CGatewayInterface* iModule;
	
	};

_LIT(KMoLrStep3,"MoLrStep3");

#endif

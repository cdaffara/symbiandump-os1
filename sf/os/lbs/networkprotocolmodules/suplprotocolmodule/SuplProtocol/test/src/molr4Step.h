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
 @file molr4Step.h
 @internalTechnology
*/
#if (!defined __MOLR4_STEP_H__)
#define __MOLR4_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_suplprotocolsuitestepbase.h"
#include "suplgatewayinterface.h"

class Cmolr4Step : public CTe_suplprotocolSuiteStepBase
	{
public:
	Cmolr4Step();
	~Cmolr4Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	CSuplGatewayInterface* iModule;
	};

_LIT(Kmolr4Step,"molr4Step");

#endif

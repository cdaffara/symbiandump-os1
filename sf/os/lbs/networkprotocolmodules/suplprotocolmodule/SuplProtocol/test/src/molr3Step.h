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
 @file molr3Step.h
 @internalTechnology
*/
#if (!defined __MOLR3_STEP_H__)
#define __MOLR3_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_suplprotocolsuitestepbase.h"
#include "suplgatewayinterface.h"

class Cmolr3Step : public CTe_suplprotocolSuiteStepBase
	{
public:
	Cmolr3Step();
	~Cmolr3Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	CSuplGatewayInterface* iModule;
	};

_LIT(Kmolr3Step,"molr3Step");

#endif

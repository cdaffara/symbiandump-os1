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
 @file molr2Step.h
 @internalTechnology
*/
#if (!defined __MOLR2_STEP_H__)
#define __MOLR2_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_suplprotocolsuitestepbase.h"
#include "suplgatewayinterface.h"

class Cmolr2Step : public CTe_suplprotocolSuiteStepBase
	{
public:
	Cmolr2Step();
	~Cmolr2Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CSuplGatewayInterface* iModule;
	};

_LIT(Kmolr2Step,"molr2Step");

#endif

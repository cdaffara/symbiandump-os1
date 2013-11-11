// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CLRS - Access to line resources tests
// 
//

/**
 @file
 @internalComponent
*/

#ifndef _RPS_USAGE_EXAMPLE_H_
#define _RPS_USAGE_EXAMPLE_H_

#include "TE_TrpBasicCallStepBase.h"


_LIT(KRPSUsageExampleDescId, "RPS_Usage_Example");
_LIT(KRPSUsageExampleDesc, "RPS Usage Examples");
class RPSUsageExampleStep : public CTrpBasicCallStepBase
	{
public:
	RPSUsageExampleStep(CGlobalTestData& aGlobalTestData);
	~RPSUsageExampleStep();
	virtual TVerdict doTestStepL();
	};

#endif  // _RPS_USAGE_EXAMPLE_H_



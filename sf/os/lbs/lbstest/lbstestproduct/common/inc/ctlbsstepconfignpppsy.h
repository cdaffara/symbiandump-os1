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
// This implements a test step for configuring the NPP PSY.
// 
//

#ifndef CTLBSSTEPCONFIGNPPPSY_H
#define CTLBSSTEPCONFIGNPPPSY_H

#include "ctlbsstep.h"
#include "ctlbsserver.h"

// Literals used
_LIT(KLbsStep_ConfigNppPsy, "LbsStep_ConfigNppPsy");

class CT_LbsStep_ConfigNppPsy : public CT_LbsStep
	{
public:
	static CT_LbsStep_ConfigNppPsy* New(CT_LbsServer& aParent);
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CT_LbsStep_ConfigNppPsy(CT_LbsServer& aParent);

	};

#endif //  CTLBSSTEPCONFIGNPPPSY_H

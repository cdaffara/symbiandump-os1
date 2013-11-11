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
// This is the header file for the hybrid module configuration test step
// 
//

#ifndef CTLBSSTEPCONFIGHYBRIDMODULE_H
#define CTLBSSTEPCONFIGHYBRIDMODULE_H

#include "ctlbsstep.h"
#include "ctlbsserver.h"
#include <lbs/test/ctlbsagpshandler.h>

// Literals used
_LIT(KLbsStep_ConfigHybridModule, "LbsStep_ConfigHybridModule");

class CT_LbsStep_ConfigHybridModule : public CT_LbsStep, MT_ResponseObserver
	{
public:
	static CT_LbsStep_ConfigHybridModule* New(CT_LbsServer& aParent);
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	// from MT_ResponseObserver
	virtual void ProcessAGpsResponseMessage(const TT_LbsAGpsResponseMsg::TModuleResponseType aResponse);

private:
	CT_LbsStep_ConfigHybridModule(CT_LbsServer& aParent);

	// Communication channel between A-Gps module and Test Harness
	CT_LbsAGpsHandler* iAGpsHandler;
	
	// Configuration counter
	TInt iConfigCtr;

	// Partial verdict
	TVerdict iVerdict;
	};

#endif //  CTLBSSTEPCONFIGHYBRIDMODULE_H

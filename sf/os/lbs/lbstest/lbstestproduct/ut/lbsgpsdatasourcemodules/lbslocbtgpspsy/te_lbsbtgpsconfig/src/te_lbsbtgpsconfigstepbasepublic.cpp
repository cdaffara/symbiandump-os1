// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// te_lbsbtgpsconfigstepbasepublic.h
// lbsbtgpsconfig unit test: Base test step using public API
//

#include "te_lbsbtgpsconfigstepbasepublic.h"


CLbsBtGpsConfigStepBasePublic::CLbsBtGpsConfigStepBasePublic()
	{
	}


CLbsBtGpsConfigStepBasePublic::~CLbsBtGpsConfigStepBasePublic()
	{
	delete iConfig;
	iDeviceList.ResetAndDestroy();
	}


TVerdict CLbsBtGpsConfigStepBasePublic::doTestStepPreambleL(void)
	{
	CLbsBtGpsConfigStepBase::doTestStepPreambleL();
	
	// Open the config API
	iConfig = CLbsBtGpsConfig::NewL(*this);
	
	return TestStepResult();
	}

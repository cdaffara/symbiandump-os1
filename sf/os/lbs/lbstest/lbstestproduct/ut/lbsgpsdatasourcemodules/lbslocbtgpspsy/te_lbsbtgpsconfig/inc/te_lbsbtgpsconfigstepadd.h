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
// lbsbtgpsconfig unit test step: Add device tests 
//

#ifndef TE_LBS_BTGPSCONFIGSTEPADD_H
#define TE_LBS_BTGPSCONFIGSTEPADD_H

#include "te_lbsbtgpsconfigstepbasepublic.h"

_LIT(KLbsBtGpsConfigStepAdd,"LbsBtGpsConfigStepAdd");


class CLbsBtGpsConfigStepAdd : public CLbsBtGpsConfigStepBasePublic
	{
public:
	CLbsBtGpsConfigStepAdd();
	~CLbsBtGpsConfigStepAdd();
	
	// From CLbsBtGpsConfigStepBase:
	TBool RunTestStage(TInt aStage);

	// Test stages:
	TVerdict TestAddDeviceBasic();
	TVerdict TestAddDeviceMany();
	TVerdict TestAddDeviceDuplicate();
	TVerdict TestAddDeviceForward();
	TVerdict TestAddDeviceReverse();
	TVerdict TestAddDeviceRandom();

	};


#endif //TE_LBS_BTGPSCONFIGSTEPADD_H

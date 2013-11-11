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
// lbsbtgpsconfig unit test: Base test step using public API 
//

#ifndef TE_LBS_BTGPSCONFIGSTEPBASEPUBLIC_H
#define TE_LBS_BTGPSCONFIGSTEPBASEPUBLIC_H

#include "te_lbsbtgpsconfigstepbase.h"


class CLbsBtGpsConfigStepBasePublic : public CLbsBtGpsConfigStepBase
	{
public:
	CLbsBtGpsConfigStepBasePublic();
	~CLbsBtGpsConfigStepBasePublic();
	
	// From CTestStep:
 	TVerdict doTestStepPreambleL(void);
	
protected:
	
	CLbsBtGpsConfig* iConfig;
	RPointerArray<TLbsBtGpsDeviceInfo> iDeviceList;

	};


#endif //TE_LBS_BTGPSCONFIGSTEPBASEPUBLIC_H

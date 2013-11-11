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
// lbsbtgpsconfig unit test: Base test step using internal API
//

#ifndef TE_LBS_BTGPSCONFIGSTEPBASEINTERNAL_H
#define TE_LBS_BTGPSCONFIGSTEPBASEINTERNAL_H

#include "te_lbsbtgpsconfigstepbase.h"

#include "lbsbtgpsconfiginternal.h"
#include "lbsbtgpsdevicerecord.h"


class CLbsBtGpsConfigStepBaseInternal : public CLbsBtGpsConfigStepBase
	{
public:
	CLbsBtGpsConfigStepBaseInternal();
	~CLbsBtGpsConfigStepBaseInternal();
	
	// From CTestStep:
 	TVerdict doTestStepPreambleL(void);

protected:
	
	CLbsBtGpsConfigInternal* iConfigInternal;
	RPointerArray<TLbsBtGpsDeviceRecord> iDeviceRecordList;

	};


#endif //TE_LBS_BTGPSCONFIGSTEPBASEINTERNAL_H

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
// lbsbtgpsconfig unit test step: Multiple client tests
//

#ifndef TE_LBS_BTGPSCONFIGSTEPMULTIPLECLIENT_H
#define TE_LBS_BTGPSCONFIGSTEPMULTIPLECLIENT_H

#include "te_lbsbtgpsconfigstepbaseinternal.h"
#include "te_lbsbtgpsconfigthread.h"

_LIT(KLbsBtGpsConfigStepMultipleClient,"LbsBtGpsConfigStepMultipleClient");


class CLbsBtGpsConfigStepMultipleClient : public CLbsBtGpsConfigStepBaseInternal
	{
public:
	CLbsBtGpsConfigStepMultipleClient();
	~CLbsBtGpsConfigStepMultipleClient();
	
	// From CLbsBtGpsConfigStepBase:
	TBool RunTestStage(TInt aStage);
	
	TVerdict TestUpdates();
	TVerdict TestThreadAdd();
	TVerdict TestThreadReorder();
	TVerdict TestThreadRemove();
	
private:
	
	RPointerArray<CTestBtGpsConfigThread> iThreads;
	RPointerArray<TLbsBtGpsDeviceInfo> iThreadDeviceList;
	
	};


#endif //TE_LBS_BTGPSCONFIGSTEPMULTIPLECLIENT_H

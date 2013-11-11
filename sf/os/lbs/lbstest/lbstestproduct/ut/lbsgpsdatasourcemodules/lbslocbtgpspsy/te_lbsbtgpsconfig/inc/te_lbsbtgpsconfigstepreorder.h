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
// lbsbtgpsconfig unit test step: Reorder device tests  
//

#ifndef TE_LBS_BTGPSCONFIGSTEPREORDER_H
#define TE_LBS_BTGPSCONFIGSTEPREORDER_H

#include "te_lbsbtgpsconfigstepbasepublic.h"

_LIT(KLbsBtGpsConfigStepReorder,"LbsBtGpsConfigStepReorder");

class CLbsBtGpsConfigStepReorder : public CLbsBtGpsConfigStepBasePublic
	{
public:
	CLbsBtGpsConfigStepReorder();
	~CLbsBtGpsConfigStepReorder();
	
	// From CLbsBtGpsConfigStepBase:
	TBool RunTestStage(TInt aStage);
	
	TVerdict TestReorderDeviceRandom();
	TVerdict TestReorderDeviceNonExistant();
	
	};


#endif //TE_LBS_BTGPSCONFIGSTEPREORDER_H

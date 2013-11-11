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
// te_lbsbtgpsconfigstepremovedevice.h
// lbsbtgpsconfig unit test step: Remove device tests  
//

#ifndef TE_LBS_BTGPSCONFIGSTEPREMOVEDEVICE_H
#define TE_LBS_BTGPSCONFIGSTEPREMOVEDEVICE_H

#include "te_lbsbtgpsconfigstepbasepublic.h"

_LIT(KLbsBtGpsConfigStepRemove,"LbsBtGpsConfigStepRemove");

class CLbsBtGpsConfigStepRemove : public CLbsBtGpsConfigStepBasePublic
	{
public:
	CLbsBtGpsConfigStepRemove();
	~CLbsBtGpsConfigStepRemove();
	
	// From CLbsBtGpsConfigStepBase:
	TBool RunTestStage(TInt aStage);
	
	TVerdict TestRemoveDeviceBasic();
	TVerdict TestRemoveDeviceMany();
	TVerdict TestRemoveDeviceNonExistant();
	
	};


#endif //TE_LBS_BTGPSCONFIGSTEPREMOVEDEVICE_H

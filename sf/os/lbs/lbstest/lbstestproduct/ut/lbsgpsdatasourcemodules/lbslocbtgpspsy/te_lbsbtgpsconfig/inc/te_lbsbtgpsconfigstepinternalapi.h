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
// lbsbtgpsconfig unit test step: Internal API tests  
//

#ifndef TE_LBS_BTGPSCONFIGSTEPINTERNALAPI_H
#define TE_LBS_BTGPSCONFIGSTEPINTERNALAPI_H

#include "te_lbsbtgpsconfigstepbaseinternal.h"

_LIT(KLbsBtGpsConfigStepInternalApi,"LbsBtGpsConfigStepInternalApi");

class CLbsBtGpsConfigStepInternalApi : public CLbsBtGpsConfigStepBaseInternal
	{
public:
	CLbsBtGpsConfigStepInternalApi();
	~CLbsBtGpsConfigStepInternalApi();
	
	// From CLbsBtGpsConfigStepBase:
	TBool RunTestStage(TInt aStage);
	
	TVerdict TestUpdateDevice();
	
	};


#endif //TE_LBS_BTGPSCONFIGSTEPINTERNALAPI_H

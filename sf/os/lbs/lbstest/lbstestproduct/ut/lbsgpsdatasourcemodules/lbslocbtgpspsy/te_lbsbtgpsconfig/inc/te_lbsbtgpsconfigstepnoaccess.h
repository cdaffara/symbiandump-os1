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
// lbsbtgpsconfig unit test step: Test no access is permitted without required capabilities
//

#ifndef TE_LBS_BTGPSCONFIGSTEPNOACCESS_H
#define TE_LBS_BTGPSCONFIGSTEPNOACCESS_H

#include "te_lbsbtgpsconfigstepbasepublic.h"

_LIT(KLbsBtGpsConfigStepNoAccess,"LbsBtGpsConfigStepNoAccess");


class CLbsBtGpsConfigStepNoAccess : public CLbsBtGpsConfigStepBasePublic
	{
public:
	CLbsBtGpsConfigStepNoAccess();
	~CLbsBtGpsConfigStepNoAccess();

	// Need to override this as opening the API should leave
 	TVerdict doTestStepPreambleL(void);
	
	// From CLbsBtGpsConfigStepBase:
	TBool RunTestStage(TInt aStage);

	};


#endif //TE_LBS_BTGPSCONFIGSTEPNOACCESS_H

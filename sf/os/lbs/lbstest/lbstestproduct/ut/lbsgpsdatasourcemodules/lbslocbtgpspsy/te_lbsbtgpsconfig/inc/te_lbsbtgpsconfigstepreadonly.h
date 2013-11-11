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
// lbsbtgpsconfig unit test step: Read only API access tests 
//

#ifndef TE_LBS_BTGPSCONFIGSTEPREADONLY_H
#define TE_LBS_BTGPSCONFIGSTEPREADONLY_H

#include "te_lbsbtgpsconfigstepbasepublic.h"

_LIT(KLbsBtGpsConfigStepReadOnly,"LbsBtGpsConfigStepReadOnly");


class CLbsBtGpsConfigStepReadOnly : public CLbsBtGpsConfigStepBasePublic
	{
public:
	CLbsBtGpsConfigStepReadOnly();
	~CLbsBtGpsConfigStepReadOnly();
	
	// From CLbsBtGpsConfigStepBase:
	TBool RunTestStage(TInt aStage);

	// Test stages:
	TVerdict TestAllowed();
	TVerdict TestNotAllowed();

	};


#endif //TE_LBS_BTGPSCONFIGSTEPREADONLY_H

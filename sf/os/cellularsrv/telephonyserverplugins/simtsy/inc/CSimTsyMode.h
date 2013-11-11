// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __SIMTSYMODE_H_
#define __SIMTSYMODE_H_

/**
 * @file
 *
 * contains the class definitions to determine the TSY mode to run in (ie CDMA or GSM)
 *
 * @internalAll
 */

#include "e32def.h"
#include "SimConstants.h"
#include "testconfigfileparser.h"
#include "CSimPhone.h"

class CSimTsyMode : public CBase
	{
public:
	enum TTsyMode
		{
		/** No CDMA mode */
		ENoCdma=0,
		/** CDMA V1 mode */
		ECdmaV1=1
		};

	static TTsyMode GetMode();
	static void FreeMode();
	static TInt InitL(CSimPhone *aPhone);
	};

#endif // __SIMTSYMODE_H_

// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _BTSOCKETTIMER_H
#define _BTSOCKETTIMER_H

#include <e32base.h>

#include "bt.h"
// provided when ESOCK removes its provision of this service

class BTSocketTimer
	{
	enum Consts
		{
		KTimerGranularity = 15500,
		KTimerPriority = 50,
		};

	public:
		static void InitL();
		static void Close();

		static void Queue(TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,TDeltaTimerEntry& aTimer);
		static void Remove(TDeltaTimerEntry& aTimer);
	};

#endif //_BTSOCKETTIMER_H

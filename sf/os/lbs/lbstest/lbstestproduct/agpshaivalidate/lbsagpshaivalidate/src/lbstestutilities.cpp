// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// System
// 
//

#include <e32base.h>
#include <e32debug.h>


// Component
#include "lbstestutilities.h"
#include "lbstestloggermacros.h"

//-----------------------------------------------------------------------------
// LbsTestUtilities
//-----------------------------------------------------------------------------

// 	print a date and time to log in a readable format:
void LbsTestUtilities::PrintfDateTimeToDebugLog(TTime aTime)
	{
	TDateTime datetime = aTime.DateTime();	
	TESTLOG8(ELogP2,"%d :%d :%d :%d on %d/%d/%d", datetime.Hour(), datetime.Minute(), datetime.Second(), datetime.MicroSecond(), datetime.Day() + 1, datetime.Month() + 1, datetime.Year());	
	}


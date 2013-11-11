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
//

#ifndef LBS_TEST_UTILITIES_H
#define LBS_TEST_UTILITIES_H

//************************************************************************************************************
#include <e32base.h>

	
//-------------------------------------------------------------------------------	
/** 
Test Utility Class
*/
class LbsTestUtilities
	{
public:
	
// 	print a date and time to debug log in a readable format:	
	static void PrintfDateTimeToDebugLog(TTime aTime);	
		
private:
	}; 

#endif //LBS_TEST_UTILITIES_H

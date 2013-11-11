// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Te_LbsSuplErrorCommon.h
// All global SUPL Push API suite consts and enums
// 
//

#if (!defined __TE_LBSSUPLPUSHERRORPCOMMON__)
#define __TE_LBSSUPLPUSHERRORPCOMMON__

#include <e32base.h>

/**
Contains the enum with the ids of the errors that are simulated in the test code.

@see RTe_LbsSuplErrorPattern
*/

class Te_LbsSuplErrorId
	{	
public:	
	/**
	The ids of the errors that are simulated in the test code.
	*/
	enum 
		{
		ERProperty_Define,
		ERProperty_Get,
		ERProperty_Set,
		ERLbsSystemController_RequestSystemStartup
		};
	};
	
#endif //__TE_LBSSUPLPUSHERRORPCOMMON__

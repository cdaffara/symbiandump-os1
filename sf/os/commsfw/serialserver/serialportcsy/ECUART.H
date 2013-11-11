// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Main header file for ECUART CSY
// 
//

/**
 @file
 @internalComponent
*/


#ifndef _ECUART_H_
#define _ECUART_H_

enum TCommFault 
	{
	EBadUnit              = 0, //< wrong unit number (not used in this CSY)
	ECancelNotOutstanding = 1, //< an attempt was made to cancel a non-outstanding request
	EWrongRole            = 2, //< the role for this event was wrong
	ENegativeCount        = 3  //< a positive number was expected
	};

#endif  // _ECUART_H_

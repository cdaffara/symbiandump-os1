// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__CLIENT_UTIL_H__)
#define __CLIENT_UTIL_H__

//################# ENUMERATED TYPES #############################

enum TIrdaPanic
	{
	EIASQueryDes8Overflow,
	EIASResponseDes8Overflow,
	};

//################# CLASS DEFINITIONS #############################

NONSHARABLE_CLASS(IrdaUtil)
	{
public:
	static void Panic(TIrdaPanic aPanic);
	};

#endif // __CLIENT_UTIL_H__

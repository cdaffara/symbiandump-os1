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
// Global functions, etc
// 
//

/**
 @file
 @internalComponent
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include "CDBSTD.H"

namespace commdb_dll
	{
CDB_LIT(KServerName,_SERVER_NAME);
	}
using namespace commdb_dll;


/**
Panic
*/
GLDEF_C void CommsDbPanic(TCommsDbSvrPanic aPanic)
	{
	User::Panic(KServerName,aPanic);
	}

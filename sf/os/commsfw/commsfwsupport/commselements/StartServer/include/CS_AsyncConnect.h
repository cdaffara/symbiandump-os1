// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// RAsyncConnect Client side header
// Declares the Symbian OS RAsyncConnectBase class as a base for client side API
// of servers supporting startup synchronisation using Rendezvous
// 
//

/**
 @file 
 @internalTechnology
*/

#ifndef __CS_ASYNCCONNECT_H__
#define __CS_ASYNCCONNECT_H__

#include <e32base.h>
#include <e32svr.h>

const TUint KMaxServerNameLength = 55;
/**
 * The RAsyncConnect class
 * @internalTechnology
 * Declaration of a base for client side API 
 * of servers started asynchronously and supporting startup synchronisation using Rendezvous
 */
class RAsyncConnectBase : public RSessionBase
	{
public:
   	IMPORT_C void CreateSession(const TDesC& aServerName, const TVersion& aVersion, TInt aAsyncMessageSlots, TRequestStatus& aStatus);

	};

#endif


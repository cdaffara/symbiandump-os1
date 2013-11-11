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
// EtelPacketTestMultipleClients.h
// This is the header file for testing Multiple Clients using the Packet API 
// simultaneously. This is used primarily to exercise the mutex in the methods 
// RPacketContext::OpenNewContext() and RPacketContext::OpenNewSecondaryContext().
// 
//

/**
 @file
 @internalComponent 
*/

#if (!defined __ETELPACKETTESTMULTIPLECLIENT_H__)
#define __ETELPACKETTESTMULTIPLECLIENT_H__

#include "Te_EtelPacketTestStepBase.h"

class CEtelPacketTestMultipleClients : public CTestStepEtelPacket
{
public:
	CEtelPacketTestMultipleClients();
	~CEtelPacketTestMultipleClients();

	virtual enum TVerdict doTestStepL( void );
	static TInt ContextCreationFunction(TAny* aThreadData);
};

#endif //(__ETELPACKETTESTMULTIPLECLIENT_H__)

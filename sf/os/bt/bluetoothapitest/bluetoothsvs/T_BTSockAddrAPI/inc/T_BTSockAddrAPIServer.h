/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#if (!defined __T_BT_SOCK_ADDR_API_SERVER_H__)
#define __T_BT_SOCK_ADDR_API_SERVER_H__

//	EPOC Includes
#include <test/testblockcontroller.h>
#include <test/testserver2.h>

//	user Includes
#include "T_RfcommSockAddrData.h"
#include "T_BTServiceSecurityData.h"
#include "T_BTSockAddrData.h"
#include "T_SockAddrData.h"


class CTestBlock : public CTestBlockController
	{
public:
	inline CTestBlock();
	inline ~CTestBlock();

	inline CDataWrapper* CreateDataL( const TDesC& aData );
	};

class CT_BTSockAddrAPIServer : public CTestServer2
	{
public:
	inline CT_BTSockAddrAPIServer();
	inline ~CT_BTSockAddrAPIServer();

	static CT_BTSockAddrAPIServer* NewL();

	inline CTestBlockController*	CreateTestBlock();
	};

#include "T_BTSockAddrAPIServer.inl"

#endif /* __T_BT_SOCK_ADDR_API_SERVER_H__ */

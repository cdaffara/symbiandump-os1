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


#if (!defined __T_BT_SOCK__API_SERVER_H__)
#define __T_BT_SOCK__API_SERVER_H__

//	EPOC Includes
#include <rsshared.h>
#include <test/testblockcontroller.h>
#include <test/testserver2.h>

//	User Includes
#include "T_CBluetoothSocketData.h"

#include "T_RfcommSockAddrData.h"
#include "T_BTServiceSecurityData.h"
#include "T_BTSockAddrData.h"
#include "T_SockAddrData.h"
#include "T_RSocketData.h"
#include "T_BTDevAddrData.h"


class CTestBlock : public CTestBlockController
	{
public:
	inline CTestBlock();
	inline ~CTestBlock();

	inline CDataWrapper* CreateDataL( const TDesC& aData );
	};

class CT_BTSockAPIServer : public CTestServer2
	{
public:
	inline CT_BTSockAPIServer();
	inline ~CT_BTSockAPIServer();

	static CT_BTSockAPIServer* NewL();

	inline CTestBlockController*	CreateTestBlock();
	};

#include "T_BTSockAPIServer.inl"

#endif /* __T_BT_SOCK__API_SERVER_H__ */

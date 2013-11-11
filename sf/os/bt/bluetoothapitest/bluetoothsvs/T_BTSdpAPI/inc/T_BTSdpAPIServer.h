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


#if (!defined __T_BT_SDP_API_SERVER_H__)
#define __T_BT_SDP_API_SERVER_H__

//	EPOC Includes
#include <test/testblockcontroller.h>
#include <test/testserver2.h>

//	User Includes
#include "T_DataSdpAttrValueNil.h"
#include "T_DataSdpAttrValueBoolean.h"
#include "T_DataSdpAgent.h"
#include "T_DataCElementParser.h"
#include "T_DataCSdpAttrValueURL.h"
#include "T_DataCSdpSearchPattern.h"
#include "T_DataRSdp.h"
#include "T_DataRSdpDatabase.h"
#include "T_DataSdpAttrIdMatchList.h"
#include "T_DataSdpAttrValueInt.h"
#include "T_DataSdpAttrValueString.h"
#include "T_DataSdpAttrValueUint.h"
#include "T_DataSdpAttrValueUUID.h"
#include "T_DataSdpUtil.h"
#include "T_DataSdpIntBuf8.h"
#include "T_DataSdpIntBuf16.h"
#include "T_DataSdpIntBuf32.h"
#include "T_DataSdpIntBuf64.h"
#include "T_DataSdpAttrValueDES.h"
#include "T_DataSdpAttrValueDEA.h"

#include "T_BTDevAddrData.h"
#include "T_CBluetoothSocketData.h"
#include "T_RfcommSockAddrData.h"
#include "T_BTServiceSecurityData.h"

class CTestBlock : public CTestBlockController
	{
public:
	inline CTestBlock();
	inline ~CTestBlock();

	inline CDataWrapper* CreateDataL( const TDesC& aData );
	};
	
class CT_BTSdpAPIServer : public CTestServer2
	{
public:
	inline CT_BTSdpAPIServer();
	inline ~CT_BTSdpAPIServer();

	static CT_BTSdpAPIServer* NewL();

	inline CTestBlockController*	CreateTestBlock();
	};

#include "T_BTSdpAPIServer.inl"

#endif /* __T_BT_SDP_API_SERVER_H__ */

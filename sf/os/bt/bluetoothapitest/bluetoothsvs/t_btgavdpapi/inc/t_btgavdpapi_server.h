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


#if (!defined __T_BT_GAVDP_API_SERVER_H__)
#define __T_BT_GAVDP_API_SERVER_H__

//	EPOC Includes
#include <test/testblockcontroller.h>
#include <test/testserver2.h>

//	User Includes
#include "t_data_rgavdp.h"

#include "T_AvdtpSEPInfoData.h"
#include "T_SEIDData.h"
#include "T_AvdtpServiceCapabilityData.h"
#include "T_AvdtpMediaTransportCapabilitiesData.h"
#include "T_AvdtpReportingCapabilitiesData.h"
#include "T_AvdtpRecoveryCapabilitiesData.h"
#include "T_AvdtpContentProtectionCapabilitiesData.h"
#include "T_AvdtpMediaCodecCapabilitiesData.h"
#include "T_NonSBCCodecCapabilitiesChild.h"
#include "T_NonSBCCodecCapabilitiesData.h"

#include "T_SocketServData.h"
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
	
class CT_BTGavdpAPIServer : public CTestServer2
	{
public:
	inline CT_BTGavdpAPIServer();
	inline ~CT_BTGavdpAPIServer();

	static CT_BTGavdpAPIServer* NewL();

	inline CTestBlockController*	CreateTestBlock();
	};

#include "t_btgavdpapi_server.inl"

#endif /* __T_BT_GAVDP_API_SERVER_H__ */

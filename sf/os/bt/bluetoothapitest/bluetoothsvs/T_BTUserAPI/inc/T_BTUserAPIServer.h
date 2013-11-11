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


#if (!defined __T_BT_USER_API_SERVER_H__)
#define __T_BT_USER_API_SERVER_H__

//	EPOC Includes
#include <rsshared.h>
#include <test/testblockcontroller.h>
#include <test/testserver2.h>
#include <bluetoothav.h>

//	User Includes
#include "T_BTAccessRequirementsData.h"
#include "T_BluetoothPhysicalLinksData.h"
#include "T_BluetoothSynchronousLinkData.h"
#include "T_BTPhysicalLinkAdapterData.h"
#include "T_RfcommRemotePortParamsData.h"
#include "T_RfcommRPNTransactionData.h"
#include "T_L2CapConfigData.h"
#include "T_L2CapSockAddrData.h"
#include "T_InquirySockAddrData.h"
#include "T_BTSyncBandWidthData.h"
#include "T_BTSyncPacketsData.h"
#include "T_SocketServData.h"

#include "T_AvdtpServiceCapabilityData.h"
#include "T_AvdtpContentProtectionCapabilitiesData.h"
#include "T_AvdtpHeaderCompressionCapabilitiesData.h"
#include "T_AvdtpMediaCodecCapabilitiesData.h"
#include "T_NonSBCCodecCapabilitiesChild.h"
#include "T_NonSBCCodecCapabilitiesData.h"
#include "T_AvdtpMediaTransportCapabilitiesData.h"
#include "T_AvdtpRecoveryCapabilitiesData.h"
#include "T_AvdtpReportingCapabilitiesData.h"
#include "T_AvdtpSEPInfoData.h"
#include "T_AvdtpServiceCategoriesData.h"
#include "T_AvdtpSockAddrData.h"
#include "T_ConvertToSymbianErrorData.h"
#include "T_SEIDData.h"

#include "T_SockAddrData.h"
#include "T_BTSockAddrData.h"
#include "T_BTDevAddrData.h"
#include "T_BTServiceSecurityData.h"
#include "T_CBluetoothSocketData.h"
#include "T_RfcommSockAddrData.h"
#include "T_RSocketData.h"


class CTestBlock : public CTestBlockController
	{
public:
	inline CTestBlock();
	inline ~CTestBlock();

	inline CDataWrapper* CreateDataL( const TDesC& aData );
	};
class CT_BTUserAPIServer : public CTestServer2
	{
public:
	inline CT_BTUserAPIServer();
	inline ~CT_BTUserAPIServer();
	static  CT_BTUserAPIServer*	NewL();
	inline CTestBlockController*	CreateTestBlock();
	};

#include "T_BTUserAPIServer.inl"
#endif /* __T_BT_USER_API_SERVER_H__ */

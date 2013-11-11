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


/*@{*/
//Gavdp API
_LIT(KDataRGavdp,				"RGavdp");

//from T_BTUserAPI
_LIT(KSocketServ,							"RSocketServ");
_LIT(KAvdtpSEPInfo,							"TAvdtpSEPInfo");
_LIT(KSEID,									"TSEID");
_LIT(KAvdtpMediaTransportCapabilities,		"TAvdtpMediaTransportCapabilities");
_LIT(KAvdtpReportingCapabilities,			"TAvdtpReportingCapabilities");
_LIT(KAvdtpRecoveryCapabilities,			"TAvdtpRecoveryCapabilities");
_LIT(KAvdtpContentProtectionCapabilities,	"TAvdtpContentProtectionCapabilities");
_LIT(KNonSBCCodecCapabilities,				"TNonSBCCodecCapabilities");

//from T_BTSdpAPI
_LIT(KDataRSdp,              	"RSdp");
_LIT(KDataRSdpdatabase,			"RSdpDatabase");
_LIT(KDataSdpAttrValueDES,		"CSdpAttrValueDES");
_LIT(KDataSdpAttrValueString,	"CSdpAttrValueString");
_LIT(KDataSdpAttrValueUint,		"CSdpAttrValueUint");

//from T_BTSockAddrAPI
_LIT(KDataBTDevAddr,			"TBTDevAddr");
_LIT(KDataCBluetoothSocket,		"CBluetoothSocket");
_LIT(KDataRfcommSockAddr,		"TRfcommSockAddr");
_LIT(KDataBTServiceSecurity,	"TBTServiceSecurity");
/*@}*/

inline CTestBlock::CTestBlock()
    {
    }

inline CTestBlock::~CTestBlock()
    {
    }

inline CDataWrapper* CTestBlock::CreateDataL( const TDesC& aData )
	{
	CDataWrapper* wrapper = NULL;
   	if ( aData==KDataRGavdp() )
   		{
   		wrapper=CT_DataRGavdp::NewL();
   		}	
   	//from T_BTUserAPI
	else if (aData==KSocketServ)
   		{
   		wrapper=CT_SocketServData::NewL();
   		}	
   	else if (aData==KAvdtpSEPInfo)
   		{
   		wrapper=CT_AvdtpSEPInfoData::NewL();
   		}
   	else if (aData==KSEID)
   		{
   		wrapper=CT_SEIDData::NewL();
   		}
   	else if (aData==KAvdtpMediaTransportCapabilities)
   		{
   		wrapper=CT_AvdtpMediaTransportCapabilitiesData::NewL();
   		}
   	else if (aData==KAvdtpReportingCapabilities)
   		{
   		wrapper=CT_AvdtpReportingCapabilitiesData::NewL();
   		}
   	else if (aData==KAvdtpRecoveryCapabilities)
   		{
   		wrapper=CT_AvdtpRecoveryCapabilitiesData::NewL();
   		}
   	else if (aData==KAvdtpContentProtectionCapabilities)
   		{
   		wrapper=CT_AvdtpContentProtectionCapabilitiesData::NewL();
   		}
   	else if (aData==KNonSBCCodecCapabilities)
   		{
   		wrapper=CT_NonSBCCodecCapabilitiesData::NewL();
   		}
  	//From T_BTSockAPI and T_BTSockAddrAPI
   	else if (aData==KDataBTDevAddr)
   		{
   		wrapper=CT_BTDevAddrData::NewL();
   		}
   	else if (aData==KDataCBluetoothSocket)
   		{
   		wrapper=CT_CBluetoothSocketData::NewL();
   		}
   	else if (aData==KDataRfcommSockAddr)
   		{
   		wrapper=CT_TRfcommSockAddrData::NewL();
   		}
   	else if(aData==KDataBTServiceSecurity)
   		{
   		wrapper=CT_BTServiceSecurityData::NewL();
   		}
	return wrapper;
	}


inline CT_BTGavdpAPIServer::CT_BTGavdpAPIServer()
    {
    }
	
inline CT_BTGavdpAPIServer::~CT_BTGavdpAPIServer() {}

inline CTestBlockController*	CT_BTGavdpAPIServer::CreateTestBlock()
	{
	return new CTestBlock();
	}

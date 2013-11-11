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

_LIT(KBTAccessRequirements,					"TBTAccessRequirements");
_LIT(KBluetoothPhysicalLinks,				"CBluetoothPhysicalLinks");
_LIT(KBluetoothSynchronousLink,				"CBluetoothSynchronousLink");
_LIT(KBTPhysicalLinkAdapter,				"RBTPhysicalLinkAdapter");
_LIT(KRfcommRemotePortParams,				"TRfcommRemotePortParams");
_LIT(KRfcommRPNTransaction,					"TRfcommRPNTransaction");
_LIT(KL2CapConfig,							"TL2CapConfig");
_LIT(KL2CAPSockAddr,						"TL2CAPSockAddr");
_LIT(KTInquirySockAddr,						"TInquirySockAddr");
_LIT(KTBTSyncBandwidth,						"TBTSyncBandwidth");
_LIT(KTBTSyncPackets,						"TBTSyncPackets");
_LIT(KSocketServ,							"RSocketServ");

_LIT(KAvdtpContentProtectionCapabilities,	"TAvdtpContentProtectionCapabilities");
_LIT(KAvdtpHeaderCompressionCapabilities,	"TAvdtpHeaderCompressionCapabilities");
_LIT(KNonSBCCodecCapabilities,				"TNonSBCCodecCapabilities");
_LIT(KAvdtpMediaTransportCapabilities,		"TAvdtpMediaTransportCapabilities");
_LIT(KAvdtpRecoveryCapabilities,			"TAvdtpRecoveryCapabilities");
_LIT(KAvdtpReportingCapabilities,			"TAvdtpReportingCapabilities");
_LIT(KAvdtpSEPInfo,							"TAvdtpSEPInfo");
_LIT(KAvdtpServiceCategories,				"TAvdtpServiceCategories");
_LIT(KAvdtpSockAddr,						"TAvdtpSockAddr");
_LIT(KConvertToSymbianError,				"ConvertToSymbianError");
_LIT(KSEID,									"TSEID");

//from T_BTSockAddrAPI
_LIT(KDataTSockAddr,						"TSockAddr");
_LIT(KDataTBTSockAddr,						"TBTSockAddr");
_LIT(KDataBTDevAddr,						"TBTDevAddr");
_LIT(KDataBTServiceSecurity,				"TBTServiceSecurity");
_LIT(KDataCBluetoothSocket,					"CBluetoothSocket");
_LIT(KDataRfcommSockAddr,					"TRfcommSockAddr");
_LIT(KDataRSocket,							"RSocket");

// --------------------- CTestBlock -----------------------------------

inline CTestBlock::CTestBlock()
    {
    }
    
inline CTestBlock::~CTestBlock()
    {
    }

inline CDataWrapper* CTestBlock::CreateDataL( const TDesC& aData )
	{
	CDataWrapper* wrapper = NULL;
   	if (aData==KBTAccessRequirements)
   		{
   		wrapper=CT_BTAccessRequirementsData::NewL();
   		}
   	else if (aData==KBluetoothPhysicalLinks)
  		{
   		wrapper=CT_BluetoothPhysicalLinksData::NewL();
   		}
   	else if (aData==KBluetoothSynchronousLink)
   		{
   		wrapper=CT_BluetoothSynchronousLinkData::NewL();
   		}
   	else if (aData==KBTPhysicalLinkAdapter)
   		{
   		wrapper=CT_BTPhysicalLinkAdapterData::NewL();
   		}
   	else if(aData==KRfcommRemotePortParams)
   		{
   		wrapper=CT_RfcommRemotePortParamsData::NewL();
   		}	
   	else if (aData==KRfcommRPNTransaction)
   		{
   		wrapper=CT_RfcommRPNTransactionData::NewL();
   		}			
   	else if (aData==KL2CapConfig)
   		{
   		wrapper=CT_L2CapConfigData::NewL();
   		}
   	else if (aData==KL2CAPSockAddr)
   		{
   		wrapper=CT_L2CAPSockAddrData::NewL();
   		}			
   	else if (aData==KTInquirySockAddr)
   		{
   		wrapper=CT_InquirySockAddrData::NewL();
   		}			
   	else if (aData==KTBTSyncBandwidth)
   		{
   		wrapper=CT_BTSyncBandwidthData::NewL();
   		}
	else if (aData==KTBTSyncPackets)
   		{
   		wrapper=CT_BTSyncPacketsData::NewL();
   		}	
	else if (aData==KSocketServ)
   		{
   		wrapper=CT_SocketServData::NewL();
   		}	
	else if (aData==KDataTSockAddr)
   		{
   		wrapper=CT_SockAddrData::NewL();
   		}	
	else if (aData==KDataTBTSockAddr)
   		{
   		wrapper=CT_BTSockAddrData::NewL();
   		}	
	else if (aData==KDataBTDevAddr)
   		{
   		wrapper=CT_BTDevAddrData::NewL();
   		}	
	else if (aData==KDataBTServiceSecurity)
   		{
   		wrapper=CT_BTServiceSecurityData::NewL();
   		}	
	else if (aData==KDataCBluetoothSocket)
   		{
   		wrapper=CT_CBluetoothSocketData::NewL();
   		}	
	else if (aData==KDataRfcommSockAddr)
   		{
   		wrapper=CT_TRfcommSockAddrData::NewL();
   		}
	else if (aData==KDataRSocket)
   		{
   		wrapper=CT_RSocketData::NewL();
   		}
   	else if (aData==KAvdtpContentProtectionCapabilities)
   		{
   		wrapper=CT_AvdtpContentProtectionCapabilitiesData::NewL();
   		}
   	else if (aData==KAvdtpHeaderCompressionCapabilities)
   		{
   		wrapper=CT_AvdtpHeaderCompressionCapabilitiesData::NewL();
   		}
   	else if (aData==KNonSBCCodecCapabilities)
   		{
   		wrapper=CT_NonSBCCodecCapabilitiesData::NewL();
   		}
   	else if (aData==KAvdtpMediaTransportCapabilities)
   		{
   		wrapper=CT_AvdtpMediaTransportCapabilitiesData::NewL();
   		}
   	else if (aData==KAvdtpRecoveryCapabilities)
   		{
   		wrapper=CT_AvdtpRecoveryCapabilitiesData::NewL();
   		}
   	else if (aData==KAvdtpReportingCapabilities)
   		{
   		wrapper=CT_AvdtpReportingCapabilitiesData::NewL();
   		}
   	else if (aData==KAvdtpSEPInfo)
   		{
   		wrapper=CT_AvdtpSEPInfoData::NewL();
   		}
   	else if (aData==KAvdtpServiceCategories)
   		{
   		wrapper=CT_AvdtpServiceCategoriesData::NewL();
   		}
   	else if (aData==KAvdtpSockAddr)
   		{
   		wrapper=CT_AvdtpSockAddrData::NewL();
   		}
   	else if (aData==KConvertToSymbianError)
   		{
   		wrapper=CT_ConvertToSymbianErrorData::NewL();
   		}
   	else if (aData==KSEID)
   		{
   		wrapper=CT_SEIDData::NewL();
   		}
   	
   	return wrapper;
	}

// --------------------- CT_BTUserAPIServer -----------------------------------

inline CT_BTUserAPIServer::CT_BTUserAPIServer()
    {
    }
    
inline CT_BTUserAPIServer::~CT_BTUserAPIServer()
    {
    }

inline CTestBlockController*	CT_BTUserAPIServer::CreateTestBlock()
	{
	return new CTestBlock();
	}

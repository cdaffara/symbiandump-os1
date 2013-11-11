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
//Sdp API
_LIT(KDataSdpAttrValueNil,		"CSdpAttrValueNil");
_LIT(KDataSdpAttrValueBoolean,	"CSdpAttrValueBoolean");
_LIT(KDataSdpAgent,				"CSdpAgent");
_LIT(KDataCElementParser,		"CElementParser");
_LIT(KDataCSdpAttrValueURL,		"CSdpAttrValueURL");
_LIT(KDataCSdpSearchPattern,	"CSdpSearchPattern");
_LIT(KDataRSdp,              	"RSdp");
_LIT(KDataRSdpdatabase,			"RSdpDatabase");
_LIT(KDataSdpAttrIdMatchList,	"CSdpAttrIdMatchList");
_LIT(KDataSdpAttrValueInt,		"CSdpAttrValueInt");
_LIT(KDataSdpAttrValueString,	"CSdpAttrValueString");
_LIT(KDataSdpAttrValueUint,		"CSdpAttrValueUint");
_LIT(KDataSdpAttrValueUUID,		"CSdpAttrValueUUID");
_LIT(KDataSdpUtil,				"SdpUtil");
_LIT(KDataSdpIntBuf8,			"TSdpIntBuf8");
_LIT(KDataSdpIntBuf16,			"TSdpIntBuf16");
_LIT(KDataSdpIntBuf32,			"TSdpIntBuf32");
_LIT(KDataSdpIntBuf64,			"TSdpIntBuf64");
_LIT(KDataSdpAttrValueDES,		"CSdpAttrValueDES");
_LIT(KDataSdpAttrValueDEA,		"CSdpAttrValueDEA");

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
   	if ( aData==KDataSdpAttrValueNil() )
   		{
   		wrapper=CT_DataSdpAttrValueNil::NewL();
   		}
   	else if ( aData==KDataSdpAttrValueBoolean() )
   		{
   		wrapper=CT_DataSdpAttrValueBoolean::NewL();
   		}
   	else if ( aData==KDataSdpAgent() )
   		{
   		wrapper=CT_DataSdpAgent::NewL();
   		}
   	else if ( aData==KDataCElementParser() )
   		{
   		wrapper=CT_DataCElementParser::NewL();
   		}
   	else if ( aData==KDataCSdpAttrValueURL() )
   		{
   		wrapper=CT_DataCSdpAttrValueURL::NewL();
   		}
   	else if ( aData==KDataCSdpSearchPattern() )
   		{
   		wrapper=CT_DataCSdpSearchPattern::NewL();
   		}
   	else if ( aData==KDataRSdp() )
   		{
   		wrapper=CT_DataRSdp::NewL();
   		}
   	else if ( aData==KDataRSdpdatabase() )
   		{
   		wrapper=CT_DataRSdpdatabase::NewL();
   		}
   	else if ( aData==KDataSdpAttrIdMatchList() )
   		{
   		wrapper=CT_DataSdpAttrIdMatchList::NewL();
   		}
   	else if ( aData==KDataSdpAttrValueInt() )
   		{
   		wrapper=CT_DataSdpAttrValueInt::NewL();
   		}
   	else if ( aData==KDataSdpAttrValueString() )
   		{
   		wrapper=CT_DataSdpAttrValueString::NewL();
   		}
   	else if ( aData==KDataSdpAttrValueUint() )
   		{
   		wrapper=CT_DataSdpAttrValueUint::NewL();
   		}
   	else if ( aData==KDataSdpAttrValueUUID() )
   		{
   		wrapper=CT_DataSdpAttrValueUUID::NewL();
   		}
   	else if ( aData==KDataSdpUtil() )
   		{
   		wrapper=CT_DataSdpUtil::NewL();
   		}
   	else if ( aData==KDataSdpIntBuf8() )
   		{
   		wrapper=CT_DataSdpIntBuf8::NewL();
   		}
   	else if ( aData==KDataSdpIntBuf16() )
   		{
   		wrapper=CT_DataSdpIntBuf16::NewL();
   		}		
   	else if ( aData==KDataSdpIntBuf32() )
   		{
   		wrapper=CT_DataSdpIntBuf32::NewL();
   		}
   	else if ( aData==KDataSdpIntBuf64() )
   		{
   		wrapper=CT_DataSdpIntBuf64::NewL();
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
   	else if ( aData==KDataSdpAttrValueDES() )
   		{
   		wrapper=CT_DataSdpAttrValueDES::NewL();
   		}
   	else if ( aData==KDataSdpAttrValueDEA() )
   		{
  		wrapper=CT_DataSdpAttrValueDEA::NewL();
   		}
	return wrapper;
	}


inline CT_BTSdpAPIServer::CT_BTSdpAPIServer()
    {
    }
	
inline CT_BTSdpAPIServer::~CT_BTSdpAPIServer() {}

inline CTestBlockController*	CT_BTSdpAPIServer::CreateTestBlock()
	{
	return new CTestBlock();
	}

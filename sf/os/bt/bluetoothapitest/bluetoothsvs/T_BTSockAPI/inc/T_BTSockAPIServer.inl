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


_LIT(KDataCBluetoothSocket,		"CBluetoothSocket");
_LIT(KDataRfcommSockAddr,		"TRfcommSockAddr");
_LIT(KDataBTServiceSecurity,	"TBTServiceSecurity");
_LIT(KDataBTSockAddr,			"TBTSockAddr");
_LIT(KDataSockAddr,				"TSockAddr");
_LIT(KDataRSocket,				"RSocket");
_LIT(KDataBTDevAddr,			"TBTDevAddr");

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
   	if (aData==KDataCBluetoothSocket)
   		{
   		wrapper=CT_CBluetoothSocketData::NewL();
   		}
   	else if (aData==KDataRfcommSockAddr)
  		{
   		wrapper=CT_TRfcommSockAddrData::NewL();
   		}
   	else if (aData==KDataBTSockAddr)
   		{
   		wrapper=CT_BTSockAddrData::NewL();
   		}
   	else if (aData==KDataSockAddr)
   		{
   		wrapper=CT_SockAddrData::NewL();
   		}
   	else if(aData==KDataBTServiceSecurity)
   		{
   		wrapper=CT_BTServiceSecurityData::NewL();
   		}	
   	else if (aData==KDataRSocket)
   		{
   		wrapper=CT_RSocketData::NewL();
   		}			
   	else if (aData==KDataBTDevAddr)
   		{
   		wrapper=CT_BTDevAddrData::NewL();
   		}			
	return wrapper;
	}

// --------------------- CT_BTSockAPIServer -----------------------------------

inline CT_BTSockAPIServer::CT_BTSockAPIServer()
    {
    }
    
inline CT_BTSockAPIServer::~CT_BTSockAPIServer()
    {
    }

inline CTestBlockController*	CT_BTSockAPIServer::CreateTestBlock()
	{
	return new CTestBlock();
	}

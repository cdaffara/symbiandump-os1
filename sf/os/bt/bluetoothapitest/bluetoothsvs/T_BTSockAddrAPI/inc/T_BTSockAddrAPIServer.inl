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


_LIT(KDataRfcommSockAddr,		"TRfcommSockAddr");
_LIT(KDataBTServiceSecurity,	"TBTServiceSecurity");
_LIT(KDataBTSockAddr,			"TBTSockAddr");
_LIT(KDataSockAddr,				"TSockAddr");
/*@}*/

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
   	if (aData==KDataRfcommSockAddr)
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
	return wrapper;
	}

// --------------------- CT_BTSockAddrAPIServer -----------------------------------

inline CT_BTSockAddrAPIServer::CT_BTSockAddrAPIServer()
    {
    }
    
inline CT_BTSockAddrAPIServer::~CT_BTSockAddrAPIServer()
    {
    }

inline CTestBlockController*	CT_BTSockAddrAPIServer::CreateTestBlock()
	{
	return new CTestBlock();
	}

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


#include "T_RSocketData.h"
#include "T_BTUtil.h"

/*@{*/
//Constant Literals used.

_LIT(KNamedProtocol,							"named_protocol");

_LIT(KCmdRSocketServerConnect,					"RSocketServerConnect");
_LIT(KCmdRSocketServerClose,					"RSocketServerClose");
_LIT(KCmdRSocketOpen,							"RSocketOpen");
_LIT(KCmdRSocketClose,							"RSocketClose");
_LIT(KCmdInfo,									"Info");

// Other
_LIT_SECURITY_POLICY_C2(KProcPolicy, ECapabilityNetworkServices, ECapabilityNetworkControl);
/*@}*/

CT_RSocketData* CT_RSocketData::NewL()
	{
	CT_RSocketData*	ret=new (ELeave) CT_RSocketData();
	return ret;
	}

CT_RSocketData::CT_RSocketData()
	{
	}

CT_RSocketData::~CT_RSocketData()
	{

	}


TBool CT_RSocketData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool								retVal=ETrue;
	
	//-------------------------------------------------------------------------
	// RSocket (This class is not a full test suite just a helper to test CBluetoothSocket)
	//-------------------------------------------------------------------------		

	if (aCommand==KCmdRSocketServerConnect)
		{
		CloseSocketServer();				
		User::LeaveIfError(iSocketServer.Connect());			
		}
	else if (aCommand==KCmdRSocketServerClose)
		{
		CloseSocketServer();
		}

	else if (aCommand==KCmdRSocketOpen)
		{	
		RSocketOpen(aSection);	
		}
	else if (aCommand==KCmdRSocketClose)
		{
		iRSocket.Close();
		}
	else if (aCommand==KCmdInfo)
		{
		Info(aSection);
		}		
		
	else
		{
		retVal=EFalse;
		}

	return retVal;
	}
	
TAny* CT_RSocketData::GetObject()
	{
	return &iRSocket;
	}
	
void CT_RSocketData::CloseSocketServer()
	{
	if(iSocketServer.Handle())
		{
		iSocketServer.Close();
		}	
	}	
	
	
void CT_RSocketData::RSocketOpen(const TDesC& aSection)
	{	
	TPtrC	namedProtocol;
	if( GetStringFromConfig(aSection, KNamedProtocol(), namedProtocol))
		{
		INFO_PRINTF2(_L("Opening RSocket with: %S protocol"), &namedProtocol);			
		TProtocolDesc pInfo;
		User::LeaveIfError(iSocketServer.FindProtocol(namedProtocol,pInfo));		
		iRSocket.Open(iSocketServer, pInfo.iAddrFamily, pInfo.iSockType, pInfo.iProtocol);
		iRSocket.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy().Package());
		}
	else
		{
		ERR_PRINTF1(_L("GetStringFromConfig failed"));
		SetBlockResult(EFail);	
		}	
	}
	
void CT_RSocketData::Info(const TDesC& /*aSection*/)	
	{
	TProtocolDesc protocolDesc;
	TInt err = iRSocket.Info(protocolDesc);
	SetError(err);
	}	

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


#include "T_SockAddrData.h"


//Constant Literals used.

//TSockAddr
_LIT(KCmdConstruct, 							"Construct");
_LIT(KCmdDistructor,							"~");

/*@}*/

CT_SockAddrData* CT_SockAddrData::NewL()
	{
	CT_SockAddrData*	ret=new (ELeave) CT_SockAddrData();
	return ret;
	}

CT_SockAddrData::CT_SockAddrData()
:   iTSockAddr(NULL)
	{
	}

CT_SockAddrData::~CT_SockAddrData()
	{
	Cleanup();
	}
void CT_SockAddrData::Cleanup()
	{
	delete iTSockAddr;	
	iTSockAddr=NULL;
	}

TBool CT_SockAddrData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& /*aSection*/, const TInt /*aAsyncErrorIndex*/)
	{
	TBool								retVal=ETrue;
	
	//-------------------------------------------------------------------------
	// TSockAddr (Currently only used for other classes Copy constructor(s))
	//-------------------------------------------------------------------------		

	if ( aCommand==KCmdConstruct )
		{
		INFO_PRINTF1(_L("TSockAddr Constructor Call"));
		delete iTSockAddr;
		iTSockAddr=NULL;
		TRAPD( err, iTSockAddr = new (ELeave) TSockAddr() );
		SetError(err);
		}
	else if ( aCommand==KCmdDistructor )
		{				
		Cleanup();
		}
	else
		{
		retVal=EFalse;
		}

	return retVal;
	}
	
TAny* CT_SockAddrData::GetObject()
	{
	return iTSockAddr;
	}

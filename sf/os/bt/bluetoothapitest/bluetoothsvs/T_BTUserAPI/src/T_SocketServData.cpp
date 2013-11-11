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

#include "T_SocketServData.h"

/*@{*/
//Parameters
_LIT(KMessageSlots,											"messageslots");

//Commands
_LIT(KCmdRSocketServ,										"RSocketServ");
_LIT(KCmdConnect,											"Connect");
_LIT(KCmdClose,												"Close");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_SocketServData* CT_SocketServData::NewL()
	{
	CT_SocketServData*	ret=new (ELeave) CT_SocketServData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_SocketServData::CT_SocketServData()
:	iData(NULL)
	{
	}

void CT_SocketServData::ConstructL()
	{
	}

CT_SocketServData::~CT_SocketServData()
	{
	DestroyData();
	}

TAny* CT_SocketServData::GetObject()
	{
	return iData;
	}


void CT_SocketServData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iData = static_cast<RSocketServ*> (aAny);
	}

void CT_SocketServData::DisownObjectL()
	{
	iData = NULL;
	}

void CT_SocketServData::DestroyData()
	{
	delete iData;
	iData=NULL;	
	}

inline TCleanupOperation CT_SocketServData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_SocketServData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	RSocketServ* arg=static_cast<RSocketServ*>(aAny);
	delete arg;
	}

TBool CT_SocketServData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;
	
	if ( aCommand==KCmdRSocketServ )
		{
		DoCmdRSocketServ();
		}
	else if ( aCommand==KCmdConnect )
		{
		DoCmdConnect(aSection);
		}
	else if ( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test RSocketServ()
*/
void CT_SocketServData::DoCmdRSocketServ()
	{
	INFO_PRINTF1(_L("RSocketServ Constructor Call"));
	DestroyData();
	
	iData = new (ELeave) RSocketServ();
	}


/**
Test Connect()
*/
void CT_SocketServData::DoCmdConnect(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("RSocketServ Connect Call"));

	TInt	messageSlots = 0;
	TInt	err = 0;
	if(GetIntFromConfig(aSection, KMessageSlots(), messageSlots) )
		{
		err = iData->Connect(messageSlots);
		}
	else
		{
		err = iData->Connect();
		}
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("RSocketServ Connect failed with error %d"), err);
		SetError(err);
		}
	}


/**
Test Close()
*/
void CT_SocketServData::DoCmdClose()
	{
	INFO_PRINTF1(_L("RSocketServ Close Call"));

	iData->Close();
	}

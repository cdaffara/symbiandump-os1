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

#include "T_BTSyncPacketsData.h"
#include "T_BTUtil.h"

/*@{*/
// Parameter
_LIT(KTBTSyncPacket, "tbtsyncpacket");

//Commands
_LIT(KCmdTBTSyncPackets, "TBTSyncPackets");

//	Logging
_LIT(KLogMissingParameter,			"Missing parameter '%S'");
/*@}*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_BTSyncPacketsData* CT_BTSyncPacketsData::NewL()
	{
	CT_BTSyncPacketsData*	ret=new (ELeave) CT_BTSyncPacketsData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_BTSyncPacketsData::CT_BTSyncPacketsData()
:	iData(NULL)
	{
	}

void CT_BTSyncPacketsData::ConstructL()
	{
	}

CT_BTSyncPacketsData::~CT_BTSyncPacketsData()
	{
	DestroyData();
	}

//	Service methods
TAny* CT_BTSyncPacketsData::GetObject()
	{
	return iData;
	}

void CT_BTSyncPacketsData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iData = static_cast<TBTSyncPackets*> (aAny);
	}

void CT_BTSyncPacketsData::DisownObjectL()
	{
	iData = NULL;
	}

void CT_BTSyncPacketsData::DestroyData()
	{
	delete iData;
	iData=NULL;	
	}

inline TCleanupOperation CT_BTSyncPacketsData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_BTSyncPacketsData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	TBTSyncPackets* arg=static_cast<TBTSyncPackets*>(aAny);
	delete arg;
	}

TBool CT_BTSyncPacketsData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdTBTSyncPackets )
		{
		DoCmdTBTSyncPackets(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test TBTSyncPackets()
*/
void CT_BTSyncPacketsData::DoCmdTBTSyncPackets(const TDesC& aSection)
	{
	DestroyData();

	TBTSyncPackets::TSyncPackets	syncPacket;
	if ( CT_BTUtil::ReadTBTSyncPackets(*this, syncPacket, aSection, KTBTSyncPacket()) )
		{
		INFO_PRINTF1(_L("TBTSyncPackets Constructor Call"));
		iData = new (ELeave) TBTSyncPackets(syncPacket);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KTBTSyncPacket());
		SetBlockResult(EFail);	
		}
	
	if (iData == NULL) 
		{
		ERR_PRINTF1(_L("TBTSyncPackets is NULL"));
		SetBlockResult(EFail);	
		}
	}

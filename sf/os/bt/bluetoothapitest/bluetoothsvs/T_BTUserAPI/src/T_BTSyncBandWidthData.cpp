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

#include "T_BTSyncBandWidthData.h"

/*@{*/
//Parameters
_LIT(KBandWidth, "bandwidth");

//Commands
_LIT(KCmdTBTSyncBandwidth, "TBTSyncBandwidth");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_BTSyncBandwidthData* CT_BTSyncBandwidthData::NewL()
	{
	CT_BTSyncBandwidthData*	ret=new (ELeave) CT_BTSyncBandwidthData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_BTSyncBandwidthData::CT_BTSyncBandwidthData()
:	iData(NULL)
	{
	}

void CT_BTSyncBandwidthData::ConstructL()
	{
	}

CT_BTSyncBandwidthData::~CT_BTSyncBandwidthData()
	{
	DestroyData();
	}

//	Service methods
TAny* CT_BTSyncBandwidthData::GetObject()
	{
	return iData;
	}

void CT_BTSyncBandwidthData::SetObjectL(TAny* aAny)
	{
	//called when loading up persistent data
	DestroyData();
	iData = static_cast<TBTSyncBandwidth*> (aAny);
	}

void CT_BTSyncBandwidthData::DisownObjectL()
	{
	iData = NULL;
	}

void CT_BTSyncBandwidthData::DestroyData()
	{
	delete iData;
	iData=NULL;	
	}

inline TCleanupOperation CT_BTSyncBandwidthData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_BTSyncBandwidthData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	TBTSyncBandwidth* arg=static_cast<TBTSyncBandwidth*>(aAny);
	delete arg;
	}

TBool CT_BTSyncBandwidthData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdTBTSyncBandwidth )
		{
		DoCmdTBTSyncBandwidth(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test TBTSyncBandwidth()
*/
void CT_BTSyncBandwidthData::DoCmdTBTSyncBandwidth(const TDesC& aSection)
	{
	DestroyData();

	TInt	bandWidth=0;
	if( !GetIntFromConfig(aSection, KBandWidth(), bandWidth) )
		{
		INFO_PRINTF1(_L("TBTSyncBandwidth Constructor Call"));
		iData = new (ELeave) TBTSyncBandwidth();		
		}
	else
		{
		INFO_PRINTF1(_L("TBTSyncBandwidth Constructor Call"));
		iData = new (ELeave) TBTSyncBandwidth((TUint) bandWidth);		
		}

	if (iData == NULL) 
		{
		ERR_PRINTF1(_L("TBTSyncBandwidth is NULL"));
		SetBlockResult(EFail);
		}
	}

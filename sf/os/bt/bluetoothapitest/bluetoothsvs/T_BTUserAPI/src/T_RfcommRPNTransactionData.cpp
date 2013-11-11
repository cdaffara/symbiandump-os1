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

#include "T_RfcommRPNTransactionData.h"

/*@{*/
//Parameters
_LIT(KParamMask,										"mask");
_LIT(KObject,											"object");

//Commands
_LIT(KCmdRfcommRPNTransaction,							"TRfcommRPNTransaction");
_LIT(KCmdParamMask,										"SetParamMask");
_LIT(KCmdPortParams,									"SetPortParams");

//	Logging
_LIT(KLogMissingParameter,			"Missing parameter '%S'");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_RfcommRPNTransactionData* CT_RfcommRPNTransactionData::NewL()
	{
	CT_RfcommRPNTransactionData*	ret=new (ELeave) CT_RfcommRPNTransactionData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_RfcommRPNTransactionData::CT_RfcommRPNTransactionData()
:	iData(NULL)
	{
	}

void CT_RfcommRPNTransactionData::ConstructL()
	{
	}

CT_RfcommRPNTransactionData::~CT_RfcommRPNTransactionData()
	{
	DestroyData();
	}

TAny* CT_RfcommRPNTransactionData::GetObject()
	{
	return iData;
	}

void CT_RfcommRPNTransactionData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iData = static_cast<TRfcommRPNTransaction*> (aAny);
	}

void CT_RfcommRPNTransactionData::DisownObjectL()
	{
	iData = NULL;
	}

void CT_RfcommRPNTransactionData::DestroyData()
	{
	delete iData;
	iData=NULL;	
	}

inline TCleanupOperation CT_RfcommRPNTransactionData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_RfcommRPNTransactionData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	TRfcommRPNTransaction* arg=static_cast<TRfcommRPNTransaction*>(aAny);
	delete arg;
	}

TBool CT_RfcommRPNTransactionData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdRfcommRPNTransaction )
		{
		DoCmdTRfcommRPNTransaction();
		}
	else if ( aCommand==KCmdParamMask )
		{
		DoCmdParamMask(aSection);
		}
	else if ( aCommand==KCmdPortParams )
		{
		DoCmdPortParams(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test TRfcommRPNTransaction()
*/
void CT_RfcommRPNTransactionData::DoCmdTRfcommRPNTransaction()
	{
	DestroyData();
	INFO_PRINTF1(_L("TRfcommRPNTransaction Constructor Call"));
	iData = new (ELeave) TRfcommRPNTransaction();
	
	if (iData == NULL) 
		{
		ERR_PRINTF1(_L("TRfcommRPNTransaction is NULL"));
		SetBlockResult(EFail);
		}
	}


/**
Set iParaMask value
*/
void CT_RfcommRPNTransactionData::DoCmdParamMask(const TDesC& aSection)
	{
	TInt dataInput = 0;
	if(GetIntFromConfig(aSection, KParamMask(), dataInput) )
		{
		iData->iParamMask = (TUint16) dataInput;
		INFO_PRINTF2(_L("iParamMask value (%d)"), iData->iParamMask);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KParamMask());
		SetBlockResult(EFail);		
		}
	}

/**
Set iPortParams
*/
void CT_RfcommRPNTransactionData::DoCmdPortParams(const TDesC& aSection)
	{
	TPtrC	object;
	
	if(GetStringFromConfig(aSection, KObject(), object) )
		{
		INFO_PRINTF1(_L("Create a TRfcommRemotePortParams object for iPortParams"));
		TRfcommRemotePortParams* temp = static_cast<TRfcommRemotePortParams*>(GetDataObjectL(object));
		iData->iPortParams = *temp;
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KObject());
		SetBlockResult(EFail);
		}
	}

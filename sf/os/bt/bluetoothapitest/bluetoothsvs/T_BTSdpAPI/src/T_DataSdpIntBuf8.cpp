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

#include "T_DataSdpIntBuf8.h"

/*@{*/
//Parameters
_LIT(KInputInt,				"inputInt");

//Commands
_LIT(KCmdTSDPIntBuf8, 		"NewIntBuf");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpIntBuf8* CT_DataSdpIntBuf8::NewL()
	{
	CT_DataSdpIntBuf8*	ret=new (ELeave) CT_DataSdpIntBuf8();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpIntBuf8::CT_DataSdpIntBuf8()
:	iSdpIntBuf8(NULL)
	{
	}

void CT_DataSdpIntBuf8::ConstructL()
	{
	}

CT_DataSdpIntBuf8::~CT_DataSdpIntBuf8()
	{
	DestroyData();
	}


void CT_DataSdpIntBuf8::DestroyData()
	{
	delete iSdpIntBuf8;
	iSdpIntBuf8=NULL;
	}
	
void CT_DataSdpIntBuf8::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iSdpIntBuf8 = static_cast<TSdpIntBuf<TUint8>*> (aAny);
	}

void CT_DataSdpIntBuf8::DisownObjectL()
	{
	iSdpIntBuf8 = NULL;
	}
	
inline TCleanupOperation CT_DataSdpIntBuf8::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpIntBuf8::CleanupOperation(TAny* aAny)
	{
	TSdpIntBuf<TUint8>* arg=static_cast<TSdpIntBuf<TUint8>*>(aAny);
	delete arg;
	}

/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
TBool CT_DataSdpIntBuf8::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdTSDPIntBuf8 )
		{
		DoCmdNewIntBuf8L(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


void CT_DataSdpIntBuf8::DoCmdNewIntBuf8L(const TDesC & aSection)
	{
	TInt	theValue=0;
	DestroyData();
	if( GetIntFromConfig(aSection, KInputInt(), theValue))
		{
		iSdpIntBuf8 = new (ELeave) TSdpIntBuf<TUint8>((TUint8)theValue);
		INFO_PRINTF2(_L("TSdpIntBuf8 input value:(%d)"), theValue);
		INFO_PRINTF2(_L("TSdpIntBuf8 Object Call: 1st byte:(%d)"), (*iSdpIntBuf8)[0]);
		TUint8 	firstByte=theValue & 0xFF;
    		
		if( firstByte!=(*iSdpIntBuf8)[0] )
			{
			ERR_PRINTF1(_L("Data is not as expected!"));
			SetBlockResult(EFail);
			}
		INFO_PRINTF2(_L("TSdpIntBuf8 Call: 1st byte:(%d)"), firstByte);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KInputInt());
		SetBlockResult(EFail);
		}
	}





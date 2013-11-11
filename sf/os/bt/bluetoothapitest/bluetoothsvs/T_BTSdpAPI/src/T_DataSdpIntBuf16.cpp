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

#include "T_DataSdpIntBuf16.h"

/*@{*/
//Parameters
_LIT(KInputInt,				"inputInt");

//Commands
_LIT(KCmdTSDPIntBuf16, 		"NewIntBuf");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpIntBuf16* CT_DataSdpIntBuf16::NewL()
	{
	CT_DataSdpIntBuf16*	ret=new (ELeave) CT_DataSdpIntBuf16();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpIntBuf16::CT_DataSdpIntBuf16()
:	iSdpIntBuf16(NULL)
	{
	}

void CT_DataSdpIntBuf16::ConstructL()
	{
	}

CT_DataSdpIntBuf16::~CT_DataSdpIntBuf16()
	{
	DestroyData();
	}


void CT_DataSdpIntBuf16::DestroyData()
	{
	delete iSdpIntBuf16;
	iSdpIntBuf16=NULL;
	}
	
void CT_DataSdpIntBuf16::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iSdpIntBuf16 = static_cast<TSdpIntBuf<TUint16>*> (aAny);
	}

void CT_DataSdpIntBuf16::DisownObjectL()
	{
	iSdpIntBuf16 = NULL;
	}
	
inline TCleanupOperation CT_DataSdpIntBuf16::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpIntBuf16::CleanupOperation(TAny* aAny)
	{
	TSdpIntBuf<TUint16>* arg=static_cast<TSdpIntBuf<TUint16>*>(aAny);
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
TBool CT_DataSdpIntBuf16::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdTSDPIntBuf16 )
		{
		DoCmdNewIntBuf16L(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

void CT_DataSdpIntBuf16::DoCmdNewIntBuf16L(const TDesC & aSection)
	{
	TInt	theValue=0;

	if( GetIntFromConfig(aSection, KInputInt(), theValue))
		{
		iSdpIntBuf16 = new (ELeave) TSdpIntBuf<TUint16>((TUint16)theValue);

		TUint8 tempPtr[sizeof(TUint16)];
		TUint8 tempValue;
		TUint16 numValue = theValue;
		INFO_PRINTF2(_L("TSdpIntBuf16 input value:(%d)"), theValue);
		INFO_PRINTF3(_L("TSdpIntBuf16 Object Call: 1st byte:(%d),2nd byte:(%d)"),(*iSdpIntBuf16)[0],(*iSdpIntBuf16)[1]);

		for (TInt numRange = sizeof(TUint16) -1; numRange >= 0; numRange--)
			{
			tempValue = numValue & 0xFF; // transfer the lowest byte..
			tempPtr[numRange] = tempValue; //.. into the right hand side of the descriptor
			numValue	>>=8;
			
			if ( tempPtr[numRange]!=(*iSdpIntBuf16)[numRange] )
				{
				ERR_PRINTF1(_L("Data is not as expected!"));
				SetBlockResult(EFail);
				}
			}

		INFO_PRINTF3(_L("TSdpIntBuf16 Call:1st byte:(%d),2nd byte:(%d)"),tempPtr[0],tempPtr[1]);
		
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KInputInt());
		SetBlockResult(EFail);
		}
	}






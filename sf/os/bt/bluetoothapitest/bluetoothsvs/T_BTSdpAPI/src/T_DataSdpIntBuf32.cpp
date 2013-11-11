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

#include "T_DataSdpIntBuf32.h"

/*@{*/
//Parameters
_LIT(KInputInt,				"inputInt");

//Commands
_LIT(KCmdTSDPIntBuf32, 		"NewIntBuf");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpIntBuf32* CT_DataSdpIntBuf32::NewL()
	{
	CT_DataSdpIntBuf32*	ret=new (ELeave) CT_DataSdpIntBuf32();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpIntBuf32::CT_DataSdpIntBuf32()
:	iSdpIntBuf32(NULL)
	{
	}

void CT_DataSdpIntBuf32::ConstructL()
	{
	}

CT_DataSdpIntBuf32::~CT_DataSdpIntBuf32()
	{
	DestroyData();
	}


void CT_DataSdpIntBuf32::DestroyData()
	{
	delete iSdpIntBuf32;
	iSdpIntBuf32=NULL;
	}
	
void CT_DataSdpIntBuf32::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iSdpIntBuf32 = static_cast<TSdpIntBuf<TUint32>*> (aAny);
	}

void CT_DataSdpIntBuf32::DisownObjectL()
	{
	iSdpIntBuf32 = NULL;
	}
	
inline TCleanupOperation CT_DataSdpIntBuf32::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpIntBuf32::CleanupOperation(TAny* aAny)
	{
	TSdpIntBuf<TUint32>* arg=static_cast<TSdpIntBuf<TUint32>*>(aAny);
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
TBool CT_DataSdpIntBuf32::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdTSDPIntBuf32 )
		{
		DoCmdNewIntBuf32L(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

void CT_DataSdpIntBuf32::DoCmdNewIntBuf32L(const TDesC & aSection)
	{
	TInt	theValue=0;

	if( GetIntFromConfig(aSection, KInputInt(), theValue))
		{
		iSdpIntBuf32 = new (ELeave) TSdpIntBuf<TUint32>((TUint32)theValue);
		INFO_PRINTF5(_L("TSdpIntBuf32 Object Call: 1st byte:(%d),2nd byte:(%d),3rd byte:(%d),4th byte:(%d)"),(*iSdpIntBuf32)[0],(*iSdpIntBuf32)[1],(*iSdpIntBuf32)[2],(*iSdpIntBuf32)[3]);

		TUint8 tempPtr[sizeof(TUint32)];
		TUint8 tempValue;
		TUint32 numValue = theValue;

		for (TInt numRange = sizeof(TUint32) -1; numRange >= 0; numRange--)
			{
			tempValue = numValue & 0xFF; // transfer the lowest byte..
			tempPtr[numRange] = tempValue; //.. into the right hand side of the descriptor
			numValue >>= 8;
			if ( tempPtr[numRange]!=(*iSdpIntBuf32)[numRange] )
				{
				ERR_PRINTF1(_L("Data is not as expected!"));
				SetBlockResult(EFail);
				}
			}

		INFO_PRINTF5(_L("TSdpIntBuf32:1st byte:(%d),2nd byte:(%d),3rd byte:(%d),4th byte:(%d)"),tempPtr[0],tempPtr[1],tempPtr[2],tempPtr[3]);
		
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KInputInt());
		SetBlockResult(EFail);
		}
	}





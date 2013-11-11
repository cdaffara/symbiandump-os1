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

#include "T_DataSdpIntBuf64.h"

/*@{*/
//Parameters
#ifndef __Symbian_OS_v9_1__
_LIT(KInputInt,				"inputInt");
#endif

//Commands
_LIT(KCmdTSDPIntBuf64, 		"NewIntBuf");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpIntBuf64* CT_DataSdpIntBuf64::NewL()
	{
	CT_DataSdpIntBuf64*	ret=new (ELeave) CT_DataSdpIntBuf64();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpIntBuf64::CT_DataSdpIntBuf64()
:	iSdpIntBuf64(NULL)
	{
	}

void CT_DataSdpIntBuf64::ConstructL()
	{
	}

CT_DataSdpIntBuf64::~CT_DataSdpIntBuf64()
	{
	DestroyData();
	}


void CT_DataSdpIntBuf64::DestroyData()
	{
	delete iSdpIntBuf64;
	iSdpIntBuf64=NULL;
	}
	
void CT_DataSdpIntBuf64::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iSdpIntBuf64 = static_cast<TSdpIntBuf<TUint64>*> (aAny);
	}

void CT_DataSdpIntBuf64::DisownObjectL()
	{
	iSdpIntBuf64 = NULL;
	}
	
inline TCleanupOperation CT_DataSdpIntBuf64::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpIntBuf64::CleanupOperation(TAny* aAny)
	{
	TSdpIntBuf<TUint64>* arg=static_cast<TSdpIntBuf<TUint64>*>(aAny);
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
TBool CT_DataSdpIntBuf64::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdTSDPIntBuf64 )
		{
		DoCmdNewIntBuf64L(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

void CT_DataSdpIntBuf64::DoCmdNewIntBuf64L(const TDesC & aSection)
	{
	TPtrC	stringValue;
	if( GetStringFromConfig(aSection, KInputInt(), stringValue))
		{
		TLex16	lex(stringValue);
		TInt64	theValue=0;
		lex.Val(theValue, EDecimal);
		INFO_PRINTF2(_L("Given TUint64 = %lu"), (TUint64)theValue);

		iSdpIntBuf64 = new (ELeave) TSdpIntBuf<TUint64>((TUint64)theValue);
		INFO_PRINTF5(_L("TSdpIntBuf64 Object Call: 1st byte:(%d),2nd byte:(%d),3rd byte:(%d),4th byte:(%d)"),(*iSdpIntBuf64)[0],(*iSdpIntBuf64)[1],(*iSdpIntBuf64)[2],(*iSdpIntBuf64)[3] );
		INFO_PRINTF5(_L("TSdpIntBuf64 Object Call: 5th byte:(%d),6th byte:(%d),7th byte:(%d),8th byte:(%d)"),(*iSdpIntBuf64)[4],(*iSdpIntBuf64)[5],(*iSdpIntBuf64)[6],(*iSdpIntBuf64)[7] );
		TUint8 tempPtr[sizeof(TUint64)];
		TUint8 tempValue;
		TUint64 numValue = theValue;

		for (TInt numRange = sizeof(TUint64) -1; numRange >= 0; numRange--)
			{
			tempValue = numValue & 0xFF; // transfer the lowest byte..
			tempPtr[numRange] = tempValue; //.. into the right hand side of the descriptor
			numValue	>>= 8;
			if ( tempPtr[numRange]!=(*iSdpIntBuf64)[numRange] )
				{
				ERR_PRINTF1(_L("Data is not as expected!"));
				SetBlockResult(EFail);
				}
			}
		INFO_PRINTF5(_L("TSdpIntBuf64:1st byte:(%d),2nd byte:(%d),3rd byte:(%d),4th byte:(%d)"),tempPtr[0],tempPtr[1],tempPtr[2],tempPtr[3]);
		INFO_PRINTF5(_L("TSdpIntBuf64:5th byte:(%d),6th byte:(%d),7th byte:(%d),8th byte:(%d)"),tempPtr[4],tempPtr[5],tempPtr[6],tempPtr[7]);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KInputInt());
		SetBlockResult(EFail);
		}
	}


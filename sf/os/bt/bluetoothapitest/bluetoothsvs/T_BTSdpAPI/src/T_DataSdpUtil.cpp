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

#include "T_DataSdpUtil.h"
#include <es_sock.h>

/*@{*/

//Commands
_LIT(KCmdPutUint,		 									"PutUint");
_LIT(KCmdGetUint,		 									"GetUint");
_LIT(KCmdPutUint64,		 									"PutUint64");
_LIT(KCmdGetUint64,		 									"GetUint64");
_LIT(KCmdPutUint128,		 								"PutUint128");
_LIT(KCmdGetUint128,		 								"GetUint128");

//Parameters
_LIT(KInt, 													"integer");
_LIT(KNumberOfBytes, 										"numberOfBytes");
_LIT(KObject, 												"object");
_LIT(KNumber, 												"number");
_LIT(KLower, 												"low");
_LIT(KHigher, 												"high");

/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpUtil* CT_DataSdpUtil::NewL()
	{
	CT_DataSdpUtil*	ret=new (ELeave) CT_DataSdpUtil();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpUtil::CT_DataSdpUtil()
	{
	}

void CT_DataSdpUtil::ConstructL()
	{
	}

CT_DataSdpUtil::~CT_DataSdpUtil()
	{
	DestroyData();
	}

void CT_DataSdpUtil::SetObjectL(TAny* /*aAny*/)
	{
	}

void CT_DataSdpUtil::DestroyData()
	{
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
TBool CT_DataSdpUtil::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdPutUint )
		{
		DoCmdPutUint(aSection);
		}
	else if ( aCommand==KCmdGetUint )
		{
		DoCmdGetUint(aSection);
		}
	else if ( aCommand==KCmdPutUint64 )
		{
		DoCmdPutUint64(aSection);
		}
	else if ( aCommand==KCmdGetUint64 )
		{
		DoCmdGetUint64(aSection);
		}
	else if ( aCommand==KCmdPutUint128 )
		{
		DoCmdPutUint128(aSection);
		}
	else if ( aCommand==KCmdGetUint128 )
		{
		DoCmdGetUint128(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test PutUint()
*/
void CT_DataSdpUtil::DoCmdPutUint(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("SdpUtil PutUint call"));
	TPckgBuf<TUint64>	valuePackaged;
	TUint8*				dataPtr=&valuePackaged[0];

	TInt								theValue = 0;
	if( GetIntFromConfig(aSection, KInt(), theValue) )
		{
		TInt64 theInt = theValue;

		if( GetIntFromConfig(aSection, KNumberOfBytes(), theValue) )
			{
			TInt numberOfBytes = theValue;
			SdpUtil::PutUint(dataPtr, theInt, numberOfBytes);

			TInt64 temp = 0;
			TInt64 expectedValue = 0;
			for(TInt i=numberOfBytes-1; i >=0; --i)
				{
				temp = dataPtr[i]<<(8*(numberOfBytes-i-1));
				expectedValue = expectedValue + temp;
				}
			INFO_PRINTF2(_L("SdpUtil PutUint Call: actual = %d"), theInt);
			INFO_PRINTF2(_L("SdpUtil PutUint Call: expected = %d"), expectedValue);

			if( expectedValue != theInt )
				{
				ERR_PRINTF1(_L("PutUint is not as expected!"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Missing parameter %S"), &KNumberOfBytes());
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KNumberOfBytes());
		SetBlockResult(EFail);
		}
	}

/**
Test GetUint()
*/
void CT_DataSdpUtil::DoCmdGetUint(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("SdpUtil GetUint Call"));

	TPtrC								theString;
	if ( GetStringFromConfig(aSection, KObject(), theString) )
		{
		HBufC8*	buffer8=HBufC8::NewLC(theString.Length());
		buffer8->Des().Copy(theString);
		TPtr8	theString8=buffer8->Des();

		TUint	theTUint = SdpUtil::GetUint(theString8);
		INFO_PRINTF2(_L("SdpUtil GetUint Call: actual = %d"), theTUint);

		TUint expectedValue = 0;
		switch(theString.Length())
			{
		case 0:
			expectedValue = 0;
			break;
		case 1:
			expectedValue = theString8[0];
			break;
		case 2:
			expectedValue = BigEndian::Get16(theString8.Ptr());
			break;
		case 4:
			expectedValue = BigEndian::Get32(theString8.Ptr());
			break;
		default:
			expectedValue = 0;
			break;
			}
		INFO_PRINTF2(_L("SdpUtil GetUint Call: expected = %d"), expectedValue);

		if( theTUint!=expectedValue )
			{
			ERR_PRINTF1(_L("GetUint is not as expected!"));
			SetBlockResult(EFail);
			}
		CleanupStack::PopAndDestroy(buffer8);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KObject());
		SetBlockResult(EFail);
		}
	}

/**
Test PutUint64()
*/
void CT_DataSdpUtil::DoCmdPutUint64(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("SdpUtil PutUint64 call"));

	TInt								theValue = 0;
	TPckgBuf<TUint64>					valuePackaged;
	TUint8*								dataPtr=&valuePackaged[0];

	if( GetIntFromConfig(aSection, KNumber(), theValue) )
		{
		TUint64 theNumber = theValue;
		SdpUtil::PutUint64(dataPtr, theNumber);

		TUint64 temp = 0;
		TUint64 expectedValue = 0;
		for(TInt i=sizeof(TUint64)-1; i>=0; i--)
			{
			temp = dataPtr[i]<<(8*(sizeof(TUint64)-i-1));
			expectedValue = expectedValue + temp;
			}
		INFO_PRINTF2(_L("SdpUtil PutUint64 Call: actual = %lu"), theNumber);
		INFO_PRINTF2(_L("SdpUtil PutUint64 Call: expected = %lu"), expectedValue);

		if( expectedValue != theNumber )
			{
			ERR_PRINTF1(_L("PutUint64 is not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KNumber());
		SetBlockResult(EFail);
		}
	}

/**
Test GetUint64()
*/
void CT_DataSdpUtil::DoCmdGetUint64(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("SdpUtil GetUint64 Call"));

	TPtrC								theString;
	if( GetStringFromConfig(aSection, KObject(), theString) )
		{
		HBufC8*	buffer8=HBufC8::NewLC(theString.Length());
		buffer8->Des().Copy(theString);
		TPtr8	theString8=buffer8->Des();

		TUint64 theNumber = 0;
		SdpUtil::GetUint64(theString8, theNumber);
		INFO_PRINTF2(_L("SdpUtil GetUint64 Call: actual = %lu"), theNumber);

		const TUint8* dataPtr = theString8.Ptr();
		TUint64 expectedValue = (static_cast<TUint64>(dataPtr[0])<<56) | (static_cast<TUint64>(dataPtr[1])<<48) | (static_cast<TUint64>(dataPtr[2])<<40) | (static_cast<TUint64>(dataPtr[3])<<32) | (dataPtr[4]<<24) | (dataPtr[5]<<16) | (dataPtr[6]<<8) | dataPtr[7];
		INFO_PRINTF2(_L("SdpUtil GetUint64 Call: expected = %lu"), expectedValue);

		if( theNumber!=expectedValue )
			{
			ERR_PRINTF1(_L("GetUint64 is not as expected!"));
			SetBlockResult(EFail);
			}
		CleanupStack::PopAndDestroy(buffer8);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KObject());
		SetBlockResult(EFail);
		}
	}

/**
Test PutUint128()
*/
void CT_DataSdpUtil::DoCmdPutUint128(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("SdpUtil PutUint128 call"));

	TInt								theValue = 0;
	TPckgBuf<TUint64>					valuePackaged;
	TUint8*								dataPtr=&valuePackaged[0];

	if( GetIntFromConfig(aSection, KLower(), theValue) )
		{
		TUint64 lower = theValue;
		if( GetIntFromConfig(aSection, KHigher(), theValue) )
			{
			TUint64 higher = theValue;
			SdpUtil::PutUint128(dataPtr, lower, higher);
			INFO_PRINTF2(_L("SdpUtil PutUint128 Call: actual(Lower) = %lu"), lower);
			INFO_PRINTF2(_L("SdpUtil PutUint128 Call: actual(Higher) = %lu"), higher);

			TUint64 tempLow = 0;
			TUint64 tempHigh = 0;
			TInt offset = sizeof(TUint64);
			TUint64 lowExpectedValue = 0;
			TUint64 highExpectedValue = 0;
			for(TInt i=offset-1; i>=0; i--)
				{
				tempLow = dataPtr[i+offset]<<(8*(sizeof(TUint64)-i-1));
				lowExpectedValue = lowExpectedValue + tempLow;

				tempHigh = dataPtr[i]<<(8*(sizeof(TUint64)-i-1));
				highExpectedValue = highExpectedValue + tempHigh;
				}
			INFO_PRINTF2(_L("SdpUtil PutUint128 Call: expected(Lower) = %lu"), lowExpectedValue);
			INFO_PRINTF2(_L("SdpUtil PutUint128 Call: expected(Higher) = %lu"), highExpectedValue);
			if( (lowExpectedValue != lower) || (highExpectedValue != higher) )
				{
				ERR_PRINTF1(_L("PutUint128 is not as expected!"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Missing parameter %S"), &KHigher());
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KHigher());
		SetBlockResult(EFail);
		}
	}

/**
Test GetUint128()
*/
void CT_DataSdpUtil::DoCmdGetUint128(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("SdpUtil GetUint128 Call"));

	TPtrC								theString;
	if( GetStringFromConfig(aSection, KObject(), theString) )
		{
		HBufC8*	buffer8=HBufC8::NewLC(theString.Length());
		buffer8->Des().Copy(theString);
		TPtr8	theString8=buffer8->Des();

		TUint64 lower = 0;
		TUint64 higher = 0;
		SdpUtil::GetUint128(theString8, lower, higher);
		INFO_PRINTF3(_L("SdpUtil GetUint128 Call: actual(Lower/Higher) = %lu/%lu"), lower, higher);

		const TUint8* dataPtr = theString8.Ptr();
		TUint64 highExpectedValue = (static_cast<TUint64>(dataPtr[0])<<56) | (static_cast<TUint64>(dataPtr[1])<<48) | (static_cast<TUint64>(dataPtr[2])<<40) | (static_cast<TUint64>(dataPtr[3])<<32) | (dataPtr[4]<<24) | (dataPtr[5]<<16) | (dataPtr[6]<<8) | dataPtr[7];
		TUint64 lowExpectedValue = (static_cast<TUint64>(dataPtr[8])<<56) | (static_cast<TUint64>(dataPtr[9])<<48) | (static_cast<TUint64>(dataPtr[10])<<40) | (static_cast<TUint64>(dataPtr[11])<<32) | (dataPtr[12]<<24) | (dataPtr[13]<<16) | (dataPtr[14]<<8) | dataPtr[15];
		INFO_PRINTF3(_L("SdpUtil GetUint128 Call: expected(Lower/Higher) = %lu/%lu"), lowExpectedValue, highExpectedValue);

		if( (lowExpectedValue != lower) || (highExpectedValue != higher) )
			{
			ERR_PRINTF1(_L("GetUint128 is not as expected!"));
			SetBlockResult(EFail);
			}
		CleanupStack::PopAndDestroy(buffer8);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KObject());
		SetBlockResult(EFail);
		}
	}

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

#include "DataWrapperBase.h"

#define SECS_TO_MICROSEC(x)  (x*1000000)

/*@{*/
///	Constant Literals used.
_LIT(KPrefixHex,				"0x");
_LIT(KPrefixOctal,				"0");
_LIT(KSuffixBinary,				"b");

_LIT(KIncludeSection,			"include");
_LIT(KFile,						"file%d");
_LIT(KMatch,					"*{*,*}*");
_LIT(KStart,					"{");
_LIT(KSeparator,				",");
_LIT(KEnd,						"}");
_LIT(KDataRead,					"INI READ : %S %S %S");
_LIT(KTagPointX,				"%S_x");
_LIT(KTagPointY,				"%S_y");
_LIT(KTagRectMinX,				"%S_minX");
_LIT(KTagRectMaxX,				"%S_maxX");
_LIT(KTagRectMinY,				"%S_minY");
_LIT(KTagRectMaxY,				"%S_maxY");
_LIT(KTagSizeHeight,			"%S_height");
_LIT(KTagSizeWidth,				"%S_width");
_LIT(KEnumElements,				"enum_elements");
_LIT(KPrefixHexKey,				"0x%S");

_LIT(KCmdUtilityPromptMessage,	"utilityPromptMessage");
_LIT(KPromptText,				"message");
_LIT(KDelay,					"delay");
_LIT(KExpectedKey,				"keyCode");
/*@}*/

const TInt	KDefaultDelay	=	10;
const TInt	KBounceErrKey	=	50000;

CDataWrapperBase::CDataWrapperBase()
:	CDataWrapper()
	{
	}

CDataWrapperBase::~CDataWrapperBase()
/**
 * Public destructor
 */
	{
	iInclude.ResetAndDestroy();
	iBuffer.ResetAndDestroy();
	iFs.Close();
	}

void CDataWrapperBase::InitialiseL()
	{
	iTimer.CreateLocal();
	CDataWrapper::InitialiseL();
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TPtrC		fileName;
	TBool		moreData=ETrue;
	TBool		index=0;
	while ( moreData )
		{
		tempStore.Format(KFile(), ++index);
		moreData=GetStringFromConfig(KIncludeSection, tempStore, fileName);

		if (moreData)
			{
			CIniData*	iniData=CIniData::NewL(fileName);
			CleanupStack::PushL(iniData);
			iInclude.Append(iniData);
			CleanupStack::Pop(iniData);
			}
		}
	User::LeaveIfError(iFs.Connect());
	}

TBool CDataWrapperBase::GetBoolFromConfig(const TDesC& aSectName,const TDesC& aKeyName,TBool& aResult)
/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TBool reference passed in possible values TRUE, FALSE
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The value of the boolean
 * @return TBool - ETrue for found, EFalse for not found
 */
	{
	TBool	ret=EFalse;
	TPtrC	result;
	TRAPD(err, ret=GetCommandStringParameterL(aSectName, aKeyName, result));
	if ( err != KErrNone )
		{
		ret=EFalse;
		}
	if ( ret )
		{
		_LIT(KTrue,"true");
		aResult=(result.FindF(KTrue) != KErrNotFound);
		}

	return ret;
	}

TBool CDataWrapperBase::GetIntFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult)
/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TInt reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The value of the integer
 * @return TBool - ETrue for found, EFalse for not found
 */
	{
	TPtrC	result;
	TBool	ret=EFalse;
	TRAPD(err, ret=GetCommandStringParameterL(aSectName, aKeyName, result));
	if ( err != KErrNone )
		{
		ret=EFalse;
		}
	if ( ret )
		{
		TLex	lex(result);
		ret=(lex.Val(aResult)==KErrNone);
		}

	return ret;
	}

TBool CDataWrapperBase::GetStringFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult)
/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TPtrC reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - Reference to the string on the heap
 * @return TBool - ETrue for found, EFalse for not found
 */
	{
	TBool	ret=EFalse;
	TRAPD(err, ret=GetCommandStringParameterL(aSectName, aKeyName, aResult));
	if ( err != KErrNone )
		{
		ret=EFalse;
		}
	return ret;
	}

TBool CDataWrapperBase::GetHexFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult)
/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TInt reference passed in. The value can optionally be prefixed with 0x
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The integer value of the Hex input
 * @return TBool - ETrue for found, EFalse for not found
 */
	{
	TPtrC	result;
	TBool	ret=EFalse;
	TRAPD(err, ret=GetCommandStringParameterL(aSectName, aKeyName, result));
	if ( err != KErrNone )
		{
		ret=EFalse;
		}
	if ( ret )
		{
		TLex	lex;
		if( result.FindC(KPrefixHex)==KErrNone )
			{
			lex=result.Mid(KPrefixHex().Length());
			}
		else
			{
			lex=result;
			}
		ret=(lex.Val((TUint &)aResult, EHex)==KErrNone);
		}

	return ret;
	}

TBool CDataWrapperBase::GetUintFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint& aResult)
/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TUint reference passed in.
 * If the value is prefixed with 0x the value is read as a hexidecimal value
 * If the value is suffixed with b the value is read as a binary value
 * If the value is prefixed with a 0 the value is read as an octal value
 * If it does not match the above it is read in as an integer
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The integer value of the Hex input
 * @return TBool - ETrue for found, EFalse for not found
 */
	{
	TPtrC	result;
	TBool	ret=EFalse;
	TRAPD(err, ret=GetCommandStringParameterL(aSectName, aKeyName, result));
	if ( err != KErrNone )
		{
		ret=EFalse;
		}
	if ( ret )
		{
		TLex	lex(result);
		if( result.FindC(KPrefixHex)==KErrNone )
			{
			lex=result.Mid(KPrefixHex().Length());
			ret=(lex.Val(aResult, EHex)==KErrNone);
			}
		else
			{
			TInt	binarySuffixPosition=result.Length()-KSuffixBinary().Length();
			if ( result.FindC(KSuffixBinary)==binarySuffixPosition )
				{
				lex=result.Left(binarySuffixPosition);
				ret=(lex.Val(aResult, EBinary)==KErrNone);
				}
			else
				{
				if( result.FindC(KPrefixOctal)==KErrNone )
					{
					ret=(lex.Val(aResult, EOctal)==KErrNone);
					}
				else
					{
					TInt	intResult;
					ret=(lex.Val(intResult)==KErrNone);
					if ( ret )
						{
						aResult=(TUint)intResult;
						}
					}
				}
			}
		}

	return ret;
	}

TBool CDataWrapperBase::GetEnumFromConfig(const TDesC& aSectName, const TDesC& aKeyName, const TEnumEntryTable* aTable, TInt& aResult)
	{
	TPtrC	str;
	TBool	ret=GetStringFromConfig(aSectName, aKeyName, str);

	if ( ret )
		{
		TBool	found=EFalse;
		TInt	index=0;
		while ( (aTable[index].iValue!=-1) && !found )
			{
			if ( aTable[index].iString==str )
				{
				found=ETrue;
				aResult=aTable[index].iValue;
				}
			else
				{
				++index;
				}
			}

		if ( !found )
			{
			ret=GetIntFromConfig(aSectName, aKeyName, aResult);
			}
		}

	return ret;
	}

TBool CDataWrapperBase::GetOrFromConfig(const TDesC& aSectName, const TDesC& aKeyName, const TEnumEntryTable* aTable, TUint& aResult)
	{
	TPtrC 	str;
	TBool	ret=GetStringFromConfig(aSectName, aKeyName, str);
	if ( ret )
		{
		TUint	temp=0;
		ret=ProcessString(str, temp, aTable);
		if ( ret )
			{
			aResult=temp;
			}
		}

	return ret;
	}

TBool CDataWrapperBase::ProcessString(const TDesC& aStr, TUint& aResult, const TEnumEntryTable* aTable)
	{
	TBool	ret=ETrue;
	TInt	location=aStr.Match(_L("*|*"));
	if( location!=KErrNotFound )
		{
		// Converting Left part of the data
		TPtrC	tempStr=aStr.Left(location);
		ret=ProcessString(tempStr, aResult, aTable);

		// Converting right data can be with another "|"
		tempStr.Set(aStr.Mid(location+1));

		TUint	temp;
		if ( ProcessString(tempStr, temp, aTable) )
			{
			aResult|=temp;
			}
		else
			{
			ret=EFalse;
			}
		}
	else
		{
		ret=ProcessEntry(aStr, aResult, aTable);
		}

	return ret;
	}

TBool CDataWrapperBase::ProcessEntry(const TDesC& aStr, TUint& aResult, const TEnumEntryTable* aTable)
	{
	TBool	ret=ETrue;

	TBool	found=EFalse;
	TInt	index=0;
	while ( (aTable[index].iValue!=-1) && !found )
		{
		if ( aTable[index].iString==aStr )
			{
			found=ETrue;
			aResult=aTable[index].iValue;
			}
		else
			{
			++index;
			}
		}

	if ( !found )
		{
		TUint	flags;
		TLex	lex(aStr);
		ret=(lex.Val(flags, EHex)==KErrNone);
		if ( ret )
			{
			aResult=flags;
			}
		}

	return ret;
	}

TBool CDataWrapperBase::GetCommandStringParameterL(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult)
	{
	TBool	ret=EFalse;

	if ( aSectName.Length()!=0 )
		{
		ret=CDataWrapper::GetStringFromConfig(aSectName, aKeyName, aResult);

		for ( TInt index=iInclude.Count(); (index>0) && (!ret); )
			{
			ret=iInclude[--index]->FindVar(aSectName, aKeyName, aResult);
			if ( ret )
				{
				INFO_PRINTF4(KDataRead, &aSectName, &aKeyName, &aResult);
				}
			}
		}

	if ( ret )
		{
		if ( aResult.Match(KMatch)!=KErrNotFound )
			{
			//	We have an entry of the format
			//	entry =*{section,entry}*
			//	where * is one or more characters
			//	We need to construct this from other data in the ini file replacing {*,*}
			//	with the data from
			//	[section]
			//	entry =some_value
			HBufC*	buffer=HBufC::NewLC(aResult.Length());
			buffer->Des().Copy(aResult);

			TInt	startLength=KStart().Length();
			TInt	sparatorLength=KSeparator().Length();
			TInt	endLength=KEnd().Length();
			TInt	bufferLength;
			TInt	start;
			TInt	sparator;
			TInt	end;
			TPtrC	remaining;
			TLex	lex;
			do
				{
				bufferLength=buffer->Length();
				start=buffer->Find(KStart);

				remaining.Set(buffer->Des().Right(bufferLength-start-startLength));
				sparator=remaining.Find(KSeparator);
				remaining.Set(remaining.Right(remaining.Length()-sparator-sparatorLength));
				sparator += (start + startLength);

				end=remaining.Find(KEnd) + sparator + sparatorLength;

				TPtrC	sectionName(buffer->Ptr()+start+startLength, sparator-start-startLength);
				TPtrC	keyName(buffer->Ptr()+sparator+sparatorLength, end-sparator-sparatorLength);
				sectionName.Set(TLex(sectionName).NextToken());
				keyName.Set(TLex(keyName).NextToken());

				TInt	entrySize=0;
				TPtrC	entryData;
				TBool	found=CDataWrapper::GetStringFromConfig(sectionName, keyName, entryData);
				for ( TInt index=iInclude.Count(); (index>0) && (!found);  )
					{
					found=iInclude[--index]->FindVar(sectionName, keyName, entryData);
					}
				if ( found )
					{
					entrySize=entryData.Length();
					}

				TInt	newLength=start + bufferLength - end - endLength + entrySize;
				HBufC*	bufferNew=HBufC::NewLC(newLength);
				bufferNew->Des().Copy(buffer->Ptr(), start);
				if ( entrySize>0 )
					{
					bufferNew->Des().Append(entryData);
					}
				bufferNew->Des().Append(buffer->Ptr() + end + endLength, bufferLength - end - endLength);
				CleanupStack::Pop(bufferNew);
				CleanupStack::PopAndDestroy(buffer);
				buffer=bufferNew;
				CleanupStack::PushL(buffer);
				}
			while ( buffer->Match(KMatch)!=KErrNotFound );
			iBuffer.Append(buffer);
			CleanupStack::Pop(buffer);
			aResult.Set(*buffer);
			INFO_PRINTF4(KDataRead, &aSectName, &aKeyName, &aResult);
			}
		}

	return ret;
	}

TBool CDataWrapperBase::GetCommandStringParameter(const TDesC& aParameterName, const TDesC& aSection, TPtrC& aResult, TText8 *aFileName, TInt aLine, TBool aMandatory)
	{
	TBool	ret = GetStringFromConfig(aSection, aParameterName, aResult);
	if (aMandatory && !ret)
		{
		Logger().LogExtra(aFileName, aLine, ESevrErr, _L("No %S"), &aParameterName);
		SetBlockResult(EFail);
		}
	return ret;
	}

TBool CDataWrapperBase::GetCommandIntParameter(const TDesC& aParameterName, const TDesC& aSection, TInt& aResult, TText8* aFileName, TInt aLine, TBool aMandatory)
	{
	TBool	ret = GetIntFromConfig(aSection, aParameterName, aResult);
	if (aMandatory && !ret)
		{
		Logger().LogExtra(aFileName, aLine, ESevrErr, _L("No %S"), &aParameterName);
		SetBlockResult(EFail);
		}
	return ret;
	}

TBool CDataWrapperBase::GetCommandBoolParameter(const TDesC& aParameterName, const TDesC& aSection, TBool& aResult, TText8 *aFileName, TInt aLine, TBool aMandatory)
	{
	TBool	ret = GetBoolFromConfig(aSection, aParameterName, aResult);
	if (aMandatory && !ret)
		{
		Logger().LogExtra(aFileName, aLine, ESevrErr, _L("No %S"), &aParameterName);
		SetBlockResult(EFail);
		}
	return ret;
	}

TBool CDataWrapperBase::GetPointFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TPoint& aResult)
/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TPoint reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The value of the TPoint
 * @return TBool - ETrue for found, EFalse for not found
 */
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TInt	x;
	tempStore.Format(KTagPointX, &aKeyName);
	TBool	ret=GetIntFromConfig(aSectName, tempStore, x);
	TInt	y;
	tempStore.Format(KTagPointY, &aKeyName);
	if ( !GetIntFromConfig(aSectName, tempStore, y) )
		{
		ret=EFalse;
		}
	if ( ret )
		{
		aResult.SetXY(x, y);
		}

	return ret;
	}

TBool CDataWrapperBase::GetRectFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TRect& aResult)
/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TPoint reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The value of the TRect
 * @return TBool - ETrue for found, EFalse for not found
 */
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TInt	minX;
	tempStore.Format(KTagRectMinX, &aKeyName);
	TBool	ret=GetIntFromConfig(aSectName, tempStore, minX);

	TInt	maxX;
	tempStore.Format(KTagRectMaxX, &aKeyName);
	if ( !GetIntFromConfig(aSectName, tempStore, maxX) )
		{
		ret=EFalse;
		}

	TInt	minY;
	tempStore.Format(KTagRectMinY, &aKeyName);
	if ( !GetIntFromConfig(aSectName, tempStore, minY) )
		{
		ret=EFalse;
		}

	TInt	maxY;
	tempStore.Format(KTagRectMaxY, &aKeyName);
	if ( !GetIntFromConfig(aSectName, tempStore, maxY) )
		{
		ret=EFalse;
		}

	if ( ret )
		{
		aResult.SetRect(minX, minY, maxX, maxY);
		}

	return ret;
	}

TBool CDataWrapperBase::GetSizeFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TSize& aResult)
/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TPoint reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The value of the TPoint
 * @return TBool - ETrue for found, EFalse for not found
 */
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	TInt	height;
	tempStore.Format(KTagSizeHeight, &aKeyName);
	TBool	ret=GetIntFromConfig(aSectName, tempStore, height);
	TInt	width;
	tempStore.Format(KTagSizeWidth, &aKeyName);
	if ( !GetIntFromConfig(aSectName, tempStore, width) )
		{
		ret=EFalse;
		}
	if ( ret )
		{
		aResult.SetSize(width, height);
		}

	return ret;
	}

TBool CDataWrapperBase::GetUint8FromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint8& aResult)
	{
	TUint	temp;
	TBool	ret=GetUintFromConfig(aSectName, aKeyName,temp);
	if ( ret )
		{
		aResult=(TUint8)temp;
		}

	return ret;
	}

void CDataWrapperBase::Timedelay(TInt aTimeoutInSecs)
/*Utility function to produce time delay
@param aTimeoutInSecs Times in micro seconds
*/
	{
	TRequestStatus	status;
	iTimer.After(status, aTimeoutInSecs);
	User::WaitForRequest(status);
	}

TBool CDataWrapperBase::TranslateEnumToString(CDataWrapper& aDataWrapper,
										const TDesC& aEnum,
										const TInt aValue,
										TDesC& aTranslation)
/* Function translates
*/
	{
	TBool ret = ETrue;

	RBuf desToWrapInt;
	RBuf desToFormHex;
	TInt intError = desToWrapInt.Create(KMaxFullName);
	TInt hexError = desToFormHex.Create(KMaxFullName);
	if(intError == KErrNone && hexError == KErrNone)
		{
		desToFormHex.NumFixedWidth(aValue, EHex, 8);
		desToWrapInt.Format(KPrefixHexKey, &desToFormHex);
		if (!aDataWrapper.GetStringFromConfig(aEnum, desToWrapInt, static_cast<TPtrC&>(aTranslation)))
			{
			ret = EFalse;
			}
		}
	else
		{
		ret = EFalse;
		}

	if(hexError == KErrNone)
		{
		desToFormHex.Close();
		}
	if(intError == KErrNone)
		{
		desToWrapInt.Close();
		}

	return ret;
	}

TBool CDataWrapperBase::ProcessEnumList(CDataWrapper& aDataWrapper,
									const TDesC& aStr,
									const TDesC& aEnum,
									const TUint& aValue,
									TDes& aTranslation)
	{
	TBool	ret=ETrue;
	TInt	location=aStr.Match(_L("*|*"));
	if( location!=KErrNotFound )
		{
		// Converting Left part of the data
		TPtrC	tempStr=aStr.Left(location);
		ret=ProcessEnumList(aDataWrapper, tempStr, aEnum, aValue, aTranslation);

		// Converting right data can be with another "|"
		tempStr.Set(aStr.Mid(location+1));

		if ( !ProcessEnumList(aDataWrapper, tempStr, aEnum, aValue, aTranslation) )
			{
			ret=EFalse;
			}
		}
	else
		{
		TInt value;
		if (aDataWrapper.GetHexFromConfig(aEnum, aStr, value ))
			{
			if (value & aValue)
				{
				if ( aTranslation.Length() )
					{
					aTranslation.Append(_L("|"));
					}

				aTranslation.Append(aStr);
				}
			}
		else
			{
			ret = EFalse;
			}
		}

	return ret;
	}

TBool CDataWrapperBase::TranslateNumberToEnumStringL(CDataWrapper& aDataWrapper,
									   const TDesC& aSection,
									   const TInt aValue,
									   TDes& aTranslation)
	{
	TBool ret = EFalse;

	TPtrC	enumElements;
	if ( aDataWrapper.GetStringFromConfig(aSection, KEnumElements, enumElements) )
		{
		ret = ProcessEnumList(aDataWrapper, enumElements, aSection, aValue, aTranslation);
		}

	return ret;
	}
/**
 * Function which creates a consoles and writes to and reads from the console
 *
 * @param aMsg			Message wrote to the console
 * @param aDelayMiliSec  	waiting period for user input, read from config file
 * @param aKeyCode		specifies key required to be pressed to pass the test
 *
 * @leave					System wide error
 */

void CDataWrapperBase::PromptMessageL(const TDesC& aMsg,
								const TInt aDelayMiliSec,
								TKeyCode& aKeyCode)
	{
	CConsoleBase* console = GetConsoleL();
	CleanupStack::PushL(console);

	if( aMsg.Length() != 0 )
		{
		console->Printf(aMsg);
		}
	TRequestStatus keyStatus(KRequestPending);
	TRequestStatus timerStatus(KRequestPending);
	aKeyCode=EKeyNull;
	iTimer.After(timerStatus,SECS_TO_MICROSEC(aDelayMiliSec));
	console->Read(keyStatus);
	User::WaitForRequest(keyStatus,timerStatus);
	if (keyStatus!=KRequestPending)
		{
		//Keypressed
		iTimer.Cancel();
		aKeyCode =console->KeyCode();
		User::WaitForRequest(timerStatus);
		}
	else
		{
		console->ReadCancel();
		SetBlockResult(EFail);
		}

	CleanupStack::PopAndDestroy(console);
	}
/**
 * Process a command read from the script file calling the required function
 *
 * @param aSection			The section in the ini containing data for the command
 * @param aCommand			Command to be called
 *
 * @leave					System wide error
 */

TBool CDataWrapperBase::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{

	if ( aCommand==KCmdUtilityPromptMessage )
		{
		DoCmdUtilityPromptMessageL(aSection);
		}
	else
		{
		ERR_PRINTF1(_L("No Function Matches Requested Command"));
		SetBlockResult(EFail);
		}

	return ETrue;
	}
/**
 * Functon called through TEF for manual verification which writes a message to the console and reads user input keyboard
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @leave					System wide error
 */

void CDataWrapperBase::DoCmdUtilityPromptMessageL(const TDesC& aSection)

	{
	INFO_PRINTF1(_L("DoCmdUtilityPromptMessageL called"));

	TPtrC	message;
	TBool   hasMessage =  GetStringFromConfig(aSection, KPromptText(), message);

	if( !hasMessage )
		{
		INFO_PRINTF1(_L("Prompt message not specified in the ini file"));
		}

	// 10 second default delay
	TInt	delay = KDefaultDelay;
	GetIntFromConfig(aSection, KDelay(), delay);
	TKeyCode keyCode;
	do
		{
		PromptMessageL(message, delay, keyCode);
		}while (keyCode>KBounceErrKey);
	INFO_PRINTF2(_L("Key %d pressed"), keyCode);
	TInt expectedKey;
	if( GetIntFromConfig(aSection, KExpectedKey(), expectedKey) )
		{
		// compares key pressed with key in config file
		if(keyCode != (TKeyCode)expectedKey)
			{
			ERR_PRINTF3(_L("expectedKey=%d pressed key=%d"), expectedKey, keyCode);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("KeyCode not specified in the ini file"));
		SetBlockResult(EFail);
		}

	}

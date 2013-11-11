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

#include "datawrapperbase.h"

/*@{*/
///	Constant Literals used.
_LIT(KPrefixHex,"0x");
_LIT(KPrefixOctal,"0");
_LIT(KSuffixBinary,			"b");

_LIT(KIncludeSection,		"include");
_LIT(KFile,					"file%d");
_LIT(KMatch,				"*{*,*}*");
_LIT(KStart,				"{");
_LIT(KSeparator,			",");
_LIT(KEnd,					"}");
_LIT(KDataRead,				"INI READ : %S %S %S");

/*@}*/

CDataWrapperBase::CDataWrapperBase()
:	CDataWrapper()
	{
	}

/**
 * Public destructor
 */
CDataWrapperBase::~CDataWrapperBase()
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

/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TBool reference passed in possible values TRUE, FALSE
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The value of the boolean
 * @return TBool - ETrue for found, EFalse for not found 
 */
TBool CDataWrapperBase::GetBoolFromConfig(const TDesC& aSectName,const TDesC& aKeyName,TBool& aResult)
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

/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TInt reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The value of the integer
 * @return TBool - ETrue for found, EFalse for not found 
 */
TBool CDataWrapperBase::GetIntFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult)
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

/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TPtrC reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - Reference to the string on the heap
 * @return TBool - ETrue for found, EFalse for not found 
 */
TBool CDataWrapperBase::GetStringFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult)
	{
	TBool	ret=EFalse;
	TRAPD(err, ret=GetCommandStringParameterL(aSectName, aKeyName, aResult));
	if ( err != KErrNone )
		{
		ret=EFalse;
		}
	return ret;
	}

/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TInt reference passed in. The value can optionally be prefixed with 0x
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The integer value of the Hex input
 * @return TBool - ETrue for found, EFalse for not found 
 */	
TBool CDataWrapperBase::GetHexFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult)
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
TBool CDataWrapperBase::GetUintFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint& aResult)
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

/**
 *   Return array of string parameters i.e. key=a1,a2,a3 returns array which contains
 *   String a1, a2 and a3.
 *   @return ret - EFalse if can't get a String parameter from Config file.  ETrue if KErrNone
 */
TBool CDataWrapperBase::GetArrayRectFromConfig(const TDesC& aSectName, const TDesC& aKeyName, RPointerArray<HBufC>& aResult)
	{
	TBool	ret=EFalse;
	TPtrC completeArray;
	
	TRAPD(err, ret=GetCommandStringParameterL(aSectName, aKeyName, completeArray));
	if ( err != KErrNone )
		{
		ret=EFalse;
		}

    TLex16 lex(completeArray); // Here we have the array as a string i.e. "a1,a2,a3"
    TBuf<256> buf;
    TChar chr;
    
    while(!lex.Eos())
        {
        chr = lex.Get();
        // Check if there was a list separator
        if ((chr == ',') && (lex.Peek() == '('))
            {
            HBufC* param = buf.AllocLC();
            buf.Zero();
            aResult.Append(param);
            CleanupStack::Pop(param); // pointer to buf is stored in RPointerArray
            }
        // If not separator character we can store the character into array
        else
            {
            buf.Append(chr);
            }
        }
    // Remember to put last token into array (,a3)
    HBufC* param = buf.AllocLC();
    aResult.Append(param);
    CleanupStack::Pop(param);
    
    return ret;
	}

/**
 * Reads the parameter	asociated to the specified command
 * @param aSectName	Section on ini file
 * @param aKeyName		Name of the parameter
 * @param aResult			descriptor containing parameter
 * @return TBool  ETrue for found, EFalse for not found 
 */
TBool CDataWrapperBase::GetCommandStringParameterL(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult)
	{
	TBool	ret=EFalse;

	if ( aSectName.Length()!=0 )
		{
		ret=CDataWrapper::GetStringFromConfig(aSectName, aKeyName, aResult);

		for ( TInt index=iInclude.Count(); (index>0) && (!ret); )
			{
			ret=iInclude[--index]->FindVar(aSectName, aKeyName, aResult);
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
			INFO_PRINTF4(KDataRead, &aSectName, &aKeyName , &aResult);
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

/**
 * Utility function to produce time delay
 * @param aTimeoutInSecs Times in micro seconds
 */
void CDataWrapperBase::Timedelay(TInt aTimeoutInSecs)
	{
	TRequestStatus	status;
	iTimer.After(status, aTimeoutInSecs);
	User::WaitForRequest(status);
	}

// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Character conversion class definitions
// 
//

/**
 @file Schconv.cpp 
*/

#include "SCHCONV.H"	
#include <utf.h>
#include "ND_STD.H"

_LIT(KUnicodeType1,"ISO-10646-UCS-2");	//< 3 names for UNICODE
_LIT(KUnicodeType2,"csUnicode");
_LIT(KUnicodeType3,"UNICODE-1-1");
_LIT(KIso88591Type1,"ISO-8859-1");	//< 3 name for ISO-8859-1

/*
_LIT(KIso88591Type2,"ISO_8859-1");
_LIT(KIso88591Type3,"latin1");
*/

// CScriptCharacterConverter definitions 

CScriptCharacterConverter* CScriptCharacterConverter::NewL()
/**
2 phased constructor for CScriptCharacterConverter, first phase.

@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CScriptCharacterConverter object.
*/
	{
	CScriptCharacterConverter* r=new(ELeave) CScriptCharacterConverter();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CScriptCharacterConverter::CScriptCharacterConverter()
	: iDefaultCharSet(KIso88591Type1)
/**
Private constructor for CScriptCharacterConverter, used in the first phase of construction.
*/
	{}

void CScriptCharacterConverter::ConstructL()
/**
Instantiates member variables.

@exception Leaves if file server Connect() fails or CCnvCharacterSetConverter::NewL() leaves.
*/
	{
	User::LeaveIfError(iFs.Connect());
	iConverter=CCnvCharacterSetConverter::NewL();
	}

CScriptCharacterConverter::~CScriptCharacterConverter()
/**
Destructor.
Closes file server.
Deletes iConverter.
*/
	{
	iFs.Close();
	delete iConverter;
	}

HBufC8* CScriptCharacterConverter::ConvertL(const TDesC8& aString, const TDesC& aCharSet)
/**
Calls ConvertLC.
*/
	{
	HBufC8* buf;
	buf=ConvertLC(aString,aCharSet);
	CleanupStack::Pop(); //buf
	return buf;
	}

HBufC8* CScriptCharacterConverter::ConvertL(const TDesC16& aString, const TDesC& aCharSet)
/**
Calls ConvertLC.
*/
	{
	HBufC8* buf;
	buf=ConvertLC(aString,aCharSet);
	CleanupStack::Pop(); //buf
	return buf;
	}

HBufC8* CScriptCharacterConverter::ConvertLC(const TDesC8& aString, const TDesC& aCharSet)
/**
Converts from ISO-8859-1 to aCharSet or if aCharSet is empty to the default
character set. Returns the result in the form of an 8-bit buffer ready for sending
to comm port. Leaves with KErrCharacterConversionError if there is an error in
conversion.
*/
	{
	TBuf<KMaxCharacterTypeLength> type=aCharSet;
	if (type.Length()==0)
		type.Copy(iDefaultCharSet);
	
	HBufC8* buf=HBufC8::NewLC(aString.Length()); // must put this on the cleanup stack first as we're leaving it there

// first convert to unicode
	if (iConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierIso88591,iFs)!=CCnvCharacterSetConverter::EAvailable)
		User::Leave(KErrCharacterSetDoesNotExist);
	HBufC16* buf16=HBufC16::NewLC(aString.Length());
	TPtr16 unicode(buf16->Des());
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	TInt ret=iConverter->ConvertToUnicode(unicode,aString,state);
	if (ret!=KErrNone)	// this conversion is from ISO-8859-1 to unicode, so we know the length
		User::Leave(KErrCharacterConversionError);	

	if (IsUnicode(type))
		{
		TPtr8 resString(buf->Des());
		CopyToEightBitDataL(resString,unicode);
		}
	else
		{
		PrepareToConvertL(type);
		TPtrC16 remainingUnicode(unicode);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		FOREVER
			{
			TPtr8 resString(buf->Des());
			ret=iConverter->ConvertFromUnicode(resString,remainingUnicode,state);
			if (ret<KErrNone)
				{
				User::Leave(KErrCharacterConversionError);	
				break;
				}
			else
				{
				if (ret==KErrNone)
					break;
				buf->ReAllocL(buf->Length()+ret);
				}
			remainingUnicode.Set(remainingUnicode.Right(ret));
			}
		}

	CleanupStack::PopAndDestroy();						// buf16
	return buf;
	}
		
HBufC8* CScriptCharacterConverter::ConvertLC(const TDesC16& aString, const TDesC& aCharSet)
/**
Converts from UNICODE to aCharSet or if aCharSet is empty to the default
character set. Returns the result in th form of an 8-bit buffer ready for sending
to comm port. Leaves with KErrCharacterConversionError if there is an error in
conversion.
*/
	{
	TBuf<KMaxCharacterTypeLength> type=aCharSet;
	if (type.Length()==0)
		type.Copy(iDefaultCharSet);
	
	HBufC8* buf=HBufC8::NewLC(aString.Length());;
	TPtrC16 remainingUnicode(aString);
	TInt ret=KErrNone;
	
	if (IsUnicode(type))
		{
		TPtr8 resString(buf->Des());
		CopyToEightBitDataL(resString,aString);
		}
	else	// other types
		{ 
		PrepareToConvertL(type);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		FOREVER
			{
			TPtr8 resString(buf->Des());
			ret=iConverter->ConvertFromUnicode(resString,remainingUnicode,state);
			if (ret<KErrNone)
				{
				User::Leave(KErrCharacterConversionError);	
				break;
				}
			else
				{
				if (ret==KErrNone)
					break;
				buf->ReAllocL(buf->Length()+ret);
				}
			remainingUnicode.Set(remainingUnicode.Right(ret));
			}
		}

	return buf;
	}

void CScriptCharacterConverter::ConvertFromDefaultL(const TDesC8& aSrc,TDes16& aTrg)
/**
Converts aString from the default character to set to Unicode
Should have been passed a big enough buffer for the conversion to happen in one go
so leaves if the return value from the conversion is not 0
*/
	{
	DoConvertFromDefaultToUnicodeL(aSrc,aTrg);
	}

void CScriptCharacterConverter::SetDefaultCharSet(const TDesC& aCharSet)
/**
Sets default character set.
*/
	{
	iDefaultCharSet=aCharSet;
	}

void CScriptCharacterConverter::PrepareToConvertL(const TDesC& aNameToMatch)
/**
Prepares to convert.
*/
	{
	TBuf8<KMaxCharacterTypeLength> type;
	type.Copy(aNameToMatch);
	const TInt charSetId=iConverter->ConvertStandardNameOfCharacterSetToIdentifierL(type,iFs);
	if ((charSetId==0) || (iConverter->PrepareToConvertToOrFromL(charSetId,iFs)!=CCnvCharacterSetConverter::EAvailable))
		User::Leave(KErrCharacterSetDoesNotExist);
	}

void CScriptCharacterConverter::CopyToEightBitDataL(TDes8& aTrg, const TDesC16& aSrc)
/**
Copies to eight bit.
*/
	{
	TInt len=aSrc.Length()*2;
	if (len>aTrg.MaxLength())
		User::Leave(KErrOverflow);
	TUint8* ptrToBuf=(TUint8*)aSrc.Ptr();
	aTrg.Copy(ptrToBuf,len);
	}	

void CScriptCharacterConverter::CopyFromEightBitDataL(TDes16& aTrg, const TDesC8& aSrc)

	{
	TInt len=aSrc.Length()/2;	// if source length is odd then we can't use the last 8 bits anyway
	if (len>aTrg.MaxLength())
		User::Leave(KErrOverflow);
	TUint16* ptrToBuf=(TUint16*)aSrc.Ptr();
	aTrg.Copy(ptrToBuf,len);
	}	

void CScriptCharacterConverter::DoConvertFromDefaultToUnicodeL(const TDesC8& aSrc,TDes16& aTrg)
/**
Converts aString from the default character to set to unicode
*/
	{
	__ASSERT_DEBUG(aSrc.Length()<=KRxBufferSize, User::Invariant());
	__ASSERT_DEBUG(aTrg.MaxLength()>=KRxBufferSize, User::Invariant());

	TInt state=CnvUtfConverter::KStateDefault;
	TInt ret=KErrNone;
	aTrg.SetLength(0);

	if (IsUnicode(iDefaultCharSet))
		CopyFromEightBitDataL(aTrg,aSrc);
	else
		{
		PrepareToConvertL(iDefaultCharSet);
		ret=iConverter->ConvertToUnicode(aTrg,aSrc,state);
		}

	if (ret!=KErrNone)
		User::Leave(KErrCharacterConversionError);
	}

TBool CScriptCharacterConverter::IsUnicode(const TDesC& aType) const
/**
Checks if type is unicode.
*/
	{
	if ((aType.CompareF(KUnicodeType1)==KErrNone) 
		|| (aType.CompareF(KUnicodeType2)==KErrNone)
		|| (aType.CompareF(KUnicodeType3)==KErrNone))
		return ETrue;
	return EFalse;
	}

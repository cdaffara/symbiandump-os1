// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalAll
*/

#include <ir_sock.h>
#include "client_util.H"

EXPORT_C TIASResponse::TIASResponse()
/** Default constructor. Marks the response type as missing.

@see TIASDataType */
	{
	SetLength(1);
	operator[](0)=EIASDataMissing;
	}

void TIASResponse::SetToInteger(TUint anInteger)
//
// set the response data to the specified integer
//
	{
	SetLength(5);
	operator[](0)=(TUint8)EIASDataInteger;
	BigEndian::Put32(WPtr()+1,anInteger);
	}

void TIASResponse::SetToCharString(const TDesC8& aCharString)
//
// set the response data to the specified integer
//
	{
	__ASSERT_ALWAYS(aCharString.Length()<=256,IrdaUtil::Panic(EIASResponseDes8Overflow));
	SetLength(3);
	operator[](0)=(TUint8)EIASDataUserString;				// Type field
	operator[](1)=(TUint8)EIASCharSetUserStringASCII;		// Character set
	operator[](2)=(TUint8)aCharString.Size();				// String Size

	Append(aCharString);
	}

#ifdef _UNICODE
void TIASResponse::SetToCharString(const TDesC16& aWideString)
//
// set the response data to the specified integer
//
	{
	__ASSERT_ALWAYS(aWideString.Size()<=256,IrdaUtil::Panic(EIASResponseDes8Overflow));
	SetLength(3);
	operator[](0)=(TUint8)EIASDataUserString;				// Type field
	operator[](1)=(TUint8)EIASCharSetUserStringUnicode;		// Character Set
	operator[](2)=(TUint8)aWideString.Size();				// String Size

	TUint8* narrowPtr = (TUint8*)(aWideString.Ptr());
	TPtr8  narrowString(narrowPtr,aWideString.Size(),aWideString.Size());
	Append(narrowString);
	}
#endif

void TIASResponse::SetToOctetSeq(const TDesC8& aBinData)
//
// set the response data to the specified integer
//
	{
	__ASSERT_ALWAYS(aBinData.Length()<=256,IrdaUtil::Panic(EIASResponseDes8Overflow));
	SetLength(3);
	operator[](0)=(TUint8)EIASDataOctetSequence;			// Type field
	BigEndian::Put16(WPtr()+1,(TUint16)aBinData.Length());
	Append(aBinData);
	}

//
// Suck out an octet sequence if there is one.
//
EXPORT_C TInt TIASResponse::GetInteger(TInt &aResult,TInt anIndex) const
/** Returns the response integer.

This is called if the response type indicates an integer.

@param aResult On return, contains the 32-bit response integer value. 
@param anIndex Reserved for future use. This argument must be allowed to default 
to 0 and must not be overriden. 
@return KErrNone, if successful. KErrCorrupt, if the response type is not an 
integer. */
	{
	if (Type()!=EIASDataInteger)
		return KErrCorrupt;
	// IAS Integers are 4 bytes, zero indexed with one length byte.
	if (Length()<5+anIndex*4)
		return KErrCorrupt;
	aResult=BigEndian::Get32(Ptr()+1);
	return KErrNone;
	}

//
// Suck out an octet sequence if there is one.
//
EXPORT_C TInt TIASResponse::GetOctetSeq(TDes8 &aResult,TInt anIndex) const
/** Returns the response binary data.

@param aResult On return, contains the the binary data. The number of bytes 
returned can never be greater than (KMaxQueryStringLength - 3). 
@param anIndex Reserved for future use. 
@return KErrNone, if successful. KErrCorrupt, if the response type is not binary 
data. KErrNotSupported, if a non-zero value has been specified for anIndex. */
	{
	if (anIndex>0)
		return KErrNotSupported;
	if (Type()!=EIASDataOctetSequence)
		return KErrCorrupt;

	aResult.Copy(Ptr()+3,operator[](2));
	return KErrNone;	
	}

//
// Suck out an ASCII/ISO string if there is one.
//
EXPORT_C TInt TIASResponse::GetCharString(TDes8 &aResult,TInt anIndex) const
/** Returns the response string.

This is called if the response type indicates a string.

@param aResult On return, an 8 bit modifiable descriptor containing the response 
string. The length of the response string can never be greater than the value 
of (KMaxQueryStringLength - 3). 
@param anIndex Reserved for future use. This argument must be allowed to default 
to 0 and must not be overriden. 
@return If successful, one of the TIASCharSet enumerator values defining the 
character set encoding of the response string. KErrNotSupported, if a non-zero 
value has been specified for anIndex. KErrCorrupt, if the response type is 
not a string. 
@see TIASResponse::Type() */
	{
	if (anIndex>0)
		return KErrNotSupported;
	if (Type()!=EIASDataUserString)
		return KErrCorrupt;

	TUint8 charSet = operator[](1);
	TInt returnValue = static_cast<TInt>(charSet);
	aResult.Copy(Ptr()+3,operator[](2));

	return returnValue;
	}

//
// Suck out an ASCII/ISO string if there is one.
//
EXPORT_C const TPtrC8 TIASResponse::GetCharString8(TInt anIndex) const
/** Returns a non-modifiable pointer descriptor representing the response string.

This is called if the response type indicates a string.

@param anIndex Reserved for future use. This argument must be allowed to default 
to 0 and must not be overriden. 
@return A non-modifiable pointer descriptor representing the response string. 
If the response type is not a string or the string is flagged as being UNICODE, 
then this pointer descriptor is empty, i.e. it has zero length. 
@see TIASResponse::Type() */
	{
	TPtr8 p(NULL,0);

	if (anIndex>0)
		return p;
	if (Type()!=EIASDataUserString || operator[](3)==EIASCharSetUserStringUnicode)
		return p;

	p.Set(WPtr()+3,operator[](2),operator[](2));

	return TPtrC8(p);
	}

//
// Suck out a unicode string if there is one.
//
EXPORT_C TInt TIASResponse::GetCharString(TDes16 &/*aResult*/,TInt /*anIndex*/) const
/** Gets a TDes16 string of unicode characters from the TIASResponse object. */
	{
	if (Type()!=EIASDataUserString && operator[](3)!=EIASCharSetUserStringUnicode)
		return KErrCorrupt;
	return KErrNotSupported;
	}

//
// Suck out a unicode string if there is one.
//
EXPORT_C const TPtrC16 TIASResponse::GetCharString16(TInt /*anIndex*/) const
/** Gets a TPtrC16 string of unicode characters from the TIASResponse object. */
	{
	return TPtr16(NULL,0);
	}

EXPORT_C TBool TIASResponse::IsList() const
	{
	return operator[](0)>0;
	}

EXPORT_C TInt TIASResponse::NumItems() const
	{
	return operator[](0);
	}

EXPORT_C TIASDataType TIASResponse::Type() const
	{
	if ((operator[](0)>EIASDataMissing)&&(operator[](0)<=EIASDataUserString))
		return  (TIASDataType)operator[](0);
	else
		return EIASDataMissing;
	}

// EOF

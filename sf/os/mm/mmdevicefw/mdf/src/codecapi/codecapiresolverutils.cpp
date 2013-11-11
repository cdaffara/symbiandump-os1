// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <mdf/codecapiresolverutils.h>
#include "codecapiresolverconsts.h"


/**
 Creates a new CCodecApiOpaqueData object.
 @param  aOpaqueData
 		 A reference to the opaque data value.
 @return A pointer to the newly constructed match data object.
 */
EXPORT_C CCodecApiOpaqueData* CCodecApiOpaqueData::NewL(const TDesC8& aOpaqueData)
	{
	CCodecApiOpaqueData* result = CCodecApiOpaqueData::NewLC(aOpaqueData);
	CleanupStack::Pop(result);
	return result;
	}


/**
 Creates a new CCodecApiOpaqueData object and leaves a pointer to it on the cleanup stack.
 @param  aOpaqueData
 		 A reference to the opaque data value.
 @return A pointer to the newly constructed match data object.
 */
EXPORT_C CCodecApiOpaqueData* CCodecApiOpaqueData::NewLC(const TDesC8& aOpaqueData)
	{
	CCodecApiOpaqueData* result = new (ELeave) CCodecApiOpaqueData(aOpaqueData);
	CleanupStack::PushL(result);
	result->ConstructL();
	return result;
	}


/**
 Constructor.
 @param  aOpaqueData
 		 A reference to the opaque data value.
 */
CCodecApiOpaqueData::CCodecApiOpaqueData(const TDesC8& aOpaqueData) :
	iOpaqueData(aOpaqueData)
	{
	}

/**
 Sets up the data inside the class by calling <code>ParseTaggedDataL()</code>. 
 */
void CCodecApiOpaqueData::ConstructL()
	{
	ParseTaggedDataL();
	}


/**
 Destructor.
 */
CCodecApiOpaqueData::~CCodecApiOpaqueData()
	{
	delete iInputDataFormat;
	delete iOutputDataFormat;
	}

/*
 Parses the opaque data and calls <code>ProcessTaggedDataL()</code> to set the
 data member of the class.
 */
void CCodecApiOpaqueData::ParseTaggedDataL()
	{
	TInt readPosition = 0;
	TBool moreData = (iOpaqueData.Length()>0) ? ETrue : EFalse;
	while (moreData)
		{
		// Assume that this segment will begin with a tag
		TPtrC8 restOfData = iOpaqueData.Mid(readPosition);
		TInt endPos = restOfData.MatchF(KTagMatch);
		if (endPos == KErrNotFound)
			{
			User::Leave(KErrCorrupt);
			}		
		// Extract the tag
		TPtrC8 tag = restOfData.Left(KTagLength);
		readPosition += KTagLength;
		// Find the next tag
		restOfData.Set(iOpaqueData.Mid(readPosition));
		endPos = restOfData.MatchF(KTagMatch);
		TPtrC8 tagData;
		if (endPos == KErrNotFound)
			{
			// If we didn't find a tag, we must be at the end of the data
			tagData.Set(restOfData);
			readPosition = restOfData.Length();
			moreData = EFalse;
			}
		else
			{
			tagData.Set(restOfData.Left(endPos));
			readPosition += endPos;
			}
		ProcessTaggedDataL(tag, tagData);		
		}
	}



/**
 Sets member data to the value of the given opaque data, based upon the tag value provided. 
 @param  aTag
 	   	 A constant reference to a tag from the opaque data. Can be \<i\>, \<s\> and \<d\>.
 @param  aData
 	   	 The data associated with a tag.
 */
void CCodecApiOpaqueData::ProcessTaggedDataL(const TDesC8& aTag, const TDesC8& aData)
	{
	if (aTag==KMediaUid)
		{
		SetMediaUidL(aData);
		return;
		}
	if (aTag==KInputPortFormat)
		{
		HBufC8* dataFormat = aData.AllocL();
		delete iInputDataFormat;
		iInputDataFormat = dataFormat;
		return;
		}
	if (aTag==KOutputPortFormat)
		{
		HBufC8* dataFormat = aData.AllocL();
		delete iOutputDataFormat;
		iOutputDataFormat = dataFormat;
		return;
		}
	User::Leave(KErrCorrupt);	
	}


/**
 Sets the value of the media UID data.
 @param  aData
 		 The value to set the media uid to.
 */
void CCodecApiOpaqueData::SetMediaUidL(const TDesC8& aData)
	{
	ConvertTextToUidL(aData, iTypeUid);
	}


/**
 Converts a given string to an integer.
 @param  aData
 		 The value to be converted.
 @return The converted value
 */
TUint CCodecApiOpaqueData::ConvertTextToTUintL(const TDesC8& aData)
	{
	// Determine whether hex or decimal then parse as such
	_LIT8(K0x, "0x");
	TInt lenHexMarker = K0x().Length();
	TUint value = 0;

	// simple white space left trim (only handles spaces)
	int index = 0;
	while (aData[index]==' ')
		{
		index++;
		}
	TPtrC8 data = aData.Mid(index);
	if (((data.Left(lenHexMarker).CompareF(K0x) == 0)) && (data.Length() >= 3))
		{
		// Only take the characters after "0x"
		TLex8 lex(data.Mid(lenHexMarker));
		User::LeaveIfError(lex.Val(value, EHex));
		}
	else if (data.Length() > 0)
		{
		TLex8 lex(data);
		User::LeaveIfError(lex.Val(value, EDecimal));
		}
	else
		{
		User::Leave(KErrCorrupt);
		}		
	return value;
	}


/**
 Converts a given string to a UID.
 @param  aData
 		 The value to be converted.
 @param  aData
 		 The value after conversion.
 */
void CCodecApiOpaqueData::ConvertTextToUidL(const TDesC8& aData, TUid& aUid)
	{
	_LIT8(K0x, "0x");	
	if ((aData.Length() == 10) && (aData.FindF(K0x) == 0))
		{
		// only take the right 8 characters (ie discard the "0x")
		TLex8 lex(aData.Right(8));
		TUint32 value = 0;
		User::LeaveIfError(lex.Val(value, EHex));
		aUid.iUid = value;
		}
	else
		{
		User::Leave(KErrCorrupt);
		}		
	}


/**
 Returns the value of the type UID data member.
 @return The value of the type UID data member.
 */
EXPORT_C TUid CCodecApiOpaqueData::TypeUid() const
	{
	return iTypeUid;
	}
	

/**
 Returns the value of the input data type.
 @return The value of the input data type.
 */	
EXPORT_C const TDesC8& CCodecApiOpaqueData::InputDataType() const
	{
	if (iInputDataFormat)
		{
		return *iInputDataFormat;
		}
	return KNullDesC8;
	}


/**
 Returns the value of the output data type.
 @return The value of the output data type.
 */	
EXPORT_C const TDesC8& CCodecApiOpaqueData::OutputDataType() const
	{
	if (iOutputDataFormat)
		{
		return *iOutputDataFormat;
		}
	return KNullDesC8;
	}


/**
 Compares the input data type with the argument of the method.
 @param  aDataType
 		 The value to be compared to.
 @return ETrue if the argument of the method and the input data 
 		 type are the same.
 */
EXPORT_C TBool CCodecApiOpaqueData::CompareInputDataType(const TDesC8& aDataType)
	{	
	if(iInputDataFormat)
		{
		const TDesC8& i = *iInputDataFormat;
		if(aDataType.CompareF(i) == 0)
			{
			return ETrue;
			}			
		}	
	return EFalse;
	}
	
	
/**
 Compares the output data type with the argument of the method.
 @param  aDataType
 		 The value to be compared to.
 @return ETrue if the argument of the method and the output data
 		 type are the same.
 */	
EXPORT_C TBool CCodecApiOpaqueData::CompareOutputDataType(const TDesC8& aDataType)
	{
	if(iOutputDataFormat)
		{
		if(aDataType.CompareF(*iOutputDataFormat) == 0)
			{
			return ETrue;
			}		
		}	
	return EFalse;
	}	
	
	

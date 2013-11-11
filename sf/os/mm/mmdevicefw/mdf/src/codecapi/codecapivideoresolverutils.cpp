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

#include <mdf/codecapivideoresolverutils.h>
#include <mmf/devvideo/devvideoplay.h>
#include "codecapiresolverconsts.h"

/**
 Creates a new CCodecApiVideoOpaqueData object.
 @param  aOpaqueData
 		 A reference to the opaque data value.
 @return A pointer to the newly constructed match data object.
 */
EXPORT_C CCodecApiVideoOpaqueData* CCodecApiVideoOpaqueData::NewL(const TDesC8& aOpaqueData)
	{
	CCodecApiVideoOpaqueData* result = CCodecApiVideoOpaqueData::NewLC(aOpaqueData);
	CleanupStack::Pop(result);
	return result;
	}


/**
 Creates a new CCodecApiOpaqueData object and leaves a pointer to it on the cleanup stack.
 @param  aOpaqueData
 		 A reference to the opaque data value.
 @return A pointer to the newly constructed match data object.
 */
EXPORT_C CCodecApiVideoOpaqueData* CCodecApiVideoOpaqueData::NewLC(const TDesC8& aOpaqueData)
	{
	CCodecApiVideoOpaqueData* result = new (ELeave) CCodecApiVideoOpaqueData(aOpaqueData);
	CleanupStack::PushL(result);
	result->ConstructL();
	return result;	
	}


/**
 Constructor
 @param  aOpaqueData
 		 A reference to the opaque data value.
 */
CCodecApiVideoOpaqueData::CCodecApiVideoOpaqueData(const TDesC8& aOpaqueData) :
	CCodecApiOpaqueData(aOpaqueData),
	iManufacturer(NULL)
	{
	}

/**
 Sets up the data inside the class by calling <code>ParseTaggedDataL()</code>. 
 */
void CCodecApiVideoOpaqueData::ConstructL()
	{
	ParseTaggedDataL();
	}


/**
 Returns the value of the maximun picture size data member.
 @return The maximum picture size.
 */
EXPORT_C const TSize& CCodecApiVideoOpaqueData::MaxPictureSize() const
	{	
	return iMaxPictureSize;
	}

/**
 Returns the value of the manufacturer name data member.
 @return Constant reference to the manufactured name.
 */	
EXPORT_C const TDesC8& CCodecApiVideoOpaqueData::Manufacturer() const
	{
	if (iManufacturer)
		{
		return *iManufacturer;
		}
	return KNullDesC8;
	}

/**
 Returns the array with values for the picture rate and size.
 @return Array containing the picture rate and size values.
 */	
EXPORT_C const RArray<TPictureRateAndSize>& CCodecApiVideoOpaqueData::MaxPictureRates() const
	{
	return iMaxPictureRates;
	}


/**
 Sets the values in the TPictureRateAndSize structure.
 @param aData
		A constant reference to the data containting values for TPictureRateAndSize.
 */	
void CCodecApiVideoOpaqueData::SetPictureRateAndSizeL(const TDesC8& aData)
	{
	TPictureRateAndSize pictureRateAndSize;
	TInt position = 0;
	TUint32 value = 0;
	TPtrC8 restOfData = aData;
	position = aData.MatchF(KComma);
	if (position == KErrNotFound)
		{
		User::Leave(KErrCorrupt);
		}		
	TLex8 lex(restOfData.Left(position));
	User::LeaveIfError(lex.Val(value, EDecimal));	
	pictureRateAndSize.iPictureRate = value;
	position++;
	restOfData.Set(aData.Mid(position));
	position = restOfData.MatchF(KComma);
	if (position == KErrNotFound)
		{
		User::Leave(KErrCorrupt);
		}		
	lex = restOfData.Left(position);
	User::LeaveIfError(lex.Val(value, EDecimal));	
	pictureRateAndSize.iPictureSize.iWidth = value;
	position++;
	restOfData.Set(restOfData.Mid(position));
	lex = restOfData;
	User::LeaveIfError(lex.Val(value, EDecimal));
	pictureRateAndSize.iPictureSize.iHeight = value;		
	
	iMaxPictureRates.AppendL(pictureRateAndSize);
	}

/**
 Sets the values in the TPictureRateAndSize structure.
 @param aData
		A constant reference to the data containting values for TPictureRateAndSize.
 */	
void CCodecApiVideoOpaqueData::SetMaxPictureSizeL(const TDesC8& aData)
	{
	TPictureRateAndSize pictureRateAndSize;
	TInt position = 0;
	TPtrC8 restOfData = aData.Mid(position);
	position = restOfData.MatchF(KComma);
	if (position == KErrNotFound)
		{
		User::Leave(KErrCorrupt);
		}		
	iMaxPictureSize.iWidth = ConvertTextToTUintL(restOfData.Left(position));
	position++;
	restOfData.Set(aData.Mid(position));
	iMaxPictureSize.iHeight = ConvertTextToTUintL(restOfData);
	}

/**
 Sets member data to the value of the given opaque data, based upon the tag value provided. 
 @param  aTag
 		 A constant reference to a tag from the opaque data. Can be \<m\>, \<l\>, \<p\>.
 @param  aData
 		 The data associated with a tag.
 */
void CCodecApiVideoOpaqueData::ProcessTaggedDataL(const TDesC8& aTag, const TDesC8& aData)
	{
	if (aTag == KManufacturer)
		{
		if(!iManufacturer)
			{
			iManufacturer = aData.AllocL();
			return;		
			}
		else
			{
			User::Leave(KErrCorrupt);
			}		
		}
	if (aTag == KMaxPictureSize)
		{
		SetMaxPictureSizeL(aData);
		return;
		}
	if (aTag == KListOfPictureRateAndSize)
		{
		return SetPictureRateAndSizeL(aData);
		}
	CCodecApiOpaqueData::ProcessTaggedDataL(aTag, aData);
	}


/**
 Destructor
 */
CCodecApiVideoOpaqueData::~CCodecApiVideoOpaqueData()
	{
	delete iManufacturer;
	iMaxPictureRates.Reset();
	iMaxPictureRates.Close();
	}
	

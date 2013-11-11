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

#include "LbsAssistanceReferenceLocationLayout.h"
#include "LbsAssistanceNavigationModelLayout.h"
#include "LbsAssistanceReferenceTimeLayout.h"
#include "LbsAssistanceIonosphericModelLayout.h"
#include "LbsAssistanceAlmanacLayout.h"
#include "LbsAssistanceBadSatListLayout.h"
#include "LbsAssistanceAquisitionAssistanceLayout.h"
#include "LbsAssistanceUtcModelLayout.h"
#include <e32base.h>
#include "LbsAssistanceInternal.h"

typedef void (*TCopyFunction) (const TUint32* aSource, TUint32* aDest);

/**
RDataBuilderBase constructor
@internalComponent
 */
RDataBuilderBase::RDataBuilderBase(const TLbsAssistanceDataLayoutMap* aLayout)
:	iBody(TLbsAssistanceDataBody(aLayout))
	{}

/**
DataBuffer returns a TPtrC8 
@return Returns pointer to data buffer
 */
EXPORT_C TPtrC8 RDataBuilderRootBase::DataBuffer() const
	{
	ASSERT(iData);	//iData must not be null
	return TPtrC8(reinterpret_cast<const TUint8*>(iData), iDataSize);
	}

/**
DataBufferAccess returns a TPtr8. This allows
copying another's Builder buffer into this 
builder's
@return Returns pointer to data buffer
 */
EXPORT_C TPtr8 RDataBuilderRootBase::DataBufferAccess()
	{
	ASSERT(iData);	//iData must not be null
	return TPtr8(reinterpret_cast<TUint8*>(iData), iDataSize, iDataSize);
	}

/**
TimStamp returns the time stamp of this builder
@return the time stamp of this builder
 */
EXPORT_C const TTime& RDataBuilderRootBase::TimeStamp() const
{
    ASSERT(iBody.iDataPtr);
	return *(iBody.iUtcTimeStampPtr);
}

/**
TimStamp Set's the builder's time stamp to the 
UTC time at the time this method is called.
@return Returns the time stamp of this builder
 */
EXPORT_C void RDataBuilderRootBase::SetTimeStamp(const TTime& aUtcTimeStamp)
{
	ASSERT(iBody.iDataPtr);
	*(iBody.iUtcTimeStampPtr) = aUtcTimeStamp;	
}

/**
IsOpen returns ETrue if the builder has been opened, EFalse otherwise.
@return Boolean indicating if the builder is open.
 */
EXPORT_C TBool RDataBuilderRootBase::IsOpen() const
{
	return (iData != NULL);
}

/**
Open method
@internalComponent
 */	
void RDataBuilderRootBase::DoOpenL(TInt aDataSize)
	{
	__ASSERT_ALWAYS(iData==NULL, User::Invariant());
	iData = reinterpret_cast<TUint32*>(User::AllocZL(aDataSize));
	iDataSize = aDataSize;
	iBody.iDataPtr = iData;
	}

/**
Close 
 */	
EXPORT_C void RDataBuilderRootBase::Close()
	{
	delete iData;
	iData = NULL;
	iBody.iDataPtr = NULL;
	iBody.iUtcTimeStampPtr = NULL;
	iDataSize = 0;
	}
/**
RDataBuilderRootBase constructor
@internalComponent
 */	
RDataBuilderRootBase::RDataBuilderRootBase(const TLbsAssistanceDataLayoutMap* aLayout)
:	RDataBuilderBase(aLayout),
	iData(NULL),
	iDataSize(0)
	{
	}

/**
RDataBuilderArrayBase: constructor
@internalComponent
 */
RDataBuilderArrayBase::RDataBuilderArrayBase(const TLbsAssistanceDataLayoutMap* aLayout)
:	RDataBuilderBase(aLayout)
	{
	}

TLbsAssistanceDataBody& RDataBuilderBase::Body()
	{
	return iBody;
	}
	
const TLbsAssistanceDataBody& RDataBuilderBase::Body() const
	{
	return iBody;
	}
	
//

/**
RDataReaderBase constructor
@internalComponent
 */
RDataReaderBase::RDataReaderBase(const TLbsAssistanceDataLayoutMap* aLayout)
:	iBody(TLbsAssistanceDataBody(aLayout))
	{
	}

/**
Returns empty TPtr8
@return Returns pointer to data buffer
 */
EXPORT_C TPtr8 RDataReaderRootBase::DataBuffer()
	{
	ASSERT(iData);	//iData must not be NULL.
	return TPtr8(reinterpret_cast<TUint8*>(iData), iDataSize, iDataSize);
	}

/**
Close method
 */	
EXPORT_C void RDataReaderRootBase::Close()
	{
	delete iData;
	iData = NULL;
	iBody.iDataPtr = NULL;
	iBody.iUtcTimeStampPtr = NULL;
	iDataSize = 0;
	}

/**
TimStamp returns the time stamp of this builder
@return the time stamp of this builder
 */
EXPORT_C const TTime& RDataReaderRootBase::TimeStamp() const
{
	ASSERT(iBody.iDataPtr);
	return *(iBody.iUtcTimeStampPtr);
}

/**
IsOpen returns ETrue if the reader has been opened, EFalse otherwise.
@return Boolean indicating if the reader is open.
 */
EXPORT_C TBool RDataReaderRootBase::IsOpen() const
{
	return (iData != NULL);
}

/**
Leaving Open method
@internalComponent
 */
void RDataReaderRootBase::DoOpenL(TInt aDataSize)
	{
	__ASSERT_ALWAYS(iData==NULL, User::Invariant());
	iData = reinterpret_cast<TUint32*>(User::AllocZL(aDataSize));
	iDataSize = aDataSize;
	iBody.iDataPtr = iData;
	}

/**
RDataReaderRootBase constructor
@internalComponent
 */
RDataReaderRootBase::RDataReaderRootBase(const TLbsAssistanceDataLayoutMap* aLayout)
:	RDataReaderBase(aLayout),
	iData(NULL),
	iDataSize(0)
	{
	}


/**
RDataReaderArrayBase constructor
@internalComponent
 */
RDataReaderArrayBase::RDataReaderArrayBase(const TLbsAssistanceDataLayoutMap* aLayout)
:	RDataReaderBase(aLayout)
	{
	}

TLbsAssistanceDataBody& RDataReaderBase::Body()
	{
	return iBody;
	}
	
const TLbsAssistanceDataBody& RDataReaderBase::Body() const
	{
	return iBody;
	}
	
//


//This function maps the field type to the number of 32bit quantities that it occupies
//hence the divide by 4 to get from bytes to 32bit words.
inline TInt IndexSize(TSupportedFieldTypes aFieldType)
	{
	static const TInt KIndexSizes[] =
		{
		(sizeof(TInt))/4,
		(sizeof(TUint))/4,
		1,
		(sizeof(TAlmanacSatInfoArrayLayout))/4,
		(sizeof(TAlmanacSatInfoLayout))/4,
		(sizeof(TSatelliteStatus))/4,
		(sizeof(TEphemerisParameterLayout))/4,
		(sizeof(TNavigationModelSatInfoLayout))/4,
		(sizeof(TNavigationModelSatInfoArrayLayout))/4,
		(sizeof(TBadSatListArrayLayout))/4,
		(sizeof(TEllipsoidPointAltitudeEllipsoideLayout))/4,
		(sizeof(TGpsTowAssistLayout))/4,
		(sizeof(TUtranGpsReferenceTimeLayout))/4,
		(sizeof(TSfnTowUncertainty))/4,
		(sizeof(TUtranGpsDriftRate))/4,
		(sizeof(TGpsTowAssistArrayLayout))/4,
		(sizeof(TDopplerUncertainty))/4,
		(sizeof(TExtraDopplerInfoLayout))/4,
		(sizeof(TCodePhaseSearchWindow))/4,
		(sizeof(TAzimuthAndElevationLayout))/4,
		(sizeof(TAcquisitionSatInfoLayout))/4,
		(sizeof(TAcquisitionSatInfoArrayLayout))/4,
		(sizeof(TPrimaryCpichInfoLayout))/4,
        (sizeof(TSubFrame1ReservedArrayLayout))/4,
        (sizeof(TRrlpGpsReferenceTimeLayout))/4,
		};
	return KIndexSizes[aFieldType];
	}

void CopyTUint32(const TUint32* aSource, TUint32* aDest)
	{
	*aDest = *aSource;
	}

void CopyFromDes8(const TUint32* aSource, TUint32* aDest)
	{
	const TDesC8* const desSource = reinterpret_cast<const TDesC8*>(aSource);
	TDes8PackEditor* const desDest = reinterpret_cast<TDes8PackEditor*>(aDest);
	const TInt length = desSource->Length();
	desDest->iLength = length;
	Mem::Copy(desDest->iData, desSource->Ptr(), length);
	}

void CopyToDes8(const TUint32* aSource, TUint32* aDest)
	{
	const TDes8PackEditor* const desSource = reinterpret_cast<const TDes8PackEditor*>(aSource);
	TDes8* const desDest = reinterpret_cast<TDes8*>(aDest);
	TPtrC8 desSourcePtr(desSource->iData, desSource->iLength);
	desDest->Copy(desSourcePtr);
	}

void DoSetField(TLbsAssistanceDataBody& aBuilderInfo, const TInt& aFieldId, const TUint32* aData, TSupportedFieldTypes aActualType, TCopyFunction aCopyFunc)
	{
	const TLbsAssistanceDataLayoutMap* const layout = aBuilderInfo.iLayout;
	__ASSERT_ALWAYS(aFieldId <= layout->iLayoutCount, User::Invariant());
	__ASSERT_ALWAYS(aActualType == layout->iLayout[aFieldId].iType, User::Invariant());

	const TInt dataOffsetIn32Bits = layout->iLayout[aFieldId].iOffset/4;
	TUint32* const dest = &aBuilderInfo.iDataPtr[dataOffsetIn32Bits];
	aCopyFunc(aData, dest);
	
	ASSERT(aBuilderInfo.iIndexPtr!=NULL);

	const TInt indexOffsetIn32Bits = (aBuilderInfo.iBitOffset+dataOffsetIn32Bits)/32;	//32 bits in a word.
	
	const TUint32 bitMask = 1<<((aBuilderInfo.iBitOffset+dataOffsetIn32Bits)%32);	//32 bits in a word.
	
	aBuilderInfo.iIndexPtr[indexOffsetIn32Bits] |= bitMask;
	}

void DoSetFieldAsIfTUint32(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TUint32* aData, TSupportedFieldTypes aActualType)
	{
	DoSetField(aBuilder.Body(), aFieldId, aData, aActualType, CopyTUint32);
	}

void DoClearField(TLbsAssistanceDataBody& aBuilderInfo, const TInt& aFieldId)
	{
	const TLbsAssistanceDataLayoutMap* const layout = aBuilderInfo.iLayout;
	ASSERT(aFieldId <= layout->iLayoutCount);
	ASSERT(aBuilderInfo.iIndexPtr!=NULL);
	const TInt numberOfBitsToAddOn = (layout->iLayout[aFieldId].iOffset)/4;	// /4 to get number of bytes to number of 32-bit-words
	const TInt startBit = aBuilderInfo.iBitOffset+numberOfBitsToAddOn;
	const TInt startWordOffset = startBit/32;	//32 bits in a word.
	TUint32* wordPtr = &aBuilderInfo.iIndexPtr[startWordOffset];
	
	TInt bitsLeftToCount = IndexSize(layout->iLayout[aFieldId].iType);
	
	TUint32 mask = 1<<(startBit%32);
	bitsLeftToCount--;
	
	while (bitsLeftToCount>0 && !(mask&0x80000000))	//This will break when we've got to the last bit to count or we've hit the top bit of the byte
		{			
		mask|=mask<<1;	//Keep sticking the bits on the end.
		bitsLeftToCount--;
		}
	
	(*wordPtr) &= ~mask;
	
	//From here on we can work on word boundaries
	while(bitsLeftToCount>=32)
		{
		wordPtr++;	//Move on a word
		*wordPtr = 0;	//Clear the word
		bitsLeftToCount-=32;
		}
	
	if(bitsLeftToCount == 0)
		return;		//We've reached the end.

	//Now do the top bits...
	wordPtr++;	//Move onto the last word.
	mask = 0x00000001;
	bitsLeftToCount --;
	while (bitsLeftToCount>0)	//No need to check the mask, we're not going to overrun as mask is less than or equal to 32.
		{
		mask|=mask<<1;
		bitsLeftToCount--;
		}
	
	(*wordPtr) &= ~mask;
	}

void DoClearAllFields(TLbsAssistanceDataBody& aBuilderInfo)
	{
	const TLbsAssistanceDataLayoutMap* const layout = aBuilderInfo.iLayout;
	ASSERT(aBuilderInfo.iIndexPtr!=NULL);
	const TInt startBit = aBuilderInfo.iBitOffset;
	const TInt startWordOffset = startBit/32;	//32 bits in a word.
	TUint32* wordPtr = &aBuilderInfo.iIndexPtr[startWordOffset];

	//bitsLeftToCount is calculated as the offset of the last entry + the index size for that entry	
	TInt bitsLeftToCount = ((layout->iLayout[layout->iLayoutCount-1].iOffset)/4)+IndexSize(layout->iLayout[layout->iLayoutCount-1].iType);
	
	TUint32 mask = 1<<(startBit%32);
	bitsLeftToCount--;
	
	while (bitsLeftToCount>0 && !(mask&0x80000000))	//This will break when we've got to the last bit to count or we've hit the top bit of the byte
		{			
		mask|=mask<<1;	//Keep sticking the bits on the end.
		bitsLeftToCount--;
		}
	
	(*wordPtr) &= ~mask;
	
	//From here on we can work on word boundaries
	while(bitsLeftToCount>=32)
		{
		wordPtr++;	//Move on a word
		*wordPtr = 0;	//Clear the word
		bitsLeftToCount-=32;
		}
	
	if(bitsLeftToCount == 0)
		return;		//We've reached the end.

	//Now do the top bits...
	wordPtr++;	//Move onto the last word.
	mask = 0x00000001;
	bitsLeftToCount --;
	while (bitsLeftToCount>0)	//No need to check the mask, we're not going to overrun as mask is less than or equal to 32.
		{
		mask|=mask<<1;
		bitsLeftToCount--;
		}
	
	(*wordPtr) &= ~mask;

	}

TBool DoFieldExists(const TLbsAssistanceDataBody& aAssistanceData, const TInt& aFieldId)
	{
	const TLbsAssistanceDataLayoutMap* const layout = aAssistanceData.iLayout;
	ASSERT(aFieldId <= layout->iLayoutCount);
	ASSERT(aAssistanceData.iIndexPtr!=NULL);
	const TInt numberOfBitsToAddOn = (layout->iLayout[aFieldId].iOffset)/4;	// /4 to get number of bytes to number of 32-bit-words
	const TInt startBit = aAssistanceData.iBitOffset+numberOfBitsToAddOn;
	const TInt startWordOffset = startBit/32;	//32 bits in a word.
	const TUint32* wordPtr = &aAssistanceData.iIndexPtr[startWordOffset];
	
	TInt bitsLeftToCount = IndexSize(layout->iLayout[aFieldId].iType);
	
	TUint32 mask = 1<<(startBit%32);
	bitsLeftToCount--;
	
	while (bitsLeftToCount>0 && !(mask&0x80000000))	//This will break when we've got to the last bit to count or we've hit the top bit of the byte
		{
		mask|=mask<<1;	//Keep sticking the bits on the end.
		bitsLeftToCount--;
		}
	
	if((*wordPtr)&mask)
		return ETrue;
	
	//From here on we can work on word boundaries
	while(bitsLeftToCount>=32)
		{
		wordPtr++;	//Move on a word
		if(*wordPtr)
			return ETrue;
		bitsLeftToCount-=32;
		}
	
	if(bitsLeftToCount == 0)
		return EFalse;		//We've reached the end.

	//Now do the top bits...
	wordPtr++;	//Move onto the last word.
	mask = 0x00000001;
	bitsLeftToCount --;
	
	//To do - this last bit could be done via lookup - might be slicker?
	while (bitsLeftToCount>0)	//No need to check the mask, we're not going to overrun as bitsLeftToCount is less than or equal to 32.
		{
		mask|=mask<<1;
		bitsLeftToCount--;
		}
	
	return (*wordPtr)&mask;
	}

TBool DoAnyFieldExists(const TLbsAssistanceDataBody& aAssistanceData)
	{
	TInt fieldId = 0;
	TBool found = EFalse;
	const TLbsAssistanceDataLayoutMap* const layout = aAssistanceData.iLayout;
	
	while (fieldId < layout->iLayoutCount)
		{
		if (DoFieldExists(aAssistanceData, fieldId))
			{
			found = ETrue;
			break;
			}
		fieldId++;
		}
	return found;
	}
	
TInt DoGetField(const TLbsAssistanceDataBody& aAssistanceData, const TInt& aFieldId, TUint32* aData, TSupportedFieldTypes aActualType, TCopyFunction aCopyFunc)
	{
	const TLbsAssistanceDataLayoutMap* const layout = aAssistanceData.iLayout;
	__ASSERT_ALWAYS(aFieldId <= layout->iLayoutCount, User::Invariant());
	__ASSERT_ALWAYS(aActualType == layout->iLayout[aFieldId].iType, User::Invariant());
	
	if(!DoFieldExists(aAssistanceData, aFieldId))
		return KErrNotFound;
	
	const TInt dataOffsetIn32Bits = layout->iLayout[aFieldId].iOffset/4;
	const TUint32* const source = &aAssistanceData.iDataPtr[dataOffsetIn32Bits];
	aCopyFunc(source, aData);
	
	return KErrNone;
	}

TInt DoGetFieldAsIfTUint32(const RDataReaderBase& aReader, const TInt& aFieldId, TUint32* aData, TSupportedFieldTypes aActualType)
	{
	return DoGetField(aReader.Body(), aFieldId, aData, aActualType, CopyTUint32);
	}

void DoGetChildBody(const TLbsAssistanceDataBody& aParentBody, const TInt& aFieldId, TLbsAssistanceDataBody& aChildBody)
	{
	const TLbsAssistanceDataLayoutMap* const parentLayout = aParentBody.iLayout;
	ASSERT(aFieldId <= parentLayout->iLayoutCount);
	
	const TInt dataOffsetIn32Bits = (parentLayout->iLayout[aFieldId].iOffset)/4;	//Divide by 4 to get from bytes to 32-bit quantities
	
	aChildBody.iDataPtr = &aParentBody.iDataPtr[dataOffsetIn32Bits];
	
	aChildBody.iIndexPtr = aParentBody.iIndexPtr;
	aChildBody.iBitOffset = aParentBody.iBitOffset+dataOffsetIn32Bits;
	}

//

/**
SetField
@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are interested in
@param aData data to set in field
 */
EXPORT_C void DataBuilderFuncs::SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TInt& aData)
	{
	__ASSERT_COMPILE(sizeof(TInt)==sizeof(TUint32));
	DoSetFieldAsIfTUint32(aBuilder, aFieldId, reinterpret_cast<const TUint32*>(&aData), EIntType);
	}

/**
SetField
@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are interested in
@param aData data to set in field
 */
EXPORT_C void DataBuilderFuncs::SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TDesC8& aData)
	{
	DoSetField(aBuilder.Body(), aFieldId, reinterpret_cast<const TUint32*>(&aData), EDes8Type, CopyFromDes8);
	}

/**
SetField


@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are interested in
@param aData data to set in field
 */
EXPORT_C void DataBuilderFuncs::SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TUint& aData)
	{
	__ASSERT_COMPILE(sizeof(TUint)==sizeof(TUint32));
	DoSetFieldAsIfTUint32(aBuilder, aFieldId, reinterpret_cast<const TUint32*>(&aData), EUintType);
	}

/**
SetField


@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are interested in
@param aData TDopplerUncertainty data to set in field
 */
EXPORT_C void DataBuilderFuncs::SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TDopplerUncertainty& aData)
	{
	__ASSERT_COMPILE(sizeof(TDopplerUncertainty)==sizeof(TUint32));
	DoSetFieldAsIfTUint32(aBuilder, aFieldId, reinterpret_cast<const TUint32*>(&aData), EDopplerUncertaintyType);
	}

/**
SetField


@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are interested in
@param aData TSatelliteStatus data to set in field
 */
EXPORT_C void DataBuilderFuncs::SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TSatelliteStatus& aData)
	{
	__ASSERT_COMPILE(sizeof(TSatelliteStatus)==sizeof(TUint32));
	DoSetFieldAsIfTUint32(aBuilder, aFieldId, reinterpret_cast<const TUint32*>(&aData), ESatelliteStatusType);
	}

/**
SetField


@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are interested in
@param aData TUtranGpsDriftRat data to set in field
 */
EXPORT_C void DataBuilderFuncs::SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TUtranGpsDriftRate& aData)
	{
	__ASSERT_COMPILE(sizeof(TUtranGpsDriftRate)==sizeof(TUint32));
	DoSetFieldAsIfTUint32(aBuilder, aFieldId, reinterpret_cast<const TUint32*>(&aData), EUtranGpsDriftRateType);
	}

/**
SetField


@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are interested in
@param aData TSfnTowUncertainty data to set in field
 */
EXPORT_C void DataBuilderFuncs::SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TSfnTowUncertainty& aData)
	{
	__ASSERT_COMPILE(sizeof(TSfnTowUncertainty)==sizeof(TUint32));
	DoSetFieldAsIfTUint32(aBuilder, aFieldId, reinterpret_cast<const TUint32*>(&aData), ESfnTowUncertaintyType);
	}

/**
SetField


@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are interested in
@param aData TCodePhaseSearchWindow data to set in field
 */
EXPORT_C void DataBuilderFuncs::SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TCodePhaseSearchWindow& aData)
	{
	__ASSERT_COMPILE(sizeof(TCodePhaseSearchWindow)==sizeof(TUint32));
	DoSetFieldAsIfTUint32(aBuilder, aFieldId, reinterpret_cast<const TUint32*>(&aData), ECodePhaseSearchWindowType);
	}

/**
ClearField


@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are want to clear
 */
EXPORT_C void DataBuilderFuncs::ClearField(RDataBuilderBase& aBuilder, const TInt& aFieldId)
	{
	DoClearField(aBuilder.Body(), aFieldId);
	}

/**
Clear all fields


@param aBuilder RDataBuilderBase base class
 */
EXPORT_C void DataBuilderFuncs::ClearAllFields(RDataBuilderBase& aBuilder)
	{
	TLbsAssistanceDataBody& builderInfo = aBuilder.Body();
	DoClearAllFields(builderInfo);
	}

/**
FieldExists checks if field with Id exists


@return True if field exists (ETrue)
@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are want to see if exists
 */
EXPORT_C TBool DataBuilderFuncs::FieldExists(const RDataBuilderBase& aBuilder, const TInt& aFieldId)
	{
	const TLbsAssistanceDataBody& builderInfo = aBuilder.Body();
	return DoFieldExists(builderInfo, aFieldId);
	}

/**
AnyFieldExists checks if any field Id exists
@return True if any field exists (ETrue)
@param aReader RDataReaderBase& base class
 */
EXPORT_C TBool DataBuilderFuncs::AnyFieldExists(const RDataBuilderBase& aBuilder)
	{
	const TLbsAssistanceDataBody& builderInfo = aBuilder.Body();
	if (builderInfo.iDataPtr == NULL)
		{
		// The builder is closed. No field can exist.
		return false;
		}
	else
		{
		return DoAnyFieldExists(builderInfo);
		}
	}

/**
GetFieldBuilder
@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are want to get
@param aBuilder RDataBuilderBase child
 */
EXPORT_C void DataBuilderFuncs::GetFieldBuilder(RDataBuilderBase& aParentBuilder, const TInt& aFieldId, RDataBuilderBase& aChildBuilder)
	{
	DoGetChildBody(aParentBuilder.Body(), aFieldId, aChildBuilder.Body());
	}

/**
@param aBuilder RDataBuilderArrayBase base class
@return Maximum elements in array
 */	
EXPORT_C TInt DataBuilderFuncs::MaxCount(const RDataBuilderArrayBase& aArrayBuilder)
	{
	return aArrayBuilder.Body().iLayout->iLayoutCount;
	}
/**
GetField
@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are interested in
@param aData data to get in field
@return Symbian Error code
 */	
EXPORT_C TInt DataReaderFuncs::GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TInt& aData)
	{
	__ASSERT_COMPILE(sizeof(TInt)==sizeof(TUint32));
	return DoGetFieldAsIfTUint32(aReader, aFieldId, reinterpret_cast<TUint32*>(&aData), EIntType);
	}

/**
GetField
@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are interested in
@param aData data to get in field
@return Symbian Error code
 */
EXPORT_C TInt DataReaderFuncs::GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TDes8& aData)
	{
	return DoGetField(aReader.Body(), aFieldId, reinterpret_cast<TUint32*>(&aData), EDes8Type, CopyToDes8);	
	}

/**
GetField
@param aBuilder RDataBuilderBase base class
@param aFieldId id of the field we are interested in
@param aData data to get in field
@return Symbian Error code
 */
EXPORT_C TInt DataReaderFuncs::GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TUint& aData)
	{
	__ASSERT_COMPILE(sizeof(TUint)==sizeof(TUint32));
	return DoGetFieldAsIfTUint32(aReader, aFieldId, reinterpret_cast<TUint32*>(&aData), EUintType);
	}

/**
GetField
@param aBuilder RDataReaderBase base class
@param aFieldId id of the field we are interested in
@param aData TDopplerUncertainty data to read in field
@return Symbian Error code
 */
EXPORT_C TInt DataReaderFuncs::GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TDopplerUncertainty& aData)
	{
	__ASSERT_COMPILE(sizeof(TDopplerUncertainty)==sizeof(TUint32));
	return DoGetFieldAsIfTUint32(aReader, aFieldId, reinterpret_cast<TUint32*>(&aData), EDopplerUncertaintyType);
	}

/**
GetField
@param aBuilder RDataReaderBase base class
@param aFieldId id of the field we are interested in
@param aData TSatelliteStatus data to read in field
@return Symbian Error code
 */
EXPORT_C TInt DataReaderFuncs::GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TSatelliteStatus& aData)
	{
	__ASSERT_COMPILE(sizeof(TSatelliteStatus)==sizeof(TUint32));
	return DoGetFieldAsIfTUint32(aReader, aFieldId, reinterpret_cast<TUint32*>(&aData), ESatelliteStatusType);
	}

/**
GetField
@param aBuilder RDataReaderBase base class
@param aFieldId id of the field we are interested in
@param aData TUtranGpsDriftRate data to read in field
@return Symbian Error code
 */
EXPORT_C TInt DataReaderFuncs::GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TUtranGpsDriftRate& aData)
	{
	__ASSERT_COMPILE(sizeof(TUtranGpsDriftRate)==sizeof(TUint32));
	return DoGetFieldAsIfTUint32(aReader, aFieldId, reinterpret_cast<TUint32*>(&aData), EUtranGpsDriftRateType);
	}

/**
GetField
@param aBuilder RDataReaderBase base class
@param aFieldId id of the field we are interested in
@param aData TSfnTowUncertainty data to read in field
@return Symbian Error code
 */
EXPORT_C TInt DataReaderFuncs::GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TSfnTowUncertainty& aData)
	{
	__ASSERT_COMPILE(sizeof(TSfnTowUncertainty)==sizeof(TUint32));
	return DoGetFieldAsIfTUint32(aReader, aFieldId, reinterpret_cast<TUint32*>(&aData), ESfnTowUncertaintyType);
	}
/**
GetField
@param aBuilder RDataReaderBase base class
@param aFieldId id of the field we are interested in
@param aData TCodePhaseSearchWindow data to read in field
@return Symbian Error code
 */
EXPORT_C TInt DataReaderFuncs::GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TCodePhaseSearchWindow& aData)
	{
	__ASSERT_COMPILE(sizeof(TCodePhaseSearchWindow)==sizeof(TUint32));
	return DoGetFieldAsIfTUint32(aReader, aFieldId, reinterpret_cast<TUint32*>(&aData), ECodePhaseSearchWindowType);
	}

/**
GetFieldReader
@param aParentReader RDataReaderBase base class
@param aFieldId id of the field we are want to get
@param aChildReader RDataReaderBase child
@return Symbian Error code
 */
EXPORT_C TInt DataReaderFuncs::GetFieldReader(const RDataReaderBase& aParentReader, const TInt& aFieldId, RDataReaderBase& aChildReader)
	{
	const TLbsAssistanceDataBody& parentReaderBody = aParentReader.Body();
	DoGetChildBody(parentReaderBody, aFieldId, aChildReader.Body());
	return DoFieldExists(parentReaderBody, aFieldId) ? KErrNone : KErrNotFound;
	}

/**
FieldExists checks if field with Id exists
@return True if field exists (ETrue)
@param aReader RDataReaderBase& base class
@param aFieldId id of the field we want to see if exists
 */
EXPORT_C TBool DataReaderFuncs::FieldExists(const RDataReaderBase& aReader, const TInt& aFieldId)
	{
	const TLbsAssistanceDataBody& readerInfo = aReader.Body();
	return DoFieldExists(readerInfo, aFieldId);
	}

/**
AnyFieldExists checks if any field Id exists
@return True if any field exists (ETrue)
@param aReader RDataReaderBase& base class
 */
EXPORT_C TBool DataReaderFuncs::AnyFieldExists(const RDataReaderBase& aReader)
	{
	const TLbsAssistanceDataBody& readerInfo = aReader.Body();
	if (readerInfo.iDataPtr == NULL)
		{
		// The reader is closed. No field can exist;
		return false;
		}
	else
		{
		return DoAnyFieldExists(readerInfo);
		}
	}

/**
@param aReader RDataReaderArrayBase base class
@return Maximum elements in array
 */
EXPORT_C TInt DataReaderFuncs::MaxCount(const RDataReaderArrayBase& aReader)
	{
	return aReader.Body().iLayout->iLayoutCount;
	}

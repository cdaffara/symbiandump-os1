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
//

#ifndef __LBSINTERNAL_H__
#define __LBSINTERNAL_H__

#include <lbsfields.h>

_LIT(KPositionInternalFault, "LocationClientLibInternal");

/**
 * Internal panic codes for faults in API implementation
 *
 * @internalComponent
 */
enum TPositionInternalFault
	{
	/** Algorithm for aligning data choose an offset not suitable for this data type. */
	EBadAlignment					= 0,
	/** CRequestor::iData was found to be NULL. This should never be so. */
	ERequestorDataNull				= 1,
	/** An entry in the field index for HPositionGenericInfo has an unknown data type.*/
	EUnknownDataType				= 2,
	/** Somehow, one of the copy methods has generated an error copying INTO 
	the HPositionGenericInfo */
	ECopyingIntoBufferGaveError		= 3,
	/** The latitude field within a TCoordinate (or derived) class is not between
	-90 and 90 degrees, after the NormalizeCoordinate method ran. */
	EBadLatitude					= 4,
	/** The longitude field within a TCoordinate (or derived) class is not between
	-180 and 180 degrees, after the NormalizeCoordinate method ran. */
	EBadLongitude					= 5,
	/** The start position in the HPos' buffer for something isn't aligned correctly. */
	EBufferPosNotAligned			= 6
	};
	
class TDataWrapperC;
class TDataWrapper;

NONSHARABLE_CLASS(TPositionFieldSetter)
/**
 * Class to edit the contents of the HPositionGenericInfo.
 *
 * @internalComponent
 */
	{
public:
	inline TPositionFieldSetter(HPositionGenericInfo& aInfo);
	TInt DoSetValue(TPositionFieldId aFieldId, const TDataWrapperC& aData);

private:
	inline void ClearIdIfItAlreadyExists(TPositionFieldId aFieldId);
	inline TInt FindFirstEmptyIndex(TInt& aIndex) const;
	void AlignOffset(TInt& aOffset,
					 PositionFieldManager::TFieldType aFieldType) const;
	TInt FieldLength(TInt aFieldIndexId) const;
	inline TBool DoCompress();
private:
	HPositionGenericInfo& iInfo;
	};

NONSHARABLE_CLASS(TPositionFieldGetter)
	{
public:
	inline TPositionFieldGetter(const HPositionGenericInfo& aInfo);
	TInt DoGetValue(TPositionFieldId aFieldId,
					TDataWrapper& aData) const;
private:
	inline TInt FindIndexFromFieldId(TPositionFieldId aFieldId,
									 TInt& aIndex) const;
private:
	const HPositionGenericInfo& iInfo;
	};

struct SPackedTDesC8
/**
 * Struct to reflect the layout of a 8-bit descriptor that has been copied
 * into a HPositionGenericInfo object.
 *
 * @internalComponent
 */
	{
	TInt iLength;
	TUint8 iBuf[1];
	};

struct SPackedTDesC16
/**
 * Struct to reflect the layout of a 16-bit descriptor that has been copied
 * into a HPositionGenericInfo object.
 *
 * \internal
 */
	{
	TInt iLength;
	TUint16 iBuf[1];
	};

NONSHARABLE_CLASS(TDataWrapperC)
/**
 * Wrapper around a const object that is to be copied into a HPositionGenericInfo object.
 *
 * @internalComponent
 */
	{
public:
	inline TDataWrapperC(PositionFieldManager::TFieldType aType,
	                     TInt aSize,
	                     const TAny* aObject,
	                     TInt (*aCopyFunction)(TAny* aTrg, const TAny* aSrc));
	inline void CopyTo(TAny* aCopyTo) const;
	inline TInt Size() const;
	inline PositionFieldManager::TFieldType Type() const;
private:
	/** The type of data this class is a wrapper for. */
	const PositionFieldManager::TFieldType iType;
	/** The size of the object to be copied into HPositionGenericInfo. */
	const TInt iSize;
	/** Pointer to the object to be copied. */
	const TAny* const iObject;
	/** Pointer to the function that is to do the copying. */
	TInt (*iCopyFunction)(TAny* aTrg, const TAny* aSrc);
	};

NONSHARABLE_CLASS(TDataWrapper)
/**
 * Wrapper around a mutable object that is to be copied from a HPositionGenericInfo object.
 *
 * @internalComponent
 */
	{
public:
	inline TDataWrapper(PositionFieldManager::TFieldType aType,
	                    TAny* aObject,
	                    TInt (*aCopyFunction)(TAny* aTrg, const TAny* aSrc));
	inline TInt CopyFrom(const TAny* aCopyFrom);
	inline PositionFieldManager::TFieldType Type() const;
private:
	/** The type of data this class is a wrapper for. */
	const PositionFieldManager::TFieldType iType;
	/** Pointer to the object to be edited. */
	TAny* const iObject;
	/** Pointer to the function that is to do the copying. */
	TInt (*iCopyFunction)(TAny* aTrg, const TAny* aSrc);
	};

#include "lbsinternal.inl"



#endif // __LBSINTERNAL_H__

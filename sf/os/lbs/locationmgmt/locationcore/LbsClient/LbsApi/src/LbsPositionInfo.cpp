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

#include <e32base.h>
#include <lbspositioninfo.h>
#include <lbserrors.h>
#include "LbsInternal.h"
#include "lbsfieldindex.h"

// Fix for GCCE 4.x : bug #1708 - GCC-E compilation error in lbs 
#if defined(_FOFF)
#undef _FOFF
#endif // defined(_FOFF)
#define _FOFF(c,f)          (((TInt)&(((c *)0x1000)->f))-0x1000)
// End of fix for GCCE 4.x

//------------------------------------------------------------------------------
// TPositionInfoBase
//------------------------------------------------------------------------------
EXPORT_C TPositionInfoBase::TPositionInfoBase()
/**
Default constructor. Sets up class type information.
 */
	{
	iPosClassType          = EPositionInfoUnknownClass;
	iModuleId              = TUid::Uid(0);
	iUpdateType            = EPositionUpdateGeneral;
	iPositionMode          = TPositionModuleInfo::ETechnologyUnknown;
	iPositionModeReason    = EPositionModeReasonNone;
	}

EXPORT_C TPositionModuleId TPositionInfoBase::ModuleId() const
/**
Returns the module Id of the module that gave the position fix
@return Returns the module Id of the module that gave the position fix.
 */
	{
	return iModuleId;
	}

EXPORT_C void TPositionInfoBase::SetModuleId(TPositionModuleId aModuleId)
/**
Sets the Module Id stored in this class
@param aModuleId Module Id stored in this class is set to aModuleId.
 */
	{
	iModuleId = aModuleId;
	}

EXPORT_C TPositionUpdateType TPositionInfoBase::UpdateType() const
/**
Returns the update type of the position fix.
@return Returns the update type of the position fix.
 */
	{
	return iUpdateType;
	}

EXPORT_C void TPositionInfoBase::SetUpdateType(TPositionUpdateType aUpdateType)
/**
Sets The update type of the position fix in this class
@param aUpdateType The update type of the position fix in this class is set to this.
 */
	{
	iUpdateType = aUpdateType;
	}

EXPORT_C void TPositionInfoBase::SetPositionMode(TPositionModuleInfo::TTechnologyType aMode)
/**
@param aMode The mode used to calculate the position fix contained in this class.
For example,
terminal only (autonomous), terminal-assisted, network only or network-assisted.

see TPositionModuleInfo::TTechnologyType
 */
	{
	iPositionMode = aMode;
	}

EXPORT_C TPositionModuleInfo::TTechnologyType TPositionInfoBase::PositionMode() const
/**
@return Returns the actual mode used by the positioning module to calculate the
position fix. A positioning module may not always be able to use the preferred
location calculation mode. For example, if preferred mode is "terminal-assisted"
but the phone is out of network coverage it may have used autonomous mode instead.

see TPositionModuleInfo::TTechnologyType
 */
	{
	return iPositionMode;
	}

EXPORT_C void TPositionInfoBase::SetPositionModeReason(TPositionModeReason aReason)
/**
@param aReason The mode used to calculate the position fix contained in this class
was different from the prefered mode.

see TPositionModuleInfo::TTechnologyType
 */
	{
	iPositionModeReason = aReason;
	}

EXPORT_C TPositionModeReason TPositionInfoBase::PositionModeReason() const
/**
@return Returns why the positioning mode used to calculate the position fix was different
from the prefered mode. For example, this could indicate that the phone is out of 
network coverage. 

see PositionMode
see _TPositionModeReason
 
@return The method returns a bit mask the reasons why the position module was unable
calculate the position in the prefered mode.
@return EPositionModeReasonNone is returned if the mode used was the preferred.

@return EPositionModeReasonNetworkError is returned when there is no network coverage
or the service is unavailable.

@return EPositionModeReasonLocalError represents environmental reasons that prevent
a terminal based calculation. For example, no satellites are in view.

@return EPositionModeReasonDataError is returned when there is a problem with the
initialisation data. For example, GPS assistance data was received but it was
either invalid or could not be used.
*/
	{
	return iPositionModeReason;
	}


//------------------------------------------------------------------------------
// TPositionInfo
//------------------------------------------------------------------------------
EXPORT_C TPositionInfo::TPositionInfo()
/**
Default constructor. Sets the class type and class size in the base class (TPositionInfoBase).
 */
	{
	iPosClassType |= EPositionInfoClass;
	iPosClassSize = sizeof(TPositionInfo);
	}

EXPORT_C void TPositionInfo::SetPosition(const TPosition& aPosition)
/**
Sets the TPosition stored in this class 
@param aPosition The TPosition stored in this class is set to aPosition
 */
	{
	iPosition = aPosition;
	}


EXPORT_C void TPositionInfo::GetPosition(TPosition& aPosition) const
/**
Gets the TPosition stored in this class
@param aPosition The TPosition stored in this class is copied into aPosition
 */
	{
	aPosition = iPosition;
	}


//------------------------------------------------------------------------------
// TPositionCourseInfo
//------------------------------------------------------------------------------

EXPORT_C TPositionCourseInfo::TPositionCourseInfo()
/**
Default constructor. Sets the class type and class size in the base class (TPositionInfoBase).
 */
	{
	iPosClassType |= EPositionCourseInfoClass;
	iPosClassSize = sizeof(TPositionCourseInfo);
	}

EXPORT_C void TPositionCourseInfo::GetCourse(TCourse& aCourse) const
/**
Gets the TCourse stored in this class 
@param aCourse The TCourse stored in this class is copied into aCourse.
 */
	{
	aCourse = iCourse;
	}

EXPORT_C void TPositionCourseInfo::SetCourse(const TCourse& aCourse)
/**
Sets the TCourse stored in this class 
@param aCourse The TCourse stored in this class is set to aCourse.
 */
	{
	iCourse = aCourse;
	}

//------------------------------------------------------------------------------
// HPositionGenericInfo
//------------------------------------------------------------------------------

EXPORT_C HPositionGenericInfo* HPositionGenericInfo::NewLC(TInt aBufferSize,
														   TInt aMaxFields)
/**
Leaving constructor that allocs a HPositionGenericInfo on the heap and leaves a 
pointer to the object on the cleanup stack.
@param aBufferSize Sets the size of the buffer that the HPositionGenericInfo will have.
@param aMaxFields Sets the maximum number of fields that the HPositionGenericInfo will have.
@return Pointer to a HPositionGenericInfo instance.
 */
	{
	__ASSERT_ALWAYS(aBufferSize>0, User::Panic(KPosClientFault, EPositionGenericInfoZeroBufferSize));
	__ASSERT_ALWAYS(aMaxFields>0, User::Panic(KPosClientFault, EPositionGenericInfoZeroMaxFields));
	HPositionGenericInfo* newInfo = HPositionGenericInfo::NewL(aBufferSize, aMaxFields);
	CleanupStack::PushL(newInfo);
	return newInfo;
	}

EXPORT_C HPositionGenericInfo* HPositionGenericInfo::NewL(TInt aDataBufferSize,
														   TInt aMaxFields)
/**
Leaving constructor that allocs a HPositionGenericInfo on the heap.
@param aBufferSize Sets the size of the buffer that the HPositionGenericInfo will have.
@param aMaxFields Sets the maximum number of fields that the HPositionGenericInfo will have.
@return Pointer to a HPositionGenericInfo instance.
 */
	{
	__ASSERT_ALWAYS(aDataBufferSize>0, User::Panic(KPosClientFault, EPositionGenericInfoZeroBufferSize));
	__ASSERT_ALWAYS(aMaxFields>0, User::Panic(KPosClientFault, EPositionGenericInfoZeroMaxFields));
	return static_cast<HPositionGenericInfo*>
		(User::LeaveIfNull(HPositionGenericInfo::New(aDataBufferSize, aMaxFields)));
	}

EXPORT_C HPositionGenericInfo* HPositionGenericInfo::New(TInt aDataBufferSize,
														   TInt aMaxFields)
/**
Non-leaving constructor that allocs a HPositionGenericInfo on the heap.
@param aBufferSize Sets the size of the buffer that the HPositionGenericInfo will have.
@param aMaxFields Sets the maximum number of fields that the HPositionGenericInfo will have.
@return position information in HPositionGenericInfo structure.
@return NULL if heap allocation failed.
@return Pointer to a HPositionGenericInfo instance if alloc was successful.
 */
	{
	__ASSERT_ALWAYS(aDataBufferSize>0, User::Panic(KPosClientFault, EPositionGenericInfoZeroBufferSize));
	__ASSERT_ALWAYS(aMaxFields>0, User::Panic(KPosClientFault, EPositionGenericInfoZeroMaxFields));

	TUint fieldIndexStartPos = aMaxFields*sizeof(TPositionFieldId);
	if(_FOFF(HPositionGenericInfo, iBuffer[fieldIndexStartPos])&3)
		{//Adjust fieldIndexStartPos such that iBuffer[fieldIndexStartPos] is 4-byte aligned.
		fieldIndexStartPos += 4 - (_FOFF(HPositionGenericInfo, iBuffer[fieldIndexStartPos])&3);
		}

	TUint dataStartPos = fieldIndexStartPos+aMaxFields*sizeof(TPositionFieldIndex);
	
	//iBuffer[0] must be 2 byte aligned since it is to be cast to a TPositionFieldId
	//array
	__ASSERT_DEBUG(!(_FOFF(HPositionGenericInfo, iBuffer[0])&1), 
		User::Panic(KPositionInternalFault, EBufferPosNotAligned));

	//Since fieldIndexStartPos will be 4-byte aligned into iBuffer, and 
	//sizeof(TPositionFieldIndex)==8, dataStartPos should be 4-byte aligned.
	__ASSERT_DEBUG(!(_FOFF(HPositionGenericInfo, iBuffer[dataStartPos])&3), 
		User::Panic(KPositionInternalFault, EBufferPosNotAligned));

	return new(User::Alloc(_FOFF(HPositionGenericInfo, iBuffer[aDataBufferSize+dataStartPos])))
		HPositionGenericInfo(aDataBufferSize, aMaxFields, fieldIndexStartPos, dataStartPos, ETrue);
	}

HPositionGenericInfo::HPositionGenericInfo(TInt aDataBufferSize, TInt aMaxFields,
						TUint aFieldIndexStartPoint, TUint aDataStartPoint,
						TBool aResetRequestedFields)
:	iMaxFields(aMaxFields),
	iFieldIndexStartPoint(aFieldIndexStartPoint),
	iDataStartPoint(aDataStartPoint),
	iTotalBufferSize(aDataBufferSize+aDataStartPoint)
/**
private constructor. To be used within the class 
 */
	{
	iPosClassSize = _FOFF(HPositionGenericInfo, iBuffer[iTotalBufferSize]);
	iPosClassType |= EPositionGenericInfoClass; 
	
	if(aResetRequestedFields)
		{
		ClearRequestedFields();
		}

	//Clear the index area.
	TPositionFieldIndex* const indexPtr = FieldIndexPtr();
	TPositionFieldIndex blankFieldIndex;
	for(TInt i=iMaxFields-1; i>=0; --i)
		{
		indexPtr[i] = blankFieldIndex;
		}
	}

EXPORT_C void HPositionGenericInfo::ClearPositionData()
/**
Resets the object to clear all info to that as it was on construction except the requested
fields. Use in the server to clear the info before putting together a new location fix
and passing it back to the client.
 */
	{
	(void) new(this) HPositionGenericInfo(BufferSize(), iMaxFields, iFieldIndexStartPoint,
										iDataStartPoint, EFalse);
	}

EXPORT_C void HPositionGenericInfo::ClearRequestedFields()
/**
Clears all the requested fields.
 */
	{
	TPositionFieldId* const fieldIdPtr = RequestedFieldPtr();

	for(TInt i=iMaxFields-1; i>=0; --i)
		{
		fieldIdPtr[i] = EPositionFieldNone;
		}
	}

EXPORT_C TInt HPositionGenericInfo::BufferSize() const
/**
Method returns the size of the buffer set in the constructor
@return The size of the buffer that was set in the constructor.
 */
	{
	return iTotalBufferSize-iDataStartPoint;
	}

EXPORT_C TInt HPositionGenericInfo::MaxFields() const
/**
@return The maximum field that is requestable/returnable that was set in the constructor.
 */
	{
	return iMaxFields;
	}

EXPORT_C TInt HPositionGenericInfo::SetRequestedField(TPositionFieldId aFieldId)
/**
Method sets a field as requested.
@param aFieldId Field Id to be set as requested.
@return a symbian OS error code.
@return KErrNone if aFieldId was successfully set as requested.
@return KErrArgument if aFieldId is not strictly between ::EPositionFieldNone and ::EPositionFieldIdLast.
@return KErrOverflow if KPositionMaxRequestedFields requested fields have already been set.
 */
	{
	if(aFieldId==EPositionFieldNone)
		{
		return KErrArgument;
		}
	if(IsRequestedField(aFieldId))
		{
		return KErrNone;
		}
	TInt emptyFieldOffset;
	TInt err = FindEmptyRequestedFieldOffset(emptyFieldOffset);
	if(err==KErrNone)
		{
		RequestedFieldPtr()[emptyFieldOffset] = aFieldId;
		}
	return err;
	}

EXPORT_C TInt HPositionGenericInfo::SetRequestedFields(const TPositionFieldIdList aFieldIdList)
/**
Sets multiple TPositionFieldId objects as requested.
@param aFieldIdList List of field Id's to be set as requested. The list is terminated 
with a ::EPositionFieldNone entry.
@return a symbian OS error code.
@return KErrNone if all Field Ids were successfully set as requested.
@return KErrArgument if a filed Id is not strictly between ::EPositionFieldNone and ::EPositionFieldIdLast.
@return KErrOverflow if > KPositionMaxRequestedFields requested fields are attempted to be set.
 */
	{
	TInt err = KErrNone;
	TInt i = 0;
	while ((err == KErrNone) && (aFieldIdList[i]!=EPositionFieldNone))
		{
		err = SetRequestedField(aFieldIdList[i]);
		i++;
		}
	return err;
	}

EXPORT_C TBool HPositionGenericInfo::IsRequestedField(TPositionFieldId aFieldId) const
/**
Returns whether a field has been set as requested.
@param aFieldId Field Id to search for.
@return If the requested field has been set or not.
@return ETrue if aFieldId has been set as requested.
@return EFalse if aFieldId has not been set as requested.
 */
	{
	if(aFieldId==EPositionFieldNone)
		{
		return EFalse;
		}
	const TPositionFieldId* const fieldIdPtr = RequestedFieldPtr();
	for(TInt i=0; i<iMaxFields; i++)
		{
		if(fieldIdPtr[i]==aFieldId)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

EXPORT_C TPositionFieldId HPositionGenericInfo::FirstRequestedFieldId() const
/**
Looks through the list of requested fields for the lowest valued field Id.
@return The lowest valued requested field if there are any.
@return ::EPositionFieldNone if there are no requested fields.
 */
	{
	return NextRequestedFieldId(TPositionFieldId(EPositionFieldNone));
	}

EXPORT_C TPositionFieldId HPositionGenericInfo::NextRequestedFieldId(TPositionFieldId aFieldId) const
/**
Looks for the next field Id in the list greater than aFieldId. 
@param aFieldId Function will return next field Id in the list greater than this Id.
@return Next field Id in the list greater than aFieldId.
@return ::EPositionFieldNone if there are no field Id's in the list greater than aFieldId
 */
	{
	TPositionFieldId holder = KMaxTUint16;
	TBool idFound = EFalse;
	const TPositionFieldId* const fieldIdPtr = RequestedFieldPtr();
	for(TInt i=0; i<iMaxFields; ++i)
		{
		if((fieldIdPtr[i]>aFieldId)&&(fieldIdPtr[i]<=holder))
			{
			holder = fieldIdPtr[i];
			idFound = ETrue;
			}
		}
	return idFound ? holder : TPositionFieldId(EPositionFieldNone);
	}

EXPORT_C TBool HPositionGenericInfo::IsFieldAvailable(TPositionFieldId aFieldId) const
/**
Looks through the available fields and returns whether aFieldId is in the list.
@param aFieldId This is the field Id that is searched for.
@return If the requested field is available or not.
@return ETrue if the field is available.
@return EFalse if the field is not available.
 */
	{
	if(aFieldId==EPositionFieldNone)
		{
		return EFalse;
		}
	const TPositionFieldIndex* const fieldIndexPtr = FieldIndexPtr();
	for(TInt i=0; i<iMaxFields; i++)
		{
		if(fieldIndexPtr[i].FieldId()==aFieldId)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TInt HPositionGenericInfo::FindEmptyRequestedFieldOffset(TInt& aEmptyFieldOffset) const
/**
Helper method. For internal usage only 
 */
	{
	TInt err = KErrOverflow;
	const TPositionFieldId* const fieldIdPtr = RequestedFieldPtr();
	for (TInt i=0; i<iMaxFields; ++i)
		{
		if(fieldIdPtr[i]==EPositionFieldNone)
			{
			aEmptyFieldOffset = i;
			err = KErrNone;
			break;
			}
		}
	return err;
	}


/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "lbsreffnpint.h"
#include "lbspropertykeydefs.h"
#include "lbsprocessuiddefs.h"
#include "lbsrootcenrepdefs.h"
#include <centralrepository.h>

//
// Network Location Update Interface
//

/** Position update data structure */
struct TLbsPositionUpdateData
	{
	/** Postion update error */
	TInt iError;
	/** Position extended satellite info data */
	TPositionExtendedSatelliteInfo iSatInfo;
	/** Target time for position update */
	TTime iTargetTime;
	/** Actual time for position update */
	TTime iActualTime;
	/** new - need to tell if the property has been defined but contains no 
	useful data, of if it has been populated with some real data.. 
	This is a purely internal field. */
	TBool iValidData; 
	/** A session id coupled with the reference/final network position update */
	TLbsNetSessionIdInt iSessionId;
	};

/** Descriptor package of the position update data */
typedef TPckg<TLbsPositionUpdateData> TLbsPositionUpdateDataPckg;
/** Descriptor package of the position update data */
typedef TPckgC<TLbsPositionUpdateData> TLbsPositionUpdateDataPckgC;

/** Initialise this interface.

This must be called before any call to OpenL(), to ensure that the 
RProperties it uses are correctly initialised.
*/
EXPORT_C void RLbsNetworkPositionUpdates::InitializeL()
	{
	// There is an assumption that TPositionSatelliteInfo is the biggest
	// class to be sent over this interface, so double-check
	__ASSERT_COMPILE(sizeof(TPositionSatelliteInfo) >= sizeof(TPositionInfo));
	__ASSERT_COMPILE(sizeof(TPositionSatelliteInfo) >= sizeof(TPositionCourseInfo));
	
	TSecurityPolicy readPolicy(ECapabilityLocation);
	TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);
	
	// Network reference position property
	TInt err = RProperty::Define(KNetworkReferencePositionUpdateKey, 
								 RProperty::ELargeByteArray, 
								 readPolicy, 
								 writePolicy, 
								 sizeof(TLbsPositionUpdateData));
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}

	// Network final position property
	err = RProperty::Define(KNetworkFinalPositionUpdateKey, 
							 RProperty::ELargeByteArray, 
							 readPolicy, 
							 writePolicy, 
							 sizeof(TLbsPositionUpdateData));
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}

	// Set default values for the properties										 
	TLbsPositionUpdateData defaultData;
	defaultData.iError = KErrNotFound;
	defaultData.iTargetTime = 0;
	defaultData.iActualTime = 0;
	// indicate that the property is ok, but contains no data.
	defaultData.iValidData = EFalse;
	TLbsPositionUpdateDataPckgC pckg(defaultData);
	
	User::LeaveIfError(RProperty::Set(RProcess().SecureId(), 
									  KNetworkReferencePositionUpdateKey, 
									  pckg));
	
	User::LeaveIfError(RProperty::Set(RProcess().SecureId(), 
									  KNetworkFinalPositionUpdateKey, 
									  pckg));
	
	// Save the Uid of the process which called InitializeL()
	// in the cenrep file owned by LbsRoot.
	SetCategoryUidL(KNetworkPositionUpdateCategoryKey, 
				    RProcess().SecureId());
	}

/** Shutdown and release resources used by this interface.

This function deletes the RProperties used by this interface.

This must only be called once all clients using the interface have
each called Close(). If this is not the case then it may leave with
an error code from RProperty::Delete

@see RProperty::Delete
*/
EXPORT_C void RLbsNetworkPositionUpdates::ShutDownL()
	{
	User::LeaveIfError(RProperty::Delete(KNetworkReferencePositionUpdateKey));
	User::LeaveIfError(RProperty::Delete(KNetworkFinalPositionUpdateKey));
	}

EXPORT_C RLbsNetworkPositionUpdates::RLbsNetworkPositionUpdates()
	{
	}

/** Connects a client to the network position update interface.

This call connects a client to one of the available network
position update channels.

To listen to more than one update channel, instantiate one 
RLbsNetworkPositionUpdates object for each channel.

@leave KErrArgument If aPositionId is not a valid Id.

@see RLbsNetworkPositionUpdates::TLbsNetworkPositionUpdateId
*/
EXPORT_C void RLbsNetworkPositionUpdates::OpenL(TLbsNetworkPositionUpdateId aPositionId)
	{
	__ASSERT_DEBUG(iNetworkPositionProperty.Handle() == NULL, User::Invariant());
	
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	GetCategoryUidL(KNetworkPositionUpdateCategoryKey, 
				    categoryUid);	
	
	// Attach to the position update property according to aPositionId
	TUint key(0);
	switch (aPositionId)
		{
		case ENetworkPositionReference:
			{
			key = KNetworkReferencePositionUpdateKey;
			break;
			}
		case ENetworkPositionFinal:
			{
			key = KNetworkFinalPositionUpdateKey;
			break;
			}
		default:
			{
			User::Leave(KErrArgument);
			}
		}
	User::LeaveIfError(iNetworkPositionProperty.Attach(categoryUid, key));	
	}

/** Disconnects a client from the network position update interface.
*/	
EXPORT_C void RLbsNetworkPositionUpdates::Close()
	{
	iNetworkPositionProperty.Close();
	}

/* Check that the given class type is one supported by this interface.
*/
TBool RLbsNetworkPositionUpdates::IsValidPositionInfoBaseType(TUint32 aClassType, TUint aClassSize)
	{
	TUint expectedClassSize(0);
	switch (aClassType)
		{
		case EPositionInfoClass:
			{
			expectedClassSize = sizeof(TPositionInfo);
			break;
			}
		case (EPositionInfoClass|EPositionCourseInfoClass):
			{
			expectedClassSize = sizeof(TPositionCourseInfo);
			break;
			}
		case (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass):
			{
			expectedClassSize = sizeof(TPositionSatelliteInfo);
			break;
			}
      case (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass):
            {
            expectedClassSize = sizeof(TPositionExtendedSatelliteInfo);
            break;
            }
		default:
			{
			// unsupported type, test failed
			return EFalse;
			}
		}
	
	return (aClassSize == expectedClassSize);
	}

EXPORT_C TInt RLbsNetworkPositionUpdates::GetPositionInfo(TLbsNetSessionIdInt& aSessionId,
															TPositionInfoBase& aPosInfo, 
															TTime& aTargetTime, 
															TTime& aActualTime)
	{
	__ASSERT_DEBUG(iNetworkPositionProperty.Handle()!=NULL, User::Invariant());
	
	//Check that aPosInfo is either TPositionInfo, TPositionCourseInfo, TPositionSatelliteInfo or TPositionExtendedSatelliteInfo.
	const TUint32 classType = aPosInfo.PositionClassType();
	const TUint classSize = aPosInfo.PositionClassSize();
	
	if (!IsValidPositionInfoBaseType(classType, classSize))
		{
		return KErrNotSupported;
		}

	TLbsPositionUpdateData data;
	TLbsPositionUpdateDataPckg pckg(data);
	TInt err = iNetworkPositionProperty.Get(pckg);
	if(err!=KErrNone)
		{
		return err;
		}
	// is the data contained valid?
	if(data.iValidData==EFalse)
		{
		return KErrNotReady;
		}
	aTargetTime = data.iTargetTime;
	aActualTime = data.iActualTime;
	aSessionId  = data.iSessionId;
	
	// copy the postion types, this preserves the type and size of the
	// to type, so the copy is offset from the base addresses of the
	// structs. NB this relies on standard C++ class ram layout i.e. address of the
	// parent class property is always lower than the child class.
	TUint8* startCopyToAddress = reinterpret_cast<TUint8*>(&aPosInfo)+sizeof(TPositionClassTypeBase);
	const TUint8* const startCopyFromAddress = reinterpret_cast<TUint8*>(&data.iSatInfo)+sizeof(TPositionClassTypeBase);
	// NB in the future is could be very dodgy, 'cos it always assumes that the
	// strct we copy to has a TSatInfoClass in it and that this is always
	// the biggest possible derative of a TPosInfoBase class 
	const TInt numberOfBytesToCopy = classSize-sizeof(TPositionClassTypeBase);
	TUint8* endAddr = Mem::Copy(startCopyToAddress, startCopyFromAddress, numberOfBytesToCopy);
	// sanity check and return any error code
	if(endAddr != startCopyToAddress + numberOfBytesToCopy)
		{
		return KErrGeneral;
		}
	else
		{	
		return err;	
		}
	}
	
EXPORT_C TInt RLbsNetworkPositionUpdates::SetPositionInfo(const TLbsNetSessionIdInt& aSessionId,
														  const TInt& aError, 
															const TPositionInfoBase& aPositionInfo, 
															const TTime& aActualTime)
	{
	__ASSERT_DEBUG(iNetworkPositionProperty.Handle()!=NULL, User::Invariant());

	//Check that aPosInfo is either TPositionInfo, TPositionCourseInfo, or TPositionSatelliteInfo.
	const TUint32 classType = aPositionInfo.PositionClassType();
	const TUint classSize = aPositionInfo.PositionClassSize();
	
	if (!IsValidPositionInfoBaseType(classType, classSize))
		{
		return KErrNotSupported;
		}
	
	TLbsPositionUpdateData data;
	data.iError = aError;
	data.iTargetTime = 0;
	data.iActualTime = aActualTime;
	data.iSessionId = aSessionId;
	// indicate that the property is ok, and now contains no data.
	data.iValidData = ETrue;

	// Copy the full class type, size and data from aPositionInfo into the property.
	// This will set the type of the object in the property to the type being passed in.
	TUint8* startCopyToAddress = reinterpret_cast<TUint8*>(&data.iSatInfo);
	const TUint8* const startCopyFromAddress = reinterpret_cast<const TUint8*>(&aPositionInfo);
	const TInt numberOfBytesToCopy = aPositionInfo.PositionClassSize();
	TUint8* endAddr = Mem::Copy(startCopyToAddress, startCopyFromAddress, numberOfBytesToCopy);
	// sanity check and return any error code
	if(endAddr != (startCopyToAddress + numberOfBytesToCopy))
		{
		return KErrGeneral;
		}
	TLbsPositionUpdateDataPckgC pckg(data);
	TInt err = iNetworkPositionProperty.Set(pckg);
	return err;
	}
	
	
EXPORT_C void RLbsNetworkPositionUpdates::NotifyNetworkLocationUpdate(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iNetworkPositionProperty.Handle() != NULL, User::Invariant());
	iNetworkPositionProperty.Subscribe(aStatus);	
	}

EXPORT_C void RLbsNetworkPositionUpdates::CancelNotifyNetworkLocationUpdate()
	{
	__ASSERT_DEBUG(iNetworkPositionProperty.Handle() != NULL, User::Invariant());
	iNetworkPositionProperty.Cancel();	
	}

void RLbsNetworkPositionUpdates::GetCategoryUidL(TUint32 aKey, TUid& aCategoryUid)
	{
	TInt category;
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Get(aKey, category));
	aCategoryUid = TUid::Uid(category);
	CleanupStack::PopAndDestroy(rep);
	}
	
void RLbsNetworkPositionUpdates::SetCategoryUidL(TUint32 aKey, TUid aCategoryUid)
	{
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Set(aKey, TInt(aCategoryUid.iUid)));
	CleanupStack::PopAndDestroy(rep);
	}


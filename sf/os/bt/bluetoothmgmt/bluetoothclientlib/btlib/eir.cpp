// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 0
// 
//

/**
 @publishedAll
 @released
 @file
*/

#include <utf.h>
#include <bluetooth/logger.h>
#include "bttypes.h" 

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BTLIB);
#endif

void Panic(TEirWrapperPanics aCode)
	{
	User::Panic(KEirWrapperPanicName, aCode);
	}


// Size of 16/128 bit UUIDs in bytes
const TUint8 KSizeOf16BitUUID = 2;
const TUint8 KSizeOf128BitUUID = 16;

// Size of TxPowerLevel data in bytes
#define KSizeOfTxPowerLevelData 1


/** 
 Constructs an TBluetoothNameRecordWrapper object from an TNameRecord object.
 The reference of aNameRecord is stored in the object.
 This will contain sensible data only if KHostResEir was used in the Inquiry
 @param aNameRecord for Device Name and Extended Inquiry Response 
 */
EXPORT_C TBluetoothNameRecordWrapper::TBluetoothNameRecordWrapper(const TNameRecord& aNameRecord)
: iEirCodec(aNameRecord)
	{
	LOG_FUNC
	}

/**
 Obtain the Device Name from inquiry response. This may be a complete or partial name depending on what is available.
 @param aName is where the Device Name converted into Unicode format is to be stored
 @param aIsComplete indicates whether the name is complete or partial.
 @return an error code (KErrNotFound means no name is present in this EIR packet)
 */
EXPORT_C TInt TBluetoothNameRecordWrapper::GetDeviceName(TDes16& aName, TBool& aIsComplete) const
	{
	LOG_FUNC
	TPtrC8 name;
	aIsComplete = EFalse;
	TInt error = iEirCodec.GetDeviceName(name);
	if(error >= KErrNone)
		{
		if(error == EDataComplete)

			{
			aIsComplete = ETrue;
			}
		}
		
	if(error >= KErrNone)
		{
		error = CnvUtfConverter::ConvertToUnicodeFromUtf8(aName, name);
		}
		
	// in case of an error from GetData or conversion, return null name and mark it's partial
	if(error != KErrNone)
		{
		// indicate client the name is partial to invite a name request.
		aIsComplete = EFalse;
		aName.Zero();
		}

	return error;
	}

/**
 Retrieve the full list of UUIDs contained in this EIR. Please note that all types of UUID will be merged together in the single array.
 @param aUuidContainer CUuidContainer object where all UUIDs found are to be stored, including 16, 32 and 128 bit uuids
 @return an error code
 */
EXPORT_C TInt TBluetoothNameRecordWrapper::GetServiceClassUuids(RExtendedInquiryResponseUUIDContainer& aEIRContainer) const
	{
	LOG_FUNC
	TPtrC8 uuids;
	TInt error = KErrNone;
	// 16 bit UUID
	error = iEirCodec.GetData(EEirUUID16Complete, uuids);
	if(error != KErrNone)
		{
		// can't find complete tag, try partial one
		error = iEirCodec.GetData(EEirUUID16Partial, uuids);
		// find partial tag, set uuid
		// or we can't find either parital or complete tag, no 16 bit uuid exists in this eir packet
		// set complete to partial, this indicates the client there might be uuids in sdp database,
		// so client might want to do a sdp service discovery.
		aEIRContainer.SetCompleteness(RExtendedInquiryResponseUUIDContainer::EUUID16, EFalse);
		if(error == KErrNone)
			{
			// Add the uuid into the container
			error = AddUuids16(aEIRContainer, uuids);
			}
		else
			{
			// can't find either parital or complete tag, no 16 bit uuid exists in this eir packet
			// set complete to partial, this indicates the client there might be uuids in sdp database,
			// so client might want to do a sdp service discovery.
			aEIRContainer.SetCompleteness(RExtendedInquiryResponseUUIDContainer::EUUID16, EFalse);
			error = KErrNone;
			}
		}
	else
		{
		// find complete tag, set uuid
		aEIRContainer.SetCompleteness(RExtendedInquiryResponseUUIDContainer::EUUID16, ETrue);
		// Add the uuid into the container
		error = AddUuids16(aEIRContainer, uuids);
		}

	// 128 bit UUID
	if(error == KErrNone)
		{
		// There hasn't been any error in previous adding 16 bit UUID action
		error = iEirCodec.GetData(EEirUUID128Complete, uuids);
		if(error != KErrNone)
			{
			// can't find complete tag, try partial one
			error = iEirCodec.GetData(EEirUUID128Partial, uuids);
			// find partial tag, set uuid
			// or we can't find either parital or complete tag, no 128 bit uuid exists in this eir packet.
			// set complete to partial, this indicates the client there might be uuids in sdp database,
			// so client might want to do a sdp service discovery.
			aEIRContainer.SetCompleteness(RExtendedInquiryResponseUUIDContainer::EUUID128, EFalse);
			if(error == KErrNone)
				{
				// Add the uuid into the container
				error = AddUuids128(aEIRContainer, uuids);
				}
			else
				{
				// can't find either parital or complete tag, no 128 bit uuid exists in this eir packet.
				// set complete to partial, this indicates the client there might be uuids in sdp database,
				// so client might want to do a sdp service discovery.
				aEIRContainer.SetCompleteness(RExtendedInquiryResponseUUIDContainer::EUUID128, EFalse);
				error = KErrNone;
				}
			}
		else
			{
			// find complete tag, set uuid
			aEIRContainer.SetCompleteness(RExtendedInquiryResponseUUIDContainer::EUUID128, ETrue);
			// Add the uuid into the container
			error = AddUuids128(aEIRContainer, uuids);
			}
		}

	return error;
	}

/**
 Retrieve the vendor specific data from this EIR. 
 @param aDest Array where the vendor specific data is to be stored
 @return An error code (KErrNotFound means no Manufacturer Specific Data is present in this EIR packet)
 */
EXPORT_C TInt TBluetoothNameRecordWrapper::GetVendorSpecificData(TDes8& aVendorSpecificData) const
	{
	LOG_FUNC
	TPtrC8 vendorSpecificData;
	TInt error = iEirCodec.GetData(EEirVendorSpecific, vendorSpecificData);
	if(error != KErrNone)
		{
		vendorSpecificData.Set(KNullDesC8());
		}
	else
		{
		aVendorSpecificData.Copy(vendorSpecificData);
		}
	return error;
	}

/**
 Retrieve the Transmission Power Level from EIR. 
 @param aTxPowerLevel where the Transmission Power Level from this EIR is to be stored
 @return an error code (KErrNotFound means no Transmission Power Level is present in this EIR packet)
 */
EXPORT_C TInt TBluetoothNameRecordWrapper::GetTxPowerLevel(TInt8& aTxPowerLevel) const
	{
	LOG_FUNC
	TPtrC8 txPowerLevelPtr;
	TInt error = iEirCodec.GetData(EEirTxPowerLevel, txPowerLevelPtr);
	// if txPowerLevelPtr is longer than 1 byte, error KErrCorrupt
	if(error == KErrNone && (txPowerLevelPtr.Length() != KSizeOfTxPowerLevelData))
		{
		error = KErrCorrupt;
		}
	else if(error == KErrNone && (txPowerLevelPtr.Length() == KSizeOfTxPowerLevelData))
		{
		aTxPowerLevel = static_cast <TInt8>(txPowerLevelPtr[0]);
		}
	else
		{
		aTxPowerLevel = 0;
		}
	return error;
	}

/**
 Obtain the Flags from EIR if present. 
 @param aFlags a descriptor where the Flags is to be stored
 @return an error code (KErrNotFound means no Flags is present in this EIR packet)
 */
EXPORT_C TInt TBluetoothNameRecordWrapper::GetFlags(TDes8& aFlags) const
	{
	LOG_FUNC
	TPtrC8 flags;
	TInt error = iEirCodec.GetData(EEirFlags, flags);
	if(error != KErrNone)
		{
		flags.Set(KNullDesC8());
		}
	else
		{
		aFlags.Copy(flags);
		error = KErrNone;
		}
	return error;
	}

/**
 Obtain the length for Device Name in EIR if present. 
 This function should be called before GetDeviceName() to initialize size of the buffer,
 whose reference will be passed into GetDeviceName() as an argument.
 @return the length of Device Name
 */
EXPORT_C TInt TBluetoothNameRecordWrapper::GetDeviceNameLength() const
	{
	LOG_FUNC
	TPtrC8 name;
	TInt ret = iEirCodec.GetDeviceName(name);
	if(ret >= KErrNone)
		{
		ret = name.Length();
		}
	else
		{
		ret = 0;
		}
	return  ret;
	}

/**
 Obtain the length for Manufacturer Specific Data in EIR if present. 
 This function should be called before GetVendorSpecificData() to initialize size of the buffer,
 whose reference will be passed into GetVendorSpecificData() as an argument.
 @return the length of Manufacturer Specific Data
 */
EXPORT_C TInt TBluetoothNameRecordWrapper::GetVendorSpecificDataLength() const
	{
	LOG_FUNC
	TPtrC8 vendorSpecificData;
	TInt ret = iEirCodec.GetData(EEirVendorSpecific, vendorSpecificData);
	if(ret == KErrNone)
		{
		ret = vendorSpecificData.Length();
		}
	else
		{
		ret = 0;
		}
	return ret;
	}

/**
 Obtain the length for Flags in EIR if present. 
 This function should be called before GetFlags() to initialize size of the buffer,
 whose reference will be passed into GetFlags() as an argument.
 @return the length of Flags
 */
EXPORT_C TInt TBluetoothNameRecordWrapper::GetFlagsLength() const
	{
	LOG_FUNC
	TPtrC8 flags;
	TInt ret = iEirCodec.GetData(EEirFlags, flags);
	if(ret == KErrNone)
		{
		ret = flags.Length();
		}
	else
		{
		ret = 0;
		}
	return ret;
	}

TInt TBluetoothNameRecordWrapper::AddUuids16(RExtendedInquiryResponseUUIDContainer& aEIRContainer, TPtrC8& aUuids) const
	{
	TInt error = KErrNone;
	TInt length = aUuids.Length() / KSizeOf16BitUUID;
	TUUID uuid;
	
	for(TInt i = 0; i < length; i++)
		{
		__ASSERT_DEBUG(((aUuids.Length() - i*KSizeOf16BitUUID)% KSizeOf16BitUUID) == 0, Panic(EEirBadUuid16List));
		TRAP(error, uuid.SetFromLittleEndianL(aUuids.Mid(i*KSizeOf16BitUUID, KSizeOf16BitUUID)));
		if(error == KErrNone)
			{
			error = aEIRContainer.UUIDs().Add(uuid);
			}
		else
			{
			break;
			}
		}
	
	return error;
	}

TInt TBluetoothNameRecordWrapper::AddUuids128(RExtendedInquiryResponseUUIDContainer& aEIRContainer, TPtrC8& aUuids) const
	{
	TInt error = KErrNone;
	TInt length = aUuids.Length() / KSizeOf128BitUUID;
	TUUID uuid;
	
	for(TInt i = 0; i < length; i++)
		{
		__ASSERT_DEBUG(((aUuids.Length() - i*KSizeOf128BitUUID)% KSizeOf128BitUUID) == 0, Panic(EEirBadUuid128List));
		TRAP(error, uuid.SetFromLittleEndianL(aUuids.Mid(i*KSizeOf128BitUUID, KSizeOf128BitUUID)));
		if(error == KErrNone)
			{
			error = aEIRContainer.UUIDs().Add(uuid);
			}
		else
			{
			break;
			}
		}
	
	return error;
	}

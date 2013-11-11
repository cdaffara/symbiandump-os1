// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef THCIEVENTBASE_H
#define THCIEVENTBASE_H

#include <e32def.h>
#include <bluetooth/hci/hcievents.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hcitypes.h>
#include <bttypespartner.h>

class MHCICommandEventObserver;
class MHCIDataEventObserver;

/**
Represents the common data for a HCI event.

All of the event classes have been design to be used in a synchronous manner. All data is used by 
reference and hence any other object that requires persistence of the event parameters should make
their own copy of them.
*/
NONSHARABLE_CLASS(THCIEventBase)
	{
public:
	/**
	Getter for the event code.
	@return The event code that describes the event.
	*/
	IMPORT_C THCIEventCode EventCode() const;

	/**
	Getter for the error code.

	Although this error code is not defined for all events in the HCI specification, the error code
	is in the base class so that the command queue can use the event error in its decisions without casting
	to a derived class.
	Those events that do not contain an error code in the specification should return with THCIErrorCode::EOK.

	@return The error code contained in the event.
	*/
	IMPORT_C THCIErrorCode ErrorCode() const;

	/**
	Static factory and despatcher method.
	
	This method creates an object of the appropriate derived type according to the event code
	in the event data supplied. Depending upon the type of event, it is then passed to the
	supplied Command Event observer or Data Event observer as appropriate. The event object is
	created on the stack and is therefore destroyed before CreateAndSendEvent returns.
	
	@param aEventData The data packet containing the event
	@param aEventObserver The observer for handling command-related events
	@param aDataEventObserver The observer for handling data-related events
	@param aObserver Additional observer parameter to allow the licensee extra future flexibility
	*/
	IMPORT_C static	TInt CreateAndSendEvent(const TDesC8& aEventData, MHCICommandEventObserver& aEventObserver, MHCIDataEventObserver& aDataEventObserver, TAny* aObserver=NULL);
	
protected:
	/**
	Constructor used for creating a faked event
	@param aEventCode The event code for the event
	@param aParameterTotalLength The total length of the event parameters
	@param aEventData Empty buffer where the faked event data will be built.
	Every event starts with a one-byte event code and one-byte parameter total length field, so
	these are managed in this base class. The data after this is event specific and has length
	equal to Parameter Total Length. It is the responsibility of the derived class faking
	constructor to populate the rest of the aEventData buffer and then to set the member
	variable iEventData to reference this buffer when it is complete.
	Note that the aEventData buffer must persist as long as the object itself. It must be
	empty intially, and must be big enough to hold all the data for the given event type.
	*/
	THCIEventBase(THCIEventCode aEventCode, TUint8 aParameterTotalLength, TDes8& aEventData);	
	/**
	Normal constructor. This is used to create an event object to wrap event data received from the Controller
	@param aEventData Buffer holding event data received from the Controller
	*/
	IMPORT_C THCIEventBase(const TDesC8& aEventData);	

	/**
	Retrieves a TInt8 from specified offset within the event data descriptor.
	@param aOffset offset of value in event data.

	@return Value at offset.
	*/
	IMPORT_C TInt8 AsTInt8(TInt aOffset) const;

	/**
	Retrieves a TInt8 from specified offset and index within the event data descriptor.
	Is intended to be used with events returning variable number of parameters.
	
	@param aOffset offset of value in event data.
	@param aArrayBlockSize size of one array entry, i.e. all parameters for an array entry.
	@param aIndex index into the array of TUint8s of this event.

	@return Value at offset and index.
	*/
	IMPORT_C TInt8 AsTInt8(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const;

	/**
	Retrieves a TUint8 from specified offset within the event data descriptor.
	@param aOffset offset of value in event data.

	@return Value at offset.
	*/
	IMPORT_C TUint8 AsTUint8(TInt aOffset) const;

	/**
	Retrieves a TUint8 from specified offset and index within the event data descriptor.
	Is intended to be used with events returning variable number of parameters.
	
	@param aOffset offset of value in event data.
	@param aArrayBlockSize size of one array entry, i.e. all parameters for an array entry.
	@param aIndex index into the array of TUint8s of this event.

	@return Value at offset and index.
	*/
	IMPORT_C TUint8 AsTUint8(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const;

	/**
	Retrieves a TUint16 from specified offset within the event data descriptor.
	@param aOffset offset of value in event data.

	@return Value at offset.
	*/
	IMPORT_C TUint16 AsTUint16(TInt aOffset) const;

	/**
	Retrieves a TUint16 from specified offset and index within the event data descriptor.
	Is intended to be used with events returning variable number of parameters.
	
	@param aOffset offset of value in event data.
	@param aArrayBlockSize size of one array entry, i.e. all parameters for an array entry.
	@param aIndex index into the array of TUint16s of this event.

	@return Value at offset and index.
	*/
	IMPORT_C TUint16 AsTUint16(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const;

	/**
	Retrieves a TUint32 from specified offset within the event data descriptor.
	@param aOffset offset of value in event data.
	@param aSize how many bytes to get.

	@return Value at offset.
	*/
	IMPORT_C TUint32 AsTUint32(TInt aOffset, TUint8 aSize) const;

	/**
	Retrieves a TUint32 from specified offset and index within the event data descriptor.
	Is intended to be used with events returning variable number of parameters.
	
	@param aOffset offset of value in event data.
	@param aArrayBlockSize size of one array entry, i.e. all parameters for an array entry.
	@param aIndex index into the array of TUint32s of this event.
	@param aSize how many bytes to get.

	@return Value at offset and index.
	*/
	IMPORT_C TUint32 AsTUint32(TInt aOffset, TInt aArrayBlockSize, TInt aIndex, TUint8 aSize) const;

	/**
	Retrieves a TUint64 from specified offset within the event data descriptor.
	@param aOffset offset of value in event data.

	@return Value at offset.
	*/
	IMPORT_C TUint64 AsTUint64(TInt aOffset) const;

	/**
	Retrieves a TUint64 from specified offset and index within the event data descriptor.
	Is intended to be used with events returning variable number of parameters.
	
	@param aOffset offset of value in event data.
	@param aArrayBlockSize size of one array entry, i.e. all parameters for an array entry.
	@param aIndex index into the array of TUint64s of this event.

	@return Value at offset and index.
	*/
	IMPORT_C TUint64 AsTUint64(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const;

	/**
	Retrieves a TDesC8& from specified offset within the event data descriptor.
	@param aOffset offset of value in event data.

	@return Value at offset.
	*/
	IMPORT_C TPtrC8 AsString(TInt aOffset) const;
	
	/**
	Retrieves a Null terminated TDesC8& from specified offset within the event data descriptor.
	@param aOffset offset of value in event data.

	@return Value at offset.
	*/
	IMPORT_C TPtrC8 AsNullTerminatedString(TInt aOffset) const;

	/**
	Retrieves a Null terminated Bluetooth Device Name from a specified event data descriptor.

	@return Bluetooth Device Name at offset.
	*/
	IMPORT_C TPtrC8 AsBluetoothDeviceName(TInt aOffset) const;
	
	/**
	Retrieves a TBTDevAddr from specified offset within the event data descriptor.
	@param aOffset offset of value in event data.

	@return Value at offset.
	*/
	IMPORT_C TBTDevAddr AsDevAddr(TInt aOffset) const;

	/**
	Retrieves a TBTDevAddr from specified offset and index within the event data descriptor.
	Is intended to be used with events returning variable number of parameters.
	
	@param aOffset offset of value in event data.
	@param aArrayBlockSize size of one array entry, i.e. all parameters for an array entry.
	@param aIndex index into the array of TBTDevAddrs of this event.

	@return Value at offset and index.
	*/
	IMPORT_C TBTDevAddr AsDevAddr(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const;

	/**
	Retrieves a TBTDevAddr from specified offset within the event data descriptor.
	@param aOffset offset of value in event data.

	@return Value at offset.
	*/
	IMPORT_C TBluetoothSimplePairingHash AsSimplePairingHash(TInt aOffset) const;

	/**
	Retrieves a TBTDevAddr from specified offset within the event data descriptor.
	@param aOffset offset of value in event data.

	@return Value at offset.
	*/
	IMPORT_C TBluetoothSimplePairingRandomizer AsSimplePairingRandomizer(TInt aOffset) const;

	
	/**
	Retrieves a TBTLinkKey from specified offset within the event data descriptor.
	@param aOffset offset of value in event data.

	@return Value at offset.
	*/
	IMPORT_C TBTLinkKey AsLinkKey(TInt aOffset) const;

	/**
	Retrieves a TBTLinkKey from specified offset and index within the event data descriptor.
	Is intended to be used with events returning variable number of parameters.
	
	@param aOffset offset of value in event data.
	@param aArrayBlockSize size of one array entry, i.e. all parameters for an array entry. 
	@param aIndex index into the array of TBTLinkKeys of this event.

	@return Value at offset and index.
	*/
	IMPORT_C TBTLinkKey AsLinkKey(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const;
	
	/**
	Retrieves a THCIConnectionHandle from specified offset within the event data descriptor.
	@param aOffset offset of value in event data.

	@return Value at offset.
	*/
	IMPORT_C THCIConnectionHandle AsConnectionHandle(TInt aOffset) const;
	
	/**
	Retrieves a THCIConnectionHandle from specified offset and index within the event data 
	descriptor. Is intended to be used with events returning variable number of parameters.

	@param aOffset offset of value in event data.
	@param aArrayBlockSize size of one array entry, i.e. all parameters for an array entry.
	@param aIndex index into the array of THCIConnectionHandle of this event.

	@return Value at offset and index.
	*/
	IMPORT_C THCIConnectionHandle AsConnectionHandle(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const;

	/**
	Writes an Event code to the event data descriptor.
	ALL the write functions are intended to be used ONE AFTER ANOTHER in the order the objects
	need to be written to the event data. Thus:
		PutEventCode(n);
		PutTUint8(int1);
		PutTUint16(int2);
	will product an event string n int1 int2
	It is NOT possible to write these into the string in the order n, int2, int1 and get them to produce the 
	event string shown. This is very similar to the way you construct commands.
	
	@param aValue the event code to write
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutEventCode(THCIEventCode aValue, TDes8& aEventData);

	/**
	Writes a TInt8 to the event data descriptor.
	
	@param aValue the value to write
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutTInt8(TInt8 aValue, TDes8& aEventData);

	/**
	Writes a TUint8 to the event data descriptor.
	
	@param aValue the value to write
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutTUint8(TUint8 aValue, TDes8& aEventData);

	/**
	Writes a TUint16 to the event data descriptor.
	@param aValue the value to write
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutTUint16(TUint16 aValue, TDes8& aEventData);

	/**
	Writes a TUint32 to the event data descriptor.
	@param aValue the value to write
	@param aSize how many bytes to write.
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutTUint32(TUint32 aValue, TUint8 aSize, TDes8& aEventData);

	/**
	Writes a TUint64 to the event data descriptor.
	@param aValue the value to write
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutTUint64(TUint64 aValue, TDes8& aEventData);

	/**
	Writes a TDesC8& to the event data descriptor.
	@param aString the string to write
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutString(const TDesC8& aString, TDes8& aEventData);

	/**
	Writes a TDesC8& to the event data descriptor padding with '\0' up to the required length.
	@param aString the string to write
	@param aRequiredLength the required length the writen string is to be
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutPaddedString(const TDesC8& aString, TInt aRequiredLength, TDes8& aEventData);

	/**
	Writes a TBTDevAddr to the event data descriptor.
	@param aValue the value to write
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutDevAddr(const TBTDevAddr& aBdaddr, TDes8& aEventData);
	
	/**
	Writes a TBTSimplePairingHashC to the event data descriptor.
	@param aValue the value to write
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutSimplePairingHash(const TBluetoothSimplePairingHash& aHash, TDes8& aEventData);

	/**
	Writes a TBTSimplePairingRandomizerR to the event data descriptor.
	@param aValue the value to write
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutSimplePairingRandomizer(const TBluetoothSimplePairingRandomizer& aRandomizer, TDes8& aEventData);

	
	/**
	Writes a TBTLinkKey to the event data descriptor.
	@param aValue the value to write
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutLinkKey(const TBTLinkKey aValue, TDes8& aEventData);

	/**
	Writes a THCIConnectionHandle to the event data descriptor.
	@param aConnectionHandle the value to write
	@param aEventData modifiable descriptor buffer where the faked event is being created
	*/
	void PutConnectionHandle(THCIConnectionHandle aConnectionHandle, TDes8& aEventData);

private:
	THCIEventBase(const THCIEventBase&);

	// Helper for CreateAndSendEvent
	static TInt CreateAndSendCommandCompleteEvent(const TDesC8& aEventData,
												  MHCICommandEventObserver&);

public:
	// Event layout constants - lengths/offsets of fields that occur at the start of all the events
	const static TUint KEventCodeOffset = 0;
	const static TUint KEventCodeLength = 1;
	const static TUint KTotalParameterLengthOffset = KEventCodeLength;
	const static TUint KTotalParameterLengthLength = 1;
	const static TUint KEventCommonFieldsLength = KEventCodeLength + KTotalParameterLengthLength;

protected:
	THCIErrorCode iErrorCode;
	TPtrC8 iEventData;

private:
	// Reserved for extending the class while maintaining binary compatability.
	TUint32 iReserved1;
	TUint32 iReserved2;
	};

#endif // THCIEVENTBASE_H

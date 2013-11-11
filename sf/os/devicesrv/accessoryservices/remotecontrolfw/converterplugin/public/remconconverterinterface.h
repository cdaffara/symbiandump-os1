// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef REMCONCONVERTERINTERFACE_H
#define REMCONCONVERTERINTERFACE_H

#include <e32base.h>
#include <remcon/messagetype.h>

/** 
The UID of the converter API. If the converter API ever has to change, a new 
UID and associated M class will be created. New implementations of 
CRemConConverterPlugin may implement the new API. Old (non-updated) converters 
will still work as long as RemCon supports the old API.
*/
const TInt KRemConConverterInterface1 = 0x1020D40A;

/**
Mixin for the converter API.
*/
class MRemConConverterInterface
	{
public:
	/** 
	Called by the server to get the converter to convert from the interface 
	(API) format to the bearer-specific format.
	@param aInterfaceUid The interface UID.
	@param aOperationId Operation ID.
	@param aData Data associated with the operation.
	@param aMsgType The type of the message.
	@param aBearerData Converted output.
	*/
	virtual TInt InterfaceToBearer(TUid aInterfaceUid, 
		TUint aOperationId,
		const TDesC8& aData,
		TRemConMessageType aMsgType, 
		TDes8& aBearerData) const = 0;
	
	/** 
	Called by the server to get the converter to convert from the bearer-
	specific format to the interface (API) format.
	@param aBearerData Bearer input.
	@param aInterfaceUid The interface UID.
	@param aOperationId Operation ID.
	@param aMsgType The type of the message.
	@param aData Data associated with the operation.
	*/
	virtual TInt BearerToInterface(const TDesC8& aBearerData,
		TUid& aInterfaceUid, 
		TUint& aOperationId,
		TRemConMessageType& aMsgType, 
		TDes8& aData) const = 0;

	/**
	Called by the server to find out if this converter converts between the formats 
	of this interface and bearer.
	@param aInterfaceUid The interface UID.
	@param aBearerUid The bearer UID.
	*/
	virtual TBool SupportedUids(const TUid& aInterfaceUid, const TUid& aBearerUid) const = 0;
	
	/**
	Called by the server to find out if this converter converts between the formats 
	of this bearer interface and RemCon interface.
	@param aInterfaceData The identifier of this interface in a bearer defined format.
	@param aBearerUid The bearer UID.
	*/
	virtual TBool SupportedInterface(const TDesC8& aInterfaceData, const TUid& aBearerUid) const = 0;
	};

#endif // REMCONCONVERTERINTERFACE_H

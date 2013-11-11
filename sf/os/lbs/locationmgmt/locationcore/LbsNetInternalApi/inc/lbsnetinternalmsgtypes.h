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
// LbsNetInternalMsgTypes.cpp
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef LBSNETINTERNALMSGTYPES_H
#define LBSNETINTERNALMSGTYPES_H

#include "lbscommoninternaldatatypes.h"



//
// Message classes between:
//    - the Network Request Handler and Network Gateway
//    - the Network Request Handler and AGPS Manager
//

const TInt KMaximumMessageSizeBytes = 1576;

/** Base class for messages sent between the Network Gateway and Network Request Handler.
as well as between the Network Gateway and AGPS Manager.

This class defines the types of messages and reserves a buffer to store
information, but it can't be used itself. Instead derived classes will
implement each of the types of messages and supply accessor functions.

See the prototype of a location request message below.
*/
class TLbsNetInternalMsgBase
	{
public:
	/** List of all possible message types.
	
	Each type of message listed here must be implemented as a
	separate class that inherits from TLbsNetInternalMsgBase.
	*/
	enum TLbsNetInternalMsgType
		{
		EUnknown = 0,
		ELocationRequest,			// NG			--> NRH
		ELocationResponse,			// NRH			--> NG
		ETransmitLocationRequest,	// NRH			--> NG
		ETransmitLocationCancel,	// NRH			--> NG
		EPrivacyRequest,			// NG			--> NRH
		EPrivacyResponse,			// NRH			--> NG
		ESessionComplete,			// NG           --> NRH or AGPS-Manager
		ESelfLocationRequest,		// AGPS-Manager --> NG
		ESelfLocationCancel,		// AGPS-Manager --> NG
		EAssistanceDataRequest,		// AGPS-Manager --> NG
		EAssistanceDataResponse,	// NG           --> AGPS-Manager
		ESystemStatusAdvice,		// AGPS-Manager --> NG
		ENetworkLocationUpdate,		// NG			--> NRH
		ECancelFromPrivacyController,// NRH			--> NG
		ECellLocationRequest,       // PSY     		--> NG
		ECellLocationResponse,      // NG           --> PSY 
		ECellLocationCancel         // PSY     		--> NG
		};

public:
	IMPORT_C TLbsNetInternalMsgBase();
	
	IMPORT_C TLbsNetInternalMsgType Type() const;
	IMPORT_C const TLbsNetSessionIdInt& SessionId() const;
	IMPORT_C TInt32 Size() const;

	IMPORT_C TLbsNetInternalMsgBase& operator = (const TLbsNetInternalMsgBase& aSrc);

protected:
	// Derived classes set their type by using these constructors.
	IMPORT_C TLbsNetInternalMsgBase(TLbsNetInternalMsgType aType,
						   TInt aSizeInBytes);	
	IMPORT_C TLbsNetInternalMsgBase(TLbsNetInternalMsgType aType, 
						   TInt aSizeInBytes,
						   const TLbsNetSessionIdInt& aSessionId);

private:
	// All messages have a type, size and session identifier
	TLbsNetInternalMsgType iType;
	TInt32 iSize;
	TLbsNetSessionIdInt iSessionId;
	};
	

#endif // LBSNETINTERNALMSGTYPES_H

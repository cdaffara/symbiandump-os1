// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file for the Automated Test Tool
// 
//

/**
 @file 
 @publishedAll
 @released
*/

#ifndef __CS_SUBCONEVENTS_H__
#define __CS_SUBCONEVENTS_H__
#include <e32base.h>
#include <e32std.h>
#include <comms-infras/metadata.h>
#include <comms-infras/metatype.h>

#include <es_sock.h>
using Meta::RMetaDataEComContainer;

const TUint32 KSubConGenericEventParamsGranted      =  0x1;
const TUint32 KSubConGenericEventDataClientJoined   =  0x2;
const TUint32 KSubConGenericEventDataClientLeft     =  0x4;
const TUint32 KSubConGenericEventSubConDown         =  0x8;
const TUint32 KSubConGenericEventParamsChanged      = 0x10;
const TUint32 KSubConGenericEventParamsRejected     = 0x20;

class CSubConGenEventParamsGranted : public CSubConNotificationEvent
/** Subconnection parameters granted event.

Provides an interface for granted parameters event. This event will contain
a negotiated generic parameter set and zero or more negotiated extension parameter sets.

Notification of this event occurs after a request to RSubConnection::SetParameters()  
has been made and negotiation with the network has been completed.
A notification will be received for each family contained in the parameter 
bundle that was negotiated successfully. This event presents a generic set 
and zero or more extension sets (providing they are supported by the 
underlying sub-connection provider technology) of the parameter family 
identified by the Id return from GetFamily().

The following operations are also provided:

GetGenericSet()/SetGenericSet() - Accessor function for Generic Parameter Sets.
GetExtensionSet() - Get extension parameter set of a particular index.
AddExtensionSetL() - Add extension parameter set.
GetFamily()/SetFamily() - Accessor function for parameter family type.

@publishedAll
@released since v9.1 */
{
public:
	inline static CSubConGenEventParamsGranted* NewL();

	inline CSubConGenEventParamsGranted();

	inline const CSubConGenericParameterSet* const GetGenericSet() const;
	inline void SetGenericSet(CSubConGenericParameterSet* aGenericSet);

	inline const CSubConExtensionParameterSet* const GetExtensionSet(TUint aIndex) const;
	inline void AddExtensionSetL(CSubConExtensionParameterSet* aExtensionSet);
	inline TInt GetNumExtensionSets();

	inline TUint32 GetFamily() const;
	inline void SetFamily(TUint32 aFamily);

	virtual ~CSubConGenEventParamsGranted();

protected:
	DATA_VTABLE

	TUint32 iFamily;
	CSubConGenericParameterSet* iGenericSet;
	RMetaDataEComContainer iExtensionSets;
	};

class CSubConGenEventParamsChanged : public CSubConGenEventParamsGranted
/** Subconnection parameters changed event.

Provides an interface for changed parameters event. This event will contain
only error code i.e, the reason for changing already negotiated parameters.

This event occurs when the properties of a parameter family has been renegotiated 
due to some event on the network. It is not sent in response to a request to change 
the properties. The change could be the result of an error or just that the level 
of QoS has improved/worsened. If a new set of parameters are available they'll be 
presented as with the CSubConGenEventParamsGranted event. 

The error status is presented via the Error() method.


@publishedAll
@released since v9.1 */
	{
public:
	inline static CSubConGenEventParamsChanged* NewL();

	inline CSubConGenEventParamsChanged();

	inline TInt Error() const;
	inline void SetError(TInt aError);

	virtual ~CSubConGenEventParamsChanged();

protected:
	DATA_VTABLE

	TInt iError;
	};

class CSubConGenEventParamsRejected : public CSubConNotificationEvent
/** Subconnection parameters rejected event.

Provides an interface for rejected parameters event. This event will occur if
the negotiation with the network has failed for some reason. It will contain
the family type for which rejected event is generated & corresponding error 
code i.e, the reason for rejecting the parameter set.

Notification of this event occurs after a request to SetParameters() has been made 
and negotiation with the network has failed for some reason. It could be an error 
within the handset software/configuration, or that the network could not provide 
the acceptable (minimum) level of QoS. The reason for failure and the parameter 
family are presented by the accessor methods Error() and FamilyId(). Like the 
CSubConGenEventParamsGranted event, a notification for CSubConGenEventParamsRejected 
is received for each family in the parameter bundle that could not be negotiated successfully.

The following operations are also provided:

Error()/SetError() - Accessor function for error value.
FamilyId()/SetFamilyId() - Accessor function for family parameter type.

@publishedAll
@released since v9.1 */
	{
public:
	inline static CSubConGenEventParamsRejected* NewL();

	inline CSubConGenEventParamsRejected();

	inline TInt Error() const;
	inline void SetError(TInt aError);
	inline TInt FamilyId() const;
	inline void SetFamilyId ( TInt aFamilyId );

	virtual ~CSubConGenEventParamsRejected();

protected:
	DATA_VTABLE

	TInt iError;
	TInt iFamilyId;
	};

class CSubConGenEventDataClientBase : public CSubConNotificationEvent
/** Provides interface for source and destination end points of the data client 
along with IAP of the connection.

It is a base class for CSubConGenEventDataClientJoined & CSubConGenEventDataClientLeft

The following operations are also provided:

GetIap()/SetIap() - Accessor functions for Internet Access Point.
GetSourceAdress()/SetSourceAdress() - Accessor functions for source end points.
GetDestAdress()/SetDestAdress() - Accessor functions for destination end points.

@publishedAll
@released since v9.1 */
	{
public:
	inline CSubConGenEventDataClientBase();
	inline TUint GetIap() const;
	inline void SetIap(TUint aIap);

	inline const TSockAddr& GetSourceAdress() const;
	inline void SetSourceAddress(const TSockAddr& aSourceAddress);

	inline const TSockAddr& GetDestAdress() const;
	inline void SetDestAddress(const TSockAddr& aDestAddress);

	virtual ~CSubConGenEventDataClientBase();

protected:
	DATA_VTABLE

	TSockAddr iSourceAddress;
	TSockAddr iDestAddress;
	TUint	  iIap;
	};

class CSubConGenEventDataClientJoined : public CSubConGenEventDataClientBase
/** Data client joined event.

When an end-point(socket) was added in a sub-connection, CSubConGenEventDataClientJoined event is generated.

The base class CSubConGenEventDataClientBase contains sorce & destination 
end points along with IAP of the connection.

@publishedAll
@released since v9.1 */
	{
public:
	inline static CSubConGenEventDataClientJoined* NewL();

	inline CSubConGenEventDataClientJoined();
	virtual ~CSubConGenEventDataClientJoined();

protected:
	DATA_VTABLE
	};

class CSubConGenEventDataClientLeft : public CSubConGenEventDataClientBase 
/** Data client left event.

When an end-point(socket) was removed from a sub-connection, CSubConGenEventDataClientLeft event is generated.

The base class CSubConGenEventDataClientBase contains sorce & destination 
end points along with IAP of the connection.

@publishedAll
@released since v9.1 */
{
public:
	inline static CSubConGenEventDataClientLeft* NewL();

	inline CSubConGenEventDataClientLeft();

	virtual ~CSubConGenEventDataClientLeft();

protected:
	DATA_VTABLE
	};

class CSubConGenEventSubConDown : public CSubConNotificationEvent
/** Subconnection down event.

This event occurs when the underlying sub-connection has been lost.
This could be due to request for it to be closed, or some error on the network.
The error status is presented via the Error() method

@publishedAll
@released since v9.1 */
{
public:
	inline static CSubConGenEventSubConDown* NewL();

	inline CSubConGenEventSubConDown();

	inline TInt Error() const;
	inline void SetError(TInt aError);

	virtual ~CSubConGenEventSubConDown();

protected:
	DATA_VTABLE

	TInt iError;
	};


#include <cs_subconevents.inl>
#endif	// __CS_SUBCONEVENTS_H__


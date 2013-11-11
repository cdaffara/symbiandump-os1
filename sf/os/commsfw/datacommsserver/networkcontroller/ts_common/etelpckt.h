// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ETELPCKT_H__
#define __ETELPCKT_H__

#include "etel.h"


class TPacketDataConfigBase 
/** Base class for the packet configuration classes.

The Context configuration, QoS profile (Requested and Negotiated) and QoS 
Capabilities classes inherit from this class. It is outside the scope of all 
the Packet data classes. 

To allow future configuration structures (e.g. TContextConfigWCDMA, 
TContextConfigPDC, etc.) to be added in such a way that a completely different 
structure may be defined and passed into the same functions as this one, all 
existing configuration structures are derived from this thin base class 
which provides just one function, ExtensionId(). This function returns the 
one protected member variable, iExtensionId. A new structure would define an 
associated new constant to be set in iExtensionId on construction, which would 
then be queried by the TSY prior to unpackaging the structure. 

@see RPacketContext::TContextConfigGPRS
@see RPacketContext::TContextConfigCDMA 
@see RPacketContext::TContextConfigR99_R4  
@see RPacketContext::TContextConfigR5

@publishedPartner
@released

*/
	{
public:
	/**
	Flags identifying the different packet networks.
	*/
	enum 
		{
		/**
		GPRS Release 97/98 packet network.
		*/
		KConfigGPRS= 0x01,				
		/**
		GPRS/UMTS Release 99 and UMTS Release 4 networks.
		*/ 
		KConfigRel99Rel4 = 0x04,
		/**
		UMTS/IMS 3GPP Release 5 networks.
		*/ 
		KConfigRel5 = 0x05,		
		};
		
	/** This member returns the type of class.
	
	@return The type of class. */
	inline TInt ExtensionId(){return iExtensionId;};
protected:
	/**
	Defines the type of class: either KConfigGPRS, KConfigCMDA or KConfigRel99Rel4, 
	which identifies the type of packet network (GPRS R97/98, CDMA, Release 99 or 4 or 5)
	*/
	TInt iExtensionId;	
	};
	
	
class RPacketService
	{
public:
	TInt Open(RPhone& aPhone);
	void Close();

	void CancelAsyncRequest(TInt aReqToCancel) const;

	enum TStatus
		{
		EStatusUnattached,	// unattached to network & inactive packet data connection
		EStatusAttached,	// attached to network & inactive packet data connection
		EStatusActive,		// attached to network & active packet data connection
		EStatusSuspended	// attached to network & suspended packet data connection
		};

	TInt GetStatus(TStatus& aPacketStatus) const;

	enum TMSClass
		{
		EMSClassDualMode,			// Active Simultaneous PS & CS calls supported (ClassA)
		EMSClassSuspensionRequired, // Active CS & Suspended PS simultaneous calls supported (ClassB)
		EMSClassAlternateMode,		// Active CS or Active PS only call supported (ClassC)
		EMSClassCircuitSwitchedOnly,// Active CS only call supported (ClassC)
		EMSClassPacketSwitchedOnly, // Active PS only call supported (ClassC)
		EMSClassUnknown				// Unknown what configuration is supported
		};

	void GetMSClass(TRequestStatus& aStatus, TMSClass& aCurrentClass, TMSClass& aMaxClass) const;

	enum TRegistrationStatus
		{
		ENotRegisteredNotSearching,
		ERegisteredOnHomeNetwork,
		ENotRegisteredSearching,
		ERegistrationDenied,
		EUnknown,
		ERegisteredRoaming, // foreign network
		ENotRegisteredButAvailable,	
		ENotRegisteredAndNotAvailable
		};
	
	void GetNtwkRegStatus(TRequestStatus& aStatus, TRegistrationStatus& aRegistrationStatus) const;
	
	enum TAttachMode
		{
		EAttachWhenPossible,// instruct phone stack to attach to packet network whenever it can
		EAttachWhenNeeded	// instruct phone stack to attach to network only when required (eg: activating a connection)
		};

	TInt GetAttachMode(TAttachMode& aMode) const;

private:
	CEtelBehaviourBase* iTestBehaviour;
	};

#endif // __ETELPCKT_H__


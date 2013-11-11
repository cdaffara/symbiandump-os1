// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SECEVENT_H
#define SECEVENT_H

#include <e32std.h>

//forward decs
class CBTSecMan;
class CBTAccessRequester;
class CBTAuthorisor;
class CBTNumericComparator;
class CBTPasskeyEntry;
class CHCIFacade;
class CPhysicalLink;
class CPhysicalLinksManager;
class CBTInquiryMgr;
class CBTHostResolver;
class CSecNotifierRequester;
class CBTAccessRequesterStateFactory;

/**
BT Security Event base class.
*/
class TBTSecEvent
	{
public:
	enum TEventType
		{
		EStart,
		EPhysicalLinkUp,
		EPhysicalLinkDown,
		ERequestAuthentication,
		EAuthenticationRequested,
		EAuthenticationComplete,
		ERequestEncryptionChange,
		EEncryptionChangeRequested,
		EEncryptionChangeComplete,
		ERequestAuthorisation,
		EAuthorisationRequested,
		EAuthorisationComplete,
		// Simple Pairing events
		EHostsupportedFeatures,
		EIOCapsRequested,
		EIOCapsResponse,
		EUserConfirmationRequested,
		EUserConfirmationRequestComplete,
		// we probably shouldn't have this as an event, secman should handle this statically (and assert debug).
		EPasskeyNotfication,
		EKeypressEntry,
		EKeypressComplete,
		ERemoteOOBDataRequested,
		ERemoteOOBDataRequestComplete,
		EAuthoriseSspIOCapsRequested,
		EUndefinedEvent,
		};

public:
	TBTSecEvent(TEventType aEvent, TInt aError = KErrNone)
		: iEvent(aEvent)
		, iError(aError)
		{}

public:
	TEventType Event() const { return iEvent; }
	TInt Error() const { return iError; }
	
private:
	TEventType iEvent;
	TInt iError;
	};

class TBTSecEventStart : public TBTSecEvent
	{
public:
	TBTSecEventStart()
		: TBTSecEvent(EStart, KErrNone)
		{}
	static TBTSecEventStart* Cast(TBTSecEvent* aEvent);
	};

class TBTSecEventPhysicalLinkUp : public TBTSecEvent
	{
public:
	TBTSecEventPhysicalLinkUp(TInt aError = KErrNone)
		: TBTSecEvent(EPhysicalLinkUp, aError)
		{}
	static TBTSecEventPhysicalLinkUp* Cast(TBTSecEvent* aEvent);
	};

class TBTSecEventRemoteOOBDataRequest: public TBTSecEvent
	{
public:
	TBTSecEventRemoteOOBDataRequest(TInt aError = KErrNone)
		: TBTSecEvent(ERemoteOOBDataRequested, aError)
		{}
	static TBTSecEventRemoteOOBDataRequest* Cast(TBTSecEvent* aEvent);
	};

class TBTSecEventAuthenticationComplete : public TBTSecEvent
	{
public:
	TBTSecEventAuthenticationComplete(TInt aError = KErrNone)
		: TBTSecEvent(EAuthenticationComplete, aError)
		{}
	static TBTSecEventAuthenticationComplete* Cast(TBTSecEvent* aEvent);
	};

class TBTSecEventEncryptionChangeComplete : public TBTSecEvent
	{
public:
	TBTSecEventEncryptionChangeComplete(TInt aError = KErrNone)
		: TBTSecEvent(EEncryptionChangeComplete, aError)
		{}
	static TBTSecEventEncryptionChangeComplete* Cast(TBTSecEvent* aEvent);
	};

class TBTSecEventAuthenticationRequested: public TBTSecEvent
	{
public:
	TBTSecEventAuthenticationRequested(TInt aError = KErrNone)
		: TBTSecEvent(EAuthenticationRequested, aError)
		{}
	static TBTSecEventAuthenticationRequested* Cast(TBTSecEvent* aEvent);
	};

class TBTSecEventRequestAuthentication: public TBTSecEvent
	{
public:
	TBTSecEventRequestAuthentication(TInt aError = KErrNone)
		: TBTSecEvent(ERequestAuthentication, aError)
		{}
	static TBTSecEventRequestAuthentication* Cast(TBTSecEvent* aEvent);
	};

class TBTSecEventAuthorisationRequested: public TBTSecEvent
	{
public:
	TBTSecEventAuthorisationRequested(TInt aError = KErrNone)
		: TBTSecEvent(EAuthorisationRequested, aError)
		{}
	static TBTSecEventAuthorisationRequested* Cast(TBTSecEvent* aEvent);
	};

class TBTSecEventRequestAuthorisation: public TBTSecEvent
	{
public:
	TBTSecEventRequestAuthorisation(TInt aError = KErrNone)
		: TBTSecEvent(ERequestAuthorisation, aError)
		{}
	static TBTSecEventRequestAuthorisation* Cast(TBTSecEvent* aEvent);
	};

class TBTSecEventIoCapabilityRequested : public TBTSecEvent
	{
public:
	TBTSecEventIoCapabilityRequested(TInt aError = KErrNone)
		: TBTSecEvent(EIOCapsRequested, aError)
		{}
	static TBTSecEventIoCapabilityRequested* Cast(TBTSecEvent* aEvent);
	};

class TBTSecEventIoCapabilityResponse : public TBTSecEvent
	{
public:
	TBTSecEventIoCapabilityResponse(THCIIoCapability aIoCapability,
									THCIOobDataPresence aOobDataPresent,
									THCIAuthenticationRequirement aAuthenticationRequirements,
									TInt aError = KErrNone)
		: TBTSecEvent(EIOCapsResponse, aError)
		, iIoCapability(aIoCapability)
		, iOobDataPresent(aOobDataPresent)
		, iAuthenticationRequirements(aAuthenticationRequirements)
		{}
	
	static TBTSecEventIoCapabilityResponse* Cast(TBTSecEvent* aEvent);
		
	THCIIoCapability IoCapability() const { return iIoCapability; }
	THCIOobDataPresence OobDataPresent() const { return iOobDataPresent;  }
	THCIAuthenticationRequirement AuthenticationRequirements() const { return iAuthenticationRequirements; }

private:
	THCIIoCapability iIoCapability;
	THCIOobDataPresence iOobDataPresent;
	THCIAuthenticationRequirement iAuthenticationRequirements;
	};

class TBTSecEventUserConfirmationRequest : public TBTSecEvent
	{
public:
	TBTSecEventUserConfirmationRequest(TUint32 aNumericValue, TInt aError = KErrNone)
		: TBTSecEvent(EUserConfirmationRequested, aError)
		, iNumericValue(aNumericValue)
		{}
	
	static TBTSecEventUserConfirmationRequest* Cast(TBTSecEvent* aEvent); 
		
	TUint32 NumericValue() const { return iNumericValue; }

private:
	TUint32 iNumericValue;
	};

class TBTSecEventUserConfirmationComplete : public TBTSecEvent
	{
public:
	TBTSecEventUserConfirmationComplete(TInt aError)
		: TBTSecEvent(EUserConfirmationRequestComplete, aError)
		, iError(aError)
		{}

	TInt Status() const { return iError; }
	
private:
	TInt iError;
	};

class TBTSecEventAuthorisationComplete : public TBTSecEvent
	{
public:
	TBTSecEventAuthorisationComplete(TBool aAccessAllowed)
		: TBTSecEvent(EAuthorisationComplete, KErrNone)
		, iAccessAllowed(aAccessAllowed)
		{}
	
	static TBTSecEventAuthorisationComplete* Cast(TBTSecEvent* aEvent);

	TInt AccessAllowed() const { return iAccessAllowed; }
	
private:
	TInt iAccessAllowed;
	};

class TBTSecEventPasskeyNotification : public TBTSecEvent
	{
public:
	TBTSecEventPasskeyNotification(TUint32 aPasskey, TInt aError = KErrNone)
		: TBTSecEvent(EPasskeyNotfication, aError)
		, iPasskey(aPasskey)
		{}
		
	TUint32 Passkey() const { return iPasskey; }

private:
	TUint32 iPasskey;
	};

class TBTSecEventKeypressEntry : public TBTSecEvent
	{
public:
	TBTSecEventKeypressEntry(TUint32 aNotificationType, TInt aError = KErrNone)
		: TBTSecEvent(EKeypressEntry, aError)
		, iNotificationType(aNotificationType)
		{}
	
	static TBTSecEventKeypressEntry* Cast(TBTSecEvent* aEvent);
		
	TUint8 NotificationType() const { return iNotificationType; }

private:
	TUint8 iNotificationType;
	};

#endif // SECEVENT_H

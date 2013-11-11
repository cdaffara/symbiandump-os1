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

#ifndef __ETEL_H__
#define __ETEL_H__

#include <e32base.h>

/** Major version number.*/
const TUint KEtelMajorVersionNumber=1;
/** Minor version number. */
const TUint KEtelMinorVersionNumber=0;
/** Build number. */
const TUint KEtelBuildVersionNumber=606;
/**
@internalComponent
*/
IMPORT_C void PanicClient(TInt aFault);

//
// Etel Core Error Definitions
//

const TInt KErrEtelCoreBase = -2000;

/** The client is not the call owner and has attempted to perform an operation 
that requires ownership.*/
const TInt KErrEtelNotCallOwner=KErrEtelCoreBase;
/** An attempt has been made to load an ETel (TSY) module which contains a phone 
with the same name as one already loaded. */
const TInt KErrEtelDuplicatePhoneName=KErrEtelCoreBase-1;
/** An attempt has been made to acquire call ownership when the requesting client 
is already the call owner. */
const TInt KErrEtelAlreadyCallOwner=KErrEtelCoreBase-2;
/** A connection has been terminated because the carrier has been lost. */
const TInt KErrEtelNoCarrier=KErrEtelCoreBase-3;
/** An attempt to establish a connection with the RCall::Dial() function has failed 
because the remote party's line was busy. */
const TInt KErrEtelBusyDetected=KErrEtelCoreBase-4;
/** An attempt has been made to transfer call ownership, but no other clients are 
interested in acquiring ownership. */
const TInt KErrEtelNoClientInterestedInThisCall=KErrEtelCoreBase-5;
/** The TSY has failed to initialise the modem. This may be occur if the modem 
is disconnected during initialisation, or if an attempt is made to initialise 
an unsupported modem. */
const TInt KErrEtelInitialisationFailure=KErrEtelCoreBase-6;
/** An attempt has been made to perform an operation which requires the call to 
be connected - when the call is not connected. E.g. RCall:LoanDataPort(). */
const TInt KErrEtelCallNotActive=KErrEtelCoreBase-7;
/** A connection request has failed because there is no answer. */
const TInt KErrEtelNoAnswer=KErrEtelCoreBase-8;
/** A connection request has failed because there is no dial tone. For example, 
if there is no connection between the modem and the network. */
const TInt KErrEtelNoDialTone=KErrEtelCoreBase-9;
/** An attempt to configure the port has failed because the hardware cannot match 
the desired configuration. For example, if an attempt is made to configure 
the comms port to a baud rate it cannot support. */
const TInt KErrEtelConfigPortFailure=KErrEtelCoreBase-10;
/** Out of memory error - global chunk cannot be created. */
const TInt KErrEtelFaxChunkNotCreated=KErrEtelCoreBase-11;
/** The client is not the fax owner and has attempted to perform an operation that 
requires fax ownership.

The fax client should be used to access fax functionality. Consequently, client 
code should not return this error. */
const TInt KErrEtelNotFaxOwner=KErrEtelCoreBase-12;
/** An attempt has been made to perform an operation which requires port ownership. 
However, the client does not own the port. */
const TInt KErrEtelPortNotLoanedToClient=KErrEtelCoreBase-13;
/** An operation has failed because the modem has been incorrectly configured. */
const TInt KErrEtelWrongModemType=KErrEtelCoreBase-14;
/** An operation has failed because the modem does not understand the request - 
possible bug in modem. */
const TInt KErrEtelUnknownModemCapability=KErrEtelCoreBase-15;
/** The call has already placed an 'answer incoming call' request. Only 1 request 
of a type (e.g. fax, voice, data) can be outstanding on the server at a time. */
const TInt KErrEtelAnswerAlreadyOutstanding=KErrEtelCoreBase-16;
/** An attempt to establish a connection with the modem has failed because the 
modem cannot be detected. */
const TInt KErrEtelModemNotDetected=KErrEtelCoreBase-17;
/** Operation failed because the modem settings are corrupt. */
const TInt KErrEtelModemSettingsCorrupt=KErrEtelCoreBase-18;
/** The Data port is in use by another call or application. */
const TInt KErrEtelPortAlreadyLoaned=KErrEtelCoreBase-19;
/** Another call active. Close it before opening a new one. */
const TInt KErrEtelCallAlreadyActive=KErrEtelCoreBase-20;
class CEtelBehaviourBase;
//
//
// RTelSubSessionBase
//
//
/**
A base class used in the derivation of RCall, RLine, and RPhone. It has no user
accessible functions. 

@publishedPartner
@released
*/
class RTelSubSessionBase 
	{
public:
	inline RTelSubSessionBase() {};
	};
class RTelServer : public CBase
	{
public:
	TInt Connect();
	void Close();

	enum TNetworkType
		{
		ENetworkTypeWiredAnalog,
		ENetworkTypeWiredDigital,
		ENetworkTypeMobileAnalog,
		ENetworkTypeMobileDigital,
		ENetworkTypeUnknown
		};
	
	struct  TPhoneInfo
		{
		TNetworkType iNetworkType;
		TName iName;
		TUint iNumberOfLines;
		TUint iExtensions;
		};
	
	TInt EnumeratePhones(TInt& aNoOfPhones) const;
	TInt GetPhoneInfo(const TInt aIndex,TPhoneInfo& aInfo) const;
	TInt GetTsyName(const TInt aIndexOfPhone, TDes& aTsyName) const;

	TInt LoadPhoneModule(const TDesC& aFileName);
	TInt UnloadPhoneModule(const TDesC& aFileName) const;

	CEtelBehaviourBase* TestBehaviour() const;

private:
	static CEtelBehaviourBase* CreateTestBehaviourL(const TDesC& aFileName);

private:
	CEtelBehaviourBase* iTestBehaviour;
	};

class RPhone
	{
public:
	TInt Open(RTelServer& aSession,const TDesC& aName);
	void Close();

	void CancelAsyncRequest(TInt aReqToCancel) const;
	
	enum TCapsFlags
		{
		KCapsUnknown=0x00000001,
		KCapsData=0x00000002,
		KCapsFaxClassOne=0x0000004,
		KCapsFaxClassOnePointZero=0x0000008,
		KCapsFaxClassTwo=0x00000010,
		KCapsFaxClassTwoPointZero=0x00000020,
		KCapsFaxClassTwoPointOne=0x00000040,
		KCapsVoice=0x00000080,
		KCapsEventModemDetection=0x00010000,
		KCapsStealCommPort=0x00020000
		};

	struct TCaps
		{
		TUint iFlags;
		};

	TInt GetCaps(TCaps& aCaps) const;

	CEtelBehaviourBase* TestBehaviour() const;

protected:
	CEtelBehaviourBase* iTestBehaviour;
	};

class RCall
	{
public:
	struct  TCommPort
		{
		TFileName iCsy;
		TName iPort;
		};
	};


#endif // __ETEL_H__


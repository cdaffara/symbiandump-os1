/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file lbsnetprotocolproxy.h
 @internalComponent
 @test
*/
#if (!defined LBSNETPROTOCOLPROXY_H)
#define LBSNETPROTOCOLPROXY_H

#include <lbs/test/lbsnetprotocolproxydefs.h>

/**
Interface for use by test code to interface with
the Network Protocol stub implementation

>>Example code for CallL()

If we wish to for the following function to be called

virtual void ProcessPrivacyRequest(const TLbsNetSessionId& aSessionId,
                                   TBool aEmergency, 
                                   const TLbsNetPosRequestPrivacy& aPrivacy,
                                   const TLbsExternalRequestInfo& aRequestInfo) = 0;
                                   
Then we would perform the following:

const TLbsNetSessionId sessionId = foo;
TBool emergency = bar; 
const TLbsNetPosRequestPrivacy privacy = baz,
const TLbsExternalRequestInfo requestInfo = wibble;

MNetProtocolProxy::CallL(ENetMsgProcessPrivacyRequest, &sessionId, &emergency, &privacy, &requestInfo);

// Note - 
// The Protocol proxy may automatically handle session Ids, 
// and so we might decide to remove them from the list, (or at least ignore it/expect NULL)
// So alternatively the call might look like the below instead.
MNetProtocolProxy::CallL(ENetMsgProcessPrivacyRequest, NULL, emergency, privacy, requestInfo);
MNetProtocolProxy::CallL(ENetMsgProcessPrivacyRequest, emergency, privacy, requestInfo);

>>Example code for WaitForResponse() and GetArgs()

If we are waiting for a response from the function

virtual void RespondLocationRequest(
                                    const TLbsNetSessionId& aSessionId,
                                    TInt  aReason,
                                    const TPositionInfoBase& aPosInfo) = 0;

Then we would perform the following:

//Use whatever test assert is appropriate
TESTL(MNetProtocolProxy::WaitForResponse(10*1000*1000) == ENetMsgRespondLocationRequest);

//If we're interested in the return values, we can check
//Create the objects for the values to be written to
TLbsNetSessionId* sessionId = NULL;
TInt reason;
TPositionInfoBase* posInfo = NULL;

// GetArgs will write the values of the arguments into the function 
MNetProtocolProxy::GetArgs(ENetMsgRespondLocationRequest, &sessionId, &reason, &posInfo);

 
@internalComponent
@test
*/
class MNetProtocolProxy
	{
public:

/**
Immediately tells the Network Protocol Proxy to call the function provided.

@param aAction
Identifies the function to call.

@param ...
The arguments for the function call. 
*/
	virtual void CallL(TNetProtocolActionType aAction, ...) = 0;

/**
Waits until either
  a function has been called by the Network Gateway on the Protocol Module, 
  the timeout expires

@param aTimeout
The time in microseconds
@param aResponse
Returns the function called in the response
*/
	virtual TNetProtocolResponseType WaitForResponse(TInt aTimeout, TUint aResponseFilterMask) = 0;

/**
Waits asynchronously until either
  a function has been called by the Network Gateway on the Protocol Module, 
  the timeout expires

@param aTimeout
@param aStatus The request status to signal the response to
The time in microseconds
@param aResponse
Returns the function called in the response
*/
	virtual void WaitForResponseL(TInt aTimeout, TRequestStatus& aStatus) = 0;
	
/**
 This function cancels the asynchronous request made for waiting for a response.
 */
	virtual void CancelWait() = 0;

/**
This function retrieves the arguments of the last response function call.
It non-integer arguments on the heap, and pushes them onto the cleanup stack.

@param aResponse
The function call for which we're expecting a response

@param ...
The arguments for the last response function call

@return
The number of arguments it has pushed onto the cleanup stack
*/
	virtual TInt GetArgsLC(TNetProtocolResponseType aResponse, ...) = 0;

	};

/**
Implementation of Network Protocol Proxy test code interface.
 
@internalComponent
@test
*/
class CNetProtocolProxy : public CBase, public MNetProtocolProxy
	{
public:
	IMPORT_C static CNetProtocolProxy* NewL();
	~CNetProtocolProxy();

	void CallL(const TNetProtocolActionType aAction, ...);
	TNetProtocolResponseType WaitForResponse(TInt aTimeout, TUint aResponseFilerMask=0);
	void WaitForResponseL(TInt aTimeout, TRequestStatus& aStatus);
	void CancelWait();
	TInt GetArgsLC(TNetProtocolResponseType aResponse, ...);
	TBool Filtered(TNetProtocolResponseType aResponseType, TUint aResponseFilerMask);

private:
	CNetProtocolProxy();
	void ConstructL();
	
private:
	static void CallGetCurrentCapabilitiesL(CWritePropPipe& aPipe, VA_LIST aArgs);
	static void CallProcessStatusUpdateL   (CWritePropPipe& aPipe, VA_LIST aArgs);
	static void CallProcessPrivacyRequestL (CWritePropPipe& aPipe, VA_LIST aArgs);	
	static void CallProcessLocationRequestL(CWritePropPipe& aPipe, VA_LIST aArgs);
	static void CallProcessSessionCompleteL(CWritePropPipe& aPipe, VA_LIST aArgs);
	static void CallProcessAssistanceDataL (CWritePropPipe& aPipe, VA_LIST aArgs);
	static void CallProcessLocationUpdateL (CWritePropPipe& aPipe, VA_LIST aArgs);
	
	static TInt GetENetMsgRespondPrivacyRequestArgsL	(CReadPropPipe& aPipe, VA_LIST aList);
	static TInt GetENetMsgRespondLocationRequestArgsL	(CReadPropPipe& aPipe, VA_LIST aList);
	static TInt GetENetMsgRequestTransmitLocationArgsL	(CReadPropPipe& aPipe, VA_LIST aList);
	static TInt GetENetMsgCancelTransmitLocationArgsL	(CReadPropPipe& aPipe, VA_LIST aList);
	static TInt GetENetMsgRequestAssistanceDataArgsL	(CReadPropPipe& aPipe, VA_LIST aList);
	static TInt GetENetMsgRequestSelfLocationArgsL		(CReadPropPipe& aPipe, VA_LIST aList);
	static TInt GetENetMsgCancelSelfLocationArgsL		(CReadPropPipe& aPipe, VA_LIST aList);
	static TInt GetENetMsgRequestNetworkLocationArgsL	(CReadPropPipe& aPipe, VA_LIST aList);
	static TInt GetENetMsgCancelNetworkLocationArgsL	(CReadPropPipe& aPipe, VA_LIST aList);
	static TInt GetENetMsgAdviceSystemStatusArgsL		(CReadPropPipe& aPipe, VA_LIST aList);

private:
	TNetProtocolResponseType iLastResponse;
	CWritePropPipe* iActionWriter;
	CReadPropPipe* iResponseReader;
	};

#endif // LBSNETPROTOCOLPROXY_H

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
// Implentation of functions shared between all implementations 
// of the LBS Network Protocol Module interface/ECom plugin.
// 
//

#include <e32base.h>
#include <ecom/ecom.h>

#include <lbs/lbslocerrors.h>

#include <lbs/lbsnetclasstypes.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "lbsnetextintversions.h"
#endif

//=============================================================================
// MLbsNetworkProtocolObserver
//=============================================================================


EXPORT_C TVersion MLbsNetworkProtocolObserver::Version() const
	{
	return TVersion(0, 0, 0);
	}


EXPORT_C void GetCurrentCapabilities
	(
	TLbsNetPosCapabilities& /*aCapabilities*/
	)
	{
	
	}

EXPORT_C void MLbsNetworkProtocolObserver::ProcessStatusUpdate
	(
	TLbsNetProtocolServiceMask /*aActiveServiceMask*/
	)
	{
	}

EXPORT_C void MLbsNetworkProtocolObserver::ProcessPrivacyRequest
	(
	const TLbsNetSessionId& /*aSessionId*/,
	TBool /*aEmergency*/, 
	const TLbsNetPosRequestPrivacy& /*aPrivacy*/,
	const TLbsExternalRequestInfo& /*aRequestInfo*/
	)
	{
	}

EXPORT_C void MLbsNetworkProtocolObserver::ProcessLocationRequest
	(
	const TLbsNetSessionId& /*aSessionId*/,
	TBool /*aEmergency*/,
	TLbsNetProtocolService /*aService*/, 
	const TLbsNetPosRequestQuality& /*aQuality*/,
	const TLbsNetPosRequestMethod& /*aMethod*/
	)
	{
	}


EXPORT_C void MLbsNetworkProtocolObserver::ProcessSessionComplete
	(
	const TLbsNetSessionId& /*aSessionId*/,
	TInt  /*aReason*/
	)
	{
	}

EXPORT_C void MLbsNetworkProtocolObserver::ProcessAssistanceData
	(
	TLbsAsistanceDataGroup /*aDataMask*/,
	const RLbsAssistanceDataBuilderSet& /*aData*/,
	TInt /*aReason*/
	)
	{
	}
	

EXPORT_C void ProcessLocationUpdate
	(
	const TLbsNetSessionId&  /*aSessionId*/,
	const TPositionInfoBase& /*aPosInfo*/
	)
	{
	}

EXPORT_C TAny* MLbsNetworkProtocolObserver::ExtendedInterface
	(
	TInt  /*aFunctionNumber*/,
	TAny* /*aPtr1*/,
	TAny* /*aPtr2*/
	)
	{
	return NULL;
	}


//=============================================================================
// MLbsNetworkProtocolObserver2
//=============================================================================

EXPORT_C TVersion MLbsNetworkProtocolObserver2::Version() const
	{
	return TVersion(2, 0, 0);
	}

/**
Should not be used. Provides for future expansion of the observer interface. 
@released
*/
EXPORT_C TAny* MLbsNetworkProtocolObserver2::ExtendedInterface
	(
	TInt  /*aFunctionNumber*/,
	TAny* /*aPtr1*/,
	TAny* /*aPtr2*/
	)
	{
	return NULL;
	}


/**
A new version of the MLbsNetworkProtocolObserver::ProcessAssistanceData() method. 
The only difference between the original and the new version of the method is 
an extra parameter that provides the LBS subsystem with a list of sessions ids. 
The most common scenario is when the assistance data is related to one session only.
For details see MLbsNetworkProtocolObserver::ProcessAssistanceData().

Currently the functionality of this method is identical to the MLbsNetworkProtocolObserver::ProcessAssistanceData() and the list of session ids
passed through the aSessionIdArray parameter may be empty.

@param aDataMask [In] Is a bitmask that specifies which subsets of GPS assistance data
          are included in the aData parameter.

@param aData [In] A container that holds all of the GPS assistance data to be delivered. 

@param aReason [In] Delivery status for the GPS assistance data being supplied. KErrNone is passed if the
       specified assistance data has been successfully retrieved from the network and delivered.
       Otherwise, an error code is passed indicating the reason why the assistance data references in
       aData could not be obtained. 

@param aSessionIdArray [In] A list of session ids the assistance data is related to.

*/
EXPORT_C void MLbsNetworkProtocolObserver2::ProcessAssistanceData
	(
	TLbsAsistanceDataGroup /*aDataMask*/,
	const RLbsAssistanceDataBuilderSet& /*aData*/,
	TInt /*aReason*/,
	const TLbsNetSessionIdArray& /*aSessionIdArray*/
	)
	{
	}
										
							
/**
It is the original MLbsNetworkProtocolObserver::ProcessAssistanceData() method
and should not be used by protocol modules implementing the extended version of the interface.

See the new, public overload.

@internalTechnology
@released
*/
EXPORT_C void MLbsNetworkProtocolObserver2::ProcessAssistanceData
	(
	TLbsAsistanceDataGroup /*aDataMask*/,
	const RLbsAssistanceDataBuilderSet& /*aData*/,
	TInt /*aReason*/
	)
	{
	}

//=============================================================================
// CLbsNetworkProtocolBase
//=============================================================================

// wraps ECom object instantiation
/**
Creates an instance of the CLbsNetworkProtocolBase implementation with the given Uid.

@publishedPartner
@released

@param aImplementationUid Identifies the protocol module to load.
@param aParams The parameters passed to the protocol module when it is
	instantiated. These include the a reference to the network protocol
	observer. The observer is used by the protocol module to report
	incomming messages and responses to requests made by the LBS subsystem.
	
@see TLbsNetProtocolModuleParams
@see MLbsNetworkProtocolObserver
*/
EXPORT_C CLbsNetworkProtocolBase* CLbsNetworkProtocolBase::NewL
	(
	TUid aImplementationUid,
	TLbsNetProtocolModuleParams& aParams
	)
	{
	TAny* ptr = REComSession::CreateImplementationL(aImplementationUid, 
													_FOFF(CLbsNetworkProtocolBase, iDtor_ID_Key),
													reinterpret_cast<TAny*>(&aParams));

	return reinterpret_cast<CLbsNetworkProtocolBase*>(ptr);
	}  


//-----------------------------------------------------------------------------
// CLbsNetworkProtocolBase - Methods requiring implementation
//-----------------------------------------------------------------------------
/**
Standard destructor that handles the ECom destruction.

The LBS subsystem invokes the destructor on the protocol module
when it wishes to unload the module. The protocol module should
terminate all current requests close all connections.

*/  

EXPORT_C CLbsNetworkProtocolBase::~CLbsNetworkProtocolBase()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}


EXPORT_C void CLbsNetworkProtocolBase::RespondPrivacyRequest
	(
	const TLbsNetSessionId&    /*aSessionId*/,
	const TLbsPrivacyResponse& /*aResponse*/
	)
	{
	}


EXPORT_C void CLbsNetworkProtocolBase::RespondLocationRequest
	(
	const TLbsNetSessionId& /*aSessionId*/,
	TInt  /*aReason*/,
	const TPositionInfoBase& /*aPosInfo*/
	)
	{
	}


EXPORT_C void CLbsNetworkProtocolBase::RequestTransmitLocation
	(
	const TLbsNetSessionId& /*aSessionId*/,
	const TDesC& /*aDestination*/,
	TInt /*aPriority*/
	)
	{
	}



EXPORT_C void CLbsNetworkProtocolBase::CancelTransmitLocation
	(
	const TLbsNetSessionId& /*aSessionId*/,
	TInt  /*aReason*/
	)
	{
	}



EXPORT_C void CLbsNetworkProtocolBase::RequestAssistanceData
	(
	TLbsAsistanceDataGroup /*aDataRequestMask*/
	)
	{
	}

EXPORT_C void CLbsNetworkProtocolBase::RequestNetworkLocation
	(
	const TLbsNetSessionId& /*aSessionId*/,
	const TLbsNetPosRequestOptionsBase& /*aOptions*/
	)
	{
	}



EXPORT_C void CLbsNetworkProtocolBase::CancelNetworkLocation
	(
	const TLbsNetSessionId& /*aSessionId*/,
	TInt /*aReason*/
	)
	{
	}


EXPORT_C void CLbsNetworkProtocolBase::RequestSelfLocation
	(
	const TLbsNetSessionId& /*aSessionId*/,
	const TLbsNetPosRequestOptionsBase& /*aOptions*/
	)
	{
	}



EXPORT_C void CLbsNetworkProtocolBase::CancelSelfLocation
	(
	const TLbsNetSessionId& /*aSessionId*/,
	TInt /*aReason*/
	)
	{
	}

/**
 * This method is used to retrieve pointers to extended interfaces, if
 * supported by derived classes.
 *
 * @param aFunctionNumber The identifier of the extended interface.
 *
 * @param aPtr1 Reserved for future expansion.
 * 
 * @param aPtr2 Reserved for future expansion.
 * 
 */
EXPORT_C TAny* CLbsNetworkProtocolBase::ExtendedInterface
	(
	TInt aFunctionNumber,
	TAny* /*aPtr1*/,
	TAny* /*aPtr2*/
	)
	{
	switch (aFunctionNumber)
		{
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
		case EExtInterfaceCount:
			return reinterpret_cast<TAny*>(EExtInterfaceMax-1);
#else
		case ELbsNetExtInterfaceCount:
			return reinterpret_cast<TAny*>(ELbsNetExtInterfaceMax-1);
#endif
		}
	return NULL;
	}


//=============================================================================
// CLbsNetworkProtocolBase2
//=============================================================================

/**
RequestTransmitLocation() is called by the LBS subsystem to instruct the protocol module to
send the device's position to a remote third-party. 

New code should use the extended version of this method defined below.

@param aSessionId Contains the session identifier. The session identifier is generated by the LBS
       susbsystem and is used in all corresponding and subsequent responses and requests. 

@param aDestination Specifies the remote third-party that is the target for the location
       information transfer. 

@param aPriority A protocol specific value that determines the precedence of the request. A high
       priority may result in the currently active request being stopped. 
@see CancelTransmitRequest() 
@see MLbsNetworkProtocolObserver::ProcessLocationRequest() 
@see MLbsNetworkProtocolObserver::ProcessSessionComplete()

@internalComponent
@released
 
*/
EXPORT_C void CLbsNetworkProtocolBase2::RequestTransmitLocation
	(
	const TLbsNetSessionId& aSessionId,
	const TDesC& aDestination,
	TInt aPriority
	)
	{
#pragma message("NOTE: Update NG, NG unit tests and enable the panic")
	//User::Panic(KNetProtocolApiFault, ENetProtocolApiExtendedInterfaceMisuse);
	TLbsNetPosRequestOptionsTechnology dummyOpt;
	dummyOpt.SetPosMode(TPositionModuleInfo::ETechnologyUnknown);
	RequestTransmitLocation(aSessionId,aDestination,aPriority,dummyOpt);
	}

/**
RequestTransmitLocation() is called by the LBS subsystem to instruct the protocol module to
send the device's position to a remote third-party. 

This method automatically cancels any outstanding transmit request and replaces it with the
new request. 

The parameter aSessionId is generated by the LBS subsystem and is used to connect all
corresponding responses and further requests. The same session ID will be used by the LBS
subsystem itself in subsequent calls to the protocol module via CLbsNetworkProtocolBase. The same 
session ID must also be supplied by the protocol module when it
responds to the LBS subsystem or issues related requests via MLbsNetworkProtocolObserver. 

For example, a request to transmit the location will normally result in the network
generating a location request to determine the terminal's current position. This location
request is passed to the LBS subsystem via
MLbsNetworkProtocolObserver::ProcessLocationRequest() using the session-id as the
initiating RequestTransmitLocation().

Note: If the LBS subsystem does wish to cancel a currently active transmit request and
immediately replace it with a new request it should only use RequestTransmitLocation(). It
should not attempt to explicitly cancel the outstanding request using
CancelTransmitLocation(). Issuing a cancel and then submitting a subsequent transmit
request can lead to inefficiencies at the network protocol level. 

@param aSessionId Contains the session identifier. The session identifier is generated by the LBS
       susbsystem and is used in all corresponding and subsequent responses and requests. 

@param aDestination Specifies the remote third-party that is the target for the location
       information transfer. 

@param aPriority A protocol specific value that determines the precedence of the request. A high
       priority may result in the currently active request being stopped. 

@param aOptions This must be of type TLbsNetPosRequestOptionsTechnology.

@see CancelTransmitRequest() 
@see MLbsNetworkProtocolObserver::ProcessLocationRequest() 
@see MLbsNetworkProtocolObserver::ProcessSessionComplete()

*/
EXPORT_C void CLbsNetworkProtocolBase2::RequestTransmitLocation
	(
	const TLbsNetSessionId& /*aSessionId*/,
	const TDesC& /*aDestination*/,
	TInt /*aPriority*/,
	const TLbsNetPosRequestOptionsBase& /* aOptions */
	)
	{
	}


/**
It is the original CLbsNetworkProtocolBase::RespondPrivacyRequest() method
and is not supported by this version of the interface.

See the new, public overload.

@internalComponent
@released
*/
EXPORT_C void CLbsNetworkProtocolBase2::RespondPrivacyRequest
	(
	const TLbsNetSessionId& aSessionId,
	const TLbsPrivacyResponse& aResponse)
	{
#pragma message("NOTE: Update NG, NG unit tests and enable the panic")
	//User::Panic(KNetProtocolApiFault, ENetProtocolApiExtendedInterfaceMisuse);
	RespondPrivacyRequest(aSessionId, aResponse, KErrNone);
	}


/**
The basic functionality of the RespondPrivacyRequest method is identical to the 
one defined by CLbsNetworkProtocolBase::RespondPrivacyRequest().
The only extension is a response reason code providing additional information 
if a privacy request has been rejected.

@param aSessionId [In] Is the same session identifier was passed by the protocol module when it called
       the MLbsNetworkProtocolObserver::ProcessPrivacyRequest() privacy request 

@param aResponse [In] The response to the privacy request to send to the network. This indicates whether
       the terminal has accepted or rejected the request for its current location. 

@param aReason [In] If a request has been accepted the reason code is always KErrNone.
       In case of a rejection the following are possible. KErrNone if the corresponding request has 
       been rejected by a user; KErrServerBusy if the corresponding request has been rejected because 
       the LBS subsystem is processing another request; KErrPositionHighPriorityReceive if processing
       the corresponding request has been stopped because the LBS subsystem received a request of higher
       priority; KErrArgument if the request contained an unrecognised argument; KErrNotSupported if the 
       requested advice type is not supported by the LBS Subsystem, eg "Stealth Mode"; KErrGeneral
       if the LBS Subsystem has suffered a runtime error.

@see CLbsNetworkProtocolBase::RespondPrivacyRequest()
*/
EXPORT_C void CLbsNetworkProtocolBase2::RespondPrivacyRequest
	(
	const TLbsNetSessionId& /*aSessionId*/,
	const TLbsPrivacyResponse& /*aResponse*/,
	TInt /*aReason*/)
	{
	}

/**
CancelExternalLocation() is called by the LBS subsystem to instruct the Protocol Module to
stop an ongoing MT-LR session.

Normally after receiving the cancel request the Protocol Module should complete the session 
by calling MLbsNetworkProtocolObserver::ProcessSessionComplete() with KErrCancel 
reason code, however if a cancel request is too late (e.g. a position has already been calculated 
and sent to the network), MLbsNetworkProtocolObserver::ProcessSessionComplete() may be called 
with the KErrNone reason code indicating the success of the session. 

It is also acceptable that the Protocol Module returns some extra information, such as a reference 
location or assistance data before completing the session.

@param aSessionId [In] Is the same session identifier that was passed by the protocol module when it called
       MLbsNetworkProtocolObserver::ProcessPrivacyRequest() to start external positioning.

@param aReason [In] KErrCancel if the session has been cancelled explicitly by a user or by an application 
       acting on behalf of a user. KErrPositionHighPriorityReceive if the session has been cancelled due to a 
       conflict with a session of higher priority. 

@see MLbsNetworkProtocolObserver::ProcessPrivacyRequest()
@see CLbsNetworkProtocolBase::RespondLocationRequest()

*/
EXPORT_C void CLbsNetworkProtocolBase2::CancelExternalLocation
	(
	const TLbsNetSessionId& /*aSessionId*/,
	TInt /*aReason*/
	)
	{
	}


/**
It is the original CLbsNetworkProtocolBase::RequestAssistanceData() method
and is not supported by this version of the interface.

See the new, public overload.

@internalComponent
@released
*/
EXPORT_C void CLbsNetworkProtocolBase2::RequestAssistanceData
	(
	TLbsAsistanceDataGroup aDataRequestMask
	)
	{
#pragma message("NOTE: Update NG, NG unit tests and enable the panic")
	//User::Panic(KNetProtocolApiFault, ENetProtocolApiExtendedInterfaceMisuse);
	const TLbsNetSessionIdArray dummySessionIdArray;
	RequestAssistanceData(aDataRequestMask, dummySessionIdArray);
	}


/**
A new, extended version of the CLbsNetworkProtocolBase::RequestAssistanceData() method. 
The only difference between the original and the new version of the method is 
an extra parameter that provides a Protocol Module with a list of sessions the request 
is related to.

Currently the functionality of this method is identical to the 
CLbsNetworkProtocolBase::RequestAssistanceData() and the aSessionIdArray parameter should 
be ignored.

In the future the list of session ids may be used to allow a Protocol Module supporting 
concurrent positioning sessions to avoid requesting assistance data from remote peers 
(e.g. SUPL servers) that do not have an outstanding location request 
(CLbsNetworkProtocolBase::ProcessLocationRequest() has not been invoked yet) or the 
LBS subsystem has not started processing the request yet.

Please note that the LBS subsystem combines all concurrent location requests, therefore the required set 
of assistance data is not session specific. This means that a remote peer 
may be asked for more assistance data items than it actually supports.

@param aDataRequestMask [In] Is a bitmask that specifies which subsets of GPS assistance data are
	      required. If aDataRequestMask is set to zero, this indicates that no further assistance
	      data is needed. 

@param aSessionIdArray [In] A list of session ids the assistance data is related to. The parameter is 
          currently not supported and should be ignored.

@see CLbsNetworkProtocolBase::RequestAssistanceData()
*/
EXPORT_C void CLbsNetworkProtocolBase2::RequestAssistanceData
	(
	TLbsAsistanceDataGroup /*aDataRequestMask*/,
	const TLbsNetSessionIdArray& /*aSessionIdArray*/
	)
	{
	}


/**
 * This method is used to retrieve pointers to extended interfaces, if
 * supported by derived classes.
 *
 * @param aFunctionNumber The identifier of the extended interface.
 *
 * @param aPtr1 Reserved for future expansion.
 * 
 * @param aPtr2 Reserved for future expansion.
 * 
 * @released
 * @internalTechnology
 */
EXPORT_C TAny* CLbsNetworkProtocolBase2::ExtendedInterface
	(
	TInt aFunctionNumber,
	TAny* aPtr1,
	TAny* aPtr2
	)
	{
	switch (aFunctionNumber)
		{
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
		case EExtInterface2:
#else
		case ELbsNetExtInterface2:
#endif
			return reinterpret_cast<TAny*>(this);
		}
	return CLbsNetworkProtocolBase::ExtendedInterface(aFunctionNumber,aPtr1,aPtr2);
	}


//=============================================================================
// TLbsNetProtocolModuleParams
//=============================================================================

EXPORT_C TLbsNetProtocolModuleParams::TLbsNetProtocolModuleParams
	(
	MLbsNetworkProtocolObserver& aObserver
	)
	:
	iObserver(aObserver)
	{
	iClassType = ELbsNetProtocolModuleParamsClass;
	iClassSize = sizeof(TLbsNetProtocolModuleParams);
	}
	
	

EXPORT_C MLbsNetworkProtocolObserver& TLbsNetProtocolModuleParams::Observer() const
	{
	return iObserver;
	}


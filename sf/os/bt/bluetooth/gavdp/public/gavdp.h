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
// forms the basis of the control plane for (Bluetooth) AV apps.
// 
//

#ifndef _GAVDP_H
#define _GAVDP_H

#include <bluetoothav.h>

class MGavdpUser
/**
Mixin class for GAVDP-related events.

To enforce thinking about the sequence of GAVDP events, these are pure
@publishedPartner
@released
*/
	{
public:
/**
Callback when a signalling channel has successfully been created
Note this can be called by GAVDP without the client having called RGavdp::Connect
The reason being that as soon as a GAVDP session is created AVDTP begins listening
Therefore a remote device can connect to us.
The GAVDP client can check the address to confirm if the device is that they originally Connect()ed
@param aDeviceAddr the Bluetooth device address for which a signalling channel has been created
*/
	virtual void GAVDP_ConnectConfirm(const TBTDevAddr& aDeviceAddr) =0;


/**
Callback when a remote SEP has been discovered
@param aSEP the details of the remote SEP
@see TAvdtpSEPInfo
*/
	virtual void GAVDP_SEPDiscovered(const TAvdtpSEPInfo& aSEP) =0;


/**
Callback when no more SEPs have been discovered on the remote device.
This may be called without GAVDP_SEPDiscovered being called if the remote has no SEPs
(such a remote would be in contravention of the specification however)
*/
	virtual void GAVDP_SEPDiscoveryComplete() =0;	


/**
Callback when a capability from a remote SEP has been retrieved
The client takes owenership of the capability given
@param aCapability pointer to a capability. Ownership is transferred to the MGavdpUser implementor
@see TAvdtpServiceCapability
*/
	virtual void GAVDP_SEPCapability(TAvdtpServiceCapability* aCapability) =0; // transfers ownership


/**
Callback when remote SEP has no more capabilities to be discovered
*/
	virtual void GAVDP_SEPCapabilityComplete() =0;	

/**
Callback when the locally-initiated Abort stream has successfully completed
*/	
	virtual void GAVDP_AbortStreamConfirm()=0;

/**
Callback when the locally-initiated Start stream has successfully completed
*/	
	virtual void GAVDP_StartStreamsConfirm()=0;

/**
Callback when the locally-initiated Suspend stream has successfully completed
*/	
	virtual void GAVDP_SuspendStreamsConfirm()=0;

/**
Callback when the locally-initiated Security Control message successfully completed
@param aResponseData The data returned by the ACP - may be the same or different to that supplied by INT
*/	
	virtual void GAVDP_SecurityControlConfirm(const TDesC8& aResponseData)=0;

/**
Callback when locally-initiated (re)configuration procedure successfully completes
*/	
	virtual void GAVDP_ConfigurationConfirm()=0;	// configuration complete and SEP selected *AND* reconfigure confirm
	
	
/**
Callback when remote is beginning to configure a local SEP
This cannot be rejected by the client at this stage - it must examine each
capability proposed by the GAVDP peer.
@param aLocalSEID the SEID of the local SEP being configured
@param aRemoteSEID the SEID of the remote SEP configuring us
*/
	virtual void GAVDP_ConfigurationStartIndication(TSEID aLocalSEID, TSEID aRemoteSEID) =0;
	
	
/**
Callback when each capability is being proposed by the peer during configuration
Each capability can be refused, but doing so completes whole configure operation
@return the client's result - eg whether it is accepting or rejecting the configuration
@param aCapability The capability being proposed. Ownership transferred to client,iff processes in the function return KErrNone.
*/
	virtual TInt GAVDP_ConfigurationIndication(TAvdtpServiceCapability* aCapability) =0;

/**
Callback when remotely initiated configuration of local SEP is completed	
The client can still error, as the local side may have expected to see more capabilities proposed
@return The client's acceptance or rejection of the whole configuration
*/	
	virtual TInt GAVDP_ConfigurationEndIndication() =0;

/**
Callback when the remote is trying to Start a stream
@param aSEID the SEID of the local SEP being started
@return The client's acceptance or rejection of the stream start. The return value should reflect the outcome
of the client's ability to handle this command.

The allowable error codes are defined in the Bluetooth SIG AVDTP specification and profiles design to operate
above that layer, such as GAVDP. However those "local" error code values should be offset by the Symbian
KErrAvdtpSignallingErrorBase (-18045) to convert to system-wide, absolute error codes.

For example; the error code for NOT_SUPPORTED_COMMAND = 25
Therefore, the system code would be (-18045)-25 = -18070
*/
	virtual TInt GAVDP_StartIndication(TSEID aSEID) =0;


/**
Callback when the remote is trying to Suspend a stream
@param aSEID the SEID of the local SEP being suspended
@return The client's acceptance or rejection of the suspension. The return value should reflect the outcome
of the client's ability to handle this command.

The allowable error codes are defined in the Bluetooth SIG AVDTP specification and profiles design to operate
above that layer, such as GAVDP. However those "local" error code values should be offset by the Symbian
KErrAvdtpSignallingErrorBase (-18045) to convert to system-wide, absolute error codes.

For example; the error code for NOT_SUPPORTED_COMMAND = 25
Therefore, the system code would be (-18045)-25 = -18070
*/
	virtual TInt GAVDP_SuspendIndication(TSEID aSEID) =0;


/**
Callback when the remote is sending Security Control information
@param aSEID the SEID of the local SEP for which security control is for
@param aSecurityDataInOut Security Data inbound. The client may change this descriptor for the outbound response
@return The client's acceptance or rejection of the security control indication.
*/
	virtual TInt GAVDP_SecurityControlIndication(TSEID aSEID, TDes8& aSecurityDataInOut) =0;

/**
Callback when the remote has Aborted the stream
@param aSEID the SEID of the local SEP which has been Aborted
*/
	virtual void GAVDP_AbortIndication(TSEID aSEID) =0;
	
/**
Callback when the remote end has Released a stream
The user-plane bearers will subsequently error with KErrDisconnected.
@param aSEID the SEID of the local SEP which was been released
*/
	virtual void GAVDP_ReleaseIndication(TSEID aSEID) =0;


/**
Callback when a bearer has been created for a session in the stream.
The session the bearer is for can be examined from the socket address given in the callback
Each socket created in this way is created on the client's socket server session,
and responsibility for socket closure is ceded to the client.
@param aSocket the socket created on the client's socket server session.
@param aAddress the socket address (containing SEID, Session type)
@see TAvdtpSockAddr
*/
	virtual void GAVDP_BearerReady(RSocket aSocket, const TAvdtpSockAddr& aAddress)=0;


/**
Callback when a GAVDP error occurs
@param aError system-wide or GAVDP or AVDTP error code, for example 
			  KErrDisconnected if the signalling channel is disconnected
			  KErrNotReady if GAVDP or AVDTP is not in a valid state to perform an action
			  KErrUnknown if a stream is not currently known to AVDTP.  This is likely to occur 
						  in the case where a user has closed their bearer socket(s) and GAVDP
						  tries to perform an operation requiring a stream.  
			  KErrDied if an unrecoverable internal error occurs (in which case clients should re-start GAVDP)
			  Clients should note that for system wide error codes the reasons given are for
			  errors generated by AVDTP.  There is a small chance that they can be passed up in 
			  another situation if the error originates from a call on another part of the system.
@param aErrorData extra data relating to the error - the client can deduce the nature based on their GAVDP state and the spec
@see TAvdtpConfigRejectPckgBuf as an example of what may be passed back
*/
	virtual void GAVDP_Error(TInt aError, const TDesC8& aErrorData) =0;
	};
	
class CGavdp;

NONSHARABLE_CLASS(RGavdp)
/**
Represents a single GAVDP session to a remote device.
If an app wishes to (eg) listen after connecting a GAVDP link it should 
instantiate another RGavdp.

Note this class uses internal active objects to provide the underlying 
services. Clients should be careful to allow these objects to run.

One RGavdp represents one remote device.
If an inbound connection is made then a new RGavdp must be instantiated to 
form a stream to another device.
Once RGavdp instance may be used to manage more than 1 local SEP. This is 
useful if the client only ever intends to support one active stream, but 
wishes to select from >1 codec.
Alternatively >1 instance of RGavdp may be created, with a desire to manage 
each local SEP with each RGavdp instance. This would allow >1 active stream.

@publishedPartner
@released
*/
	{
public:
	IMPORT_C RGavdp();

	IMPORT_C TInt Open(MGavdpUser& aServiceUser, RSocketServ& aSocketServer);
	IMPORT_C TInt Listen();

	IMPORT_C void Close();
	IMPORT_C void Cancel();
	
	IMPORT_C void Connect(const TBTDevAddr& aRemoteAddr);
	IMPORT_C void DiscoverRemoteSEPs();
	IMPORT_C void GetRemoteSEPCapabilities(TSEID aSEID, const TAvdtpServiceCategories& aInterestingCategories);

	IMPORT_C TInt RegisterSEP(TAvdtpSEPInfo& aInfo);

	IMPORT_C TInt BeginConfiguringRemoteSEP(TSEID aRemoteSEID, TSEID aLocalSEID);
	IMPORT_C TInt BeginConfiguringLocalSEP(TSEID aSEID);
	IMPORT_C TInt AddSEPCapability(const TAvdtpServiceCapability& aCapability);
	IMPORT_C void CommitSEPConfiguration();
	
	IMPORT_C void StartStream(TSEID aSEID);
	IMPORT_C void SuspendStream(TSEID aSEID);
	IMPORT_C void AbortStream(TSEID aSEID);
	
	IMPORT_C void SendSecurityControl(TSEID aSEID, const TDesC8& aSecurityData);
	
	IMPORT_C TInt CreateBearerSockets(TSEID aSEID, TBool aIgnored1, TBool aIgnored2);
	IMPORT_C TInt MaxSecurityControlLength();

	IMPORT_C TInt CreateBearerSockets(TSEID aSEID);
	IMPORT_C TInt Shutdown();
public:
	void UnbindBody();
	
private:
	CGavdp*			iGavdpImp;
	};

/**
Panics = client doing something wrong
**/

enum TGavdpPanic
	{
	EGavdpBadState = 0,
	EGavdpResponseDataExpected = 1,
	EGavdpSEPMustBeRegisteredBeforeListen = 2,
	EGavdpSEPMustBeRegisteredBeforeConnect = 3,
	EGavdpAlreadyOpen = 4,
	EGavdpNotOpen = 5,
	EGavdpUserSuppliedBadSEID = 6,
	EGavdpBadRemoteAddress = 7,
	EGavdpClientHasRepliedInCorrectly = 8,
	/** @deprecated Should use EGavdpShutdownNotPermittedAfterRemoteSEPConfigured instead */
	EGavdpShutdownNotPermittedWhenStreaming = 9,
	EGavdpShutdownNotPermittedAfterRemoteSEPConfigured = 9,
	/**Reference value used to determine if error value is below max panic value */
	EGavdpMaxPanicErr = -940,
	/**Panic used when the user attempts to create a bearer whilst one is already open */
	EGavdpOpenInBadState = -999,
	/**Reference value used to determine if error value exceeds min panic value */
	EGavdpMinPanicErr = -999,
	};
		
#endif //_GAVDP_H

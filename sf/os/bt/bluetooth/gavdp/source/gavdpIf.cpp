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

#include <gavdp.h>
#include "gavdpInternal.h"

/**
Default Constructor
*/
EXPORT_C RGavdp::RGavdp() 
: iGavdpImp(NULL)
	{
	}

/**
Open a GAVDP session.
@note not to be confused with the "Open" primitive in the GAVDP spec. This is 
merely opening the handle in the Symbian OS sense.
@param aServiceUser a reference to a MGavdpUser mixin for GAVDP callback 
events.
@param aSocketServer reference to the client's Socket Server session. 
@return system-wide error
@see RegisterSEP
*/
EXPORT_C TInt RGavdp::Open(MGavdpUser& aServiceUser, RSocketServ& aSocketServer)
	{
	__ASSERT_ALWAYS(iGavdpImp==NULL,Panic(EGavdpAlreadyOpen));
	
	TRAPD(res, iGavdpImp = CGavdp::NewL(aServiceUser, aSocketServer, *this));
	return res;
	}
	
/**
Closes the GAVDP session and frees all resources associated with it.
This function should not be called from within a client's implementation of 
any MGavdpUser (upcall) method.
@note not to be confused with the "Close" primitive in the GAVDP spec. This is 
merely closing the handle in the Symbian OS sense.
@note However, if this is Closed when streams are active, the Bluetooth 
subsystem shall Release all Streams managed by this RGavdp instance.
@note that the closure of any bearer sockets created remains the 
responsibility of the client.
*/
EXPORT_C void RGavdp::Close()
	{
	delete iGavdpImp;
	iGavdpImp = NULL;
	}
	
/**
Cancel the outstanding client request.
As per usual Symbian OS Cancel semantics, this method does not guarantee that 
the outstanding request was "undone".
Therefore to reset each sides' state the client should consider sending an 
Abort too.
@see RGavdp::AbortStream
*/
EXPORT_C void RGavdp::Cancel()
	{
	if(iGavdpImp!=NULL)
		{
		iGavdpImp->Cancel();	
		}
	}

/**
Connects a GAVDP session - i.e. creates an AVDTP signalling channel to a 
remote device.
The signalling channel may already exist, in which case this session is added 
as a user of that channel.
The necessary multiplexing is implemented in AVDTP, so the client need not be 
concerned with this.
@param aRemoteAddr the Bluetooth device address of the remote GAVDP entity.
*/
EXPORT_C void RGavdp::Connect(const TBTDevAddr& aRemoteAddr)
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	iGavdpImp->Connect(aRemoteAddr);
	}
	
/**
Discover the Stream endpoints (SEPs) on the remote device.
Callbacks on MGavdpUser will be called as a result of this call.
@pre a Signalling channel must have been successfully created
@see MGavdpUser::GAVDP_SEPDiscovered
*/
EXPORT_C void RGavdp::DiscoverRemoteSEPs()
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	iGavdpImp->DiscoverRemoteSEPs();
	}
	
/**
Get the capabilities of a remote Stream Endpoint (SEP).
Callbacks on MGavdpUser will be called for the intersection of capabilities 
that the remote supports and that the local GAVDP user is interested in using. 
@param aSEID the ID of the remote SEP for which capabilities are to be 
retrieved.
@param aInterestingCategories the categories the local GAVDP client is 
interested in.
*/
EXPORT_C void RGavdp::GetRemoteSEPCapabilities(TSEID aSEID, const TAvdtpServiceCategories& aInterestingCategories)
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	iGavdpImp->GetRemoteSEPCapabilities(aSEID, aInterestingCategories);
	}
	
/**
Begin configuring (or reconfiguring) a remote SEP.
Subsequent calls are used to add each configuration required.
@see RGavdp::AddSEPCapability
@param aRemoteSEID the ID of the remote SEP.
@param aLocalSEID the ID of the local SEP.
@return system-wide error.
*/	
EXPORT_C TInt RGavdp::BeginConfiguringRemoteSEP(TSEID aRemoteSEID, TSEID aLocalSEID)
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	return iGavdpImp->BeginConfiguringRemoteSEP(aRemoteSEID, aLocalSEID);
	}
	
/**
Begin configuring (or reconfiguring) a local SEP.
Subsequent calls are used to add each configuration required.
@see RGavdp::AddSEPCapability
@param aSEID the ID of the local SEP.
@return system-wide error.
*/	
EXPORT_C TInt RGavdp::BeginConfiguringLocalSEP(TSEID aSEID)
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	return iGavdpImp->BeginConfiguringLocalSEP(aSEID);
	}
	
/**
Add a particular capability into a SEP.
When called on a local SEP this equates to adding capabilities into the SEP.
When called on a remote SEP this equates to configuring the SEP.
@param aCapability a capability
@see TAvdtpServiceCapability
@return system-wide error.
*/	
EXPORT_C TInt RGavdp::AddSEPCapability(const TAvdtpServiceCapability& aCapability)
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	return iGavdpImp->AddSEPCapability(aCapability);
	}
	
/**
Finalises the (re)configuration of the SEP
*/
EXPORT_C void RGavdp::CommitSEPConfiguration()
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	iGavdpImp->CommitSEPConfiguration();
	}
	
/**
Start stream
@param aSEID the SEID of the local or remote SEP that requires starting
*/
EXPORT_C void RGavdp::StartStream(TSEID aSEID)
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	iGavdpImp->StartStream(aSEID);
	}
	
/**
Suspend stream
@note that the remote may not support Suspend, and an 
AVDTP NOT_SUPPORTED_COMMAND error may result.
In this case the client should consider Closing the stream.
@param aSEID the SEID of the local or remote SEP that requires suspending.
*/
EXPORT_C void RGavdp::SuspendStream(TSEID aSEID)
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	iGavdpImp->SuspendStream(aSEID);
	}
	
/**
Abort stream
@note that once complete the state of the SEP will be Idle. Thus to restream, 
the SEP has to be reconfigured, opened and started.
@param aSEID the SEID of the local or remote SEP that requires aborting.
*/	
EXPORT_C void RGavdp::AbortStream(TSEID aSEID)
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	iGavdpImp->AbortStream(aSEID);
	}
	
/**
Send security control data to remote SEP.
The meaning of this is dependent on the configured Content Protection method. 
@see TAvdtpContentProtectionCapabilities
@param aSEID the seid of the remote SEP
@param aSecurityData the security data
*/
EXPORT_C void RGavdp::SendSecurityControl(TSEID aSEID, const TDesC8& aSecurityData)
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	iGavdpImp->SendSecurityControl(aSEID, aSecurityData);	
	}

/**
Register an extra local SEP.
@param aInfo reference containing SEP information. Note the SEID assigned by 
AVDTP will be written back into aInfo.
@return system-wide error
*/
EXPORT_C TInt RGavdp::RegisterSEP(TAvdtpSEPInfo& aInfo)
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	return iGavdpImp->RegisterSEP(aInfo);
	}
	
/**
Listen for inbound signalling channel connections.
This is useful when the signalling channel is lost.
Note: This function will not connect the session to any existing connected 
signalling channels.
The session will only be connected upon the next channel being connected. 
Users should use Connect instead if they intend to connect to existing 
signalling channels.
@pre At least SEP shall have been registered
@see RGavdp::RegisterSEP(TAvdtpSEPInfo& aInfo)
@panic The client thread will be panicked if Listen is called prior to any 
SEPs being registered.
@return system-wide error code
*/
EXPORT_C TInt RGavdp::Listen()
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	return iGavdpImp->Listen();
	}

/**
Creates bearers (in the form of sockets) for sessions on the stream.
The sessions are media + optional reporting + optional recovery.
The sockets will be created on the client's Socket Server session.
The sockets may then be passed into RTP for its use.
If the sockets are passed to RTP the client still has the responsibility of 
closing them.
 - i.e. neither RTP nor GAVDP will close the sockets.
The reporting/recovery config is already known by the stream. All the required
bearers will be created.

@param aSEID The SEID of the remote SEP for which the sockets are required.
@param aIgnored1 Source compatibility place holder
@param aIgnored2 Source compatibility place holder
@return system-wide error code

@see RGavdp::CreateBearerSockets(TSEID aSEID)
@deprecated
*/
EXPORT_C TInt RGavdp::CreateBearerSockets(TSEID aSEID, TBool /*aIgnored1*/, TBool /*aIgnored2*/)
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	return iGavdpImp->CreateBearerSockets(aSEID);
	}

/**
Creates bearers (in the form of sockets) for sessions on the stream.
The sessions are media + optional reporting + optional recovery.
The sockets will be created on the client's Socket Server session.
The sockets may then be passed into RTP for its use.
If the sockets are passed to RTP the client still has the responsibility of 
closing them.
 - i.e. neither RTP nor GAVDP will close the sockets.
The reporting/recovery config is already known by the stream. All the required
bearers will be created. 

@param aSEID The SEID of the remote SEP for which the sockets are required.
@return system-wide error code
*/
EXPORT_C TInt RGavdp::CreateBearerSockets(TSEID aSEID)
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	return iGavdpImp->CreateBearerSockets(aSEID);
	}

/**
This function returns the deprecated value KMaxAvdtpSecurityControlInfo so as 
to ensure that the return value is what partners will expect during the 
transition period from the use of the old constant to this new API.
When the constant is removed from BluetoothAV.h it should be moved to an 
internal header.
@return KMaxAvdtpSecurityControlInfo
*/
EXPORT_C TInt RGavdp::MaxSecurityControlLength()
	{
	return KMaxAvdtpSecurityControlInfo;
	}

/**
Disconnects the RGavdp session from the signalling channel, but does not destroy the local SEPs owned
by the signalling channel. Must only be called if the remote SEP has not yet been configured -
if the remote SEP has been configured then AbortStream() should be called.

@return system-wide error code
@panic If remote SEP configured
*/

EXPORT_C TInt RGavdp::Shutdown()
	{
	__ASSERT_ALWAYS(iGavdpImp!=NULL,Panic(EGavdpNotOpen));
	TInt ret = iGavdpImp->Shutdown();
	if (ret==KErrNotReady) 
		{
		Panic(EGavdpShutdownNotPermittedAfterRemoteSEPConfigured);
		}
	return ret;
	}
	
void RGavdp::UnbindBody()
	{
	iGavdpImp = NULL;
	}

	
// EOF

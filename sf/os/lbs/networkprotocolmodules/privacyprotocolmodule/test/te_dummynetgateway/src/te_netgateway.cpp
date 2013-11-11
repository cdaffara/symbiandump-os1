// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// netgateway.cpp
// Client API to the LBS Network Simulator for 
// the (test) LBS protocol module.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include <e32base.h>
#include <e32debug.h>
#include <ecom/ecom.h>
#include <lbs/lbsadmin.h>
#include "lbsdevloggermacros.h"
#include "csecureprocessasbase.h"
#include <lbs/lbsnetcommon.h>
#include "te_netgateway.h"


// moved from the netgatewaymain.cpp file to maintain neutrality of that file.
CBase* CSecureProcessASBase::CreateRootObjectL()
	{	
	CNetworkGateway* server = CNetworkGateway::NewL();	
	return server;
	}

TInt CSecureProcessASBase::EntryPointL(CBase* /*aBaseObj*/)
	{
	// EntryPointL not used by CNetGateway
	return KErrNone;
	}

void CSecureProcessASBase::Final()
	{
	}
	
		
//
// CNetworkGateway
//

/**
*/
CNetworkGateway* CNetworkGateway::NewL()
	{
	CNetworkGateway* self = new (ELeave) CNetworkGateway;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

/**
*/
CNetworkGateway::CNetworkGateway() 
	{
	}

/**
*/	
CNetworkGateway::~CNetworkGateway()
	{
	delete iNetworkProtocol;
	delete iNetworkRequestChannel;
	delete iCloseDownRequestDetector;
		
	REComSession::FinalClose();
	}

/**
*/	
void CNetworkGateway::ConstructL()
	{
	RProcess process;
	iCloseDownRequestDetector = CLbsCloseDownRequestDetector::NewL(this, process.SecureId());	
	iNetworkRequestChannel = CNetworkRequestChannel::NewL(*this);
	
	LoadProtocolModuleL();
	}

/** Select and load a Network Protocol Module.

There can only ever be one protocol module loaded at any time.
The protocol module is loaded during the Network Gateway initialisation
and can't be changed once it is running.
*/
void CNetworkGateway::LoadProtocolModuleL()
	{
	//TLbsProtocolModuleId moduleId;
	// Load the selected implementation
	TLbsNetProtocolModuleParams params(*this);
	iNetworkProtocol = CLbsNetworkProtocolBase::NewL(TUid::Uid(0X1028373C), params);
	}

//---------------------------------------------------------------------------------------------------
// Messages from a Protocol Module (MLbsNetworkProtocolObserver)
//---------------------------------------------------------------------------------------------------
void CNetworkGateway::GetCurrentCapabilities(TLbsNetPosCapabilities& /*aCapabilities*/) const
	{
	LBSLOG(ELogP2, "CNetworkGateway::GetCurrentCapabilities:");
	
	// TODO: Fill in the position method(s) for aCapabilities.
	// For now, assume that the protocol(s) information should 
	// already be filled in by the protocol module.
	}

void CNetworkGateway::ProcessStatusUpdate(TLbsNetProtocolServiceMask /*aActiveServiceMask*/)
	{
	LBSLOG(ELogP2, "CNetworkGateway::ProcessStatusUpdate:");
	//LBSLOG2(ELogP2, "Active Service Mask : 0x%x", aActiveServiceMask);
	
	// TODO: It is TBD exactly what/where this update should go.
	// For now, just consume it.
	}

void CNetworkGateway::ProcessPrivacyRequest(const TLbsNetSessionId& aSessionId,
										    TBool aEmergency, 
										    const TLbsNetPosRequestPrivacy& aPrivacy,
										    const TLbsExternalRequestInfo& aRequestInfo)
	{
	LBSLOG(ELogP2, "CNetworkGateway::ProcessPrivacyRequest:");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());
	LBSLOG2(ELogP2, "Emergency : %s", (aEmergency ? "ETrue" : "EFalse"));
	LBSLOG3(ELogP2, "Privacy : (%d, %d)", aPrivacy.RequestAdvice(),
										  aPrivacy.RequestAction());

	// Convert aPrivacy to TLbsNetPosRequestPrivacyInt.
	TLbsNetPosRequestPrivacyInt privacyInt;
	privacyInt.SetRequestAdvice((TLbsNetPosRequestPrivacyInt::TLbsRequestAdviceInt)
			aPrivacy.RequestAdvice());
	privacyInt.SetRequestAction((TLbsNetPosRequestPrivacyInt::TLbsRequestActionInt)
			aPrivacy.RequestAction());
										  
	TLbsNetMtLrRequestMsg msg(
			TLbsNetSessionIdInt(aSessionId.SessionOwner(), aSessionId.SessionNum()),
			aEmergency, privacyInt, aRequestInfo);
	iNetworkRequestChannel->SendNetRequestMessage(msg);
	
	}

void CNetworkGateway::ProcessLocationRequest(const TLbsNetSessionId& /* aSessionId */,
											 TBool /*aEmergency*/,
											 TLbsNetProtocolService /*aService*/, 
											 const TLbsNetPosRequestQuality& /*aQuality*/,
											 const TLbsNetPosRequestMethod& /*aMethod*/)
	{
	LBSLOG(ELogP2, "CNetworkGateway::ProcessLocationRequest:");
	}

void CNetworkGateway::ProcessSessionComplete(const TLbsNetSessionId& aSessionId,
											 TInt  aReason)
	{
	LBSLOG(ELogP2, "CNetworkGateway::ProcessSessionComplete:");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	LBSLOG2(ELogP2, "Reason : %d", aReason);
	
	// The NRH gets all session complete messages and may ignore
	// them if the Session Owner is NLM or AGPS manager, but
	// no Location Request processing is taking place.
	TLbsNetSessionCompleteMsg msgNRH(
		TLbsNetSessionIdInt(aSessionId.SessionOwner(), aSessionId.SessionNum()),
		aReason);
	iNetworkRequestChannel->SendNetRequestMessage(msgNRH);
	}

void CNetworkGateway::ProcessAssistanceData(TLbsAsistanceDataGroup /* aDataMask */,
											const RLbsAssistanceDataBuilderSet& /*aData*/,
						   					TInt /*aReason */)
	{
	LBSLOG(ELogP2, "CNetworkGateway::ProcessAssistanceData:");
	}

void CNetworkGateway::ProcessLocationUpdate(const TLbsNetSessionId&  /*aSessionId*/,
											 const TPositionInfoBase& /*aPosInfo*/)
	{
	LBSLOG(ELogP2, "CNetworkGateway::ProcessNetworkBasedLocation:");
	}

//---------------------------------------------------------------------------------------------------
// Messages from the Network Request Handler (MNetworkRequestObserver)
//---------------------------------------------------------------------------------------------------
/** Process incoming messages from the Network Request Handler.

This function is called for each message that is received from
the NRH. In most cases all it has to do is pass on the message 
contents by calling the appropriate function on the Network 
Protocol Module.
*/	
void CNetworkGateway::ProcessNetRequestMessage(const TLbsNetInternalMsgBase& aMessage)
	{
	LBSLOG(ELogP2, "CNetworkGateway::ProcessNetRequestMessage:");
	LBSLOG2(ELogP2, "Type : %d", aMessage.Type());
		
	switch (aMessage.Type())
		{
	   	case TLbsNetInternalMsgBase::EPrivacyResponse:
			{
			const TLbsNetMtLrReponseMsg& msg = reinterpret_cast<const TLbsNetMtLrReponseMsg&>(aMessage);
			iNetworkProtocol->RespondPrivacyRequest(
				TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()),
				(const CLbsNetworkProtocolBase::TLbsPrivacyResponse&) msg.Response());
					    
			break;
			}
		default:
			{
			break;
			}
		}
	}

//---------------------------------------------------------------------------------------------------
// Other, private.
//---------------------------------------------------------------------------------------------------

/* Observer that is called when LbsRoot has requested the Network Gateway to shutdown.
*/
void CNetworkGateway::OnProcessCloseDown()
	{
	// For now, simply close down as soon as possible.
	// In future the close down may need to be asynchronous,
	// to allow the buffers in CAgpsChannel and CNetRequestChannel
	// to be processed normally.
	iNetworkRequestChannel->Cancel();
	
	CActiveScheduler::Stop();
	}


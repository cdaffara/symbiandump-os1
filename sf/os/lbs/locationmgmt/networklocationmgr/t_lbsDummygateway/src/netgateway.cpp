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
#include "netgateway.h"
#include "lbsngnlmint.h"


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
CNetworkGateway::CNetworkGateway() :
	iCurrentNetLocReqId(KInvalidNetSessionId),
	iNetLocReqLocReqReceived(EFalse)
	{
	}

/**
*/	
CNetworkGateway::~CNetworkGateway()
	{
	delete iNetworkLocationChannel;
	
	delete iCloseDownRequestDetector;
	
	REComSession::FinalClose();
	}

/**
*/	
void CNetworkGateway::ConstructL()
	{
	iCloseDownRequestDetector = CLbsCloseDownRequestDetector::NewL(this, KLbsNetLocManagerUid);	
	iNetworkLocationChannel = CNetworkLocationChannel::NewL(*this);
	}


void CNetworkGateway::GetCurrentCapabilities(TLbsNetPosCapabilities& /*aCapabilities*/) const
	{
	}

void CNetworkGateway::ProcessStatusUpdate(TLbsNetProtocolServiceMask /*aActiveServiceMask*/)
	{
	}

void CNetworkGateway::ProcessPrivacyRequest(const TLbsNetSessionId& /*aSessionId*/,
										    TBool /*aEmergency*/, 
										    const TLbsNetPosRequestPrivacy& /*aPrivacy*/,
										    const TLbsExternalRequestInfo& /*aRequestInfo*/)
	{
	}

void CNetworkGateway::ProcessLocationRequest(const TLbsNetSessionId& /*aSessionId*/,
											 TBool /*aEmergency*/,
											 TLbsNetProtocolService /*aService*/, 
											 const TLbsNetPosRequestQuality& /*aQuality*/,
											 const TLbsNetPosRequestMethod& /*aMethod*/)
	{
	}

void CNetworkGateway::ProcessSessionComplete(const TLbsNetSessionId& /*aSessionId*/,
											 TInt  /*aReason*/)
	{
	}
	
	
void CNetworkGateway::ProcessAssistanceData(TLbsAsistanceDataGroup /*aDataMask*/,
											const RLbsAssistanceDataBuilderSet& /*aData*/,
						   					TInt /*aReason*/)
	{
	}

void CNetworkGateway::ProcessNetworkBasedLocation(const TPositionInfoBase& /*aPosInfo*/)
	{
	}


void CNetworkGateway::ProcessLocationUpdate(const TLbsNetSessionId& /*aSessionId*/,
										const TPositionInfoBase& /*aPosInfo*/)
	{
	}


void CNetworkGateway::ProcessNetworkLocationMessage(const TLbsNetLocMsgBase& aMessage)
	{
	LBSLOG(ELogP2, "CNetworkGateway::ProcessNetworkLocationMessage:");
	LBSLOG2(ELogP3, "Type : %d", aMessage.Type());

	switch (aMessage.Type())
		{
		case TLbsNetLocMsgBase::ENetLocMsgNetworkLocationRequest:
			{
			TInt reason = KErrNone;
			const TLbsNetLocNetworkLocationRequestMsg& req = static_cast<const TLbsNetLocNetworkLocationRequestMsg&>(aMessage);
			iCurrentNetLocReqId = req.SessionId();
			
			if (req.Quality().MaxFixTime() == 0)//answer req with incorrect session id
				{		
				iCurrentNetLocReqId.SetSessionNum(999);
				TLbsNetLocNetworkLocationCompleteMsg msg(iCurrentNetLocReqId, reason);	
				iNetworkLocationChannel->SendNetworkLocationMessage(msg);
				}

			else if (req.Quality().MaxFixTime() == 1)//answer req with unknown response type
				{		
				TLbsNetLocNetworkLocationCancelMsg msg(iCurrentNetLocReqId, reason);
				iNetworkLocationChannel->SendNetworkLocationMessage(msg);
				}
				
			else
				{
				TLbsNetLocNetworkLocationCompleteMsg msg(iCurrentNetLocReqId, reason);	
				iNetworkLocationChannel->SendNetworkLocationMessage(msg);
				}
				
			// for testing return a fixed test location
			TPositionCourseInfo posInfo;
			TPosition pos;
			pos.SetCoordinate(50, 80);
			posInfo.SetPosition(pos);
			
			TCourse course;
            course.SetSpeed(10.0);
            course.SetVerticalSpeed(20.0);
            course.SetHeading(30.0);
            course.SetSpeedAccuracy(1.0);
            course.SetVerticalSpeedAccuracy(2.0);
            course.SetHeadingAccuracy(3.0);
            course.SetCourse(40.0);
            course.SetCourseAccuracy(4.0);
            
            posInfo.SetCourse(course);
			
			iNetworkLocationChannel->SetReferencePosition(iCurrentNetLocReqId, posInfo);
			break;
			}
		case TLbsNetLocMsgBase::ENetLocMsgNetworkLocationCancel:
			{
			const TLbsNetLocNetworkLocationCancelMsg& cancel = static_cast<const TLbsNetLocNetworkLocationCancelMsg&>(aMessage);
			// for test netlocmanager, return the location info directly without sending request to network protocol
			iCurrentNetLocReqId = cancel.SessionId();
			TInt reason = KErrNone;
			TLbsNetLocNetworkLocationCompleteMsg msg(iCurrentNetLocReqId, reason);
			iNetworkLocationChannel->SendNetworkLocationMessage(msg);
			break;
			}
		case TLbsNetLocMsgBase::ENetLocMsgNetworkLocationComplete:
		default:
			{
			break;
			}
		}
	}

void CNetworkGateway::ProcessAssistanceDataRequest(TLbsAsistanceDataGroup /*aDataMask*/)
	{
	}
	

void CNetworkGateway::ProcessNetRequestMessage(const TLbsNetInternalMsgBase& /*aMessage*/)
	{
	}

/* Observer that is called when LbsRoot has requested the Network Gateway to shutdown.
*/
void CNetworkGateway::OnProcessCloseDown()
	{
	// For now, simply close down as soon as possible.
	// In future the close down may need to be asynchronous,
	// to allow the buffers in CAgpsChannel and CNetRequestChannel
	// to be processed normally.
	
	iNetworkLocationChannel->Cancel();
	CActiveScheduler::Stop();
	}


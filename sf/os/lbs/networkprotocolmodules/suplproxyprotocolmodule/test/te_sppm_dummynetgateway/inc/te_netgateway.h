// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// netgateway.h
// Definition of the main objects in the  Dummy Network Gateway.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef LBSNETGATEWAY_H_
#define LBSNETGATEWAY_H_

#include <e32base.h>
#include <e32property.h>
#include <lbs/lbsnetclasstypes.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include "te_netrequestchannel.h"
#include "lbsprocesssupervisor.h"


const TLbsNetSessionId KInvalidNetSessionId = TLbsNetSessionId(TUid::Uid(0x00000001), 0xACACACAC);


/** The main class in the Network Gateway. 

This class acts as the junction point where messages to/from
the network module are translated into messsages sent to/from
the various components of the LBS sub-system.

It contains a number of active objects to service the asynchronous
requests coming from the other LBS components via either
publish & subscribe (RProperty) or a message queue (RMsgQueue).

It also implements the observer interface from the Network Protocol
plugin module in order to receive messages from the network.

*/
class CNetworkGateway : public CBase, 
						public MLbsNetworkProtocolObserver,
						public MNetworkRequestObserver,
					    public MLbsProcessCloseDown
						
	{
public:
	static CNetworkGateway* NewL();	
	~CNetworkGateway();

protected:
	// Process messages coming in from other LBS sub-components
	
	//from MNetworkRequestObserver
	void ProcessNetRequestMessage(const TLbsNetInternalMsgBase& aMessage);
			
protected:
	// Process messages coming in from the network
	// (see MLbsNetworkProtocolObserver)
	void GetCurrentCapabilities(TLbsNetPosCapabilities& aCapabilities) const;

	void ProcessStatusUpdate(TLbsNetProtocolServiceMask aActiveServiceMask);

	void ProcessPrivacyRequest(const TLbsNetSessionId& aSessionId,
							   TBool aEmergency, 
							   const TLbsNetPosRequestPrivacy& aPrivacy,
							   const TLbsExternalRequestInfo& aRequestInfo);
	
	void ProcessLocationRequest(const TLbsNetSessionId& aSessionId,
								TBool aEmergency,
								TLbsNetProtocolService aService, 
								const TLbsNetPosRequestQuality& aQuality,
								const TLbsNetPosRequestMethod& aMethod);

	void ProcessSessionComplete(const TLbsNetSessionId& aSessionId,
								TInt  aReason);

	void ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
							   const RLbsAssistanceDataBuilderSet& aData,
							   TInt aReason);
	
	void ProcessLocationUpdate(const TLbsNetSessionId& aSessionId,
								const TPositionInfoBase& aPosInfo);

protected:
	// Process the close-down request from LbsRoot
	void OnProcessCloseDown();
		
private:
	CNetworkGateway();
	void ConstructL();
	
	void LoadProtocolModuleL();
	
private:
	CNetworkRequestChannel* iNetworkRequestChannel;	
	CLbsNetworkProtocolBase* iNetworkProtocol;
	CLbsCloseDownRequestDetector* iCloseDownRequestDetector;
	};


#endif // LBSNETGATEWAY_H_

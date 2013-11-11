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
//

#ifndef __SUPLPROXYPOSSUBSESSION_H__
#define __SUPLPROXYPOSSUBSESSION_H__

#include <e32base.h>
#include "suplproxyserver.h"
#include "lbssuplproxyservermsgenums.h"
#include "suplproxyprotocol.h"
#include "suplpositioningprotocolfsm.h"
#include "suplrrlpstatemachine.h"
#include "lbssuplproxyposclient.h"
#include "lbstimer.h"
#include "suplproxysuplinforetriever.h"

/** 
CSuplProxyPosSubSession
*/
class CSuplProxyPosSubSession : public CObject, public MSuplPositioningProtocolFsmObserver, public MPosProtocolResponseObserver
	{
	public:
		// Constructor
		CSuplProxyPosSubSession(MSuplProxyPosProtocol& aPosProtocol, TInt aRequestId);
		virtual ~CSuplProxyPosSubSession();
		
		static CSuplProxyPosSubSession* NewL(MSuplProxyPosProtocol& aPosProtocol, TInt aRequestId, TInt aRefLocSourceId);
		void ServiceL(const RMessage2& aMessage);

	    // from MSuplPositioningProtocolFsmObserver
		void ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, TInt aReason);
		void ProcessPositioningRequest(const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod);					
		void PositioningPayloadToNetwork(const CSuplPosPayload* aPositioningPayload);
		void PositioningProtocolError(const TInt& aError);
		void PositioningSessionEnded(); 
	    
		// from MPosProtocolResponseObserver
		void HandleRrlpLocationResponse(TInt aReason, const TPositionInfoBase& aPosInfo);
		void HandleSetAssistanceDataMask(TInt aReason, TLbsAsistanceDataGroup aDataRequestMask);
	protected:
		
	private:
		void ConstructL(TInt aRefLocSourceId);
		
		void HandleGetPosition(const RMessage2& aMessage);
		void HandleCancelGetPosition(const RMessage2& aMessage);
		void HandleClose(const RMessage2& aMessage);
		void HandleResetRequestId(const RMessage2& aMessage);
		void HandleSessionComplete(const RMessage2& aMessage);
		void HandlePosPayloadL(const RMessage2& aMessage);
		void HandleGetSuplInfo(const RMessage2& aMessage);
		void HandleCancelGetSuplInfo(const RMessage2& aMessage);
		void HandleSetPosMethod(const RMessage2& aMessage);
		void HandleSetSlpAddressL(const RMessage2& aMessage);
		void ReportPositioningProtocolError();	
		
	private:
		// Assistance data builder
		RLbsAssistanceDataBuilderSet	iBuilderSet;	
	    // interface for talking to LBS subsystem:
	    MSuplProxyPosProtocol& iPosProtocol;
	    // requestid 
	    TInt iRequestId;
	    // RRLP state machine (decodes payloads)
	    CSuplRrlpFsm* iRrlpFsm;
	    // whether RRLP FSM has told us that the RRLP Session is being ended (next pospayload received will be the last)
	    TBool	iSessionEnded;
	    // Handles the GetSuplInfo calls and the cancelling of these requests
	    CSuplProxySuplInfoRetriever* iSuplInfoRetriever;
	};

#endif // __SUPLPROXYPOSSUBSESSION_H__

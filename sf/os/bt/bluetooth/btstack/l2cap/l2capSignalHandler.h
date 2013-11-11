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
// This is the base class for handling L2CAP signalling.
// 
//

#ifndef L2CAPSIGNALHANDLER_H
#define L2CAPSIGNALHANDLER_H

#include <e32std.h>
#include <e32base.h>
#include <bt_sock.h>

#include "l2constants.h"
#include "L2types.h"

#include "L2CapPDU.h"

class HConnectionResponse;
class HConnectionRequest;
class HEchoRequest;
class HEchoResponse;
class HConfigureRequest;
class HConfigureResponse;
class HDisconnectRequest;
class HDisconnectResponse;
class HInformationRequest;
class HInformationResponse;
class HCommandReject;
class HInvalidCommand;

class CL2CAPConnectionSAP;


NONSHARABLE_CLASS(CL2CapSignalHandler) : public CBase
	{
public:
	virtual ~CL2CapSignalHandler();
	virtual HL2CapPDU* GetPDU();

	virtual TBool HandleIncomingCFrame(HCFramePDU* aCFrame);

	// Process incoming commands
	virtual TBool HandleConnectionResponse(HConnectionResponse* aConnectionResponse);
	virtual TBool HandleConnectionRequest(HConnectionRequest* aConnectionRequest);
	virtual TBool HandleEchoRequest(HEchoRequest* aEchoRequest);
	virtual TBool HandleEchoResponse(HEchoResponse* aEchoResponse);
	virtual TBool HandleConfigureRequest(HConfigureRequest* aConfigRequest);
	virtual TBool HandleConfigureResponse(HConfigureResponse* aConfigResponse);
	virtual TBool HandleDisconnectRequest(HDisconnectRequest* aDisconnectRequest);	
	virtual TBool HandleDisconnectResponse(HDisconnectResponse* aDisconnectResponse);	
	virtual TBool HandleInformationRequest(HInformationRequest* aInformationRequest);
	virtual TBool HandleInformationResponse(HInformationResponse* aInformationResponse);
	virtual TBool HandleCommandReject(HCommandReject* aCommandReject);
	virtual TBool HandleInvalidCommand(HInvalidCommand* aInvalidCommand);

	virtual void Error(TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) = 0;

	virtual void CommandResponseFailure(HL2CapCommand* aCommand) = 0;

	void SendInvalidCIDCommandReject(TUint8 aId, TL2CAPPort aSourceCID, TL2CAPPort aDestinationCID);

	void AddToOutgoingQueue(HL2CapCommand* aCommand);
	TBool IsDuplicateCommandRequest(HL2CapCommand* aCommand);
	
	void ChannelConfigured();

	void HandleLinkError();
	void DrainPendingCommands();
	void FlushPendingCommands();
	void FlushAwaitingResponseCommands();
	void FlushAllQueues();

	TUint8 CurrentRTXTimerDuration(TUint8 aBaseRTXTimerDuration) const;

	inline CL2CAPMux& Mux() const;
	
	
protected:
	CL2CapSignalHandler(CL2CAPMux* aMuxer = 0);
	
	void DeleteCommands(TDblQue<HL2CapCommand>& aCommandsToDelete );
	
	HL2CapCommand* FindMatchingOutstandingRequest(TUint8 aExpectedCommandCode, TUint8 aId);
	
	virtual void PendingCommandsDrained() = 0;

private:

// Class Members.
protected:
	enum
		{
		EMatchAnyL2CAPRequest = 0,
		};
		
	CL2CAPMux* iMuxer;

	TBool iDrainPendingCommands;

	TDblQue<HL2CapCommand> iPendingCommands;
	TDblQue<HL2CapCommand> iCommandsAwaitingResponse;
	TDblQue<HL2CapCommand> iSentResponses;
	};


inline CL2CAPMux& CL2CapSignalHandler::Mux() const
	{
	return (*iMuxer);
	}
		

#endif

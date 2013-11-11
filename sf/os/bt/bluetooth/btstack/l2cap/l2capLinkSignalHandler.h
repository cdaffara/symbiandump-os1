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
// This deals with all the signalling packets that are global to the L2CAP entity.
// i.e., Echo Request / Response, Information Request / Response
// 
//

#ifndef L2CAPLINKSIGNALHANDLER_H
#define L2CAPLINKSIGNALHANDLER_H

#include <e32std.h>
#include <e32base.h>

#include "l2capSignalHandler.h"
#include "l2capEntityConfig.h"


class TL2CAPEntityConfig;
class CL2CAPProtocol;
class MEchoResponseHandler;

NONSHARABLE_CLASS(CL2CapLinkSignalHandler) : public CL2CapSignalHandler
	{
public:
	static CL2CapLinkSignalHandler* NewL(CL2CAPMux* aMuxer);
	~CL2CapLinkSignalHandler();

	virtual TInt ConstructInformationRequest(TInfoType aInfoType);
	TInt ConstructEchoRequest(const TDes8* aData, MEchoResponseHandler& aEchoResponseHandler);
	void DeregisterOutstandingEchoRequests(MEchoResponseHandler& aEchoResponseHandler);
	
	void Error(TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction);
	void LinkUp();
	
	void CommandResponseFailure(HL2CapCommand* aCommand);

	inline TUint16 SigMTU() const;
	inline TL2CAPEntityConfig& PeerEntityConfig(); //const;
	
private:
	void ConstructL();

	CL2CapLinkSignalHandler(CL2CAPMux* aMuxer);
 	TBool HandleConnectionRequest(HConnectionRequest* aConnectionRequest);
	TBool HandleEchoResponse(HEchoResponse* aEchoResponse);
	TBool HandleEchoRequest(HEchoRequest* aEchoRequest); 
	TBool HandleInformationRequest(HInformationRequest* aInformationRequest);
	TBool HandleInformationResponse(HInformationResponse* aInformationResponse);
	TBool HandleCommandReject(HCommandReject* aCommandReject);
	TBool HandleInvalidCommand(HInvalidCommand* aInvalidCommand);

	// Un-handled SAP signal handler commands.
 	TBool HandleConnectionResponse(HConnectionResponse* aConnectionResponse);
	TBool HandleConfigureRequest(HConfigureRequest* aConfigRequest);
	TBool HandleConfigureResponse(HConfigureResponse* aConfigResponse);	
	TBool HandleDisconnectRequest(HDisconnectRequest* aDisconnectRequest);	
	TBool HandleDisconnectResponse(HDisconnectResponse* aDisconnectResponse);	

	void PendingCommandsDrained();
	
private:
	TL2CAPEntityConfig iPeerL2CapEntityConfig;
	
	TUint16 iSigMTU;
	};

inline TUint16 CL2CapLinkSignalHandler::SigMTU() const
	{
	return iSigMTU;
	}

TL2CAPEntityConfig& CL2CapLinkSignalHandler::PeerEntityConfig() //const
	{
	return iPeerL2CapEntityConfig;
	}

	
#endif

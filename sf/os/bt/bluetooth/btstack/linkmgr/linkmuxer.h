// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LINKMUXER_H__
#define LINKMUXER_H__

#include <e32base.h>
#include "linkqitem.h"
#include "linkflowcontrol.h"
#include "linkutil.h"
#include <bluetooth/linkmuxnotifier.h>
#include <bluetooth/hci/hctlchannelobserver.h>

class CHCIFacade;
class CHCILinkMgr;
class MHCICommandQueue;
class CACLDataQController;
class CLinkMgrProtocol;
class CHCICmdQController;

/**
	Regulates the issuing of buffers sent down the same channel.

	The mux decides and notifies the appropriate Q to send data when the wire
	is free. The mux will need to be fed with 'the wire is free' events and 
	trigger events that will indicate reception of events (external) 
	corresponding to the different Qs -(like NumCompletedPackets, Write 
	Data CommandComplete etc)
*/
NONSHARABLE_CLASS(CLinkMuxer) : public CBase
	{
public:
	static CLinkMuxer* NewL(CLinkMgrProtocol& aLinkMgrProtocol, CHCIFacade& aHCIFacade);
	
	void RecordHostControllerToHostFlowControl(TBool aFlowFlag);
	CACLDataQController* HandleLocalReadBufferSizeResult(TUint16 aAclMaxLen, TUint8 /*aScoMaxLen*/, 
														TUint16 aNoACL, TUint16 /*aNoSCO*/);
	TInt ACLPacketMTU() const;

	CACLDataQController& DataQController() const {return *iDataController;}
	
	void ChannelsFree(THCITransportChannel aChannel); // notification of free channels
	void ChannelsClosed(THCITransportChannel aChannel); // notification of closed channels
	
	void TryToSend();	// request to send on certain channels
	
	TFlowControlMode FlowControlMode() const {return iFlowControlMode;}
	void ResetFlowControlMode();
	
#ifdef STACK_SCO_DATA
	TBool CanWriteSCOData();
#endif
	~CLinkMuxer();

private:
	void DataTrigger();
	void CommandTrigger();
	void ConstructL();
	CLinkMuxer(CLinkMgrProtocol& aLinkMgrProtocol, CHCIFacade& aHCIFacade);
	void DoSend(/*THCITransportChannel aChannel*/);	// request to send on certain channels
	static TInt TryToSendCallBackStatic(TAny* aCLinkMuxer);
	
private:
// owned resources
	CHCICmdQController*	iCommandController;
	CACLDataQController* iDataController;
	CAsyncCallBack*			iSendCallBack;
// non-owned resources
	CHCIFacade&				iHCIFacade;
	THCITransportChannel	iChannelsFree;
	TFlowControlMode		iFlowControlMode;
	TBool					iCtrlerToHostSet;
	CLinkMgrProtocol&		iLinkMgrProtocol;
	TInt					iACLPacketMTU;
	};

#endif

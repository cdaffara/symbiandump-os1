// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the CRawIPNifMain class, which provides a central point for the NIF
// to communicate with NifMan and the BCA controller. This class creates an
// interface to the IPv4 protocol when it is required.
// 
//

/**
 @file
*/

#ifndef RAWIPNIFMAIN_H
#define RAWIPNIFMAIN_H

#include <comms-infras/nifif.h>
#include <networking/packetlogger.h>
#include <in_sock.h>
#include "ProtocolIfBase.h"
#include "BcaIoController.h"
#include "MControllerObserver.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <comms-infras/nifprvar_internal.h>
#endif

class CPacketLogger;

class CRawIPNifMain : public CNifIfLink, public MControllerObserver
	{
public:
 	CRawIPNifMain(CNifIfFactory& aFactory, MNifIfNotify* aNotify);
	~CRawIPNifMain();
	void ConstructL(const TDesC& aName);
	// Used to be :Pure virtuals inherited from MContextSmObserver
	CBcaIoController* GetBcaController();
	void UpdateContextState(RPacketContext::TContextStatus aState,
		TInt aError);
	void UpdateContextConfig(
		RPacketContext::TContextConfigGPRS aConfig);

public: // Pure virtuals inherited from CNifIfBase
	virtual void Info(TNifIfInfo&) const;
	virtual TInt Send(RMBufChain& aPdu, TAny* aSource=0);
	virtual TInt Notification(TAgentToNifEventType aEvent, void * aInfo); 

public: // Virtuals inherited from CNifIfBase
	virtual void BindL(TAny *aId);
	virtual TInt Control(TUint aLevel, TUint aName, TDes8& aOption, 
		TAny* aSource);
	virtual TInt State();

public: // Pure virtuals inherited from CNifIfLink 
	virtual TInt Start();
	virtual void Stop(TInt aReason,MNifIfNotify::TAction aAction);
    virtual CNifIfBase* GetBinderL(const TDesC& aName);
	virtual void Restart(CNifIfBase* aIf);

public: // Virtuals inherited from CNifIfLink
	virtual void AuthenticateComplete(TInt aResult);
 
public: // Pure virtuals inherited from MRawIPNifObserverBase
	virtual void InitialiseL(TInitialisationState aState, TInt aError);
	virtual void ShutDown(TInitialisationState aState, TInt aError);
	virtual MNifIfNotify* GetAgent();
	
	virtual CProtocolIfBase* GetProtocolIf();

public: // Pure virtuals inherited from MControllerObserver
	// Data going upstack
	virtual void Process(RMBufChain& aPdu, TUint16 aProtocolCode);
	// Flow Control unblocked
	virtual void ResumeSending();

public:  // Inherited from MNifIfNotify
	// Notify that link is Up/Down
	virtual void LinkLayerUp();
	virtual void LinkLayerDown(TInt aError);

public:	// Send data to CBcaIoController
	TInt SendPacket(RMBufChain& aPdu, TAny* /*aSource=0*/, 
		TUint16 /*aType=KIp4FrameType*/);	

public:	// Context status retrieval.
	inline RPacketContext::TContextStatus GetContextStatus();

private:// Unowned
	// Upstack bound protocol reference
	CProtocolBase* iProtocol;

	// The protocol to use (IPv4).Note: This is not owned by this class.
	CProtocolIfBase* iProtocolIf;

private:// Owned 
	TInt iInitError; 
	TIfStatus iState; // One of EIfPending, EIfUp, EIfBusy or EIfDown.

	RPacketContext::TContextStatus iContextStatus;
	RPacketContext::TContextConfigGPRS iContextConfig;

	CBcaIoController* iBcaController;
	};

inline RPacketContext::TContextStatus CRawIPNifMain::GetContextStatus()
/**
 * Returns Context status
 *
 * @return iContextStatus The context status
 */
	{
	return iContextStatus;
	}

#endif // RAWIPNIFMAIN_H

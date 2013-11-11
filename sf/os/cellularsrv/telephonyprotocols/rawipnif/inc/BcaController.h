// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the interface to the BCA & flow control.
// 
//

/**
 @file 
*/

#ifndef BCACONTROLLER_H
#define BCACONTROLLER_H

#include <e32base.h>
#include <c32comm.h>

#include "MControllerObserver.h"
#include "Constants.h"

#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
#include "IPTagHeader.h"
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS

/** 
@internalComponent 
*/	
class CBcaController : public CBase
	{
public:
	CBcaController(MControllerObserver& aObserver);
	void BaseConstructL();
	~CBcaController();

public: // Flow Control
	void UpdateInternalFlowFlag(TFlowControl aValue);
	void UpdateContextStateFlag(TBool aValue);
	TBool IsTxPossible();

public:
	virtual void StartL() = 0;
	virtual void Stop(TInt aError = KErrNone) = 0;

#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
public: //tag headers
	void SetType(TUint16 aType);
	void AddHeader(TDes8& aDes);
	TUint16 RemoveHeader(RMBufChain& aPdu);
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS

public:  // Process Down/Up stack data packets 
	void Process(TDesC8& aPdu);
	TInt Send(RMBufChain& aPdu);
	void SendComplete();
	// Get the NIF reference.
	inline MControllerObserver& GetObserver();
protected:
	virtual void BcaSend(RMBufChain& aPdu) = 0;
	virtual void BcaProcess(TDesC8& aPdu) = 0;
	virtual void BcaSendComplete() = 0;
	virtual TInt BcaSendBufferLength() = 0;
	
protected:
	enum TSendState
		{
		EIdle,
		ESending,
		EShuttingDown
		};

	enum TInitialisationState
		{
		EStart,
		EBcaController,
		EInitialised
		};

	inline TInt GetSendState();
	inline void SendState(TSendState aState);
	
private: // Flow Control
	void ResumeSending();
	//Buffer control
	TBool IsSendQueueEmpty();
	TBool IsSendQueueFull();
	void AppendToSendQueue(RMBufChain& aPdu);
	TBool RemoveFromSendQueue(RMBufChain& aPdu);
	void EmptySendQueue();
	
private: // Unowned data.
    MControllerObserver& iObserver;
protected:
	TInt iMaxTxPacketSize;
	TInt iMaxRxPacketSize;
	
private:
	// Flow control flags
	TFlowControl iTxFlowControl;
	TBool iTxContextActive;
	TSendState iSendState;

	/** The internal packet buffer */
	RMBufPktQ iSendQueue;
	/** The maximum number of packets that we want in an internal queue */
	TInt iMaxSendQueueLen;
	/** Current number of packets in the internal packet buffer */
	TInt iNumPacketsInSendQueue;
	/** Flag to indicate whether the queue is full or not */
	TBool iFullQueueFlag;

#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
	CIPTagHeader* iIPTagHeader;
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS
	};

void CBcaController::SendState(TSendState aState)
	{
	iSendState = aState;
	}

TInt CBcaController::GetSendState()
	{
	return iSendState;
	}


MControllerObserver& CBcaController::GetObserver()
	{
	return iObserver;
	}

#endif // BCACONTROLLER_H

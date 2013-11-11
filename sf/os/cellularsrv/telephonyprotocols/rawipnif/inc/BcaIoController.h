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
// Defines the interface to the BCA interface
// 
//

/**
 @file 
*/

#ifndef BCAIOCONTROLLER_H
#define BCAIOCONTROLLER_H

#include <e32base.h>
#include <c32comm.h>
#include <networking/bca.h>
#include <networking/bcafactory.h>
// for the constants for sending/blocking
#include <comms-infras/ss_flowbinders.h>
#include "MControllerObserver.h"
#include "Constants.h"

using namespace BasebandChannelAdaptation;

class CSender;
class CReceiver;
class CBcaControl;

/** 

@internalComponent 
*/	
void Panic(TRawIPNifPanic aPanic);

/**

@internalComponent
*/
typedef MBcaFactory* (*TNewBcaFactoryL)();


/** 
@internalComponent 
*/	
class CBcaIoController : public CBase
	{
public:
	static CBcaIoController* NewL(MControllerObserver& aObserver);	
	~CBcaIoController();

	void StartL();
	void Stop(TInt aError = KErrNone);
	ESock::MLowerDataSender::TSendResult Send(RMBufChain& aPdu);
	void SendComplete();
	
	inline CSender& Sender();
	inline CReceiver& Receiver();
	inline TUint Nsapi();
	void SetBcaStackAndName(const TDesC& aBcaStack, const TDesC& aBcaName);
	inline TPtrC BcaStack()const;
	inline TPtrC BcaName()const;
	inline MBca* Bca();
	inline void SetBca(MBca* aBca);
	inline void SetPort(const TDesC& aPortName);
	inline TPtrC Port() const;
	inline void SetIapId(TUint32 aIapId);
	inline TUint32 IapId();
    inline void BlockSending(void) { iFlowBlocked = ETrue; }
    void ResumeSending();
    
    // Get the NIF reference.
    inline MControllerObserver& GetObserver() { return iObserver; }
    
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
    // tag headers
    void SetType(TUint16 aType);
    void AddHeader(TDes8& aDes);
    TUint16 RemoveHeader(RMBufChain& aPdu);
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS
    
protected:
    TUint iMaxTxPacketSize;
    TUint iMaxRxPacketSize;
    
private:
    
    CBcaIoController(MControllerObserver& aObserver);
    void ConstructL();
    
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
    
    // Flow control flags
    TSendState iSendState;
    
    inline void SendState(TSendState aState) { iSendState = aState; }
    
    
    // uplink flow control
    TBool iFlowBlocked;

    // Buffer control
    inline TBool IsSendQueueEmpty() { return iSendQueue.IsEmpty(); }
    inline TBool IsSendQueueFull() { return (iNumPacketsInSendQueue >= iMaxSendQueueLen) ? ETrue : EFalse; }

    inline void AppendToSendQueue(RMBufChain& aPdu) 
        {  
        iSendQueue.Append(aPdu);
        iNumPacketsInSendQueue++; 
        }
 
    inline TBool RemoveFromSendQueue(RMBufChain& aPdu) 
        {
        TBool ret = iSendQueue.Remove(aPdu);
         if(ret)
             {
             iNumPacketsInSendQueue--;
             }
         return ret;
        }
    
    /** The internal packet buffer */
    RMBufPktQ iSendQueue;
    /** The maximum number of packets that we want in an internal queue */
    TUint iMaxSendQueueLen;
    /** Current number of packets in the internal packet buffer */
    TInt iNumPacketsInSendQueue;
    
    MControllerObserver& iObserver;
    
    // Unowned data.
    /** Pointer to the BCA */
    MBca* iMBca;
    
    /** used to send data*/
    CSender* iSender;
    /** used to receive data*/
    CReceiver* iReceiver;
    /** used to load, open and close the BCA*/
    CBcaControl* iLoader;
    /** IAP ID used to open CommDB*/
    TUint32 iIapId;
    /** Bca name*/
    TPtrC iBcaName;
    /** Bca Stack*/
    TPtrC iBcaStack;
    /** CommPort Name*/
    TPtrC iCommPort;


	};

inline CSender& CBcaIoController::Sender()
/**
 * Returns reference to CSender object
 *
 * @return iSender The CSender reference
 */
	{
	return *iSender;
	}
	
inline CReceiver& CBcaIoController::Receiver()
/**
 * Returns reference to CReceiver object
 *
 * @return iReceiver The CReceiver reference
 */
	{
	return *iReceiver;
	}

/** returns the BCA Stack*/
inline TPtrC CBcaIoController::BcaStack()const
	{
	return iBcaStack;
	}
	
/** returns the BCA name*/
inline TPtrC CBcaIoController::BcaName()const
	{
	return iBcaName;
	}
	
/** retrurns the BCA pointer.*/	
inline MBca* CBcaIoController::Bca()
	{
	return iMBca;
	}
/** sets the BCA pointer.

* @param aBca BCA pointer.
*/
inline void CBcaIoController::SetBca(MBca* aBca)
	{
	iMBca = aBca;
	}

/** sets the IAP ID

* @param aIapId IAP ID to be set.
*/	
inline void CBcaIoController::SetIapId(TUint32 aIapId)
	{
	iIapId = aIapId;
	}

/** returns the IAP ID*/	
inline TUint32 CBcaIoController::IapId()
	{
	return iIapId;
	}

/** sets the port name

* @param aPortName port name to be set.
*/	
inline void CBcaIoController::SetPort(const TDesC& aPortName)
	{
	iCommPort.Set(aPortName);
	}

/** returns the port name*/	
inline TPtrC CBcaIoController::Port()const
	{
	return iCommPort;
	}

/** class used to load, open and shutdown the BCA and also start and shutdown the NIF.
@internalComponent 
*/
class CBcaControl : public CActive
	{
public:
	CBcaControl(CBcaIoController& aObserver);
	~CBcaControl();
public:
	void StartLoadL();
	void ShutdownBca(TInt aError);
    inline MBca* Bca();
protected:
	// Inherited from CActive.
	virtual void RunL();
	virtual void DoCancel();	
private: // Unowned data.
	CBcaIoController& iObserver;
private:
		enum TBcaState
		{
		/** Bca ready to start */
		EIdling,
		/** Bca to be closed*/
		EClosing,
		/** Bca IAP is set*/
		EIAPSet,
		/** Bca stack is set*/
		EBcaStackSet,
		/** Bca channel is open */
		EBcaOpened
		};
	
private:
	
	/** BCA owned by NIF*/	
	MBca* iMBca;
	/** used to load library*/
	TAutoClose<RLibrary> iBcaDll;
	/** BCA state*/
	TBcaState iState;
	/** NIF shut down error*/
	TInt iError; 
	};
	
/** returns the BCA pointer. */	
inline MBca* CBcaControl::Bca()
	{
	return iMBca;
	}	

#endif // BCAIOCONTROLLER_H


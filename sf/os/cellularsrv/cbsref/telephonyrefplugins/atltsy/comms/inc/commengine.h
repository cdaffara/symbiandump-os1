// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file commengine.h
// This contains CCommEngine which manage access to serial port.
// 

#ifndef __CCOMMENGINE_H__
#define __CCOMMENGINE_H__

// system include
#include <c32comm.h>
#include <callbacktimer.h>

// use include
#include "mcommobserver.h"
#include "globalphonemanager.h"

// const defination
const TInt KCommRxBufSize = 200;

//Class forward
class CCommWriter;
class CCommReader;
class CStringParser;

/**
 * The class provide the functionalities to access the serials port, write and read to or from
 * serial port. It also provide the function to find one line of string and notify its observer. 
 * 
 */
class CCommEngine : public CBase,
					public MTimerObserver

	{
	friend class CCommWriter;
	friend class CCommReader;
public:
	/**
	 * Factory function
	 * 
	 * @param aBufferSize
	 * @param aReadPriority
	 * @param aWritePriority
	 * @param aPortAccess
	 */
	static CCommEngine* NewL(TInt aBufferSize, 
						TInt aReadPriority, 
						TInt aWritePriority, 
						TPortAccess& aPortAccess);
	
	/**
	 * Factory function
	 * 
	 * @param aBufferSize
	 * @param aReadPriority
	 * @param aWritePriority
	 * @param aPortAccess
	 */
	static CCommEngine* NewLC(TInt aBufferSize, 
						TInt aReadPriority, 
						TInt aWritePriority, 
						TPortAccess& aPortAccess);
	
	/**
	 * Destructor
	 * 
	 */
	virtual ~CCommEngine();
	
public:
	/**
	 * Configure the serial port
	 * 
	 */
	TInt ConfigurePort(TCommConfig aConfiguration);
	
	/**
	 * Open the serial port
	 * 
	 * @param aDll
	 * @param aName
	 * @param aAccess
	 */
	TInt CommOpen(const TDesC& aDll, const TDesC& aName, TCommAccess aAccess);
	
	/**
	 * Close the serial port
	 * 
	 */
	void CommClose();
	
	/**
	 * Cancel comm serail IO opeation
	 * 
	 */
	void CommCancel();
	
	/**
	 * Start serail IO writing
	 * 
	 */
	void CommWrite(const TDesC8& aDes);
	
	/**
	 * Serial port writing is ready
	 * 
	 */
	void CommWriteReady();
	
	/**
	 * Cancel serial writing
	 * 
	 */
	void CommWriteCancel();
	
	/**
	 * Start serail IO reading
	 * 
	 */
	void CommRead();
	
	/**
	 * Start to read
	 * 
	 */
	void Read();
	
	/**
	 * The read will return whatever the data have been received
	 * 
	 */
	void CommReadOneOrMore();
	
	/**
	 * Cancel reading
	 */
	void CommReadCancel();
	
	/**
	 * Disconnect it from modem
	 */
	void Disconnect();
	
	/**
	 * Drop the DTR
	 */
	void DropDtr();
	
	/**
	 * Raise the DTR-- Data Terminal Ready
	 */
	void RaiseDTR();
	
	/**
	 * Drop the RTS signal-- Request to send
	 */
	void DropRTS();
	
	/**
	 * Raise the RTS signal
	 */
	void RaiseRTS();
	
	/**
	 * Clean all the buffers
	 */
	void ResetBuffers();
	
	/**
	 * Get the singnals
	 */
	TUint Signals();
	
	/**
	 * Start the timer
	 */
	void StartTimer(TInt aSecond);
	
	/**
	 * Stop the timer
	 */
	void StopTimer();
	
	/**
	 * Reset all the buffers
	 */
	void ResetReadAndWriteBuffers();
	
	/**
	 * Get the port shutdown timeout
	 */
	TInt GetPortShutdownTimeout();
	
	/**
	 * Set the port shut down timeout
	 */
	TInt SetPortShutdownTimeout(TInt aTimeout);
	
	/**
	 * Get the string parser
	 */
	CStringParser* GetStringParse() const;
public:
    /**
     * Set the notifier for get the notificaiton when one line of string was found
     */
	void SetCommReadLineNotify(MCommReadLineNotifier *aReadLineNotify);
	
	/**
	 * Set the comm engine observer
	 */
	void SetCommEngineObserver(MCommEngineObserver *aEngineObserver);
public:
 	/**
 	 * Handle read complete event
 	 */
	virtual void CommReadComplete(TInt aStatus);
	
	/**
	 * Handle write complete event
	 */
	virtual void CommWriteComplete(TInt aStatus);
protected:
    /**
     * Constructor
     */
	CCommEngine(TPortAccess& aPortAccess);
	
	/**
	 * 2nd constructor
	 */
	void ConstructL(TInt aBufferSize, TInt aReadPriority, TInt aWritePriority);	
private:
	/**
	 * From MTimerObserver
	 */
    virtual void TimerRun(TInt aError);  
    
    /**
     * Get the size of received buffer
     */
    TInt GetSizeOfRxBuffer();
private:
	/**
	 * The comm server reference
	 */
	RCommServ iCommServer;
	
	/**
	 * The comm port object
	 */
	RComm iCommPort;	
	
	/**
	 * The active object for reading data from Baseband
	 */
	CCommReader *iCommReader;
	
	/**
	 * The active object for writing AT command
	 */
	CCommWriter *iCommWriter;
	
	/**
	 * The string parser
	 */
	CStringParser *iStringParse;
	
	/**
	 * The engine observer
	 */
	MCommEngineObserver *iEngineObserver;
	
	/**
	 * The received data
	 */
	TBuf8<KCommRxBufSize> iRxBuf;
	
	/**
	 * The port specification
	 */
	TPortAccess& iPortAccess;
    
	/**
	 * The timer active object. Used to record the time after a writing request is triggered.
	 */
	CCallbackTimer* iCallbackTimer;	
	
	/**
	 * Check if the comm has been canceled.
	 */	
	TBool iCommCancel;
	};
#endif

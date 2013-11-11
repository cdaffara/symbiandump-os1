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
// Loopback.cpp
// This file implements a loopback driver for use with 3GNIF test harness.
// 
//

/**
 @file
*/

#ifndef __LOOPBACK_H__
#define __LOOPBACK_H__

#include <c32comm.h>

#include "LoopbackConfig.h"
#include "LoopbackTimer.h"
#include "LoopbackQueue.h"

const TUint KCommLowUnit=0;

#if defined(__WINS__)
const TUint KCommHighUnit=1;
#else // _MARM_
const TUint KCommHighUnit=0;
#endif


_LIT(KSerialDescription, "Packet Loopback CSY");
_LIT(KSerialName, "PKTLOOPBACK");
_LIT(KPortLoopBackCsyPanic, "PortLoopBack.csy");

enum TPortLoopBackCsyPanic
    {       
    /** Input / Output Argument Invalid */
    EPLBArgument
    };



class CHWPort;

/**
 * This class is the factory port object.  It drives the "entry point object" which
 * makes the reset of the objects do their work.  It is based on the basic serial port
 * class CSerial.
 */
NONSHARABLE_CLASS(CHWPortFactory) : public CSerial
	{
public:
    CHWPortFactory();
    ~CHWPortFactory();
	virtual CPort * NewPortL(const TUint aUnit);
	virtual void Info(TSerialInfo &aSerialInfo);
	void Remove(CHWPort* aPort);
	CHWPort* FindPort(TUint aUnit);
	TPortType PortType() const;
public: //CSerial
	TSecurityPolicy PortPlatSecCapability(TUint aPort) const;
private:
	/** All ports created by this factory */
	RArray<CHWPort*> iPorts;
	/** Retrieves settings from the 'loopback.ini' file */
	CLoopbackConfig* iLoopbackConfig;
	
	TPortType iPortType;
	};

/**
 *	This class is the object that interfaces with the commserver.  An instance of this class
 *	represents one port in the loopback driver.
 */
NONSHARABLE_CLASS(CHWPort) : public CPort, public MTimerCallBack, public MFlowControlChangeCallBack
	{
public:
	static CHWPort * NewSerialLoopbackL(TUint aUnit, TUint aDelay, TUint aBufferSize);
	static CHWPort * NewPacketLoopbackL(TUint aUnit, TUint aDelay, TUint aPacketLength, TUint aQueueLength);
private:
	CHWPort(TUint aUnit);
	void ConstructPacketLoopbackL(TUint aDelay, TUint aPacketLength, TUint aQueueLength);
	void ConstructSerialLoopbackL(TUint aDelay, TUint aBufferSize);

public:
	TUint PortName() const;
	virtual void StartRead(const TAny* aClientBuffer,TInt aLength);
	virtual void ReadCancel();
	virtual TInt QueryReceiveBuffer(TInt& aLength) const;
	virtual void ResetBuffers(TUint aFlags);
	virtual void StartWrite(const TAny* aClientBuffer,TInt aLength);
	virtual void WriteCancel();
	virtual void Break(TInt aTime);
	virtual void BreakCancel();
	virtual TInt GetConfig(TDes8& aDes) const;
	virtual TInt SetConfig(const TDesC8& aDes);
	virtual TInt SetServerConfig(const TDesC8& aDes);
	virtual TInt GetServerConfig(TDes8& aDes);
	virtual TInt GetCaps(TDes8& aDes);
	virtual TInt GetSignals(TUint& aSignals);
	virtual TInt SetSignalsToMark(TUint aSignals);
	virtual TInt SetSignalsToSpace(TUint aSignals);
	virtual TInt GetReceiveBufferLength(TInt& aLength) const;
	virtual TInt SetReceiveBufferLength(TInt aSignals);
	virtual void Destruct();
	virtual void FreeMemory();
	virtual void NotifySignalChange(TUint aSignalMask);
	virtual void NotifySignalChangeCancel();
	virtual void NotifyConfigChange();
	virtual void NotifyConfigChangeCancel();
	virtual void NotifyFlowControlChange();
	virtual void NotifyFlowControlChangeCancel();
	virtual void NotifyBreak();
	virtual void NotifyBreakCancel();
	virtual void NotifyDataAvailable();
	virtual void NotifyDataAvailableCancel();
	virtual void NotifyOutputEmpty();
	virtual void NotifyOutputEmptyCancel();
	virtual TInt GetFlowControlStatus(TFlowControl& aFlowControl);
	virtual TInt GetRole(TCommRole& aRole);
	virtual TInt SetRole(TCommRole aRole);

	virtual void StartFlowControl();
	virtual void StopFlowControl();
	
	TInt GetReadRequestStatus();
	TInt GetWriteRequestStatus();

	virtual ~CHWPort();
public:
	void SetLoopbackPort(CHWPort* aHWPort);
	TInt WriteBuf(const TAny* aClientBuffer,TInt aLength);
	void TryToCompleteRead();
	TInt GetFlowControl() const;
private:
	void StartWriteDelayTimer();

public:
	void TimerCallBack();
	/** Timer to control how long a given CPort::Write takes */
	CLoopbackTimer *iWriteDelayTimer;

private:
	/** The configuration of this port */
	TPckgBuf<TCommConfigV01> iConfig;

	/** Contains buffer for all reads and writes on this port */
	MLoopbackQueue* iReadWriteQueue;

	/** The role for this port */
	TCommRole iRole;

	/** This port's loopback port, which can be NULL before a loopback port is assigned */
	CHWPort* iLoopbackPort;
	/** Indicates if there is a read pending for later completion */
	TBool iReadPending;
	/** Should the current read (even if it is pending) complete before the buffer is full? */
	TBool iReadOneOrMore;

	/** Pointer to the client buffer to be filled by a read */
	const TAny* iClientReadBuffer;
	/** Length of the client buffer to be filled by a read */
	TInt iClientReadBufferLength;
	
	/** The port number for this port */
	TUint iPortName;
	/** The configured port type (either packet or serial) */
	TPortType iPortType;

	/** Pointer to object that listens to changes in flow control for this port - it is created on construction */
	CFlowControlChange *iFlowControlChange;
	};

#endif


// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements a loopback driver for use with the ETel Regression test harness.  This
// driver supports two ports with signalling capabilities for DCD, DSR, DTR, RTS, RI, and CTS.
// In order for the signalling to function properly, one port should be opened as a DCE, though
// the driver will allow both ports to operate as DTE's.
// By default, the driver marks or asserts (sets to 1) the following signals when the method
// SetRole is called.
// For the DTE:
// KSignalRTS	KSignalDTR
// For the DCE:
// KSignalCTS	KSignalDSR
// Note that the DCE does NOT set DCD, thus it's initial state is space or de-asserted
// (set to zero).  This was chosen because of the desire of test script authors to fully
// control DCD.
// The driver supports the following signalling configuration handshake capabilities:
// For the DTE:
// KCapsObeyCTSSupported  KCapsFailCTSSupported
// KCapsObeyDSRSupported  KCapsFailDSRSupported
// KCapsObeyDCDSupported  KCapsFailDCDSupported
// For the DCE:
// KCapsObeyRTSSupported  KCapsObeyDTRSupported
// Break, Configuration Change Notification, Flow Control Notification, and Data Available
// Notifications are not supported.
// directory exists.  The log file is named loopback.
// 
//

/**
 @file
 @note	This driver will create a log file in the Logs\ETel directory if the Logs\ETel
*/
#include <cs_port.h>
#include "LOGGER.H"
#include <d32comm.h>
#include <c32comm.h>
#include <e32hal.h>
#include <c32comm_internal.h>

#ifdef __LOGGER__XTRA
#define LOGI(AAA)	CETelLogger::WriteFormat(TRefByValue<const TDesC8>(AAA),iPortName,iSignals,&iPtr);
#else
#define LOGI(AAA)
#endif

#if defined(__VC32__) && _MSC_VER==1100
// Disable MSVC++ 5.0 aggressive warnings about non-expansion of inline functions.
#pragma warning(disable : 4710)	// function '...' not expanded
#endif

const TUint KCommLowUnit=0;
const TUint KBufferGrowthIncrement=0x1000;
const TUint KMaxBufferSize = 0x8000;

// This should be even
const TUint KLoopbackCount=8;


#define SERIAL_DESCRIPTION _L("Loopback CSY")
#define SERIAL_NAME _S("Loopback")

// #define _DEBUG_CONSOLE_

#if defined(_DEBUG_CONSOLE_)
// This class is not used in the loopback csy.  It is left here for future reference.
#include <e32twin.h>
class RDebugConsole : public RConsole
	{
public:
	RDebugConsole();
public:
	void Printf(TRefByValue<const TDesC> aFmt,...);
	};

#define DEBUG_TRACE(m) m

#else	// (_DEBUG)

#define DEBUG_TRACE(m)
//
#endif

//
// "Entry point object" makes the objects which do the work
class CHWPort;


/**
 * This class is the factory port object.  It drives the "entry point object" which
 * makes the reset of the objects do their work.  It is based on the basic serial port
 * class CSerial.
 */
class CHWPortFactory : public CSerial
	{
public:
    CHWPortFactory();
    ~CHWPortFactory();
	virtual CPort * NewPortL(const TUint aUnit);
	virtual void Info(TSerialInfo &aSerialInfo);
	void Remove(CHWPort* aPort);
public: //CSerial
	TSecurityPolicy PortPlatSecCapability(TUint aPort) const;
private:

	CHWPort* iPort[KLoopbackCount];	// pairs of ports
	CHWPort* PairedPort(TUint aPort) { return iPort[aPort^1]; } // returns the other half

	};


/**
 *	This class is the object that interfaces with the commserver.  An instance of this class
 *	represents one port in the loopback driver.
 */
class CHWPort : public CPort
	{
public:
	static CHWPort * NewL(TUint aUnit);
private:
	 CHWPort();

public:
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

	virtual ~CHWPort();
#if defined (_DEBUG_DEVCOMM)
	virtual void DoDumpDebugInfo(const RMessage2 &aMessage);
#endif
public:
	void SetLoopbackPort(CHWPort* aHWPort);
	TInt WriteBuf(const TAny* aClientBuffer,TInt aLength, TBool& aIssueComplete);
	void TryToCompleteRead();
	void UpdatePortResources();

private:
	void CheckSigsAndCompleteRead();
public:
	TPtr8 iPtr;				//< A pointer to the buffer created during class creation.

private:
	TCommRole iRole;		//< The role of this port (ECommRoleDTE or ECommRoleDCE)
	TUint iSignals;			//< The current signals for this port
	TUint iSignalMask;		//< The mask used when a NotifySignalChange request is posted.
							//< If it is clear, no request is outstanding.  It is cleared when
							//< a request is either cancelled or completed.

	TBool iWritePending;			//< True if a write is left pending
	TInt  iWritePendingLength;		//< The length of the buffer that was left pending.
	const TAny* iClientWriteBuffer; //< The buffer pointer that was pended.  A pended write
									//< implies that the WriteCompleted was NOT called when
									//< the write was posted.

	TPckgBuf<TCommServerConfigV01> iServerConfig;	//< A placeholder; no real function in loopback.
	TPckgBuf<TCommConfigV01> iConfig;	//< The configuration of this port.  Important fields for
										//< the loopback driver are: iHandShake

	CHWPort* iLoopbackPort;	//< The pointer to the loopback port to which this port is connected.

	/**
	An HBufC created during port creation, pointed to by iPtr.
	This buffer is only appended to by this object on a write request;
	this object never reads from the buffer.  iLoopbackPort is the
	object responsible for reading from this iBuf (via iPtr), and send
	the data it reads to iLoopbackPort's client (as a completion of a
	read request).
	*/
	HBufC8* iBuf;			//< An HBuf created during port creation, pointed to by iPtr.
							//< Note that this buffer can grow during operation, but it will
							//< not shrink.
	TInt iBufSize;			//< The initial size of the buffer and it's growth increment.
	TBool iReadPending;		//< Boolean indicating that a read has been left pending for later
							//< completion.

	TBool iReadOneOrMore;	//< Should the current read (even if it is pending) complete before
							//< the buffer is full?

	TBool iDataNotify;		//< Set when a data notify event is pending

	TInt iBytesWritten;		//< The number of bytes written to the current read buffer.
	TInt iPendingLength;	//< Count of how many more bytes can be written to the read buffer
	/**
	Pointer to the client buffer to be filled by a read.  The read
	actually takes the data from iLoopbackPort->iPtr to send to the
	client.
	*/
	const TAny* iClientReadBuffer;		//< Pointer to the client buffer to be filled by a read.

	TUint iPortName;		//< Unit number of this port

#if defined (_DEBUG_CONSOLE_)
	// Not used in the current loopback driver.
#if defined (_DEBUG_DEVCOMM)
	CCommDebugDumper *iDumper;		//< Pointer to debug class.
#endif
public:
	RDebugConsole iConsole;			//< Console for debugging.
#endif
	};

//
// CHWPort definitions
//
CHWPort::CHWPort() : iPtr(NULL,0,0)
/**
 * This method is the basic constructor for the CHWPort class.  In initializes
 * the iPtr member specifically to NULL.
 *
 * @param	None
 *
 * @return	None
 */
	{
	__DECLARE_NAME(_S("CHWPort"));
	}

void CloseObject(TAny* anObject)
/**
 * This method simply closes an object from the cleanup stack.  The object must contain
 * a Close method.
 *
 * @param	anObject - a TAny pointer to the object to close.
 * @return	None
 */
	{
	((CObject*)anObject)->Close();
	}

CHWPort* CHWPort::NewL(TUint aUnit)
/**
 * This method is used by the factory object to create the new CHWPort instances.
 * After newing the CHWPort object, the buffer is created, roles and signals are defaulted,
 * and names are initialized.
 *
 * @param	aUnit - the unit to create.
 *
 * @return	A pointer to the created object
 */
	{

	LOGTEXTREL(_S8("Loopback:NewL: Called"));
	LOGTEXTREL2(_L8("Loopback:NewL:  Unit %d..."), aUnit);

	CHWPort *p=new(ELeave) CHWPort;
	TCleanupItem closePort(CloseObject,p);

	CleanupStack::PushL(closePort);


	p->iBuf=HBufC8::NewL(KBufferGrowthIncrement);
	p->iBufSize=KBufferGrowthIncrement;
	p->iPtr.Set((TUint8*)p->iBuf->Ptr(),0,KBufferGrowthIncrement);

	p->iRole = ECommRoleDTE;
	p->iSignals = 0;		// start with no signals asserted.
	p->iSignalMask = 0;		// Prevents any spurrious notifications, etc.

	TName name;
	name.Format(_L("%d"),aUnit);
	p->SetName(&name);
	p->iPortName = aUnit;
#if defined (_DEBUG_CONSOLE_)
	name.Format(_L("Comm::%d"),aUnit);
	p->iConsole.SetTitle(name);
#if defined (_DEBUG_DEVCOMM)
	p->iDumper=CCommDebugDumper::NewL(p->iConsole);
#endif
#endif
	CleanupStack::Pop();

	return p;
	}

void CHWPort::SetLoopbackPort(CHWPort* aHWPort)
/**
 * This method sets up the loopback port member of the CHWPort.  It is used after
 * both ports have been created (NewL).  This allows each port to "know" to whom he is
 * connected.
 *
 * @param	aHWPort - the port which THIS port should be connected to.
 *
 * @return	None
 */
	{
	if (aHWPort != NULL)
		{
		LOGTEXT3(_L8("SetLoopbackPort:  Unit %d-%d"), iPortName, aHWPort->iPortName);
		}
	else
		{
		LOGTEXT2(_L8("Loopback:SetLoopbackPort:  Unit %d..."), iPortName);
		}

	// Now set up the loopback
	iLoopbackPort=aHWPort;

	}


void CHWPort::CheckSigsAndCompleteRead()
/**
 * This method checks the configuration of the port and the current state of the signals
 * and determines if the read should even attempt to be completed.  Based on configurations,
 * reads can fail or be left pending (KConfigObeyXXX).  This method calls TryToCompleteRead
 * if all configuration and signal state allows the read to continue.
 *
 * Note that this method is called to either complete a pended read (from WriteBuf) or
 * to complete an incoming read request (from StartRead).  Also note that this routine does
 * not check the fail flags.  These flags are checked when the read is first posted and
 * when the signals are first asserted/deasserted.  It is unnecessary (and maybe even undesirable)
 * to check the fail flags here.
 *
 * @param	None
 *
 * @return	None
 */
	{
	LOGI(_L8("CheckSigsAndCompleteRead:%04d %x \"%S\""));

	LOGTEXT2(_L8("Loopback:CheckSigsAndCompleteRead:  Unit %d..."), iPortName);


	// At this point, we must check the config flags and signals.  If we have been
	// configured to obey signals, then we must pend the read and not complete it.
	// Note that the iReadPending flag was set in StartRead().
	if (ECommRoleDTE == iRole)
		{
		if (((iConfig().iHandshake & KConfigObeyDCD) && (!(iSignals & KSignalDCD))) ||
			((iConfig().iHandshake & KConfigObeyDSR) && (!(iSignals & KSignalDSR))))
			{
			LOGI(_L8("CheckSigsAndCompleteRead DTE:%04d %x \"%S\""));
			return;
			}
		}
	else if (ECommRoleDCE == iRole)
		{
		if ((iConfig().iHandshake & (KConfigObeyDTR)) && (!(iSignals & (KSignalDTR))))
			{
			LOGI(_L8("CheckSigsAndCompleteRead DCE:%04d %x \"%S\""));
			return;
			}
		}


	// Can the request be satisfied now?  Note that we call TryToCompleteRead on THIS instance.
	// This means that CheckSigsAndCompleteRead must be called on the desired instance.
	TryToCompleteRead();
	LOGI(_L8("CheckSigsAndCompleteRead:%04d %x \"%S\""));
	}


void CHWPort::StartRead(const TAny* aClientBuffer,TInt aLength)
/**
 * This method queues a read operation to the driver.  If the read length is zero (which is a
 * special case used during initialization) the read completes immediately without being
 * concerned about any of the configuration flags.  Failure flags are checked in this routine.
 * If the port has been configured to fail (KConfigFailXXX) and the signals are NOT asserted,
 * then the read will fail immediately with KErrCommsLineFail.  Obey flags are not handled here,
 * but are handled in a different method.  The code was structured in this manner so that the
 * Obey flags are checked any time a read is attempted to be completed, not just when the read
 * is initially sent to the driver.  This is needed because reads can pend and before the reads
 * are completed the signal state could change.
 *
 * @param	aClientBuffer	- a TAny * to the buffer into which data is placed.
 * @param	aLength			- the length of the buffer.  If the length is less than zero the
 *                            read can be completed with less than length bytes available.  If
 *                            a positive length value is specified, the read will not complete
 *                            until length bytes have been read.
 *
 * @return	None
 */
	{

	//	DEBUG_TRACE((iConsole.Write(_L("DoRead \n\r"))));
	LOGI(_L8("LOGIStartRead:%04d %x \"%S\""));

	LOGTEXT2(_L8("Loopback:StartRead:  Unit %d..."), iPortName);

	// Because a length of zero is a special case, we will complete this without
	// worries about the config/fail flags.
	if(aLength==0)
		{
		ReadCompleted(KErrNone);

		LOGI(_L8("StartRead 0:%04d %x \"%S\""));

		return;
		}


	// At this point, we must check the config flags and signals.  If we have been
	// configured to fail operations if certain signals are not set, then we must
	// fail the read.
	if (ECommRoleDTE == iRole)
		{
		if (((iConfig().iHandshake & KConfigFailDCD) && (!(iSignals & KSignalDCD))) ||
			((iConfig().iHandshake & KConfigFailDSR) && (!(iSignals & KSignalDSR))))
			{
			ReadCompleted(KErrCommsLineFail);
			LOGI(_L8("XtartRead DTE:%04d %x \"%S\""));
			return;
			}
		}
	else if (ECommRoleDCE == iRole)
		{
		if ((iConfig().iHandshake & (KConfigFailDTR)) && (!(iSignals & (KSignalDTR))))
			{
			ReadCompleted(KErrCommsLineFail);
			LOGI(_L8("StartRead DCE:%04d %x \"%S\""));
			return;
			}
		}


	// At this point, we set up for the read.  If the obey flags later don't let us
	// complete the read, having this work done is vital.
	iReadOneOrMore=EFalse;
	if(aLength<0)
		{
		aLength=-aLength;
		iReadOneOrMore=ETrue;
		}

	iBytesWritten=0;
	iPendingLength=aLength;
	iClientReadBuffer=aClientBuffer;
	iReadPending=ETrue;

	// Later code will assert iLoopback, so check it now to avoid crashes during startup.
	// The CheckSigsAndCompeteRead method will actually process the Obey flags.
	if (iLoopbackPort != NULL)
		{
		CheckSigsAndCompleteRead();
		}

	LOGI(_L8("StartRead:%04d %x \"%S\""));
	}

void CHWPort::ReadCancel()
/**
 * Cancel a pending read and complete it with KErrCancel.  The handling of the CActive class
 * will by default complete any outstanding read with KErrCancel, but it is cleaner to handle
 * that processing here.
 *
 * @param	None
 *
 * @return	None
 */
	{
	LOGI(_L8("ReadCancel:%04d %x \"%S\""));

	LOGTEXT2(_L8("Loopback:ReadCancel:  Unit %d..."), iPortName);

	iReadPending=EFalse;

	ReadCompleted(KErrCancel);

	}


TInt CHWPort::WriteBuf(const TAny* aClientBuffer,TInt aLength, TBool& aIssueComplete)
/**
 * This method writes the client buffer to the loopback port.  It must take into consideration
 * all of the KConfigFailXXX and KConfigObeyXXX flags that might have been specified in the
 * configuration of the port.  Any fail configuration results with writes failing with the
 * KErrCommsLineFail error.  This method is used both when the write is originally posted to
 * the driver and when trying to finish any write that was pended by the driver based on the
 * Obey flags.  This routine does not actually complete the read, but returns a value with
 * which the caller should complete the read.  Because this routine can also leave the operation
 * pended, it uses the aIssueComplete boolean to tell the caller whether or not to complete the
 * read.
 *
 * @param	aClientBuffer	- a TAny * to the buffer which contains the data to write
 * @param	aLength			- the length of the buffer.
 * @param	aIssueComplete	- a reference to a boolean used to indicate to the caller if the
 *                            write operation should be completed.
 *							- ETrue  - Caller should issue the WriteCompleted with returned
 *							           result code.
 *							- EFalse - Caller shoud not issue the WriteCompleted.
 *
 * @return	KErrNone		- Everything is okay
 * @return	KErrCommsLineFail  - Write failed based on signal state and configuration.
 * @return	Varies			- Non-Zero return from Kernel calls
 */

	{
	TInt res=KErrNone;

	LOGI(_L8("WriteBuf:%04d %x \"%S\""));

	// Plan on completeing, so set aIssueComplete to TRUE.
	aIssueComplete = ETrue;


	LOGTEXT3(_L8("Config 0x%x  Sigs 0x%x"), iConfig().iHandshake, iSignals);

	// At this point, we must check the config flags and signals.  Note that Fail
	// flags always take precedence over Obey flags.
	if (ECommRoleDTE == iRole)
		{
		// if we are configured to fail if CD, DSR, or CTS are not present, then we
		// must fail the write here.
		if (((iConfig().iHandshake & KConfigFailDCD) && (!(iSignals & KSignalDCD))) ||
			((iConfig().iHandshake & KConfigFailDSR) && (!(iSignals & KSignalDSR))) ||
			((iConfig().iHandshake & KConfigFailCTS) && (!(iSignals & KSignalCTS))))
			{
			LOGTEXT2(_L8("WriteBuf:  DTE Failure  Unit %d..."), iPortName);
			LOGTEXT3(_L8("...Config 0x%x  Sigs 0x%x..."), (iConfig().iHandshake), iSignals);
			res=KErrCommsLineFail;
			LOGI(_L8("WriteBuf:%04d %x \"%S\""));
			return res;
			}

		// At this point, we must check the config flags and signals.  If we have been
		// configured to obey signals, then we must pend the write and not complete the
		// write.
		if (((iConfig().iHandshake & KConfigObeyDCD) && (!(iSignals & KSignalDCD))) ||
			((iConfig().iHandshake & KConfigObeyDSR) && (!(iSignals & KSignalDSR))) ||
			((iConfig().iHandshake & KConfigObeyCTS) && (!(iSignals & KSignalCTS))))
			{
			LOGTEXT2(_L8("WriteBuf:  DTE Pended:  Unit %d..."), iPortName);
			iWritePending = ETrue;
			iClientWriteBuffer = aClientBuffer;
			iWritePendingLength = aLength;
			aIssueComplete = EFalse;
			res=KErrNone;
			LOGI(_L8("WriteBuf:%04d %x \"%S\""));
			return res;
			}
		}
	else if (ECommRoleDCE == iRole)
		{
		// if we are configured to fail when DTR or RTS are not present, then
		// fail the write here.
		if (((iConfig().iHandshake & KConfigFailRTS) && (!(iSignals & KSignalRTS))) ||
			((iConfig().iHandshake & KConfigFailDTR) && (!(iSignals & KSignalDTR))))
			{
			LOGTEXT2(_L8("WriteBuf:  DCE Failure...  Unit %d..."), iPortName);
			LOGTEXT3(_L8("...Config 0x%x  Sigs 0x%x..."), (iConfig().iHandshake), iSignals);
			res=KErrCommsLineFail;
			LOGI(_L8("WriteBuf:%04d %x \"%S\""));
			return res;
			}

		// At this point, we must check the config flags and signals.  If we have been
		// configured to obey signals, then we must pend the write and not complete the
		// write.
		if (((iConfig().iHandshake & KConfigObeyRTS) && (!(iSignals & KSignalRTS))) ||
			((iConfig().iHandshake & KConfigObeyDTR) && (!(iSignals & KSignalDTR))))
			{
			LOGTEXT2(_L8("WriteBuf:  DCE Pended:  Unit %d..."), iPortName);
			LOGTEXT3(_L8("...Config 0x%x  Sigs 0x%x..."), (iConfig().iHandshake), iSignals);
			iWritePending = ETrue;
			iClientWriteBuffer = aClientBuffer;
			iWritePendingLength = aLength;
			aIssueComplete = EFalse;
			res=KErrNone;
			LOGI(_L8("WriteBuf:%04d %x \"%S\""));
			return res;
			}
		}

	// Resize the receiving buffer if it is not big enough or it is
	// more than twice as big as it needs to be for this write.
	if ((iPtr.Length() + aLength) > iBufSize)
		{
		// New buffer size will be just big enough to fit the existing
		// data + new data, rounded up to nearest multiple of
		// KBufferGrowthIncrement.
		TInt newBufSize;

		if (((iPtr.Length() + aLength) % KBufferGrowthIncrement) == 0)
			{
			newBufSize = iPtr.Length() + aLength;

			}
		else
			{
			// New buffer size = length of data already in the buffer
			//					 + length of data to be added to the buffer
			//					 + round up to next KBufferGrowthIncrement
			newBufSize = iPtr.Length() + aLength
						 + KBufferGrowthIncrement - ((iPtr.Length() + aLength)%KBufferGrowthIncrement);
			}


		// Need to resize the buffer
		// Note: iBuf should not be deleted, buffer may be emptied
		//       later if this ReAllocL fails and tmpBuffer remains NULL.
	    HBufC8* tmpBuffer = NULL;
	    TRAP(res, tmpBuffer = iBuf->ReAllocL(newBufSize));

		if (tmpBuffer == NULL || res != KErrNone)
			{
			// Could not reallocate the buffer (maybe not enough
			// memory) so write request fails.
			iWritePending = EFalse;

			// return with an Issue complete and an error message
			aIssueComplete = ETrue;
			LOGI(_L8("WriteBuf:%04d %x \"%S\""));
			return KErrNoMemory;
			}

		iBufSize = newBufSize;
		iBuf = tmpBuffer;


		// Note: the aIssueComplete flag has already been set to ETrue
		// so, don't need to set it again here.
		iWritePending = EFalse;
		}

	// Fill the receiving buffer
	if (aLength!=0)
		{
		// point to where the new data will be located in our buffer
		TPtr8 ptrWriteLocation((TUint8*)(iBuf->Ptr() + iPtr.Length()), aLength);

		res = IPCRead(aClientBuffer,ptrWriteLocation);  // Must go through correct port

		LOGTEXT2(_L8("Read \"%S\""), &ptrWriteLocation);
		if(res!=KErrNone)
			{
			LOGI(_L8("WriteBuf:%04d %x \"%S\""));
			return res;
			}

		// memory leak detection : we create a single memory leak, if our input buffer contains
		// special token string:
		_LIT8(KMemLeakToken, "--MemoryLeakToken--");
		TInt nRetVal = ptrWriteLocation.Compare(KMemLeakToken);
		if (nRetVal == 0)
			{
            // coverity [returned_pointer]
			// coverity [memory_leak] - create a memory leak intentionally, look at the comments above 
            TInt* pArr = new TInt[1024]; // deliberatly causing a memory leak here...
			}

		// set the iPtr to point to the new iBuffer and keep a record of data length
		iPtr.Set((TUint8*)iBuf->Ptr(), iPtr.Length() + aLength, iBufSize);
		}
	// Since we are completing a write on THIS instance, see if we can complete a read on
	// the loopback port.  A read could have been left pending.
	if (iLoopbackPort != NULL)
		{
		iLoopbackPort->CheckSigsAndCompleteRead();
		}

	return res;
	}

void CHWPort::TryToCompleteRead()
/**
 * This method attempts to complete reads, either as they are initially issued or at a later
 * time because they were pended when initially issued.  Data is moved from the buffer associated
 * with this port to a buffer that was supplied by the client when the read is issued.  The
 * read is completed when either the client buffer is full or when some data has been written
 * to the client buffer and the iReadOneOrMore member set.  This member data is set when then
 * initial read was passed a negative length value.  Using this method allows a read to complete
 * without filling the entire buffer.
 *
 * @param	None
 *
 * @return	None
 */

	{
	LOGI(_L8("TryToCompleteRead:%04d %x \"%S\""));

	LOGTEXT2(_L8("Loopback:TryToCompleteRead: Unit %d ..."), iPortName);

	ASSERT(iLoopbackPort);
	TPtr8& refLoopBackiPtr = iLoopbackPort->iPtr; // Loopback listening port's buffer

	// Is there a Data Available notification pending?
	TInt s = refLoopBackiPtr.Length();
	if(iDataNotify)
		{
		NotifyDataAvailableCompleted(KErrNone);
		iDataNotify=EFalse;
		}

	// Is there a Read Pending?
	if(!iReadPending)
		return;

	// If there's somthing in the buffer then try to copy that...
	TInt stillToBeWritten=iPendingLength-iBytesWritten;
	TInt len=Min(s, stillToBeWritten);

	// only point to the data we are going to read
	TPtrC8 ptr(refLoopBackiPtr.Ptr(), len);

	LOGTEXT2(_L8("Write \"%S\""), &ptr);

	// Search for terminator characters
	TBool terminatorFound = EFalse;
	const TText8* terminators = iConfig().iTerminator;
	TInt termNum;
	for (termNum=0; termNum < iConfig().iTerminatorCount; ++termNum)
		{
		TInt termIndex = ptr.Locate(terminators[termNum]);
		if (termIndex >= 0)
			{
			// Found the terminator; reduce the length of ptr and search for the next one
			len = termIndex + 1;
			ptr.Set(refLoopBackiPtr.Ptr(), len);
			terminatorFound = ETrue;
			}
		}

	TInt res=IPCWrite(iClientReadBuffer,ptr,iBytesWritten);
	if(res == KErrNone)
		{
		// Delete the read data from the buffer
		ASSERT(refLoopBackiPtr.Length() >= len);
		TPtrC8 source = refLoopBackiPtr.Right(refLoopBackiPtr.Length() - len);
		// a memory move occurs on each read. This must be inefficient
		refLoopBackiPtr.Copy(source);
		iBytesWritten+=len;
		}

	if((iBytesWritten==iPendingLength)||(iReadOneOrMore&&(s>=1))||terminatorFound)
		{
		LOGTEXT2(_L8("Loopback:TryToCompleteRead: Completing Read  Unit %d ..."), iPortName);
		iReadPending=EFalse;
		// a complete read has succeeded, now update the Ports memory usage
		iLoopbackPort->UpdatePortResources();
		ReadCompleted(res);
		}
	}

void CHWPort::UpdatePortResources()
/**
 * This method recalculate how much memory is actually required by a ports write buffer
 *
 * @param Not used
 *
 * @return None
 */
	{
	LOGTEXT2(_L8("Loopback:UpdatePortResources: Unit %d ..."), iPortName);


	TInt newBufSize = 0;

	// it is possible to have a length of 0 therefore we leave the buffer size
	// to KBufferGrowthIncrement
	if(iPtr.Length() > 0)
		{
		newBufSize = iPtr.Length() + KBufferGrowthIncrement - ((iPtr.Length()) % KBufferGrowthIncrement);
		}
	else
		{
		// the minimum buffer size is KBufferGrowthIncrement
		newBufSize = KBufferGrowthIncrement;
		}

	// if the buffer needs changing then resize it otherwise leave as is
	if(newBufSize != iBufSize)
		{
		TInt res = KErrNone;
		// Shrink the buffer to the new size calculated above. If this fails we return leaving memory as it was
		HBufC8* tmpBuffer = NULL;
	    TRAP(res, tmpBuffer = iBuf->ReAllocL(newBufSize));

		if (tmpBuffer == NULL || res != KErrNone)
			{
	   		// could not resize the buffer - return without modifying anything
	   		return;
			}

		iBuf = tmpBuffer;
		iBufSize = newBufSize;

		// update the public interface to our buffer.
		iPtr.Set((TUint8*)iBuf->Ptr(), iPtr.Length(), iBufSize);
		}
	}

TInt CHWPort::QueryReceiveBuffer(TInt& aLength) const
/**
 * This method returns the length of the buffer associated with this instance of the
 * port.
 *
 * @param	aLength			- a reference to return the length of the buffer.
 *
 * @return	KErrNone
 */
	{
	LOGTEXT2(_L8("Loopback:QueryReceiveBuffer:  Unit %d..."), iPortName);

	aLength=iPtr.Length();
	return KErrNone;
	}

void CHWPort::ResetBuffers(TUint)
/**
 * This method resets the buffer used by this loopback port
 *
 * @note Note that most ResetBuffers methods derived from CPort allow a parameter for flags.
 * This ResetBuffers method does not.
 *
 * @param	Not Used
 *
 * @return	None
 */
	{

	LOGI(_L8("ResetBuffers:%04d %x \"%S\""));

	LOGTEXT2(_L8("Loopback:ResetBuffers:  Unit %d..."), iPortName);

	// reset the length of data to zero and update port resources
	iPtr.Set((TUint8*)iBuf->Ptr(), 0, iBufSize);

	// this method will perform any memory re-sizing required on the port
	UpdatePortResources();
	}

void CHWPort::StartWrite(const TAny* aClientBuffer,TInt aLength)
/**
 * This method queues a write operation to the driver.  This method is simply the outside
 * interface to the class for writing data.  It calls a private method to actually process
 * the write.  StartWrite passes a reference to a boolean value to the private routine that
 * indicates whether or not StartWrite should issue a WriteCompleted response.  This is necessary
 * because some combinations of signals and configuration can cause writes to be pended and
 * thus NOT completed.
 *
 * @param	aClientBuffer	- a TAny * to the buffer into which data should be read.
 * @param	aLength			- the length of the data to be written.
 *
 * @return	None
 */
	{

	LOGI(_L8("StartWrite:%04d %x \"%S\""));

	LOGTEXT2(_L8("Loopback:StartWrite:  Unit %d..."), iPortName);

	// memory leak detection mechanism:
	// if special write buffer token is received is received, we create memory leak
	// this condition is triggered, when working with "te_C32_leakdetection.script"

	DEBUG_TRACE((iConsole.Write(_L("DoWrite \n\r"))));
	TBool issueComplete = ETrue;

	TInt res = KErrNone;

	if((iPtr.Length() + aLength) > KMaxBufferSize)
		{
		// we are exceeding our buffer growth size. Do not process the
		// write message
		iWritePending = EFalse;
		res = KErrNoMemory;
		}
	else
		{
		res = WriteBuf(aClientBuffer,aLength, issueComplete);
		}

	// Only complete the write if allowed to by WriteBuf.
	if (issueComplete)
		{
		LOGTEXT2(_L8("Loopback:StartWrite:  Completing Write Unit %d..."), iPortName);
		LOGTEXT2(_L8("Loopback:StartWrite:  Completing Write Unit %d..."), res);
		WriteCompleted(res);
		}

	LOGI(_L8("StartWrite:%04d %x \"%S\""));

	}

void CHWPort::WriteCancel()
/**
 * This method cancels a pending write and issues a WriteCompleted with the result
 * KErrCancel.  If no writes are pending, then this method simply returns.
 *
 * @param	None
 *
 * @return	None
 */

	{

	LOGI(_L8("WriteCancel:%04d %x \"%S\""));

	LOGTEXT2(_L8("Loopback:WriteCancel:  Unit %d..."), iPortName);


	// if there is a pending write (which could happen with the obey
	// flags), then we have to cancel the write.
	if (iWritePending)
		{
		iWritePending = EFalse;
		WriteCompleted(KErrCancel);
		}
	}

void CHWPort::Break(TInt /* aTime */)
/**
 * This method is currently not implemented in the loopback driver as breaks are
 * not supported.
 *
 * @param	Not Used
 *
 * @return	None
 */

//
// Queue a Break
//
	{}

void CHWPort::BreakCancel()
/**
 * This method is currently not implemented in the loopback driver as breaks are
 * not supported.
 *
 * @param	None
 *
 * @return	None
 */
//
// Cancel a pending break
//
	{}

TInt CHWPort::GetConfig(TDes8& aDes) const
/**
 * This gets the current configuration from the loopback driver.
 *
 * @param	aDes	- a TDes8 reference to copy the configuration into.
 *
 * @return	KErrNone
 */
	{

	LOGI(_L8("GetConfig:%04d %x \"%S\""));

	LOGTEXT2(_L8("Loopback:GetConfig:  Unit %d..."), iPortName);

	aDes.Copy(iConfig);
	return KErrNone;
	}

TInt CHWPort::SetConfig(const TDesC8& aDes)
/**
 * This sets the current configuration for the loopback driver.  Note that
 * no error checking is done when setting the configuration.
 *
 * @param	aDes	- a TDes8 reference to copy the configuration from.
 *
 * @return	KErrNone
 */
	{

	LOGI(_L8("SetConfig:%04d %x \"%S\""));

	LOGTEXT2(_L8("Loopback:SetConfig:  Unit %d..."), iPortName);

	iConfig.Copy(aDes);
	return KErrNone;
	}

TInt CHWPort::GetCaps(TDes8& aDes)
/**
 * This gets the supported capabilities from the loopback driver.  The actual capabilities of
 * the driver will vary based on the role the port is playing (DCE or DTE).  The loopback driver
 * supports capabilities via TCommCapsV01 and TCommCapsV02.
 *
 * @param	aDes				- a TDes8 reference to copy the capabilities into.
 *
 * @return	KErrNone			- Everything is okay.
 * @return	KErrNotSupported	- The length of the descriptor passed to this method indicates a
 *                                capabilities structure which we don't support.
 */
	{

	LOGI(_L8("GetCaps:%04d %x \"%S\""));

	LOGTEXT2(_L8("Loopback:GetCaps:  Unit %d..."), iPortName);

	if(aDes.Length()==sizeof(TCommCapsV01))
		{
		TCommCapsV01* commcaps=(TCommCapsV01*)(aDes.Ptr());

		// We've got all of these
		commcaps->iRate=0x3fffff;
		commcaps->iDataBits=0xf;
		commcaps->iStopBits=0x3;
		commcaps->iParity=0x1f;
		commcaps->iFifo=0x1;

		if (ECommRoleDTE == iRole)
			{
			commcaps->iHandshake= KCapsObeyCTSSupported | KCapsFailCTSSupported |
			                      KCapsObeyDSRSupported | KCapsFailDSRSupported |
			                      KCapsObeyDCDSupported | KCapsFailDCDSupported;
			}
		else
			{
			commcaps->iHandshake= KCapsObeyRTSSupported | KCapsObeyDTRSupported;
			}

		commcaps->iSignals=0x3f;
		commcaps->iSIR=0x0;
		return KErrNone;
		}
	else if(aDes.Length()==sizeof(TCommCapsV02))
		{
		TCommCapsV02* commcaps=(TCommCapsV02*)(aDes.Ptr());
	// We've got all of these
		commcaps->iRate=0x3fffff;
		commcaps->iDataBits=0xf;
		commcaps->iStopBits=0x3;
		commcaps->iParity=0x1f;
		commcaps->iFifo=0x1;

		if (ECommRoleDTE == iRole)
			{
			commcaps->iHandshake= KCapsObeyCTSSupported | KCapsFailCTSSupported |
			                      KCapsObeyDSRSupported | KCapsFailDSRSupported |
			                      KCapsObeyDCDSupported | KCapsFailDCDSupported |
			                      KCapsFreeRTSSupported | KCapsFreeDTRSupported;
			}
		else
			{
			commcaps->iHandshake= KCapsObeyRTSSupported | KCapsObeyDTRSupported;
			}

		commcaps->iSignals=0x3f;
		commcaps->iSIR=0x0;
		commcaps->iNotificationCaps=KNotifySignalsChangeSupported;
		commcaps->iRoleCaps=0x0;
		return KErrNone;
		}
	else
		return KErrNotSupported;
	}

TInt CHWPort::SetServerConfig(const TDesC8& aDes)
/**
 * This sets the current server configuration for the loopback driver.  The loopback driver
 * stores this information but does nothing with it.
 *
 * @param	aDes	- a TDes8 reference to copy the configuration from.
 *
 * @return	KErrNone
 */
	{

	LOGI(_L8("SetServerConfig:%04d %x \"%S\""));

	LOGTEXT2(_L8("Loopback:SetServerConfig:  Unit %d..."), iPortName);

	iServerConfig.Copy(aDes);
	return KErrNone;
	}

TInt CHWPort::GetServerConfig(TDes8& aDes)
/**
 * This gets the current server configuration for the loopback driver.  The loopback driver
 * stores this information but does nothing with it other than return it here.
 *
 * @param	aDes	- a TDes8 reference to copy the configuration to.
 *
 * @return	KErrNone
 */
	{

	LOGI(_L8("GetServerConfig:%04d %x \"%S\""));

	LOGTEXT2(_L8("Loopback:GetServerConfig:  Unit %d..."), iPortName);

	aDes.Copy(iServerConfig);
	return KErrNone;
	}

TInt CHWPort::GetSignals(TUint& aSignals)
/**
 * This method retrieves the current setting of the signals for THIS port.
 *
 * @param	aSignals	- A reference to a TUint to return the signal settings.
 *
 * @return	KErrNone
 */
	{

	LOGI(_L8("GetSignals:%04d %x \"%S\""));

	LOGTEXT3(_L8("Loopback:GetSignals:  Unit %d... Sigs 0x%x"), iPortName, iSignals);

	aSignals=iSignals;
	return KErrNone;
	}

TInt CHWPort::SetSignalsToMark(TUint aSignals)
/**
 * This method asserts the signals specified by the parameter aSignals.  In addition to
 * simply asserting the signals, this routine will complete any signal notification requests
 * that are outstanding.  After handling any signal notification requests, this routine will
 * also update the signal state of it's partner loopback port.  The other port's signals are
 * updated only if they are considered output signals for the role that this port is playing.
 * For example, assume that a DTE port is setting the RTS signal.  It determines that RTS is
 * an output signal for a DTE port, so it must propagate the signal to the DCE by calling
 * this routine on the DCE instance of the port.  When the DCE instance runs, it sets RTS, then
 * determines that RTS is NOT a DCE output signal, so it does NOT attempt to propagate the signal.
 * It is NOT an error for a DCE port to call this routine with DTE output signals.  In fact,
 * this behaviour is required in order to propagate signal settings from one port to the other.
 *
 * After propagating the signal settings, this method will attempt to complete reads and writes
 * that have been outstanding, based on the changed signals and the configuration of the
 * port.  For example, say that the DTE port had been configured to Obey CTS (i.e., to flow control
 * when CTS is not asserted).  When the DCE asserts CTS, this signal would be propagated to the
 * DTE port.  Then the DTE port would check the current configuration and note that CTS has
 * been asserted.  At this point, there could be operations which were pended (because of the
 * lack of CTS) that should be attempted.  This method will attempt these operations.
 *
 *
 * @param	aSignals	- a bitmask specifying which signals to assert (See definition
 *                        of KSignalDCD, KSignalCTS, etc. for bit values).
 *
 * @return	KErrNone
 */
	{

	TBool attemptRead = EFalse;
	TBool attemptWrite = EFalse;
	TUint sigsChanged;
	TUint tmpSigs;



	LOGI(_L8("SetSignalsToMark:%04d %x \"%S\""));


	LOGTEXT3(_L8("Loopback:SetSignalsToMark:  Unit %d...Sigs 0x%x"), iPortName, aSignals);

	// If no signals are passed in to set, then get out of here.  This is possible when
	// the upper layers have used the RComm::SetSignals interface.  This interface is used
	// to both space and mark signals with a single call.  Frequently, this interface is used
	// with one of the two masks set to zero.
	if (!aSignals)
		return KErrNone;

	tmpSigs = iSignals;
	iSignals |= aSignals;

	// sigsChanged contains only the signals changed by this operation, no history is contained.
	sigsChanged = (iSignals ^ tmpSigs);

	// Only complete notifications if the changed sigs were specified in the mask.
	if (sigsChanged & iSignalMask)
		{
		// Notify people that the signals have changed.
		// Note that the KSignalChanged bits are stored in sigsChanged, but passed.
		// The tmpSigs value has the state of all the signals that they were interested in
		// as specified by the iSignalMask and the Changed flags.
        tmpSigs = (((sigsChanged & iSignalMask) * KSignalChanged) | (iSignals & iSignalMask));
		SignalChangeCompleted(tmpSigs, KErrNone);
		// Reset signal mask, another NotifiySignalChange is necessary to get any
		// more signal information out of the driver.
		iSignalMask = 0;
		}

	// if I'm a DTE port and the signals changed are DTE outputs, then
    // I've got to figure out what signals to change on the DCE side (as inputs).
 	// else if I'm a DCE port and the signals changed are DCE outputs, then
    // I've got to figure out what DTE signals I've got to change.
    if ((ECommRoleDTE == iRole) && (sigsChanged & KSignalDTEOutputs))
		{
		if (iLoopbackPort)
			iLoopbackPort->SetSignalsToMark(aSignals);
        }
	else if ((ECommRoleDCE == iRole) && (sigsChanged  & KSignalDCEOutputs))
		{
		if (iLoopbackPort)
			iLoopbackPort->SetSignalsToMark(aSignals);
		}


	// if we dropped DCD, CTS, or DSR, then we need to complete any outstanding writes and
    // reads on the DTE port if they have been configured to fail.
	// We only look at the DTE input signals to see if the roles, etc. change.
	if (ECommRoleDTE == iRole)
		{

		// DCD and DSR affect both Reads and Writes, so we'll want to attempt both
		// reads and writes later.
		if (((iConfig().iHandshake & KConfigObeyDCD) && (sigsChanged & KSignalDCD)) ||
			((iConfig().iHandshake & KConfigObeyDSR) && (sigsChanged & KSignalDSR)))
			{
			attemptRead = attemptWrite = ETrue;
			}

		// CTS has NO effect on the Reads, so don't attempt to complete any reads here.
		// No need to do this if we executed the above if ...
		else if (((iConfig().iHandshake & KConfigObeyCTS) && (sigsChanged & KSignalCTS)))
			{
			attemptWrite = ETrue;
			}
		}
	// if the DTE dropped the signals (RTS and DTR) the complete actions for the DCE
	// only look at the DCE input signals.
	else if (ECommRoleDCE == iRole)
		{
		// if DTR has changed, then we need to try to complete both reads and writes.
		if (((iConfig().iHandshake & KConfigObeyDTR) && (sigsChanged & KSignalDTR)))
			{
			attemptRead = attemptWrite = ETrue;
			}
		// if RTS has changed, only attempt the Writes, RTS does not effect Reads.
		// No need to do this if we executed the above if ...
		else if (((iConfig().iHandshake & KConfigObeyRTS) && (sigsChanged & KSignalRTS)))
			{
			attemptWrite = ETrue;
			}

		}

    // Attempt to complete any writes if necessary.  Note that if we do a write for THIS
	// port, it will attempt to complete a read for the Loopback port.  Because this method
	// calls itself (on the other port), we can end up trying to complete reads and writes
	// a couple of times.  By checking the pending flags for writes and reads, we should
	// avoid this extra work (even though it probably would not hurt anything).
	if ((attemptWrite) && (iWritePending))
		{
		TBool issueComplete = EFalse;
		TInt res=WriteBuf(iClientWriteBuffer,iWritePendingLength, issueComplete);

		if (issueComplete)
			{
			WriteCompleted(res);
			}
		}

	// Attempt to complete any reads if necessary.  See comment above writes for information
	// regarding the use of the pending flags.  Note that we call this on our own port.  This
	// is by design.  If a write was attempted, then it called the the read completion on the
	// other port.
	if ((attemptRead) && (iReadPending))
	{
		CheckSigsAndCompleteRead();
	}


	return KErrNone;
	}



TInt CHWPort::SetSignalsToSpace(TUint aSignals)
/**
 * This method de-asserts the signals specified by the parameter aSignals.  In addition to
 * simply de-asserting the signals, this routine will complete any signal notification requests
 * that are outstanding.  After handling any signal notification requests, this routine will
 * also update the signal state of it's partner loopback port.  The other ports signals are
 * updated only if they are considered output signals for the role that this port is playing.
 * For example, assume that a DTE port is deasserting the RTS signal.  It determines that RTS is
 * an output signal for a DTE port, so it must propagate the signal to the DCE by calling
 * this routine on the DCE instance of the port.  When the DCE instance runs, it clears RTS, then
 * determines that RTS is NOT a DCE output signal, so it does NOT attempt to propagate the signal.
 * It is NOT an error for a DCE port to call this routine with DTE output signals.
 * In fact, this behaviour is required in order to  * propagate signal settings from one port
 * to the other.
 *
 * If signals are de-asserted, then it may be necessary to FAIL pending operations.  For example,
 * if the port is configured to Fail if DCD is de-asserted and there is a read pending, this
 * routine will complete the outstanding read with KErrCommsLineFail.
 *
 *
 * @param	aSignals	- a bitmask specifying which signals to assert (See definition
 *                        of KSignalDCD, KSignalCTS, etc. for bit values).
 *
 * @return	KErrNone
 */
	{
	TBool completeRead = EFalse;
	TBool completeWrite = EFalse;
	TUint sigsChanged;
	TUint tmpSigs;

	LOGI(_L8("SetSignalsToSpace:%04d %x \"%S\""));

	LOGTEXT3(_L8("Loopback:SetSignalsToSpace:  Unit %d...Sigs 0x%x"), iPortName, aSignals);


	// If no signals are passed in to set, then get out of here.  This is possible when
	// the upper layers have used the RComm::SetSignals interface.  This interface is used
	// to both space and mark signals with a single call.  Frequently, this interface is used
	// with one of the two masks set to zero.
	if (!aSignals)
		return KErrNone;


	// iSignals is used to store the current state of the signals only, it does not
	// include the changed masks.  This is so that history will not be reflected.
	tmpSigs = iSignals;
	iSignals &= ~aSignals;

	// sigsChanged contains only the signals changed by this operation, no history is contained.
	sigsChanged = (iSignals ^ tmpSigs);

	// Only complete notifications if the changed sigs were specified in the mask.
	if (sigsChanged & iSignalMask)
		{
		// Notify people that the signals have changed.
		// Note that the KSignalChanged bits are stored in sigsChanged, but passed.
		// The tmpSigs value has the state of all the signals that they were interested in
		// as specified by the iSignalMask and the Changed flags.
		tmpSigs = (((sigsChanged & iSignalMask) * KSignalChanged) | (iSignals & iSignalMask));
		SignalChangeCompleted(tmpSigs, KErrNone);
		// Reset signal mask, another NotifiySignalChange is necessary to get any
		// more signal information out of the driver.
		iSignalMask = 0;
		}

	// if I'm a DTE port and the signals changed are DTE outputs, then
    // I've got to figure out what signals to change on the DCE side (as inputs).
 	// else if I'm a DCE port and the signals changed are DCE outputs, then
    // I've got to figure out what DTE signals I've got to change.

	// The DTE Role could be ignored safely (Req7) but as long as the test
    // harness does not ever request notification, it won't matter.  This is
    // for potential future use.
    if ((ECommRoleDTE == iRole) && (sigsChanged & KSignalDTEOutputs))
		{
		if (iLoopbackPort)
			iLoopbackPort->SetSignalsToSpace(aSignals);
        }
	else if ((ECommRoleDCE == iRole) && (sigsChanged & KSignalDCEOutputs))
		{
		if (iLoopbackPort)
			iLoopbackPort->SetSignalsToSpace(aSignals);
		}

	// if we dropped DCD, CTS, or DSR, then we need to complete any outstanding writes and
    // reads on the DTE port if they have been configured to fail.
	// We only look at the DTE input signals to see if the roles, etc. change.
	if (ECommRoleDTE == iRole)
		{
		// DCD and DSR affect both Reads and Writes, so we'll want to complete both
		// reads and writes later.
		if (((iConfig().iHandshake & KConfigFailDCD) && (sigsChanged & KSignalDCD)) ||
			((iConfig().iHandshake & KConfigFailDSR) && (sigsChanged & KSignalDSR)))
			{
			completeRead = completeWrite = ETrue;
			}

		// CTS has NO effect on the Reads, so don't attempt to complete any reads here.
		// No need to do this if we executed the above if ...
		else if (((iConfig().iHandshake & KConfigFailCTS) && (sigsChanged & KSignalCTS)))
			{
			completeWrite = ETrue;
			}
		}
	// if the DTE dropped the signals (RTS and DTR) the complete actions for the DCE
	// only look at the DCE input signals.
	else if (ECommRoleDCE == iRole)
		{
		// if DTR has changed, then we need to try to complete both reads and writes.
		if (((iConfig().iHandshake & KConfigFailDTR) && (sigsChanged & KSignalDTR)))
			{
			completeRead = completeWrite = ETrue;
			}
		// if RTS has changed, only attempt the Writes, RTS does not effect Reads.
		// No need to do this if we executed the above if ...
		else if (((iConfig().iHandshake & KConfigFailRTS) && (sigsChanged & KSignalRTS)))
			{
			completeWrite = ETrue;
			}
		}

	// Note:  We don't have to work with the Obey flags when we set signals.  The obey flags
	// when something is set force future operations to be pended.  Operations currently pended
	// or already completed don't have any effect.  If any of the signals were treated as Active
	// Low (or Asserted means error condition) then we would have to attempt to complete
	// reads or writes.
	//

	// if we need to complete the read and there is one pending, fail it.
	if ((completeRead) && (iReadPending))
		{
		ReadCompleted(KErrCommsLineFail);
		iReadPending = EFalse;
		}

	// if we need to complete writes, do it here.
	if ((completeWrite) && (iWritePending))
		{
		WriteCompleted(KErrCommsLineFail);
		iWritePending = EFalse;
		}

	return KErrNone;
	}

TInt CHWPort::GetReceiveBufferLength(TInt& /* aLength */) const
/**
 * This method is currently not implemented in the loopback driver.  Calling this
 * method will return an error
 *
 * @param	Not Used
 *
 * @return	KErrNotSupported
 */
	{

	LOGI(_L8("GetReceiveBufferLength:%04d %x \"%S\""));
	return KErrNotSupported;
	}

TInt CHWPort::SetReceiveBufferLength(TInt /* aLength */)
/**
 * This method is currently not implemented in the loopback driver.  Calling this
 * method will return an error
 *
 * @param	Not Used
 *
 * @return	KErrNotSupported
 */
	{
	LOGI(_L8("SetReceiveBufferLength:%04d %x \"%S\""));
	return KErrNotSupported;
	}

#ifdef _DEBUG_DEVCOMM
// This code will not compile given current class structure, etc.  It is left here for
// future reference.
void CHWPort::DoDumpDebugInfo(const RMessage2 &aMessage)
	{
	TCommDebugInfoPckg d;
	if (iRole==ECommRoleDTE)
		iPort.DebugInfo(d);
	else
		iPortDCE.DebugInfo(d);
	TRAPD(leave,aMessage.WriteL(0,d));		// trap but ignore leaves
	aMessage.Complete(KErrNone);
	}
#endif

void CHWPort::Destruct()
/**
 * This method is simply deletes this instance of the port, comitting sucide.
 *
 * @param	None
 *
 * @return	None
 */
	{
	delete this;
	}



void CHWPort::NotifySignalChange(TUint aSignalMask)
/**
 * This method sets up a request to be notified when a signal change occurs on the specified
 * signals.  Later operations will send a message to the requestor indicating the signal
 * change.
 *
 * @param	aSignalMask		-	the signals that the caller is interested in monitoring.
 *
 * @return	None
 */
	{

	LOGTEXT3(_L8("Loopback:NotifySignalChange:  Unit %d...Mask 0x%x"), iPortName, aSignalMask);

	iSignalMask|=aSignalMask;

	}


void CHWPort::NotifySignalChangeCancel()
/**
 * This method cancels an outstanding request to be notified when a signal change occurs.  Any
 * outstanding signal change request will be completed with KErrCancel.
 *
 * @param	None
 *
 * @return	None
 */
	{

	LOGTEXT2(_L8("Loopback:NotifySignalChangeCancel:  Unit %d..."), iPortName);

	if (iSignalMask != 0)
		{
		// Complete any outstanding notifications with KErrCancel
		SignalChangeCompleted(0, KErrCancel);
		iSignalMask = 0;	// set mask to zero
		}
	}


void CHWPort::NotifyConfigChange()
/**
 * This method is currently not implemented in the loopback driver.
 *
 * @param	None
 *
 * @return	None
 */
	{}

void CHWPort::NotifyConfigChangeCancel()
/**
 * This method is currently not implemented in the loopback driver.
 *
 * @param	None
 *
 * @return	None
 */
	{}

void CHWPort::NotifyFlowControlChange()
/**
 * This method is currently not implemented in the loopback driver.
 *
 * @param	None
 *
 * @return	None
 */
	{}

void CHWPort::NotifyFlowControlChangeCancel()
/**
 * This method is currently not implemented in the loopback driver.
 *
 * @param	None
 *
 * @return	None
 */
	{}


void CHWPort::NotifyBreak()
/**
 * This method is currently not implemented in the loopback driver.
 *
 * @param	None
 *
 * @return	None
 */
	{}

void CHWPort::NotifyBreakCancel()
/**
 * This method is currently not implemented in the loopback driver.
 *
 * @param	None
 *
 * @return	None
 */
	{}

void CHWPort::NotifyDataAvailable()
/**
 * Wake up when data is sent by the other side
 *
 * @param	None
 *
 * @return	None
 */
	{
	iDataNotify=ETrue;
	CheckSigsAndCompleteRead();
	}

void CHWPort::NotifyDataAvailableCancel()
/**
 * Cancel data available notification
 *
 * @param	None
 *
 * @return	None
 */
	{
	iDataNotify=EFalse;
	NotifyDataAvailableCompleted(KErrCancel);
	}

void CHWPort::NotifyOutputEmpty()
/**
 * This method is currently not implemented in the loopback driver.
 *
 * @param	None
 *
 * @return	None
 */
	{}

void CHWPort::NotifyOutputEmptyCancel()
/**
 * This method is currently not implemented in the loopback driver.
 *
 * @param	None
 *
 * @return	None
 */
	{
	}

TInt CHWPort::GetFlowControlStatus(TFlowControl& /* aFlowControl */)
/**
 * This method is currently not implemented in the loopback driver.
 *
 * @param	Not Used
 *
 * @return	KErrNotSupported
 */
	{
	return KErrNotSupported;
	}

TInt CHWPort::GetRole(TCommRole& aRole)
/**
 * This method returns the current Role that this port is playing (ECommRoleDCE or ECommRoleDTE)
 *
 * @param	aRole	-	a reference to a TCommRole to return the role value in.
 *
 * @return	KErrNone
 */
	{
	LOGTEXT2(_L8("Loopback:GetRole:  Unit %d..."), iPortName);

	aRole=iRole;
	return KErrNone;
	}

TInt CHWPort::SetRole(TCommRole aRole)
/**
 * This method sets the role of the port.  Additionally, it sets the default state of the
 * signals for this type of port.  This is the first place where signals are set as a port
 * is opening.  The ports will assert their output signals with the exception of DCD (which
 * is an output signal from the DCE).  DCD is left to be driven by the test harness.
 *
 * A test could be put into place to insure that each port is in a different role.  This was
 * not done at this time for backwards compatibility reasons.
 *
 * @param	aRole	-	the TCommRole value that this port should be set to.
 *
 * @return	None
 */
	{
	LOGTEXT2(_L8("Loopback:SetRole:  Unit %d..."), iPortName);

	if (ECommRoleDTE == aRole)
		{
		SetSignalsToMark(KSignalDTR|KSignalRTS);
		}
	else // DCE
		{
		SetSignalsToMark(KSignalDSR|KSignalCTS);
		}


	// Informational test only.  This will produce output to the log file if both sides have the
	// same role set.  With both sides having the same role, none of the signal handling will
	// function properly.
#if defined (_DEBUG)
	if (iLoopbackPort)
		{
		TCommRole otherSide;
		iLoopbackPort->GetRole(otherSide);
		if (otherSide == aRole)
			LOGTEXT3(_L8("Loopback:SetRole:  Unit %d...Both sides same role %d"), iPortName, aRole);
		}
#endif

	iRole = aRole;
	return KErrNone;
	}

CHWPort::~CHWPort()
/**
 * This method is the standard destructor for the port.  It deletes the buffer
 * which was allocated to the port and resets the loopback port pointer to NULL.
 *
 * @param	None
 *
 * @return	None
 */
	{


	delete iBuf;
	iBuf=NULL;
	iPtr.Set(NULL,0,0);
	if(iLoopbackPort)
		iLoopbackPort->SetLoopbackPort(NULL);
	((CHWPortFactory*)Owner())->Remove(this);

#if defined (_DEBUG_CONSOLE_)
#if defined (_DEBUG_DEVCOMM)
	delete iDumper;
#endif
	iConsole.Close();
#endif
	}

void CHWPort::FreeMemory()
/**
 * This method is currently not implemented in the loopback driver.
 *
 * @param	None
 *
 * @return	None
 */
	{}

 CPort* CHWPortFactory::NewPortL(const TUint aUnit)
/**
 * This method creates a new port object.  It identifies the new object with the unit number that
 * is supplied.  If both ports that are supported by the CHWPortFactory object have been created,
 * then the loopback ports are initialized.
 *
 * @param	aUnit		-	The unit number to create.
 *
 * @return	CPort *		-	A pointer to the newly created object.
 */
	{
	LOGTEXT2(_L8("Loopback:NewPortL:  Unit %d"), aUnit);
	if(aUnit >= (KLoopbackCount&~1))
		User::Leave(KErrNotSupported);

	CPort* newPort;
	if (iPort[aUnit])
		{
		LOGTEXT3(_L8("Loopback:NewPortL:  Unit %d already exists! @%x"), aUnit, iPort[aUnit]);
		}
	newPort=iPort[aUnit]=CHWPort::NewL(aUnit);

	if((iPort[aUnit])&&(PairedPort(aUnit)))
		{
		iPort[aUnit]->SetLoopbackPort(PairedPort(aUnit));
		PairedPort(aUnit)->SetLoopbackPort(iPort[aUnit]);
		}
	return newPort;
	}

void CHWPortFactory::Info(TSerialInfo &aSerialInfo)
/**
 * This method fills information into the passed structure.  It is required for factory objects.
 *
 * @param	aSerialInfo		-	a reference to the structure to fill in.
 *
 * @return	None
 */
	{
	aSerialInfo.iDescription=SERIAL_DESCRIPTION;
	aSerialInfo.iName=SERIAL_NAME;
	aSerialInfo.iLowUnit=KCommLowUnit;
	aSerialInfo.iHighUnit=KLoopbackCount - 1;
	}

CHWPortFactory::CHWPortFactory()
/**
 * This method is the constructor for the factory object.
 *
 * @param	None
 *
 * @return	None
 */
	{
	__DECLARE_NAME(_S("CHWPortFactory"));
	TName name(SERIAL_NAME);
	SetName(&name);
	iVersion=TVersion(KEC32MajorVersionNumber,KEC32MinorVersionNumber,KEC32BuildVersionNumber);
	}

 void CHWPortFactory::Remove(CHWPort* aPort)
/**
 * This method removes an instance of the CHWPort from the factory package CHWPortFactory.
 *
 * @param	aPort	-	The pointer to the CHWPort pointer to be removed from the factory object.
 *
 * @return	None
 *
 * @note	If the passed in value does not match a current port, this method will panic.
 */
 	{
	LOGTEXT2(_L8("Loopback:Remove:  Port %x"), aPort);
	for(TUint i=0; i<KLoopbackCount; i++)
		{
		if(iPort[i]==aPort)
			{
			iPort[i]=NULL;
			return;
			}
		}
	User::Panic(_L("CHWPortFactory Panic"),0);
	}

CHWPortFactory::~CHWPortFactory()
/**
 * This method is the destructor for the factory object.
 *
 * @param	None
 *
 * @return	None
 *
 */
	{

	LOGDESTROY();

	}

/**
Returns capabilities for requested port
*/
TSecurityPolicy CHWPortFactory::PortPlatSecCapability(TUint /*aPort*/) const
	{
	return TSecurityPolicy(TSecurityPolicy::EAlwaysPass);
	}

extern "C"
	{
	IMPORT_C CSerial * LibEntry(void);	// Force export
	}

EXPORT_C CSerial * LibEntry(void)
/**
 * This method is the library's main entry point.  It simply new's the factory object.
 *
 * @param	None
 *
 * @return	None
 *
 */
	{

	return new CHWPortFactory;
	}


#if defined(_DEBUG_CONSOLE_)
// This code will not compile given current class structure, etc.  It is left here for
// future reference.
RDebugConsole::RDebugConsole()
	{
	Create();
	Set(_L(""),TSize(64,15));

	}

void RDebugConsole::Printf(TRefByValue<const TDesC> aFmt,...)
//
// Print to a console screen.
//
	{

	VA_LIST list;
	VA_START(list,aFmt);
	TBuf<0x100> aBuf;
	aBuf.AppendFormatList(aFmt,list);
	Write(aBuf);
	}
#endif

#if defined (_DEBUG_DEVCOMM) && defined (_DEBUG_CONSOLE_)
CCommDebugDumper* CCommDebugDumper::NewL(RDebugConsole &aConsole)
	{
	CCommDebugDumper* p=new CCommDebugDumper(aConsole);
	return p;
	}

CCommDebugDumper::CCommDebugDumper(RDebugConsole &aConsole)
	:CActive(EPriorityStandard)
	{
	iRole=ECommRoleDTE;
	iConsole=&aConsole;
	CActiveScheduler::Add(this);
	SetActive();
	iConsole->Read(iKeystroke,iStatus);
	};

CCommDebugDumper::~CCommDebugDumper()
	{
	Cancel();
	}

void CCommDebugDumper::RunL()
	{
	TInt key=iKeystroke.Code();
	switch(key)
		{
		case 'd':
		case 'D':
			{
			TCommDebugInfoPckg d;
			if (iRole==ECommRoleDTE)
				iParent->DTEPort().DebugInfo(d);
			else
				iParent->DCEPort().DebugInfo(d);
			TCommDebugInfo& debug=d();
			iConsole->Printf(_L("rxbusy  : 0x%04x, rxHeld   : 0x%04x, \n\r"),debug.iRxBusy,debug.iRxHeld);
			iConsole->Printf(_L("txbusy  : 0x%04x, txHeld   : 0x%04x, \n\r"),debug.iTxBusy,debug.iTxHeld);
			iConsole->Printf(_L("drainRx : 0x%04x, fillTx   : 0x%04x\n\r"),debug.iDrainingRxBuf,debug.iFillingTxBuf);
			iConsole->Printf(_L("Txonchar: 0x%04x, TxOffchar: 0x%04x\n\r"),debug.iTxXon,debug.iTxXoff);
			iConsole->Printf(_L("RxonChar: 0x%04x, RxOffchar: 0x%04x\n\r"),debug.iRxXon,debug.iRxXoff);
			iConsole->Printf(_L("NumTX   : 0x%04x, NumRx    : 0x%04x\n\r"),debug.iTxChars,debug.iRxChars);
			iConsole->Printf(_L("TxLen   : 0x%04x, RxLen    : 0x%04x\n\r"),debug.iTxLength,debug.iRxLength);
			iConsole->Printf(_L("TxOffset: 0x%04x, RxOffset : 0x%04x\n\r"),debug.iTxOffset,debug.iRxOffset);
			iConsole->Printf(_L("TxInts  : 0x%04x, RxInts   : 0x%04x\n\r"),debug.iTxIntCount,debug.iRxIntCount);
			}
			break;
		case 's':
		case 'S':
			{
			TUint signals=0;
			if (iRole==ECommRoleDTE)
				signals=iParent->DTEPort().Signals();
			else
				signals=iParent->DCEPort().Signals();
			iConsole->Printf(_L("Signals: "));
			if (signals&KSignalCTS)
				iConsole->Printf(_L("CTS "));
			if (signals&KSignalDSR)
				iConsole->Printf(_L("DSR "));
			if (signals&KSignalDCD)
				iConsole->Printf(_L("DCD "));
			if (signals&KSignalRTS)
				iConsole->Printf(_L("RTS "));
			if (signals&KSignalDTR)
				iConsole->Printf(_L("DTR "));
			iConsole->Printf(_L("\n\r"));
			}
			break;
		default:
			break;
		}

	SetActive();
	iConsole->Read(iKeystroke,iStatus);
	};

void CCommDebugDumper::DoCancel()
	{
	iConsole->ReadCancel();
	}

#endif

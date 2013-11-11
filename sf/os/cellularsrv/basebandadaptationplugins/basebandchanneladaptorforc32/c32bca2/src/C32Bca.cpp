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
// C32 Baseband Channel Adaptor(BCA) APIs implementation.
// This file contains all the API implementation of the C32BCA interface for Symbian OS.
// 
//

/**
 @file
 @internalComponent
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "C32BcaTraces.h"
#endif

#include "C32Bca.h"
#include <cdbcols.h>
#include <commsdat_partner.h>

using namespace BasebandChannelAdaptation;
using namespace BasebandChannelAdaptation::C32Bca;

const static TUint KVersion = ((static_cast<TUint>(KC32BcaMajorVersionNumber)) << 16) | KC32BcaMinorVersionNumber;


/**
C++ constructor for an object representing a c32 async user

@param aUser the target of the notifications originating in C32
@param aPort the C32 port to be used with this object
@param aAoPriority the priority to assign to this active object
*/
CCommBase::CCommBase(MC32User& aUser, RComm& aPort, TInt aAoPriority):
	CActive(aAoPriority),
	iUser(aUser),
	iPort(aPort)
	{
	CActiveScheduler::Add(this);
	}
		
/**
C++ destructor */		
CCommBase::~CCommBase()
	{
	}

/**
C++ constructor 
The comm-reader has a higher priority than the comm-writer, to
ensure the reader gets the chance to receive the incomming data

@param aUser the target of the notifications originating in C32
@param aPort the C32 port to be used with this object
*/
CCommReader::CCommReader(MC32User& aUser, RComm& aPort):
	CCommBase(aUser, aPort, EPriorityHigh)
	{
	}

/** 
C++ destructor */
CCommReader::~CCommReader()
	{
	Cancel();
	}

/**
Cancels the outstanding read */
void CCommReader::DoCancel()
	{
	iPort.ReadCancel();
	}
		
/**
Issues a read on the serial port 

@param aBuf the buffer to read to */		
void CCommReader::Read(TDes8& aBuf)
	{
	iPort.ReadOneOrMore(iStatus, aBuf);	
	SetActive();
	}

/**
Called on read completion */	
void CCommReader::RunL()
	{
	iUser.CommReadComplete(iStatus.Int());
	}
	
/**
C++ constructor
The comm-writer has a lower priority than the comm-reader, to
ensure the reader gets the chance to receive the incomming data

@param aUser the target of the notifications originating in C32
@param aPort the C32 port to be used with this object */	
CCommWriter::CCommWriter(MC32User& aUser, RComm& aPort):
	CCommBase(aUser, aPort, EPriorityUserInput)
	{
	}

/** 
C++ destructor */
CCommWriter::~CCommWriter()
	{
	Cancel();
	}

/**
Issues a write on the serial port */
void CCommWriter::Write(const TDesC8& aBuf)
	{
	iPort.Write(iStatus, aBuf);
	SetActive();
	}
		
/**
Called when the write completes
*/
void CCommWriter::RunL()
	{
	iUser.CommWriteComplete(iStatus.Int());
	}

/**
Cancels the outstanding write
*/
void CCommWriter::DoCancel()
	{
	iPort.WriteCancel();
	}
		
/**
C++ constructor

@param aUser the target of the notifications originating in C32
@param aPort the C32 port to be used with this object


Note on Monitor AO priority:

Monitor AO is the same priority as the Reader / Writer by design. 

We do NOT want link failure notification to complete before all possible outstanding reads / writes
have completed, to make sure that we do not lose any data that was already received from the
baseband and is buffered by the driver or the CSY.
Consider a scenario where a UDP datagram is received by the CSY, and then the serial link is
severed (e.g. cable is unplugged by the user). If the Monitor has higher priority than reader/writer,
we may get failure notification before we are notified on the read of this UDP datagram, potentially
resulting in its loss (E.g. PPP shuts down when on Link Down notification.)
*/	
CCommLinkMonitor::CCommLinkMonitor(MC32User& aUser, RComm& aPort):
	CCommBase(aUser, aPort, EPriorityStandard) 
	{
	}

/**
Sets the object to monitor the link for the configured Role, DTE or DCE

@param aMask DTE or DCE: determines when the link is considered to be down.
*/
void CCommLinkMonitor::Setup(TUint32 aMask)
	{
	iNotifyChangeSignalMask = aMask;	
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCOMMLINKMONITOR_SETUP1_1, "CommLinkMonitor setup: Notify on signal mask [0x%X]",iNotifyChangeSignalMask);
	}

/**
Requests to be notified when the link is down.

Can be called an more than once
*/
void CCommLinkMonitor::NotifyLinkDown()
	{
	ASSERT(!IsActive());
		
	// get the current state of the line so when we get notification we can tell if it really has changed	
	iSavedSignalState = iPort.Signals(iNotifyChangeSignalMask) & iNotifyChangeSignalMask;
	iPort.NotifySignalChange(iStatus, iSignals, iNotifyChangeSignalMask);

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCOMMLINKMONITOR_NOTIFYLINKDOWN1_1, "CommLinkMonitor::NotifyLinkDown: initial signals: [0x%X]", iSavedSignalState);
	
	SetActive();
	}

/**	
Called when EIA-232 signals change,i.e. on potential link failure
*/
void CCommLinkMonitor::RunL()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCOMMLINKMONITOR_RUNL1_1, "CommLinkMonitor::RunL: Signals changed [0x%X]; new signals [0x%X]", iSignals & (~0x1F), iSignals & 0x1F);
	
	// We report link failure if and only if a monitored line went from high to low.
	// Method: mask the saved signals using inverted monitored bits in the signal bitmask,
	// to filter out all transitions except for high -> low. 
	// 1 & ~0 = 1;    1 & ~1 = 0; 0 & ~1 = 0; 0 & ~0 = 0
	// This is necessary, because we may be monitoring more than one signal, and the change can be arbitrary.
	if ((~(iNotifyChangeSignalMask & iSignals)) & iSavedSignalState) 
		{		
		iUser.CommLinkDown(KErrCommsLineFail);
		}
	iSavedSignalState = iSignals & iNotifyChangeSignalMask;	// update saved state with relevant bits	
	iPort.NotifySignalChange(iStatus, iSignals, iNotifyChangeSignalMask);
	SetActive(); 
	// Note: If the link went down, the user will likely shutdown the BCA on outstanding signal notification. 
	}

/**
Cancels notification request of link failure */	
void CCommLinkMonitor::DoCancel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCOMMLINKMONITOR_DOCANCEL1_1, "CommLinkMonitor::DoCancel: Cancelling signal change notification.");
	iPort.NotifySignalChangeCancel();
	}
	




/**
* default constructor to create a C32Bca instance.
*/
CC32Bca::CC32Bca()	
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_CTOR1_1, "====== CC32Bca::CC32Bca: Shim BCA for C32 [Vesion: major= %d minor= %d] Constructed. ======", KC32BcaMajorVersionNumber, KC32BcaMinorVersionNumber);
	}

/**
2nd phase of construction: allocates member objects 


@leave when members cannot be constructed. */
void CC32Bca::ConstructL()
	{
	__ASSERT_DEBUG(!iReader, Panic(EReaderAlreadyExists));
	__ASSERT_DEBUG(!iWriter, Panic(EWriterAlreadyExists));
	__ASSERT_DEBUG(!iLinkMonitor, Panic(EMonitorAlreadyExists));
	
	// Comm is not initialized yet. We have to be Opened before it can be used. C32 panics otherwise.
	iReader = new (ELeave)CCommReader(*this, iComm);
	iWriter = new (ELeave)CCommWriter(*this, iComm);
	// Monitor is created upon user request only, we may never need it at all.
	}

/**
* Destructor 
*/	
CC32Bca::~CC32Bca()
	
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_DTOR1_1, "CC32Bca::~CC32Bca: releasing resources...");
		
	CloseCommDbConnection(); 
	CloseCommPort(); 	// Cancels reader / writer / monitor
	
	// N.B.: 
	// It is critical that these objects are cancelled before deletion. Else DoCancel operates on closed C32 port, 
	// causing C32 to panic. 
	delete iReader;
	delete iWriter;
	delete iLinkMonitor; // Note: may have never been created.

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_DTOR1_2, "CC32Bca::~CC32Bca: CC32Bca destroyed.");
	}
	
/** This method deletes the BCA itself.*/
void CC32Bca::Release()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_RELEASE1_1, "CC32Bca::Release");
	delete this;
	}
	
/**
* Informs that the BCA is required by the client(for instance, Raw IP NIF).Comms Server is
* connected, and serial port is opened.
	
* @param aStatus complete status, KErrNone if successful, C32 error code otherwise.
* @param aChannelId comm port name.
*/	
void CC32Bca::Open(TRequestStatus& aStatus, const TDesC& aChannelId)
	{
	// We don't have the ConstructL, so we have to allocate memory here.
	
	aStatus = KRequestPending;
	// avoid compiler warning.
	TRequestStatus* ptrStatus = &aStatus;
	if(iCommPortOpen)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_OPEN1_1, "Warning: C32Bca is already Opened.");
	
		User::RequestComplete(ptrStatus,KErrNone);
		return;	
		}
	
	if(aChannelId == KChannelIdNotOverridden) // Channel ID is not supplied - Comm Port Name is not overriden.
		{									 // Read it from CommDB.
		// Fetch port name from CommDB.
		TRAPD(commPortErr, ReadCommPortFromCommDbL(iCommPort));
		
		// If the column is missing from CommDB, we can get an empty string with KErrNone.
		// Opening RComm with empty string as Port name fails with ECUART, LOOOPBACK CSYs. 
		// with different error codes on 8.1 and 9.0. 
		// It would be better to intercept such invalid value and fail here.
		// This is not done, because theoretically it is possible to write a CSY that somehow takes an 
		// emty descriptor as a port name. As of December 2004, C32 does not allow that, however, this
		// may change in the future.
		// So, we just log a warning.
		if(KErrNone == commPortErr && iCommPort.Size() == 0)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_OPEN1_2, "Warning: Null string read from CommDB. Will try to open Comm Port anyway.");
			}		
		
		if(KErrNone != commPortErr) // Fatal: we do not have a port name. Can't open RComm.
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_OPEN1_3, "Error: Failed to get C32 PortName from CommDB ");
			CloseCommDbConnection();
			User::RequestComplete(ptrStatus, commPortErr);
			return;
			}		
		}
	else // Channel ID supplied - use it as Port Name.
		{
		_LIT(KDoubleColon, "::");
		TInt len1 = aChannelId.Find(KDoubleColon);

		if (len1 < KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_OPEN1_4, "** ERROR: No :: (Double Colon) in aChannelId **");
			User::RequestComplete(ptrStatus, KErrBadName);
			return;	
			}
		else
			{
			TUint numPos = len1 + KDoubleColon.iTypeLength;
						
			TInt len2 = (aChannelId.Mid(numPos)).Find(KDoubleColon);
			
			if (len2 < KErrNone)
				{
				iCommPort = aChannelId;
				}
			else
				{
				iCommPort = aChannelId.Mid(numPos);
				iCsyName = aChannelId.Mid(0, len1);
				iCsyNameOverride = ETrue;
				}
			}
		}
	// We have the Port Name at this point: either supplied as a parameter, or read from CommDB.

	if(!iCsyNameOverride) // CSY name is not provisioned by Ioctl: read it from CommDB.
		{
		TRAPD(getCsyErr, ReadCsyNameFromCommDbL(iCsyName));
		if(KErrNone != getCsyErr) // Fatal: we do not have a CSY name. Can't load CSY module.
			{
			// We do not use a hardcoded value (e.g. ECUART), because the client code or the environment are
			// confused - it is better to fail them, rather than mask their problems.
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_OPEN1_5, "Error: Failed to get CSY name from CommDB ");
			CloseCommDbConnection();
			User::RequestComplete(ptrStatus,getCsyErr);
			return;
			}
		}
	// We have the CSY name at this point, either provisioned via Ioctl, or read from CommDB.

	// Decide the Comm Role (DCE / DTE) of the port.
	TBool isCommRoleDefault = EFalse;
	// We use a concept of "Default Comm Role", because C32 has it. We could use DTE as a default
	// setting, but we prefer to call the "default" method on C32 and let it decide what the role should be.

	if(!iCommRoleOverride) // Port role not overriden via Ioctl
		{
		TRAPD(getCommRoleErr, ReadCommRoleFromCommDbL(iCommRole));
		if(KErrNone != getCommRoleErr) // Port role not specified via CommDb
			{
			isCommRoleDefault = ETrue; // Port role not specified via Ioctl or CommDb
			}
		}
	// We have the Comm Role: provisioned via Ioctl, read from CommDB, or default(DTE)
	
	CloseCommDbConnection(); // We do not read any settings once the connection is opened. 	
	// We have all the necessary C32 settings at this point.
	// Open the actual C32 session:
	
	TInt ret = iCommServ.Connect();
	if(ret)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_OPEN1_6, "Open: C32 Server connection error %d", ret);
		User::RequestComplete(ptrStatus,ret);
		return;
		}
	ret = iCommServ.LoadCommModule(iCsyName);
	
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_OPEN1_7, "Open: CSY module [%S] loaded with error %d", iCsyName, ret);
		
	if(ret)
		{
		User::RequestComplete(ptrStatus,ret);
		return;
		}
	
	if(isCommRoleDefault)
		{
		ret = iComm.Open(iCommServ, iCommPort, ECommShared);
		}
	else
		{
		ret = iComm.Open(iCommServ, iCommPort, ECommShared, iCommRole);	
		}
	
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_OPEN1_8, "Open: C32 port [%S] opened with error %d", iCommPort, ret);
					
	if(ret)
		{
		User::RequestComplete(ptrStatus,ret);
		return;
		}
	//
	//Open successfull, we update our state.
	//
	
	// We may have been closed, and are being reopened now. Make sure no errors persist from the previous session.
	iErrorOnNextRead  = KErrNone; 
	iErrorOnNextWrite = KErrNone;
	iCommPortOpen = ETrue;
	
	User::RequestComplete(ptrStatus,KErrNone);
	// Don't start control line monitoring. This is done on user request via Ioctl only.
	}
	
/**
 Shuts the BCA channel down in a graceful manner. BCA releases its resources.
 Cancels all outstanding operations: Writes, Reads and Ioctls.

 Due to the specifics of C32Bca implementation, this call completes immediately and is identical to ::Close.
 
 @param aStatus completion status: Always KErrNone.
*/	
void CC32Bca::Shutdown(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	
	CloseCommPort();
	
	// Don't notify reader / writer / monitor. Read / Write should not be called after this anyway. Let C32 panic.
	
	TRequestStatus* request = &aStatus;
	User::RequestComplete(request, KErrNone);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_SHUTDOWN1_1, "C32Bca::Shutdown: BCA shut down with error %d", KErrNone);
	}

/**
* Closes the BCA immediately. BCA releases its resources.
* cancels all Writes, Reads and Controls.
*/	
void CC32Bca::Close()
	{
	CloseCommPort();
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_CLOSE1_1, "C32Bca::Close:Close: BCA closed with error %d", KErrNone);
	}

/**
* Queues a Read on C32 serial port.

* @param aStatus complete status, KErrNone if successful, C32 error code otherwise.
* @param aBuf buffer for data to be read.
* @note The buffer is owned by the client. Client must not access / modify the buffer until the Read completes.
*/	
void CC32Bca::Read(TRequestStatus& aStatus,TDes8& aBuf)
	{
	iReadRequest = &aStatus;
	*iReadRequest = KRequestPending;

	if(iErrorOnNextRead)
		{
		User::RequestComplete(iReadRequest, iErrorOnNextRead);
		// Error persists until the BCA is closed and opened
		}
	else
		{
		iReader->Read(aBuf);	
		}
	}

/**
* Queues a Write on C32 serial port.

* @param aStatus the complete status, KErrNone if successful, C32 error code otherwise.
* @param aBuf the buffer to sent.
* @note The buffer is owned by the client. Client must not access / modify the buffer until the Write completes.
*/	
void CC32Bca::Write(TRequestStatus& aStatus,const TDesC8& aBuf)
	{
	iWriteRequest = &aStatus;
	*iWriteRequest = KRequestPending;
	
	if(iErrorOnNextWrite)
		{
		User::RequestComplete(iWriteRequest, iErrorOnNextWrite);
		// Error persists until BCA is closed and opened.
		}
	else
		{
		iWriter->Write(aBuf);	
		}
	}

/**
 Cancels the outstanding Read operation.(best effort operation: the read may have been completed already.)
*/	
void CC32Bca:: CancelRead()
	
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_CANCELREAD_1, "CancelRead: Read is cancelled by client.");
	iReader->Cancel();

	if(iReadRequest != NULL)
		{
		User::RequestComplete(iReadRequest, KErrCancel);
		}
	}

/**
 Cancels all Write operations.(best effort attempt: the write may have been completed already.)
*/	
void CC32Bca::CancelWrite()

    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_CANCELWRITE1_1, "CancelWrite: Write is cancelled by client.");

    iWriter->Cancel();

    if(iWriteRequest != NULL)
    	{
    	User::RequestComplete(iWriteRequest, KErrCancel);
    	}
    }
    

// Debug dumps:
#ifdef OST_TRACE_COMPILER_IN_USE

_LIT8(KLitOptLevelGeneric,    "KBcaOptLevelGeneric");
_LIT8(KLitOptLevelExtSerial,   "KBcaOptLevelExtSerial");
_LIT8(KLitOptLevelUnsupported, "**Unsupported**");

/**
Returns a string for an option level

@param aOptLevel BCA Option Level 
@return readable string for aOptLevel
*/
static const TDesC8& IoctlOptLevelStr(TUint32 aOptLevel)
	{
	switch(aOptLevel)
		{
		case KBcaOptLevelGeneric:
			return KLitOptLevelGeneric;
		case KBcaOptLevelExtSerial:
			return KLitOptLevelExtSerial;
		default: 
			return KLitOptLevelUnsupported;
		}
	}


_LIT8(KLitOptNameBcaVersionNumber, "KBcaOptNameVersionNumber");
_LIT8(KLitOptNameBcaCaps,    	   "KBcaOptNameBcaCaps");
_LIT8(KLitOptNameBcaSetIapId,      "KBcaOptNameBcaSetIapId");
_LIT8(KLitOptNameBcaSetBcaStack,   "KBcaOptNameBcaSetBcaStack");
_LIT8(KLitOptNameBcaResetBuffers,  "KBcaOptNameBcaResetBuffers");
_LIT8(KLitOptNameSerialPortName,   "KBcaOptNameSerialPortName");
_LIT8(KLitOptNameSerialConfig,     "KBcaOptNameSerialSerialConfig");
_LIT8(KLitOptNameSerialSetConfig,  "KBcaOptNameSerialSerialSetConfig");
_LIT8(KLitOptNameSerialSetCsyName,  "KBcaOptNameBcaSetCsyName");
_LIT8(KLitOptNameSerialSetCommRole, "KBcaOptNameSetCommRole");
_LIT8(KLitSerialSetTxRxBufferSize,  "KSerialSetTxRxBufferSize");
_LIT8(KLitSerialTxRxBufferSize, 	"KSerialTxRxBufferSize");
_LIT8(KLitSerialMonitorControlLines, "KSerialMonitorControlLines");
_LIT8(KLitSerialSetControlLines,	"KSerialSetControlLines");
_LIT8(KLitOptNameUnsupported,  	   "**Unsupported**");	


/**
Returns a string for the given option name

@param aOptLevel BCA Option Name 
@return readable string for aOptName
*/
static const TDesC8& IoctlOptNameStr(TUint32 aOptName)
	{
	switch(aOptName)
		{
		case KBCACaps:
			return KLitOptNameBcaCaps;
		case KVersionNumber:
			return KLitOptNameBcaVersionNumber;
		case KBCASetIapId:
			return KLitOptNameBcaSetIapId;
		case KBCASetBcaStack:
			return KLitOptNameBcaSetBcaStack;
		case KBCAResetBuffers:
			return KLitOptNameBcaResetBuffers;
		case KSerialPortName:
			return KLitOptNameSerialPortName;
		case KSerialConfig:
			return KLitOptNameSerialConfig;
		case KSerialSetConfig:
			return KLitOptNameSerialSetConfig;
		case KSerialSetCsyName:
			return KLitOptNameSerialSetCsyName;
		case KSerialSetCommRole:
			return KLitOptNameSerialSetCommRole;
		case KSerialSetTxRxBufferSize:
			return KLitSerialSetTxRxBufferSize;
		case KSerialTxRxBufferSize:
			return KLitSerialTxRxBufferSize;
		case KSerialMonitorControlLines:
			return KLitSerialMonitorControlLines;		
		case KSerialSetControlLines:
			return KLitSerialSetControlLines;
		default:
			return KLitOptNameUnsupported;		
		}
	}
#endif // OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG




/**
* Ioctl: asynchronously controls the C32Bca.

* @param aStatus complete status, KErrNone if successful, system-wide error code otherwise.
* @param aOptLevel option level to be used.
* @param aOptName option name to be used.
* @param aOpt an optional parameter,holds the option value on return or the option value to be set.
*/  
void CC32Bca::Ioctl(TRequestStatus& aStatus, TUint aOptLevel, TUint aOptName, TDes8& aOpt)

	{
	OstTraceDefExt4(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_1, "Ioctl: Level[%s](0x%X) Name[%s](0x%X).", IoctlOptLevelStr(aOptLevel), aOptLevel, IoctlOptNameStr(aOptName), aOptName);
	
	aStatus = KRequestPending;
	
	TInt ret(KErrNone); // If operation fails, this will be set to the failure code explicitly.
	
	// Don't check the port status. Let C32 panic. It's the caller's responsibility to make sure that
	// the port is in a valid state (for most operations, open & not having any Tx Rx ops outstanding)
	if(aOptLevel == KBcaOptLevelGeneric)
		{
		switch(aOptName)
			{
			case KBCACaps:
				{
				const TPckg<TUint> capPckg(KBcaCapSerial);
				aOpt = capPckg;			
				break;
				}
				
			case KVersionNumber:
				{
				const TPckg<TUint> versionPckg(KVersion); //lower 16 contains KC32BcaMinorversionNumber, upper 16 bit contains KC32BcaMajorVersionNumber.
				aOpt = versionPckg;
				break;
				}
				
			case KBCASetIapId:
				{
				TUint32 tempIapId = *(reinterpret_cast<const TUint32*>(aOpt.Ptr()));
				
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_2, "Ioctl: IAP ID [%d] specified.",tempIapId);
				
				if(tempIapId < 1 )
					{
					ret = KErrArgument;
					}
				else
					{
					iIapId = tempIapId;
					}
				break;
				}
				
			case KBCAResetBuffers:
				{
				TUint resetBufMaskArg = *(reinterpret_cast<const TUint32*>(aOpt.Ptr()));
				TUint resetBufMask(0x0);
		
				if(resetBufMaskArg & KResetRxBuf)
					{
					resetBufMask |= KCommResetRx;
					}
				if(resetBufMaskArg & KResetTxBuf)
					{
					resetBufMask |= KCommResetTx;
					}
				ret = iComm.ResetBuffers(resetBufMask);
				break;
				}
			
				
			default:
				{
				ret = KErrNotSupported;
				}
			}
		}		
	else if(aOptLevel == KBcaOptLevelExtSerial)
		{
		switch(aOptName)
			{
		case KSerialCaps:
			{
			iComm.Caps(aOpt);
			break;
			}
			
		case KSerialConfig:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_3, "Ioctl: KSerialConfig");

			TCommConfig cfg;
			iComm.Config(cfg);

		    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_4, "Rate[%d] DataBits[%d] StopBits[%d]", cfg().iRate, cfg().iDataBits, cfg().iStopBits);
		    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_5, "Parity[%d] Handshake[0x%x]", cfg().iParity, cfg().iHandshake);
		    OstTraceDefExt4(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_6, "ParityError[%d] Fifo[%d] SpecialRate[%d] terminatorCount[%d]", cfg().iParityError, cfg().iFifo, cfg().iSpecialRate, cfg().iTerminatorCount);
		    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_7, "Terminator[0x%x]", cfg().iTerminator);

			TPckgBuf<TCommConfig> cfgBuf(cfg);
			aOpt.Copy(cfgBuf);
			break;
			}

		case KSerialSetConfig:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_8, "Ioctl: KSerialSetConfig");

			TCommConfig cfg(*(reinterpret_cast<const TCommConfig*>(aOpt.Ptr())));

            OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_9, "Rate[%d] DataBits[%d] StopBits[%d]", cfg().iRate, cfg().iDataBits, cfg().iStopBits);
            OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_10, "Parity[%d] Handshake[0x%x]", cfg().iParity, cfg().iHandshake);
            OstTraceDefExt4(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_11, "ParityError[%d] Fifo[%d] SpecialRate[%d] terminatorCount[%d]", cfg().iParityError, cfg().iFifo, cfg().iSpecialRate, cfg().iTerminatorCount);
	        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_12, "Terminator[0x%x]", cfg().iTerminator);


			ret = iComm.SetConfig(cfg);
			break;
			}
			
		case KSerialPortName:
			{
			aOpt.Copy(iCommPort);
			break;
			}
			
		case KSerialSetCsyName:
			{
			if(!iCommPortOpen)
				{
				iCsyName.Copy(*(
			   				reinterpret_cast<const TName*>(aOpt.Ptr())
			   				));
				iCsyNameOverride = ETrue;
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_13, "Ioctl: CSY Name set to [%S]", iCsyName);
				}
			else
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_14, "Ioctl: Warning: Cannot set the CSY name because the Comm Port is already open.");
				ret = KErrAlreadyExists;
				}
			
			break;
			}
			
		case KSerialSetCommRole:
			{
			if(!iCommPortOpen)
				{
				iCommRole = *(reinterpret_cast<const TCommRole*>(aOpt.Ptr()));
				iCommRoleOverride = ETrue;
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_15, "Ioctl: Comm Role set to [%d]", iCommRole);
				}
			else
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_16, "Ioctl: Warning: Cannot set Comm Role because the Comm Port is already open.");
				ret = KErrAlreadyExists;
				}
			break;	
			}
		
		case KSerialSetTxRxBufferSize:
			{
			TInt bufSize = *(reinterpret_cast<const TInt*>(aOpt.Ptr()));
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_17, "Ioctl: Setting Rx Tx buffer size to [%d]", bufSize);
			
			iComm.SetReceiveBufferLength(bufSize);
			break;
			}
			
		case KSerialMonitorControlLines:
			{
			TRAP(ret, MonitorControlLinesL(*(reinterpret_cast<const TUint*>(aOpt.Ptr()))));
			break;
			}			
							
		case KSerialSetControlLines:
			{
			if (aOpt.Length() != sizeof(TSerialSetControlLines))
				{
				ret = KErrArgument;
				}
			else
				{
				const TSerialSetControlLines& lines = *(reinterpret_cast<const TSerialSetControlLines*>(aOpt.Ptr()));
				OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_18, "Ioctl: Setting/clearing control lines %x/%x", lines.iSetMask, lines.iClearMask);
				iComm.SetSignals(lines.iSetMask, lines.iClearMask);
				ret = KErrNone;
				}
			break;
			}

		default:
			ret = KErrNotSupported;
			}		
		}
	else
		{
		ret = KErrNotSupported;
		}
		
	
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_IOCTL1_19, "Ioctl completed with error %d", ret);
	
	TRequestStatus* ptrStatus = &aStatus;
	User::RequestComplete(ptrStatus, ret);
	}
	
/** Enables / Disables monitoring & link down reporting on the specified control lines

@param aMask bitmaks specifying the lines to monitor, OR
	KMonitorOff to turn monitoring off, OR
	KConfigFailBcaSpecificOnly to set up monitoring according to Comm Role only 
	
@leave if the monitoring object cannot be constructed, typically due to OOM.
*/
void CC32Bca::MonitorControlLinesL(TUint32 aArgMask)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_MONITORCONTROLLINESL1_1, "Ioctl: argument bitmask = [0x%X] ", aArgMask);
			
	if(iLinkMonitor) // We may have never started...  
			{
			// We are called to either stop or change the monitoring options.
			iLinkMonitor->Cancel(); // If we are not monitoring, this has no effect.
			}
	
	if(KMonitorOff == aArgMask) // Stop monitoring:
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_MONITORCONTROLLINESL1_2, "MonitorControlLinesL: Stopping Control Lines monitoring.");
		// We either never started, or we just cancelled above.
		}
	else // Start Monitoring, or change the monitored lines.
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_MONITORCONTROLLINESL1_3, "MonitorControlLinesL: Starting to monitor Control Lines.");
		
		if(!iLinkMonitor) // We are starting to monitor for the first time 
			{			
			iLinkMonitor = new (ELeave)CCommLinkMonitor(*this, iComm);
			}		
		// At this point we have a monitoring object that isn't active.
			
		// A CSY does not necessarily handle the KConfigFail* correctly. For example, if 
		// a DTR is initially low, because the terminal was not plugged in yet, we get a failure straight away,
		// So, we translate the failure arguments and monitor the relevant lines ourselves. We fail only when a line
		// is disasserted. This is absolutely necessary to support server-mode PPP, because we may need to wait 
		// an arbitrary period of time for the user to plug the terminal in.
		TUint32 lineMask(0x0);
		if(KFailBcaSpecificOnly == aArgMask) // We decide what lines to monitor
			{
			// N.B. When a DTE, fail on low DSR. This is necessary to detect a disconnection on serial links that
			// do not utilise DCD.
			lineMask = (ECommRoleDCE == iCommRole) ? KSignalDTR : KSignalDCD | KSignalDSR;
			}
		else // User explicitly specified the control lines.
			{
			// Don't bother with sanity checks on the lines. Let C32 handle it.

			if(KConfigFailCTS & aArgMask)
				{
				lineMask |= KSignalCTS;
				}
			if(KConfigFailDSR & aArgMask)
				{
				lineMask |= KSignalDSR;
				}
			if(KConfigFailDCD & aArgMask)
				{
				lineMask |= KSignalDCD;
				}
			if(KConfigFailDTR & aArgMask)
				{
				lineMask |= KSignalDTR;
				}
			if(KConfigFailRTS & aArgMask)
				{
				lineMask |= KSignalRTS;
				}
			}
			
		iLinkMonitor->Setup(lineMask);
		iLinkMonitor->NotifyLinkDown();
		
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_MONITORCONTROLLINESL1_4, "MonitorControlLinesL: Control Lines monitoring started.");
		}				
	}

/**
Closes the connection to CommDB, if it is open */
void CC32Bca::CloseCommDbConnection()
	{
	delete iCommsDat;
	iCommsDat = NULL;
	}

/**
Read Comm Port from CommDB bearer record 
@param aPortName
@leave if the value could not be read */
void CC32Bca::ReadCommPortFromCommDbL(TDes& aPortName)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_READCOMMPORTFROMCOMMDBL1_1, "CC32Bca::ReadCommPortFromCommDbL()");
	ConnectToCommDbBearerRecordL();

	TInt ret(0);
	CMDBField<TDesC>* portField = new(ELeave) CMDBField<TDesC>(KCDTIdPortName);
	CleanupStack::PushL(portField);

	portField->SetRecordId(iModemId);
	portField->SetMaxLengthL(KMaxTextLength);
	TRAP(ret,portField->LoadL(*iCommsDat));

	if(ret!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_READCOMMPORTFROMCOMMDBL1_2, "portField->LoadL(*iCommsDat) left with[%d] ", ret);
		User::Leave(ret);	
		}

	aPortName = *portField;
	CleanupStack::PopAndDestroy(portField);	
	}
	
/** 
Read the CSY name from CommDB bearer record.
@param aCsyName the CSY name
@leave if the value could not be read */
void CC32Bca::ReadCsyNameFromCommDbL(TDes& aCsyName)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_READCSYNAMEFROMCOMMDBL1_1, "CC32Bca::ReadCsyNameFromCommDbL()");
	ConnectToCommDbBearerRecordL();

	TInt ret(0);
	CMDBField<TDesC>* csyField = new(ELeave) CMDBField<TDesC>(KCDTIdCsyName);
	CleanupStack::PushL(csyField);

	csyField->SetRecordId(iModemId);
	csyField->SetMaxLengthL(KMaxTextLength);
	TRAP(ret,csyField->LoadL(*iCommsDat));

	if(ret!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_READCSYNAMEFROMCOMMDBL1_2, "csyField->LoadL(*iCommsDat) left with[%d] ", ret);
		User::Leave(ret);	
		}

	aCsyName = *csyField;
	CleanupStack::PopAndDestroy(csyField);
	}
	
/**
Read the specified Comm role (DTE / DCE) from CommDB bearer record

@param aCommRole the specified role.
@leave if the value could not be read */	
void CC32Bca::ReadCommRoleFromCommDbL(TCommRole& aCommRole)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_READCOMMROLEFROMCOMMDBL1_1, "CC32Bca::ReadCommRoleFromCommDbL()");
	ConnectToCommDbBearerRecordL();
	TUint32 role(0);

	TInt ret(0);
	CMDBField<TUint32>* roleField = new(ELeave) CMDBField<TUint32>(KCDTIdCommRole);
	CleanupStack::PushL(roleField);

	roleField->SetRecordId(iModemId);
	TRAP(ret,roleField->LoadL(*iCommsDat));

	if(ret!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_READCOMMROLEFROMCOMMDBL1_2, "roleField->LoadL(*iCommsDat) left with[%d] ", ret);
		User::Leave(ret);	
		}

	role = *roleField;
	CleanupStack::PopAndDestroy(roleField);

	aCommRole = (0 == (role & KModemCommRoleDCE)) ? ECommRoleDTE : ECommRoleDCE;
	}

/** 
Opens a connection to the ModemBearer record specified by the provisioned IAP ID.

@leave KErrNotReady, if the provisioned IAP id is invalid. System-wide error code
       if the connection cannot be opened for some other reason.
*/
void CC32Bca::ConnectToCommDbBearerRecordL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_CONNECTTOCOMMDBBEARERRECORDL1_1, "CC32Bca::ConnectToCommDbBearerRecordL()");
	if(iCommsDat != NULL) // CommDB is already open, we don't need to do anything
		{
		return;	
		}
	
	if(iIapId < 1) // Can't access CommDB if IAP ID is unknown
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_CONNECTTOCOMMDBBEARERRECORDL1_2, "iIapId[%d] is unknown", iIapId);
		User::Leave(KErrNotReady);	
		}

	//
	// Open connecton to CommDB Bearer record specified by the IAP.
	//
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	iCommsDat = CMDBSession::NewL(KCDVersion1_2);
#else
	iCommsDat = CMDBSession::NewL(KCDVersion1_1);
#endif
	
	CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
  	CleanupStack::PushL(iapRecord);
  	
  	iapRecord->SetRecordId(iIapId);
  	
  	TBool hiddenAttributeMaskValue = iCommsDat->IsSetAttributeMask(ECDHidden);
   	TBool privateAttributeMaskValue = iCommsDat->IsSetAttributeMask(ECDPrivate);
  	
  	// Check to see if viewing hidden records is enabled or not
	if(!hiddenAttributeMaskValue || !privateAttributeMaskValue)
		{
		// Reveal hidden or private IAP records if a licensee has chosen to protect a record
		// using one of these flags - the API to do this is public so internal components
		// have to support the use of such records.
		iCommsDat->SetAttributeMask(ECDHidden | ECDPrivate);
		}		  	
  	
  	TRAPD(ret,iapRecord->LoadL(*iCommsDat));
	if (ret != KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_CONNECTTOCOMMDBBEARERRECORDL1_3, "iapRecord->LoadL(*iCommsDat) left with[%d] ", ret);
		User::Leave(ret);
		}    
    
    // If we enabled viewing hidden records now disable viewing them.
    if(!hiddenAttributeMaskValue)
    	{
    	iCommsDat->ClearAttributeMask(ECDHidden);
    	}
 	if(!privateAttributeMaskValue)
		{
		iCommsDat->ClearAttributeMask(ECDPrivate);
		}		  	
   	

	iModemId = iapRecord->iBearer;
	CleanupStack::PopAndDestroy(iapRecord);
	
	if(iModemId == 0) // ID not found.
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_CONNECTTOCOMMDBBEARERRECORDL1_4, "iModemId[%d] is not found", iModemId);
		User::Leave(KErrNotFound);	
		}
	}


/** Cancels an outstanding Ioctl, if any. */
void CC32Bca::CancelIoctl()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_CANCELIOCTL1_1, "CancelIoctl(): Ioctl cancel request. No Ioctl to cancel.");
	}
	

/** Closes the Comm port and Comm Server.*/
void CC32Bca::CloseCommPort()
	{
	__ASSERT_ALWAYS(iReader,      Panic(ENullReaderOnClose));
	__ASSERT_ALWAYS(iWriter,     Panic(ENullWriterOnClose));
		
	// When we close the port, all async requests outstanding are cancelled.
	// We must make sure nobody is waiting on them. If the underlying request is cancelled,
	// but the owner AO is not, Cancel() on such AO hangs waiting for the request to complete.
	
	iReader->Cancel();
	iWriter->Cancel();
	if(iLinkMonitor) // Link monitor is constructed on client request only.
		{
		iLinkMonitor->Cancel();
		}
	
	if (iCommPortOpen)
		{
		iComm.Close(); 
		iCommServ.Close();	
		iCommPortOpen = EFalse;
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_CLOSECOMMPORT1_1, "CloseCommPort(): Session with C32 & RComm closed.");
	}

/**
C32Bca Panic function
@internalComponent
*/		
void C32Bca::Panic(TC32BcaPanic aPanic)
	{
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_PANIC1_1, "%S Panic %d", KC32BcaPanic(), aPanic);
	User::Panic(KC32BcaPanic, aPanic);
	}
	

// MC32User implementation:

// Read has completed. 
void CC32Bca::CommReadComplete(TInt aErr)
	{
	__ASSERT_ALWAYS(iReadRequest, Panic(ENullReadRequestStatus));
	__ASSERT_DEBUG(KRequestPending == iReadRequest->Int(), Panic(EReadRequestNotPending));
	
	User::RequestComplete(iReadRequest, aErr);
	}

// Write has completed.
void CC32Bca::CommWriteComplete(TInt aErr)
	{
	__ASSERT_ALWAYS(iWriteRequest, Panic(ENullWriteRequestStatus));	
	__ASSERT_DEBUG(KRequestPending == iWriteRequest->Int(), Panic(EWriteRequestNotPending));
	
	User::RequestComplete(iWriteRequest, aErr);
	}

// Upcall from the link monitor: Link has gone down.	
void CC32Bca::CommLinkDown(TInt aErr)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CC32BCA_COMMLINKDOWN1_1, "CommLinkDown: Warning: serial link has gone down with error[%d]. Erroring the outstanding Read & Write.", aErr);
	
	__ASSERT_DEBUG(KErrNone != aErr, Panic(EGeneralLogicError)); // If KErrNone, use has no way to know that the read has failed.
	if(iReader->IsActive())
		{
		CommReadComplete(aErr);
		iReader->Cancel();					
		}
	else
		{
		iErrorOnNextRead = aErr;
		}
	
	if(iWriter->IsActive())
		{
		CommWriteComplete(aErr);
		iWriter->Cancel();		
		}
	else
		{
		iErrorOnNextWrite = aErr;
		}
	}

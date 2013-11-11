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
// Implementation of the Comms reader and writer mixin class.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimSerCommTraces.h"
#endif

#include "CSimSerComm.h"
#include "CSimPhone.h"

MComm::MComm()
/**
 * Trivial constructor.
 */
	{
	iCommReader = NULL;
	iCommWriter = NULL;
	}

MComm::~MComm()
/**
 * Destructor.  Destroy the heap-based objects and close the serial ports.
 */
	{
    delete iCommReader;
	delete iCommWriter;
	iCommPort.Close();
	iCommServer.Close();
	}

void MComm::CommConstructL(TInt aReadPriority, TInt aWritePriority)
/**
 * Second phase constructor for this class.  This function creates the reader and writer
 * objects.
 * @param aReadPriority		The active scheduler priority associated with the reader class.
 * @param aWritePriority	The active scheduler priority associated with the writer class.
 */
	{
	iCommReader = new (ELeave) CCommReader(this, aReadPriority);
	iCommWriter = new (ELeave) CCommWriter(this, aWritePriority);
	};

TInt MComm::CommOpen(const TDesC& aDll, const TDesC& aName, TCommAccess aMode)
/**
 * Connect to the C32 Server, load the CSY module and open the port ready for
 * reading and writing.  If a "shared" open is requested, the function will
 * briefly attempt to exclusively open the comm port to ensure that no-one
 * is currently on the port.  This client will allow others to share the port
 * but it wants to remain in control.
 *
 * @param aDll		The CSY module name.
 * @param aName		The serial port name.
 * @param aMode		The required access mode: shared, exclusive, etc...
 * @return TInt		Standard error value.
 */
	{
	TInt err;
	if (err = iCommServer.Connect(), err!=KErrNone)
		return err;
	if (aDll.Length()>0)
		{
		if (err = iCommServer.LoadCommModule(aDll), err!=KErrNone)
			{
			iCommServer.Close();
			return err;
			}
		}
	if (err = iCommPort.Open(iCommServer, aName, ECommExclusive), err!=KErrNone)
		// if any other TSY (or other app) is using comm port, we want to return gracefully
		// with error
		{
		iCommServer.Close();
		return err;
		}
	if (aMode==ECommShared)
		{
		iCommPort.Close();
		if (err = iCommPort.Open(iCommServer, aName, aMode), err!=KErrNone)
			{
			iCommServer.Close();
			return err;
			}
		}
	return KErrNone;
	}

TInt MComm::CommOpen(const TDesC& aName, TCommAccess aMode)
/**
 * Connect to the C32 Server and open the port ready for
 * reading and writing.  If a "shared" open is requested, the function will
 * briefly attempt to exclusively open the comm port to ensure that no-one
 * is currently on the port.  This client will allow others to share the port
 * but it wants to remain in control.
 *
 * @param aName		The serial port name.
 * @param aMode		The required access mode: shared, exclusive, etc...
 * @return TInt		Standard error value.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, MCOMM_COMMOPEN1_1, "Attempting to Open Serial Port");
	TInt err;
	if (err = iCommServer.Connect(), err!=KErrNone)
		return err;
	if (err = iCommPort.Open(iCommServer, aName, ECommExclusive), err!=KErrNone)
		// if any other TSY (or other app) is using comm port, we want to return gracefully
		// with error
		{
		iCommServer.Close();
		return err;
		}
	if (aMode==ECommShared)
		{
		iCommPort.Close();
		if (err = iCommPort.Open(iCommServer, aName, aMode), err!=KErrNone)
			{
			iCommServer.Close();
			return err;
			}
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, MCOMM_COMMOPEN1_2, "Sucessfully Opened Serial Port");
	return KErrNone;
	}

void MComm::CommClose()
/**
 * Cancel any outstanding requests and close all port handles.
 */
	{
 	if (iCommReader != NULL)
 		iCommReader->Cancel();
 	
 	if (iCommWriter != NULL)
 		iCommWriter->Cancel();
	iCommPort.Close();
	iCommServer.Close();
	}

void MComm::CommWrite(const TDesC8& aDes)
/**
 * Asynchronously start to write the contents of a descriptor to the opened serial port.
 * When the write completes, the callback function CommWriteComplete will be called.
 * @param aDes		Descriptor whose contents will be written to the port.
 */
	{
	__ASSERT_ALWAYS(iCommWriter!=NULL, SimPanic(ECommWriterNotConstructed));
	iCommPort.Write(iCommWriter->StatusRef(), aDes);
	iCommWriter->Activate();
	}

void MComm::CommWriteReady()
/**
 * This function sychronously primes the serial port for writing.  The function will not
 * complete until the serial port is powered up and the handshaking lines are
 * set to allow write operations.
 */
	{
	__ASSERT_ALWAYS(iCommWriter!=NULL, SimPanic(ECommWriterNotConstructed));
	TRequestStatus aStatus;
	iCommPort.Write(aStatus, TPtrC8(NULL, 0));
	User::WaitForRequest(aStatus);
	}

void MComm::CommRead(TDes8& aDes)
/**
 * Asynchronously to read data from the opened serial port.
 * When the passed buffer is full, the read will complete, calling the CommReadComplete callback function.
 * @param aDes		Read buffer descriptor.
 */
	{
	__ASSERT_ALWAYS(iCommReader!=NULL, SimPanic(ECommReaderNotConstructed));
	iCommPort.Read(iCommReader->StatusRef(), aDes, aDes.Length());
	iCommReader->Activate();
	}

void MComm::CommReadOneOrMore(TDes8& aDes)
/**
 * Asynchronously to read data from the opened serial port.
 * As soon as a single or multiple octets have been read, the read will complete,
 * calling the CommReadComplete callback function.
 * @param aDes		Read buffer descriptor.
 */
	{
	__ASSERT_ALWAYS(iCommReader!=NULL, SimPanic(ECommReaderNotConstructed));
	iCommPort.ReadOneOrMore(iCommReader->StatusRef(), aDes);
	iCommReader->Activate();
	}

void MComm::CommReadReady()
/**
 * This function sychronously primes the serial port for reading.  The function will not
 * complete until the serial port is powered up.
 */
	{
	__ASSERT_ALWAYS(iCommReader!=NULL, SimPanic(ECommReaderNotConstructed));
	TRequestStatus aStatus;
	iCommPort.Read(aStatus,iBuf,0);
	User::WaitForRequest(aStatus);
	}

void MComm::CommCancel()
/**
 * Cancel any outstanding read or write requests.
 */
	{
	if (iCommWriter)
		iCommWriter->Cancel();
	if (iCommReader)
		iCommReader->Cancel();
	}

void MComm::CommWriteCancel()
/**
 * Cancel an outstanding write request.
 */
	{
	if (iCommWriter)
		iCommWriter->Cancel();
	}

void MComm::CommReadCancel()
/**
 * Cancel an outstanding read request.
 */
	{
	if (iCommReader)
		iCommReader->Cancel();
	}

//
// CCommWriter
//
CCommWriter::CCommWriter(MComm* aComm, TInt aPriority)
	: CActive(aPriority), iComm(aComm)
/**
 * Constructor for asynchronous writer class.
 */
	{
	__DECLARE_NAME(_S("CCommWriter"));
	CActiveScheduler::Add(this);
	}

CCommWriter::~CCommWriter()
/**
 * Destructor for writer class.
 */
	{
	Cancel();
	}

void CCommWriter::RunL()
/**
 * Active Scheduler callback function, indicating a write completion.
 */
	{
	iComm->CommWriteComplete(iStatus.Int());
	}

void CCommWriter::DoCancel()
/**
 * Cancel an outstanding write operation.
 */
	{
	iComm->iCommPort.WriteCancel();
	}

//
// CCommReader
//
CCommReader::CCommReader(MComm* aComm, TInt aPriority)
	: CActive(aPriority), iComm(aComm)
/**
 * Constructor for asynchronous reader class.
 */
	{
	__DECLARE_NAME(_S("CCommReader"));
	CActiveScheduler::Add(this);
	}

CCommReader::~CCommReader()
/**
 * Destructor for reader class.
 */
	{
	Cancel();
	}

void CCommReader::RunL()
/**
 * Active Scheduler callback function, indicating a read completion.
 */
	{
	iComm->CommReadComplete(iStatus.Int());
	}

void CCommReader::DoCancel()
/**
 * Cancel an outstanding write operation.
 */
	{
	iComm->iCommPort.ReadCancel();
	}

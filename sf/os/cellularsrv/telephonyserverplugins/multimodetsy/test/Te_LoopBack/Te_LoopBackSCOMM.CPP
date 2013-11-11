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
// This file contains the methods necessary for interacting with the Communications Server
// and ports.  The MComm mixin class's methods (open, close, read, and write) are implemented 
// here.  The RunL functions for read and write are implemented here as well as the Cancel
// functions.
// 
//

/**
 @file
*/

#include "Te_LoopBackATSTD.H"
#include "Te_LoopBackSCOMM.H"

MComm::MComm()
/**
 * This is a single phase constructor that simply initializes private member data.  An instance
 * of the MComm class is created.
 *
 * @return	None
 */
	{
	iCommReader = NULL;
	iCommWriter = NULL;
	}

MComm::~MComm()
/**
 * The MComm class destructor which deletes the iCommReader and iCommWriter of this mixin
 * class.  Additionally, port and server cleanup (close) is handled here.
 *
 * @return	None
 */
	{
    delete iCommReader;
	delete iCommWriter;
	iCommPort.Close();
	iCommServer.Close();
	}

TInt MComm::CommOpen(const TDesC& aDll, const TDesC& aName, TCommAccess aMode)
/**
 * This method implements a connection to the comm server, the loading of the specified
 * DLL (typically a CSY) and the open of a specified port.  This method is called from the 
 * ConstructL of the derived class.
 *
 * First, the method connects to the communications server.  Then, the specified DLL is 
 * loaded.  If successful, then the actual port is opened.  First the port is opened in
 * an exclusive mode.  If that fails (regardless of the mode requested by the calling 
 * application), an error is returned.  If this open is successful, and the requested mode
 * is shared, the exclusive open is released and the port is re-opened in shared mode.
 *
 * @param	aDll, the Dynamic Library name to be loaded via LoadCommModule.  This is typically
 *			a CSY.
 * @param	aName, the port name to be opened.
 * @param	aMode, the access mode for this port.  See note below.
 *
 * @return	KErrNone or errors associated with connecting to the Comm Server, errors loading
 *			the DLL, or errors opening the port.
 *
 * @note	If the specified DLL has been previously loaded, this method will return an
 *          error.
 * @note	Even if the caller specifies a shared access mode, it first must be able to open
 *			the port exclusively for this method to succeed.  Thus a second open on a shared
 *			port will fail.
 * @note	This is an overloaded function (in this case requiring a DLL name).
 */
 // Todo: add reasoning, when call loaned, the port must be reopened by others, but gsm tsy
 //       does not want this, it wants control.
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
	if (err = iCommPort.Open(iCommServer, aName, ECommExclusive, ECommRoleDCE), err!=KErrNone)
		// if any other TSY (or other app) is using comm port, we want to return gracefully
		// with error
		{
		iCommServer.Close();
		return err;
		}
	if (aMode==ECommShared)
		{
		iCommPort.Close();
		if (err = iCommPort.Open(iCommServer, aName, aMode, ECommRoleDCE), err!=KErrNone)
			{
			iCommServer.Close();
			return err;
			}
		}
	return KErrNone;
	}


TInt MComm::CommOpen(const TDesC& aName, TCommAccess aMode)
/**
 * This method implements a connection to the comm server and the open of the specified port.
 * This method assumes that the needed DLL has already been loaded into the system.
 * This method is called from the ConstructL of the derived class.
 *
 * First, the method connects to the communications server.  If successful, then the actual 
 * port is opened.  First the port is opened in an exclusive mode.  If that fails (regardless
 * of the mode requested by the calling application), an error is returned.  If this open 
 * is successful, and the requested mode is shared, the exclusive open is released and the 
 * port is re-opened in shared mode.
 *
 * @param	aName, the port name to be opened.
 * @param	aMode, the access mode for this port.  See note below.
 *
 * @return	KErrNone or errors associated with connecting to the Comm Server, errors loading
 *			the DLL, or errors opening the port.
 *
 * @note	Even if the caller specifies a shared access mode, it first must be able to open
 *			the port exclusively for this method to succeed.  Thus a second open on a shared
 *			port will fail.
 * @note	This is an overloaded function (in this case not requiring a DLL name).
 */	{
	TInt err;
	if (err = iCommServer.Connect(), err!=KErrNone)
		return err;
	if (err = iCommPort.Open(iCommServer, aName, ECommExclusive, ECommRoleDCE), err!=KErrNone)
		// if any other TSY (or other app) is using comm port, we want to return gracefully
		// with error
		{
		iCommServer.Close();
		return err;
		}
	if (aMode==ECommShared)
		{
		iCommPort.Close();
		if (err = iCommPort.Open(iCommServer, aName, aMode, ECommRoleDCE), err!=KErrNone)
			{
			iCommServer.Close();
			return err;
			}
		}
	return KErrNone;
	}

void MComm::CommClose()
/**
 * The method handles cleanup of the MComm objects.  It cancels an outstanding reads or writes
 * and closes the port and comm server sessions.
 *
 * @return	None
 */
	{
	iCommReader->Cancel();
	iCommWriter->Cancel();
	iCommPort.Close();
	iCommServer.Close();
	}

void MComm::CommConstructL(TInt aReadPriority, TInt aWritePriority)
/**
 * This method is a constructor for the CCommReader and CCommWriter classes.  This method is
 * typically called from the ConstructL of the derived class.  It instansiates a new CCommReader
 * and CCommWriter.
 *
 * @param	aReadPriority is the priority to be associated to the CCommReader active object.
 * @param	aWritePriority is the priority to be associated to the CCommWriter active object.
 * @leave	This function can leave if memory is not available to create the active objects.
 */
	{
	iCommReader = new (ELeave) CCommReader(this, aReadPriority);
	iCommWriter = new (ELeave) CCommWriter(this, aWritePriority);
	};

void MComm::CommWrite(const TDesC8& aDes)
/**
 * This method implements the write to the Comm Server.  It uses the TRequestStatus member of
 * the iCommWriter class and activates the object, indicating a request had been made.
 *
 * @param	aDes, a descriptor reference containing the data to be written to the port.
 * @return	None
 */
	{
	__ASSERT_ALWAYS(iCommWriter!=NULL, HayesPanic(EATCommand_NotConstructed));
	iCommPort.Write(iCommWriter->StatusRef(), aDes);
	iCommWriter->Activate();
	}

void MComm::CommWriteReady()
/**
 * This method is used during initialization for synchronization purposes.  It issues a write
 * of a NULL descriptor to the communications server port.  It then waits for the request to be
 * completed.  This request will be completed when the associated serial port is ready for write
 * data.
 *
 * @return	None
 */
	{
	__ASSERT_ALWAYS(iCommWriter!=NULL, HayesPanic(EATCommand_NotConstructed));
	TRequestStatus status;
	iCommPort.Write(status, TPtrC8(NULL, 0));
	User::WaitForRequest(status);
	}

void MComm::CommRead(TDes8& aDes)
/**
 * This method implements the read from the Comm Server.  The number of bytes read is specified
 * by the length member of the descriptor.  It uses the TRequestStatus member of
 * the iCommReader class and activates the object, indicating a request had been made.
 *
 * @param	aDes, a descriptor reference containing the buffer into which data will be placed.
 *
 * @return	None
 */
	{
	__ASSERT_ALWAYS(iCommReader!=NULL, HayesPanic(EATCommand_NotConstructed));
	iCommPort.Read(iCommReader->StatusRef(), aDes, aDes.Length());
	iCommReader->Activate();
	}

void MComm::CommReadOneOrMore(TDes8& aDes)
/**
 * This method pends a read for one or more characters to the comm server port.  The comm
 * server will return as much data as is ready up to the maximum length 
 * specified in the descriptor, aDes.  As soon as the available data is exhausted (after the
 * first byte) this read will be completed.  This differs from a Read which is posted with a
 * specific length in that the normal Read would pend until the number of characters specified
 * has been read (or the read timed out).  The object is activated, indicating that a request
 * has been made.
 *
 * @param  aDes  When the read completes, the contents of aDes will have been modified.
 */
	{
	__ASSERT_ALWAYS(iCommReader!=NULL, HayesPanic(EATCommand_NotConstructed));
	iCommPort.ReadOneOrMore(iCommReader->StatusRef(), aDes);
	iCommReader->Activate();
	}

void MComm::CommReadReady()
/**
 * This method is used during initialization for synchronization purposes.  It issues a read
 * of with a zero length to the communications server port.  It then waits for the request to be
 * completed.  A side effect of issuing the read to the serial port is that the serial port will
 * be powered up.
 */	{
	__ASSERT_ALWAYS(iCommReader!=NULL, HayesPanic(EATCommand_NotConstructed));
	TRequestStatus status;
	iCommPort.Read(status,iBuf,0);
	User::WaitForRequest(status);
	}

void MComm::CommCancel()
/**
 * The mixin class cancel routine which cancels any outstanding requests for the iCommWriter and
 * iCommReader.  The routine first validates the iCommWriter and iCommReader member pointers.
 *
 * @return	None.
 */
	{
	if (iCommWriter)
		iCommWriter->Cancel();
	if (iCommReader)
		iCommReader->Cancel();
	}

void MComm::CommWriteCancel()
/**
 * The mixin class cancel routine which only cancels outstanding write requests.  The 
 * routine first validates the iCommWriter pointer.
 *
 * @return	None.
 */
	{
	if (iCommWriter)
		iCommWriter->Cancel();
	}

void MComm::CommReadCancel()
/**
 * The mixin class cancel routine which only cancels outstanding read requests.  The 
 * routine first validates the iCommReader pointer.
 *
 * @return	None.
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
 * This constructor adds the CCommWriter active object to the active scheduler of the thread in
 * which it is called.  The constructor initializes the iComm member data and calls the CActive
 * method of this CBase derived class to set the priority.  A new instance of the CCommWriter is
 * created by this constructor.
 *
 * @param	aComm A pointer to the MComm class object to be stored in member data iComm.
 * @param	aPriority The priority at which this active object should be handled by the
 *			active scheduler.
 * @return	None
 */
	{
	__DECLARE_NAME(_S("CCommWriter"));
	CActiveScheduler::Add(this);
	}

CCommWriter::~CCommWriter()
/**
 * This destructor simply requests cancellation any outstanding requests for the iCommWriter object.
 *
 * @return	None.
 */
	{
	Cancel();
	}

void CCommWriter::RunL()
/**
 * This is the RunL function invoked by the active scheduler for the CCommWriter active object.
 * It in turn calls the write complete method accessible via the mixin class stored in the
 * iComm member, passing the status.  The mixin class write complete method is a pure virtual 
 * method which must be implemented by the derived class.
 *
 * @return	None
 */
	{
	iComm->CommWriteComplete(iStatus.Int());
	}

void CCommWriter::DoCancel()
/**
 * This method implments the cancel function necessary for a CActive derived object.  It simply
 * invokes the port specific method to cancel outstanding writes.
 *
 * @return	None
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
 * This constructor adds the CCommReader active object to the active scheduler of the thread in
 * which it is called.  The constructor initializes the iComm member data and calls the CActive
 * method of this CBase derived class to set the priority.  A new instance of the CCommReader is
 * created by this constructor.
 *
 * @param	aComm A pointer to the MComm class object to be stored in member data iComm.
 * @param	aPriority The priority at which this active object should be handled by the
 *			active scheduler.
 * @return	None
 */
	{
	__DECLARE_NAME(_S("CCommReader"));
	CActiveScheduler::Add(this);
	}

CCommReader::~CCommReader()
/**
 * This destructor simply requests cancellation any outstanding requests for the iCommReader object.
 *
 * @return	None.
 */
	{
	Cancel();
	}

void CCommReader::RunL()
/**
 * This is the RunL function invoked by the active scheduler for the CCommReader active object.
 * It in turn calls the read complete method accessible via the mixin class stored in the
 * iComm member, passing the status.  The mixin class read complete method is a pure virtual 
 * method which must be implemented by the derived class.
 *
 * @return	None
 */
	{
	iComm->CommReadComplete(iStatus.Int());
	}

void CCommReader::DoCancel()
/**
 * This method implments the cancel function necessary for a CActive derived object.  It simply
 * invokes the port specific method to cancel outstanding writes.
 *
 * @return	None
 */
	{
	iComm->iCommPort.ReadCancel();
	}

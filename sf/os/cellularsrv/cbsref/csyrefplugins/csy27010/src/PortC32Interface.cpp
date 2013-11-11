//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description:  This file contains the implementation for the CPortC32Interface class.
// *               This class contains methods which are invoked by C32 when the
// *               associated client RComm public API is invoked. These methods
// *               are used to create, configure, read, write, and close logical
// *               serial ports. Instances of this class are created by the CSY's
// *               Port Factory.
//

// PortC32Interface.cpp

/** @file PortC32Interface.cpp
 *
 */

#include <cdbcols.h>
#include "PortC32Interface.h"
#include "Portfactory.h"
#include "Mux0710Protocol.h"
#include "CsyMsgBufBPFrame.h"
#include "ChannelMgrCmdData.h"
#include "CommFrameReaderAo.h"
#include "CsyDebugLogger.h"
#include "CsyGlobals.h"
#include "CommFrameWriterAo.h"

CPortC32Interface* CPortC32Interface::NewL(CPortFactory& aPortFactory, 
										   CPortFactory::TC32PortInfo& aPortInfo)
/**
 * This method uses two phase construction and the cleanup stack to create
 * an instance of class CPortC32Interface.
 *
 * @param aPortFactory - Reference to the port factory
 * @param aPortInfo - Reference to the port information
 * @return Pointer to the created instance
 */
	{
	_LOG_L4C1( "CPortC32Interface::NewL");

	CPortC32Interface* self = new(ELeave) CPortC32Interface(aPortFactory, aPortInfo);
	TCleanupItem closeSelf(CPortFactory::CloseObject, self);
	CleanupStack::PushL(closeSelf);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	}

CPortC32Interface::~CPortC32Interface()
/**
 * Destructor.
 */
	{
	_LOG_L4C1( ">>CPortC32Interface::~CPortC32Interface");

	// let port factory know we are deleted
	iPortFactory.RemoveC32Port(this);
	CompleteOutstandingRequest();

	iMuxChannel->Close(this);

	if (iReadBuf)
		{
		_LOG_L3C1("Delete read buffer");
		delete iReadBuf;
		}

	_LOG_L4C1( "<<CPortC32Interface::~CPortC32Interface");
	}

CPortC32Interface::CPortC32Interface(CPortFactory& aPortFactory,
									 CPortFactory::TC32PortInfo& aPortInfo)
/**
 * Constructor.
 *
 * @param aPortFactory - Reference to the port factory
 * @param aPortInfo - Reference to the port information
 */
: CPortC32InterfaceBase(aPortFactory, aPortInfo)
	{}

void CPortC32Interface::ConstructL(void)
/**
 * Safe constructor
 */
	{
	_LOG_L4C1( "CPortC32Interface::ConstructL");
	CPortC32InterfaceBase::ConstructL();
	}

/********************************************************************************/
/*               Start of utility methods for CPortC32InterfaceBase                 */
/********************************************************************************/

TInt CPortC32Interface::QueryReceiveBuffer(TInt& aLength) const
/**
 * Called by C32 when the client queries the size of the receive buffer,
 * which returns the number of receive characters available to be read by
 * the C32 client RComm instance.
 *
 * @param aLength - Reference to client's buffer length variable
 * @return KErrNone
 */
	{
	_LOG_L4C2( "CPortC32Interface::QueryReceiveBuffer [port=%d]", GetPortNumber());

	aLength = 0;
	if (iReadBuf)
		{
		aLength = iReadBuf->iMsg.Length();
		_LOG_L4C2("iReadBuf aLength=%d", aLength);
		}
	else if (!iFramesWaitingToBeReadList.IsEmpty())
		{
		CCsyMsgBufBpFrame* frame = iFramesWaitingToBeReadList.First();
		if ((frame)&&(frame->iMsg.Length() >= (KAdvOptionHeaderSize + KChecksumSize)))
			{
#ifdef _27010ADVANCEOPTION
			aLength = frame->iMsg.Length() - (KAdvOptionHeaderSize + KChecksumSize);
#else
			aLength = frame->iMsg.Length() - (KBasicOptionHeaderSize + KChecksumSize);
#endif
			_LOG_L4C2("iFramesWaitingToBeReadList aLength=%d", aLength);
			}
		}

	return KErrNone;
	}

void CPortC32Interface::ResetBuffers(TUint aFlags)
/**
 * Called by C32 when the client requests to reset the buffers,
 * by removing all receive and/or transmit messages according to
 * the specified flags.
 *
 * @param aFlags Indicate which buffers (receive and/or transmit) should be reset
 */
	{
	_LOG_L4C2(">>CPortC32Interface::ResetBuffers [aFlags=%d]", aFlags);
	_LOG_L4C2("[port=%d]", GetPortNumber());

	if (aFlags & KCommResetRx)
		{
		_LOG_L4C1("Removing all messages intended for the C32 client");

		RemoveWaitingAllFrames();
		}

	if (aFlags & KCommResetTx)
		{
		_LOG_L4C1("Removing all messages intended for the modem");

		GetMuxChannel()->WriteCancel();
		}

	_LOG_L4C1("<<CPortC32Interface::ResetBuffers");
	}

void CPortC32Interface::SendFrameToClient(CCsyMsgBufBpFrame* aFrame)
/**
 * This method is called by a CSY Channel object when it has a single
 * frame to send to a C32 client RComm object.
 *
 * @param aFrame - Pointer to message to send to client
 */
	{
	_LOG_L4C2( "CPortC32Interface::SendFrameToClient [port=%d]", GetPortNumber());

	if (aFrame)
		{
		// check if queue is empty
		TBool trigger = iFramesWaitingToBeReadList.IsEmpty();

		iFramesWaitingToBeReadList.AddLast(*aFrame);
		if (trigger)
			{
			_LOG_L4C1("No packets already waiting");
			if (iIsReadInProgress)
				{
				_LOG_L4C1("A read is outstanding");
				ReadFromBufOrQueue();
				}
			}

		// inform client new data is available
		SetDataAvailable();
		}
	else
		{
		//MAF __ASSERT_DEBUG(EFalse, PANIC(KPanicIllegalState));
		}
	}

TBool CPortC32Interface::ReadFromBufOrQueue()
/**
 * This method is called to read from buffer or the frame list
 * It will read as much as possible.
 *
 * @return ETrue if complete the read request
 */
	{
	_LOG_L4C2( ">>CPortC32Interface::ReadFromBufOrQueue [port=%d]",GetPortNumber());

	TBool completedTheReadRequest = EFalse;

	TInt err = KErrGeneral;
	TBool cont;
	do
		{
		cont = EFalse;
		if(iReadBuf==NULL)
			{
			//Read data from the frame list
			if (!iFramesWaitingToBeReadList.IsEmpty())
				{
				_LOG_L4C1("Set to first item");
				iReadBuf = iFramesWaitingToBeReadList.First();
				if (iReadBuf)
					{
					_LOG_L4C1("iReadBuf not null");
					// remove msg buf from client list
					iFramesWaitingToBeReadList.Remove(*iReadBuf);

					// subtract checksum field
					TInt frameLength = iReadBuf->iMsg.Length();
					_LOG_L4C2("New read buffer frameLength=%d",frameLength);

					if (frameLength >= KBasicOptionHeaderSize)
						{
						iReadBuf->iMsg.SetLength(frameLength - KChecksumSize);

						// remove leading header ints from frame
#ifdef _27010ADVANCEOPTION
						iReadBuf->iMsg.Delete(0, KAdvOptionHeaderSize);
#else
						iReadBuf->iMsg.Delete(0, KBasicOptionHeaderSize);
#endif
						}
					else
						{
						_LOG_L4C1("Incorrect frame size - freeing read buffer");
						iPortFactory.GetMux0710Protocol()->AddFrameFreeQ(iReadBuf);
						iReadBuf = NULL;
						}
					}
				}
			}
		if (iReadBuf)
			{
			TInt length = -1;
			if (iClientLength - iPos < iReadBuf->iMsg.Length())
				{
				length = iClientLength - iPos;
				_LOG_L4C2("length = %d",length);
				}

			if (iConfig.iTerminatorCount > 0)
				{
				_LOG_L4C2("iTerminatorCount = %d",iConfig.iTerminatorCount);

				// First find terminator
				TInt terminatorLoc = -1;
				TInt loc;
				for (TInt i=0; i< iConfig.iTerminatorCount;i++)
					{
					loc = iReadBuf->iMsg.LocateF(iConfig.iTerminator[i]);
					if (loc > KErrNotFound)
						{
						if (terminatorLoc == KErrNotFound)
							{
							terminatorLoc = loc;
							}
						else
							{
							terminatorLoc = Min(loc,terminatorLoc);
							}
						}
					}
				if (terminatorLoc>KErrNotFound)
					{
					if (length > KErrNotFound)
						length = Min(terminatorLoc + 1,length);
					else
						length = terminatorLoc + 1;
					}
				_LOG_L4C2("length = %d",length);
				}

			_LOG_L4C2("Read buf length %d",iReadBuf->iMsg.Length());

			if ((iReadBuf->iMsg.Length() >= length) && (length > -1))
				{
				_LOG_L2C2("complete partial read: # %d ", length);

				iPartialReadBuf.Copy(&iReadBuf->iMsg[0], length);
				iReadBuf->iMsg.Delete(0, length);

				err = IPCWrite(iClientBuffer, iPartialReadBuf, iPos); 
				if (err)
					{
					_LOG_L1C2("** IPCWrite Error %d **",err);
					}

				CompleteReadRequest(err);
				err = KErrGeneral;

				completedTheReadRequest = ETrue;
				iPos = 0;

				if (iReadBuf->iMsg.Length()==0)
					{
					_LOG_L4C1("All data used - freeing read buffer");
					iPortFactory.GetMux0710Protocol()->AddFrameFreeQ(iReadBuf);
					iReadBuf = NULL;
					}
				}
			else
				{
				err = IPCWrite(iClientBuffer, iReadBuf->iMsg, iPos);
				if (err)
					{
					_LOG_L1C2("** IPCWrite Error %d **",err);
					}

				_LOG_L4C3( "Read: iPos = %d, add %d bytes", iPos, iReadBuf->iMsg.Length());

				//try read next frame in the list
				cont = ETrue; 
				iPos += iReadBuf->iMsg.Length();

				_LOG_L4C1("Freeing read buffer");
				iPortFactory.GetMux0710Protocol()->AddFrameFreeQ(iReadBuf);
				iReadBuf = NULL;
				}
			}
		}
	while (cont);

	if (iPos>0)
		{
		if (iOneOrMore)
			{
			CompleteReadRequest(err);
			iPos = 0;
			completedTheReadRequest = ETrue;
			}
		else 
			{
			//normal read and have not filled the buffer yet
			_LOG_L4C3( "Not filled buffer yet iPos = %d, iClientLength = %d", iPos, iClientLength);
			}
		}

	_LOG_L4C2( "<<CPortC32Interface::ReadFromBufOrQueue [completedTheReadRequest=%d]",completedTheReadRequest);
	return completedTheReadRequest;
	}
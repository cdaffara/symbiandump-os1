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

/** @file PortC32InterfaceIp.cpp
 *
 * This file contains the implementation for the CPortC32InterfaceIp class.
 * This class contains methods which are invoked by C32 when the
 * associated client RComm public API is invoked. These methods
 * are used to create, configure, read, write, and close logical
 * serial ports. Instances of this class are created by the CSY's
 * Port Factory.
 *
 */

#include <cdbcols.h>
#include "PortC32InterfaceIp.h"
#include "Portfactory.h"
#include "Mux0710Protocol.h"
#include "CsyMsgBufBPFrame.h"
#include "ChannelMgrCmdData.h"
#include "CommFrameReaderAo.h"
#include "CsyDebugLogger.h"
#include "CsyGlobals.h"
#include "CommFrameWriterAo.h"

CPortC32InterfaceIp* CPortC32InterfaceIp::NewL(CPortFactory& aPortFactory, 
										   CPortFactory::TC32PortInfo& aPortInfo)
/**
 * This method uses two phase construction and the cleanup stack to create
 * an instance of class CPortC32InterfaceIp.
 *
 * @param aPortFactory - Reference to the port factory
 * @param aPortInfo - Reference to the port information
 * @return Pointer to the created instance
 */
	{
	_LOG_L4C1("CPortC32InterfaceIp::NewL");

	CPortC32InterfaceIp* self =
		new(ELeave) CPortC32InterfaceIp(aPortFactory, aPortInfo);

	TCleanupItem closeSelf(CPortFactory::CloseObject, self);
	CleanupStack::PushL(closeSelf);
	self->ConstructL();
	CleanupStack::Pop(self);

	return (self);
	}

CPortC32InterfaceIp::~CPortC32InterfaceIp()
/**
 * Destructor.
 */
	{
	_LOG_L4C1(">>CPortC32InterfaceIp::~CPortC32InterfaceIp");

	_LOG_L3C1("Remove port from the port factory");

	// let port factory know we are deleted
	iPortFactory.RemoveC32Port(this);
	CompleteOutstandingRequest();

	_LOG_L3C1("Release any packets going to the C32 client");

	// remove all frames to do with the most recent IP packet
	CCsyMsgBufBpFrame* ipPacketFrame = NULL;
	iFramesReceivedForIpPacketIter.SetToFirst();
	while ((ipPacketFrame = iFramesReceivedForIpPacketIter++) != NULL)
		{
		iFramesReceivedForIpPacket.Remove(*ipPacketFrame);
		delete ipPacketFrame;
		}

	_LOG_L4C1("<<CPortC32InterfaceIp::~CPortC32InterfaceIp");
	}

CPortC32InterfaceIp::CPortC32InterfaceIp(CPortFactory& aPortFactory,
									 CPortFactory::TC32PortInfo& aPortInfo)
/**
 * Constructor.
 *
 * @param aPortFactory - Reference to the port factory
 * @param aPortInfo - Reference to the port information
 */
: CPortC32InterfaceBase(aPortFactory, aPortInfo),
   iFramesReceivedForIpPacket(_FOFF(CCsyMsgBufBpFrame, iMsgLink)),
   iFramesReceivedForIpPacketIter(iFramesReceivedForIpPacket)
	{}

void CPortC32InterfaceIp::ConstructL()
/**
 * Safe constructor
 */
	{
	_LOG_L4C1(">>CPortC32InterfaceIp::ConstructL");

	CPortC32InterfaceBase::ConstructL();

	_LOG_L4C1("<<CPortC32InterfaceIp::ConstructL");
	}

/********************************************************************************/
/*               Start of utility methods for CPortC32InterfaceBase             */
/********************************************************************************/

TInt CPortC32InterfaceIp::QueryReceiveBuffer(TInt& aLength) const
/**
 * Called by C32 when the client queries the size of the receive buffer,
 * which returns the number of receive characters available to be read by
 * the C32 client RComm instance.
 *
 * @param aLength - Reference to client's buffer length variable
 * @return KErrNone
 */
	{
	_LOG_L4C2("CPortC32InterfaceIp::QueryReceiveBuffer [port=%d]", GetPortNumber());

	aLength = 0;

	// MAF

	return KErrNone;
	}

void CPortC32InterfaceIp::ResetBuffers(TUint aFlags)
/**
 * Called by C32 when the client requests to reset the buffers,
 * by removing all receive and/or transmit messages according to
 * the specified flags.
 *
 * @param aFlags Indicate which buffers (receive and/or transmit) should be reset
 */
	{
	_LOG_L4C3(">>CPortC32InterfaceIp::ResetBuffers [aFlags=%d,port=%d]",
		aFlags,GetPortNumber());

	if (aFlags & KCommResetRx)
		{
		_LOG_L4C1("Removing all messages intended for the C32 client");

		RemoveWaitingAllFrames();

		// remove all frames to do with the most recent IP packet
		CCsyMsgBufBpFrame* ipPacketFrame = NULL;
		iFramesReceivedForIpPacketIter.SetToFirst();
		while ((ipPacketFrame = iFramesReceivedForIpPacketIter++) != NULL)
			{
			iFramesReceivedForIpPacket.Remove(*ipPacketFrame);
			delete ipPacketFrame;
			}
		}

	if (aFlags & KCommResetTx)
		{
		_LOG_L4C1("Removing all messages intended for the modem");

		GetMuxChannel()->WriteCancel();
		}

	_LOG_L4C1("<<CPortC32InterfaceIp::ResetBuffers");
	}

void CPortC32InterfaceIp::PlaceIpPacketFramesOntoFreeList()
/**
 * Place all frames to do with recent IP packet onto free list
 */
	{
	_LOG_L4C2(">>CPortC32InterfaceIp::PlaceIpPacketFramesOntoFreeList [port=%d]", GetPortNumber());

	CCsyMsgBufBpFrame* frame = NULL;
	iFramesReceivedForIpPacketIter.SetToFirst();
	while ((frame = iFramesReceivedForIpPacketIter++) != NULL)
		{
		iFramesReceivedForIpPacket.Remove(*frame);
		iPortFactory.GetMux0710Protocol()->AddFrameFreeQ(frame);
		}
	iCurrentIpPacketLength = 0;

	_LOG_L4C1("<<CPortC32InterfaceIp::PlaceIpPacketFramesOntoFreeList");
	}

void CPortC32InterfaceIp::PlaceIpPacketFramesOntoReadList()
/**
 * Place all frames to do with recent IP packet onto read list
 */
	{
	_LOG_L4C2(">>CPortC32InterfaceIp::PlaceIpPacketFramesOntoReadList [port=%d]", GetPortNumber());

	CCsyMsgBufBpFrame* frame = NULL;
	iFramesReceivedForIpPacketIter.SetToFirst();
	while ((frame = iFramesReceivedForIpPacketIter++) != NULL)
		{
		iFramesReceivedForIpPacket.Remove(*frame);
		iFramesWaitingToBeReadList.AddLast(*frame);
		}
	iCurrentIpPacketLength = 0;
	iNumberOfPacketsWaiting++;
	_LOG_L4C2("iNumberOfPacketsWaiting=%d",iNumberOfPacketsWaiting);

	if (iNumberOfPacketsWaiting == KMaxPacketsOutstandingForC32Client)
		{
		_LOG_L2C1("Client appears to be slow at reading");
		_LOG_L2C3("iNumberOfPacketsWaiting %d > KMaxPacketsOutstandingForC32Client %d)",
			iNumberOfPacketsWaiting,KMaxPacketsOutstandingForC32Client);

		iMuxChannel->SetCsyToModemFlowControl(EFlowControlOn);
		}

	_LOG_L4C1("<<CPortC32InterfaceIp::PlaceIpPacketFramesOntoReadList");
	}

void CPortC32InterfaceIp::SendFrameToClient(CCsyMsgBufBpFrame* aFrame)
/**
 * This method is called by a CSY Channel object when it has a single
 * frame to send to a C32 client RComm object.
 *
 * @param aFrame - Pointer to message to send to client
 */
	{
	_LOG_L4C2(">>CPortC32InterfaceIp::SendFrameToClient [port=%d]", GetPortNumber());

	if (aFrame)
		{
		// frame validation
		if (aFrame->iMsg.Length() <= KAdvOptionNumOfNonDataOctets)
			{
			_LOG_L1C1("** Does not appear to be a valid frame - disregarding **");

			// Place received frames on the empty list
			iPortFactory.GetMux0710Protocol()->AddFrameFreeQ(aFrame);
			PlaceIpPacketFramesOntoFreeList();

			_LOG_L4C1("<<CPortC32InterfaceIp::SendFrameToClient - bad frame");
			return;
			}

		TUint8 framePosition = aFrame->GetType4FrameSequence();
		_LOG_L4C2("framePosition = 0x%02x",framePosition);

		// read buffer validation
		if (iFramesReceivedForIpPacket.IsEmpty())
			{
			// There is no read buffer allocated for this frame
			switch (framePosition)
				{
			case KSingleFrame:
			case KStartMultiFrame:
				{
				_LOG_L4C1("Starting new IP frame list");
				iCurrentIpPacketLength = 0;
				}
				break;
			case KEndMultiFrame:
			case KMiddleMultiFrame:
			default:
				// assumption here is that because no read buffer is allocated then
				// there cannot have been a valid start frame
				_LOG_L1C1("** Missed start of packet - disregarding **");

				// Place received frame on the empty list
				iPortFactory.GetMux0710Protocol()->AddFrameFreeQ(aFrame);

				_LOG_L4C1("<<CPortC32InterfaceIp::SendFrameToClient - no start");
				return;
				}
			}
		else if ((framePosition == KSingleFrame)||(framePosition == KStartMultiFrame))
			{
			// There's been a comms error and an end packet has been lost
			_LOG_L1C1("** Unexpected Start/Single frame - disregarded previous frames **");

			// Place received frames on the empty list
			PlaceIpPacketFramesOntoFreeList();

			// Continue processing this frame
			_LOG_L4C1("Starting new IP frame list");
			iCurrentIpPacketLength = 0;
			}

		// If have got to here then IP frame list exists

		TInt length = aFrame->iMsg.Length() - KAdvOptionNumOfNonDataOctets;
		_LOG_L4C2("length=%d",length);

		// length validation
		if ((iCurrentIpPacketLength + length) > KMaxIpPacketSize)
			{
			// The length of the data in the new frame would exceed that available
			// in the read buffer.

			_LOG_L1C3("** iCurrentIpPacketLength + length %d exceeds max size %d **",
				(iCurrentIpPacketLength + length),KMaxIpPacketSize);

			// Place received frames on the empty list
			iPortFactory.GetMux0710Protocol()->AddFrameFreeQ(aFrame);
			PlaceIpPacketFramesOntoFreeList();

			_LOG_L4C1("<<CPortC32InterfaceIp::SendFrameToClient - frame sized exceeded");
			return;
			}

		// If have got to here IP frame list exists and length is valid
		iCurrentIpPacketLength += length;

		TBool addReadBufferToClientList = EFalse;
		switch (framePosition)
			{
		// Single frame	
		case KSingleFrame:
			_LOG_L4C1("Only frame");
			iFramesReceivedForIpPacket.AddFirst(*aFrame);
			addReadBufferToClientList = ETrue;
			break;

		// Multiframe
		case KStartMultiFrame:
			_LOG_L4C1("Start multiframe");
			iFramesReceivedForIpPacket.AddFirst(*aFrame);
			break;

		case KMiddleMultiFrame:
			_LOG_L4C1("Middle multiframe");
			iFramesReceivedForIpPacket.AddLast(*aFrame);
			break;

		case KEndMultiFrame:
			_LOG_L4C1("Last multiframe");
			iFramesReceivedForIpPacket.AddLast(*aFrame);
			addReadBufferToClientList = ETrue;
			break;

		// Unknown
		default:
			_LOG_L1C2("** Unknown framePosition %d - ignoring **",
				framePosition);

			// Place received frame on the empty list
			iPortFactory.GetMux0710Protocol()->AddFrameFreeQ(aFrame);
			PlaceIpPacketFramesOntoFreeList();

			_LOG_L4C1("<<CPortC32InterfaceIp::SendFrameToClient - unknown frame pos");
			return;
			}

		if (addReadBufferToClientList)
			{
			// Give iFramesWaitingToBeReadList ownership of the frames making up the IP Packet
			_LOG_L4C1("Transfer frames to client msg list");

			// check if queue is empty
			TBool trigger = iFramesWaitingToBeReadList.IsEmpty();

			PlaceIpPacketFramesOntoReadList();

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
		}
	else
		{
		_LOG_L1C1("** aFrame is null **");
		// MAF __ASSERT_DEBUG(EFalse, PANIC(KPanicIllegalState));
		}

	_LOG_L4C1("<<CPortC32InterfaceIp::SendFrameToClient");
	}

TBool CPortC32InterfaceIp::CreateIpPacketFromFrames()
/**
 * We assume that all validation has been done during SendFrameToClient so only valid
 * data exists on the iFramesWaitingToBeReadList frame list.
 *
 * @return ETrue if packet retrieved successfully
 */
	{
	_LOG_L4C2(">>CPortC32InterfaceIp::CreateIpPacketFromFrames [port=%d]", GetPortNumber());

	iIpPacket.Zero();

	TUint8	framePosition = 0;
	TInt	length = 0;
	TBool	packetRetrieved = EFalse;

	// Remove all frames to do with the same IP Packet from waiting to be read list
	CCsyMsgBufBpFrame* frame = NULL;
	iFramesWaitingToBeReadIter.SetToFirst();
	while ((frame = iFramesWaitingToBeReadIter++) != NULL)
		{
		iFramesWaitingToBeReadList.Remove(*frame);

		framePosition = frame->GetType4FrameSequence();
		_LOG_L4C2("framePosition = 0x%02x",framePosition);

		length = frame->iMsg.Length() - KAdvOptionNumOfNonDataOctets;
		_LOG_L4C2("length=%d",length);

		iIpPacket.Append(&frame->iMsg[KAdvOptionType4StartOfMessageData], length);

		iPortFactory.GetMux0710Protocol()->AddFrameFreeQ(frame);

		if ((framePosition == KSingleFrame)||(framePosition == KEndMultiFrame))
			{
			_LOG_L4C1("retrieved packet");
			packetRetrieved = ETrue;

			if (iNumberOfPacketsWaiting == KMaxPacketsOutstandingForC32Client)
				{
				_LOG_L2C1("Client appears to be reading again");

				// Assume it is okay to set FC to Off even in low free frame
				// condition since just freed a frame.

				iMuxChannel->SetCsyToModemFlowControl(EFlowControlOff);
				}

			iNumberOfPacketsWaiting--;
			_LOG_L4C2("iNumberOfPacketsWaiting=%d",iNumberOfPacketsWaiting);
			break;
			}
		}

	_LOG_L4C2("<<CPortC32InterfaceIp::CreateIpPacketFromFrames [packetRetrieved=%d]",packetRetrieved);
	return packetRetrieved;
	}

TBool CPortC32InterfaceIp::ReadFromBufOrQueue()
/**
 * This method is called to read from buffer or the frame list
 * It will read as much as possible.
 *
 * @return ETrue if complete the read request
 */
	{
	_LOG_L4C2(">>CPortC32InterfaceIp::ReadFromBufOrQueue [port=%d]", GetPortNumber());

	TBool completedRead = EFalse;
	TInt ret = KErrNone;

	_LOG_L4C2("iOneOrMore=%d",iOneOrMore);
	_LOG_L4C2("iClientLength=%d",iClientLength);

	while ((CreateIpPacketFromFrames())&&(!ret))
		{
		TInt ipPacketLength = iIpPacket.Length();
		TInt length = -1; 
		if (iClientLength - iPos < ipPacketLength)
			{
			length = iClientLength - iPos;
			_LOG_L4C2("length remaining = %d",length);
			}

		if ((length > -1) && (ipPacketLength >= length))
			{
			// MAF should partial reads be allowed?
			_LOG_L2C1("Partial read?");

			}
		else
			{
			_LOG_L4C3("read buf %d >= length %d ", ipPacketLength, length);

			ret = IPCWrite(iClientBuffer, iIpPacket, iPos);
			if (ret)
				{
				_LOG_L1C2("** IPCWrite Error %d **",ret);
				}

			_LOG_L4C3("Read: iPos = %d, add %d bytes", iPos, ipPacketLength);

			iPos += ipPacketLength;
			}
		}

	if (iPos>0)
		{
		if (iOneOrMore)
			{
			CompleteReadRequest(ret);

			iPos = 0;
			completedRead = ETrue;
			}
		else
			{
			//normal read? and have not filled the buffer yet
			_LOG_L4C3( "Not filled buffer yet iPos = %d, iClientLength = %d", iPos, iClientLength);
			}
		}
	
	_LOG_L4C2("<<CPortC32InterfaceIp::ReadFromBufOrQueue [completedRead=%d]", completedRead);
	return completedRead;
	}


/***
TBool CPortC32InterfaceIp::ReadFromBufOrQueue()
**
 * This method is called to read from buffer or the frame list
 * It will read as much as possible.
 *
 * @return ETrue if complete the read request
 *
	{
	_LOG_L4C2(">>CPortC32InterfaceIp::ReadFromBufOrQueue [port=%d]", GetPortNumber());

	TBool completedRead = EFalse;
	TInt err = KErrGeneral;
	TBool cont;
	do
		{
		cont = EFalse;
		if(iQueuedIpPacket == NULL)
			{
			//Read data from the frame list
			if (!iMsgToClientList.IsEmpty())
				{
				_LOG_L4C1("Set to first item");
				iQueuedIpPacket = iMsgToClientList.First();
				if (iQueuedIpPacket)
					{
					_LOG_L4C1("iQueuedIpPacket not null");
					// remove msg buf from client list
					iMsgToClientList.Remove(*iQueuedIpPacket);
					}
				}
			}
		if (iQueuedIpPacket)
			{
			TInt length = KErrNotFound; // <- yeaks!
			if (iClientLength - iPos < iQueuedIpPacket->iMsg.Length())
				{
				length = iClientLength - iPos;
				_LOG_L4C2("length remaining = %d",length);
				}

			if (iConfig.iTerminatorCount > 0)
				{
				_LOG_L4C2("iTerminatorCount = %d",iConfig.iTerminatorCount);

				// First find terminator
				TInt terminatorLoc = KErrNotFound; // <- yeaks!
				TInt loc;
				for (TInt i = 0; i < iConfig.iTerminatorCount; i++)
					{
					loc = iQueuedIpPacket->iMsg.LocateF(iConfig.iTerminator[i]);
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
				if (terminatorLoc > KErrNotFound)
					{
					if (length > KErrNotFound)
						length = Min(terminatorLoc + 1,length);
					else
						length = terminatorLoc + 1;
					}
				_LOG_L4C2("length = %d",length);
				}

			_LOG_L4C2("Read buf length %d",iQueuedIpPacket->iMsg.Length());

			if ((iQueuedIpPacket->iMsg.Length() >= length) && (length > KErrNotFound))
				{
				LOGTEXT2(__DEBUGLEVELMAJOR__,"complete partial read: # %d ", length);

				iPartialReadBuf.Copy(&iQueuedIpPacket->iMsg[0], length);
				iQueuedIpPacket->iMsg.Delete(0, length);

				err = IPCWrite(iClientBuffer, iPartialReadBuf, iPos); 
				if (err)
					{
					_LOG_L1C2("** IPCWrite Error %d **",err);
					}

				CompleteReadRequest(err);
				err = KErrGeneral;

				completedRead = ETrue;
				iPos = 0;

				if (iQueuedIpPacket->iMsg.Length() == 0)
					{
					_LOG_L4C1("All data used - freeing read buffer");
					AddToFreeC32BufferQ(iQueuedIpPacket);
					iQueuedIpPacket = NULL;
					}
				}
			else
				{
				LOGTEXT3(__DEBUGLEVELMAJOR__,"read buf %d >= length %d ", iQueuedIpPacket->iMsg.Length(), length);

				err = IPCWrite(iClientBuffer, iQueuedIpPacket->iMsg, iPos);

				_LOG_L4C3("Read: iPos = %d, add %d bytes", iPos, iQueuedIpPacket->iMsg.Length());

				cont = ETrue; //try read next frame in the list
				iPos = iPos + iQueuedIpPacket->iMsg.Length();

				_LOG_L4C1("Freeing read buffer");
				AddToFreeC32BufferQ(iQueuedIpPacket);
				iQueuedIpPacket = NULL;
				}
			}
		}
	while (cont);

	if (iPos>0)
		{
		if (iOneOrMore)
			{
			if (err)
				{
				_LOG_L1C2("** IPCWrite Error %d **",err);
				}

			CompleteReadRequest(err);

			iPos = 0;
			completedRead = ETrue;
			}
		else
			{
			//normal read and have not filled the buffer yet
			_LOG_L4C3( "Not filled buffer yet iPos = %d, iClientLength = %d", iPos, iClientLength);
			//iToClientMsgQAo->IssueNotificationRequest();
			}
		}

	_LOG_L4C2("<<CPortC32InterfaceIp::ReadFromBufOrQueue [ret=%d]",completedRead);
	return completedRead;
	}
***/



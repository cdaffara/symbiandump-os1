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
// * Description:  This file contains the implemenation for the CCommFrameWriterAo class.
// *               This class is used to handle the write operations to the serial port
// *               logical device driver.
//

// CommFrameWriterAo.cpp

/** @file CommFrameWriterAo.cpp
 *
 * This file contains the implemenation for the CCommFrameWriterAo class.
 * This class is used to handle the write operations to the serial port
 * logical device driver.
 */

#include "CommFrameWriterAo.h"
#include "CsyMsgBufBPFrame.h"
#include "Mux0710Protocol.h"
#include "CsyDebugLogger.h"
#include "PortC32InterfaceBase.h"

CCommFrameWriterAo* CCommFrameWriterAo::NewL(CPortFactory* aParent, CMux0710Protocol* aMux0710Protocol)
/**
 * This method uses two phase construction and the cleanup stack to
 * create an instance of class CCommFrameWriterAo. 
 *
 * @param aParent - Pointer to the parent object
 * @param aMux0710Protocol - Pointer to mux protocol object
 * @return Pointer to new instance of CCommFrameWriterAo
 */
    {
	_LOG_L4C1("CCommFrameWriterAo::NewL");

    CCommFrameWriterAo* obj = new (ELeave) CCommFrameWriterAo(aParent, aMux0710Protocol);
    CleanupStack::PushL(obj);
    obj->ConstructL();
    CleanupStack::Pop();
    return (obj);
    }


CCommFrameWriterAo::~CCommFrameWriterAo()
/**
 * Destructor.
 */
    {
	_LOG_L4C1("CCommFrameWriterAo::~CCommFrameWriterAo");

    Cancel();

 	// Remove the frames in Write Buf List
	CCsyMsgBufBpFrame* frame;
	iWriteFrameBufIter.SetToFirst();
	while ((frame = iWriteFrameBufIter++) != NULL)
		{
		iWriteFrameBufList.Remove(*frame);
		delete frame;
	 	}

 	// Remove the frames in waiting list
	iWaitingForFcOffIter.SetToFirst();
	while ((frame = iWaitingForFcOffIter++) != NULL)
		{
		iWaitingForFcOffList.Remove(*frame);
		delete frame;
	 	}
   }

CCommFrameWriterAo::CCommFrameWriterAo(CPortFactory* aParent, CMux0710Protocol* aMux0710Protocol)
/**
 * Constructor.
 * @param aParent - Pointer to the parent object
 * @param aMux0710Protocol - Pointer to mux protocol object
 */
:	CCommReadWriteBaseAo(aParent, aMux0710Protocol, KFrameWriterAoPriority),
	iWriteFrameBufList(_FOFF(CCsyMsgBufBpFrame, iMsgLink)),
	iWriteFrameBufIter(iWriteFrameBufList),
	iWaitingForFcOffList(_FOFF(CCsyMsgBufBpFrame, iMsgLink)),
	iWaitingForFcOffIter(iWaitingForFcOffList)
	{}


void CCommFrameWriterAo::ConstructL()
/**
 * Safe constructor
 */
    {
	_LOG_L4C1("CCommFrameWriterAo::ConstructL");
	SetBuffersL();
    }

void CCommFrameWriterAo::RunL()
/**
 * This method is called when a write to the LDD completes.
 */
	{
	_LOG_L4C1(" ");
	_LOG_L4C2(">>CCommFrameWriterAo::RunL [iStatus=%d] - written to LDD",
		iStatus.Int());

	if (iStatus.Int())
		{
		_LOG_L1C2("** Error writing to LDD ** [iStatus=%d]",iStatus.Int());

		if (!iCompleteWhenSent)
			{
			// The frame being sent was not the last or only one for this dlc, other
			// frames exist

			// go through list and remove other frames to send for this dlc
			RemoveAnyDlcFramesOnWriteList(iDlcNum, EFalse);

			iCompleteWhenSent = ETrue;
			}
		}

	if (iCompleteWhenSent)
		{
		iCompleteWhenSent = EFalse;

		_LOG_L3C2("Complete write [iDlcNum=%d]",iDlcNum);
		CompleteWrite(iDlcNum,iStatus.Int());
		}

	// check for another message that needs to be sent to the baseband
	CCsyMsgBufBpFrame* bpFrame = GetFrameToWrite();
	if (bpFrame)
		{
		TInt ret = KErrNone;
		do
			{
			ret = WriteFrame(bpFrame);
			if (ret)
				{
				_LOG_L1C2("** Write frame failed [ret=%d] **",ret);
				if (!iCompleteWhenSent)
					{
					// go through list and remove other frames to send for this dlc
					RemoveAnyDlcFramesOnWriteList(iDlcNum, EFalse);
					}
				}

			// Loop around if there is an error and try and send the next frame
			}
			while (ret);
		}
	else
		{
		_LOG_L3C1("Finished all writes - nothing more to send to LDD");
		}

	_LOG_L4C1("<<CCommFrameWriterAo::RunL");
	_LOG_L3C1(" "); // please leave this separator in
	}

void CCommFrameWriterAo::DoCancel()
/**
 * Cancel a pending write
 */
    {
	_LOG_L4C1("CCommFrameWriterAo::DoCancel - cancelling LDD write");

	iCommPort->WriteCancel();

 	// Remove the frames in Write Buf List
	CCsyMsgBufBpFrame* frame;
	iWriteFrameBufIter.SetToFirst();
	while ((frame = iWriteFrameBufIter++) != NULL)
		{
		iWriteFrameBufList.Remove(*frame);
		iMux0710Protocol->AddFrameFreeQ(frame);
	 	}

 	// Remove the frames in waiting list
	iWaitingForFcOffIter.SetToFirst();
	while ((frame = iWaitingForFcOffIter++) != NULL)
		{
		iWaitingForFcOffList.Remove(*frame);
		iMux0710Protocol->AddFrameFreeQ(frame);
	 	}
    }

TInt CCommFrameWriterAo::Write(CCsyMsgBufBpFrame* aBpFrame,
							   TBool aHighPriority)
/**
 * This method is called to transmit a frame to the baseband.
 *
 * @param aBpFrame - Pointer to frame
 * @param aHighPriority - Flag to indicate a high priority frame
 */
    {
	_LOG_L4C3(">>CCommFrameWriterAo::Write [aBpFrame=0x%x, aHighPriority=%d]",
		aBpFrame,aHighPriority);

	TInt ret = KErrNone;

	// 1st check if we are already transmitting a frame
	if (!IsActive())
		{
		_LOG_L4C1("Not currently writing a frame");
		ret = WriteFrame(aBpFrame);
		}
	else
		{
		// add frame to the list of frames that need to be sent to the BP
		_LOG_L2C1("Already writing a frame - add to queue");
		AddToWaitingToSendList(aBpFrame, aHighPriority);
		}

	_LOG_L4C2("<<CCommFrameWriterAo::Write [ret=%d]",ret);
	return ret;
	}

TInt CCommFrameWriterAo::WriteFrame(CCsyMsgBufBpFrame* aBpFrame)
/**
 * This method is called to transmit a frame to the baseband.
 *
 * @param aBpFrame - Pointer to frame
 */
    {
	_LOG_L4C2(">>CCommFrameWriterAo::WriteFrame [aBpFrame=0x%x]",
		aBpFrame);

	TInt ret = KErrNone;

	iDlcNum = aBpFrame->MsgDlc();
	iCompleteWhenSent = aBpFrame->CompleteWhenSent();

	if (iBuf == NULL)
		{
		_LOG_L1C1("** Failure to alloc iBuf **");

		ret = KErrNoMemory;
		_LOG_L4C2("<<CCommFrameWriterAo::WriteFrame [ret=%d]",ret);
		return ret;
		}

//*********************************************************
// MAF tidy up
#ifdef _DEBUG
	//log out frame
	TInt len = aBpFrame->iMsg.Length();
	_LOG_L3C3("[0x%02x] Tx Got Start dlc=%d", aBpFrame->iMsg[0], aBpFrame->MsgDlc());
	for(TInt k = 1; k < len-1; k++)
		{
		_LOG_L3C2("[0x%02x]",aBpFrame->iMsg[k]);
		}
	_LOG_L3C2("[0x%02x] Tx Frame End", aBpFrame->iMsg[len-1]);
	if (iMux0710Protocol->IsMuxModeEnabled())
		{
		if ((aBpFrame->iMsg[2] & 0xEF) == KCsy0710CTLUIH)
			{
#ifdef __DEBUGLOGFILE__
			TBuf8<200> tt;
#else
			TBuf16<200> tt;
#endif
			tt.Copy(aBpFrame->iMsg);

			TInt ttLength = tt.Length();
			tt.SetLength(ttLength);
#ifdef __LOGDEBUGLEVELMINOR__
			_LOG_L3C3("Sent >>>> %d: %S", aBpFrame->iMsg[1] >> 2, &tt);
#endif
			}
		}
#endif
//*********************************************************

	// copy message to local buffer
#ifdef _27010ADVANCEOPTION

	if (iMux0710Protocol->IsMuxModeEnabled())
		{
		TInt length = aBpFrame->iMsg.Length() - 1;
		TUint8 mask = 1<<5;

		iBuf->Zero();

		 // start flag
		iBuf->Append(aBpFrame->iMsg[0]);

		// check data for flag or escape character
		for (TInt i=1; i < length; i++)
			{
			TUint8 tmp = aBpFrame->iMsg[i];
			if ((tmp == KCsy0710StartEndFlag)||(tmp== KCsy0710EscapeByte))
				{
				_LOG_L3C1("Adding escape byte");
				iBuf->Append(KCsy0710EscapeByte);
				tmp = (TUint8) (tmp^mask);
				}
			iBuf->Append(tmp);
			}

		//ending flag
		iBuf->Append(aBpFrame->iMsg[length]);  
		}
	else
		iBuf->Copy(aBpFrame->iMsg);

#else

	// Basic option - no escape chars
	iBuf->Copy(aBpFrame->iMsg);

#endif

	// free the caller's frame buffer
	iMux0710Protocol->AddFrameFreeQ(aBpFrame);

	// invoke LDD
	_LOG_L3C1("Sending to LDD");
	iStatus = KRequestPending; //MAF why is this being set to KRequestPending?
 	SetActive();
	iCommPort->Write(iStatus, *iBuf, iBuf->Length());

	_LOG_L4C2("<<CCommFrameWriterAo::WriteFrame [ret=%d]",ret);
	return ret;
	}

void CCommFrameWriterAo::WriteCancel()
/**
 * Cancel the current write operation.
 * @return void
 */
    {
    _LOG_L4C1("CCommFrameWriterAo::WriteCancel");
	Cancel();
    }

void CCommFrameWriterAo::AddToWaitingToSendList(CCsyMsgBufBpFrame* aBpFrame, TBool aHighPriority)
/**
 * This method is called to by C32 Interface objects to add
 * the specified frame to the baseband queue.
 *
 * @param aBpFrame - Pointer to the frame buffer
 * @param aHighPriority - Flag to indicate a high priority frame
 */
	{
	_LOG_L4C3(">>CCommFrameWriterAo::AddToWaitingToSendList [aBpFrame=0x%x,aHighPriority=%d]",
		aBpFrame, aHighPriority);

	if (aHighPriority)
		iWriteFrameBufList.AddFirst(*aBpFrame);
	else
		iWriteFrameBufList.AddLast(*aBpFrame);

	_LOG_L4C1("<<CCommFrameWriterAo::AddToWaitingToSendList");
	}

CCsyMsgBufBpFrame* CCommFrameWriterAo::GetFrameToWrite()
/**
 * This method checks the queue for a frame that needs to be sent
 * to the baseband.  If there is a message then deque it from the queue and
 * return a pointer to it, else return NULL.
 *
 * @return Pointer to the frame to be written or NULL
 */
	{
	_LOG_L4C1(">>CCommFrameWriterAo::GetFrameToWrite");

	CCsyMsgBufBpFrame* frame = NULL;
	if (!iWriteFrameBufList.IsEmpty())
		{
		frame = iWriteFrameBufList.First();
		if (frame)
			iWriteFrameBufList.Remove(*frame);
		}

	_LOG_L4C2("<<CCommFrameWriterAo::GetFrameToWrite [frame=0x%x]",frame);
	return frame;
	}

void CCommFrameWriterAo::RemoveAnyDlcFramesOnWriteList(TUint8 aDlcNum, TBool aPlaceOnWaitList)
/**
 * This method transfers any frames on the write list from the dlc specified
 * to the waiting list.
 *
 * @param aDlcNum is the dlc number of the frames to move to the waiting list
 * @param aPlaceOnWaitList indicates whether to free the frame or place onto the
 *        wait list.
 */
	{
	_LOG_L4C3(">>CCommFrameWriterAo::RemoveAnyDlcFramesOnWriteList [aDlcNum=%d, aPlaceOnWaitList=%d]",
		aDlcNum,aPlaceOnWaitList);

	if (iWriteFrameBufList.IsEmpty())
		{
		// No frames to transfer
		_LOG_L4C1("No frames on write list");
		}
	else
		{
		CCsyMsgBufBpFrame* frame = NULL;
		iWriteFrameBufIter.SetToFirst();
		while ((frame = iWriteFrameBufIter++) != NULL)
			{
			if (frame->GetDlcNum() == aDlcNum)
				{
				iWriteFrameBufList.Remove(*frame);
				if (aPlaceOnWaitList)
					{
					_LOG_L4C1("Transferring frame to wait list");
					iWaitingForFcOffList.AddLast(*frame);
					}
				else
					{
					// Adding frame to the free list
					iMux0710Protocol->AddFrameFreeQ(frame);
					}
				}
			}
		}

	_LOG_L4C1("<<CCommFrameWriterAo::RemoveAnyDlcFramesOnWriteList");
	}

void CCommFrameWriterAo::RemoveAnyDlcFramesFromWaitList(TUint8 aDlcNum, TBool aPlaceOnWriteList)
/**
 * This method transfers any frames on the waiting list from the dlc specified
 * to the write list.
 *
 * @param aDlcNum is the dlc number of the frames to move to the write list
 */
	{
	_LOG_L4C3(">>CCommFrameWriterAo::RemoveAnyDlcFramesFromWaitList [aDlcNum=%d,aPlaceOnWriteList=%d]",
		aDlcNum,aPlaceOnWriteList);

	if (iWaitingForFcOffList.IsEmpty())
		{
		// No frames to transfer
		_LOG_L4C1("No frames on waiting list");
		}
	else
		{
		TBool writeQueueWasEmpty = iWriteFrameBufList.IsEmpty();

		CCsyMsgBufBpFrame* frame = NULL;
		iWaitingForFcOffIter.SetToFirst();
		while ((frame = iWaitingForFcOffIter++) != NULL)
			{
			if (frame->GetDlcNum() == aDlcNum)
				{
				iWaitingForFcOffList.Remove(*frame);
				if (aPlaceOnWriteList)
					{
					_LOG_L4C1("Transferring frame to write list");
					iWriteFrameBufList.AddLast(*frame);
					}
				else
					{
					// Adding frame to the free list
					iMux0710Protocol->AddFrameFreeQ(frame);
					}
				}
			}

		if ((!iWriteFrameBufList.IsEmpty())&&(writeQueueWasEmpty)&&(!IsActive()))
			{
			_LOG_L4C1("Write queue was empty and not active - triggering write");
			frame = GetFrameToWrite();
			TInt ret = WriteFrame(frame);
			if (ret)
				{
				_LOG_L1C2("Write delayed frame failed [ret=%d]",ret);

				// Place the failed frame back on the list.
				iWriteFrameBufList.AddFirst(*frame);
				}
			}
		}

	_LOG_L4C1("<<CCommFrameWriterAo::RemoveAnyDlcFramesFromWaitList");
	}

void CCommFrameWriterAo::CompleteWrite(const TUint8 aDlcNum, TInt aStatus)
/**
 * This method calls the DLC port interface to complete the write request.
 *
 * @param aDlcNum - DLC number of the port
 */
	{
	_LOG_L4C3(">>CCommFrameWriterAo::CompleteWrite [aDlcNum=%d,aStatus=%d]",
		aDlcNum,aStatus);

	if (aDlcNum)
		{
		CPortC32InterfaceBase* port = iParent->FindPortC32Interface(aDlcNum);
		if (port)
			port->CompleteWriteRequest(aStatus);
		else
			{
			_LOG_L1C2("** Port does not exist for aDlcNum=%d **",
				aDlcNum);
			// MAF __ASSERT_DEBUG(EFalse,PANIC(KPanicIllegalState));
			}
		}
	else
		{
		// MAF tell mux control channel of send result
		}

	_LOG_L4C1("<<CCommFrameWriterAo::CompleteWrite");
	}


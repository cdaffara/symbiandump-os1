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
// * Description:  This file contains the implementation for the Mux0710Protocol class.
//

// Mux0710Protocol.cpp

/** @file Mux0710Protocol.cpp
 *
 */
 
#include "Mux0710Protocol.h"
#include "Portfactory.h"
#include "PortC32Interface.h"
#include "CsyMsgBufBPFrame.h"
#include "ChannelMgrCmdData.h"
#include "CommFrameWriterAo.h"
#include "CommFrameReaderAo.h"
#include "ChannelMgrCtrl.h"
#include "CsyDebugLogger.h"

// AT Commands
_LIT8(KATCmdAttention,     "ATE0\r");
_LIT8(KATCmdDisableEcho,   "ATE0Q0V1\r");
_LIT8(KATCmdReset,   "AT%b\r");
_LIT8(KATCmdSleep,   "AT%SLEEP=0\r");

const TInt KRetryCount = 2;

CMux0710Protocol* CMux0710Protocol::NewL(CPortFactory& aPortFactory)
/**
 * This static method uses 2-phase construction to create an instance of
 * this class.
 *
 * @return Pointer to the created object
 */
	{
	_LOG_L4C1("CMux0710Protocol::NewL");

	CMux0710Protocol* p = new(ELeave) CMux0710Protocol(aPortFactory);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop(p);
	return p;
 	}

CMux0710Protocol::~CMux0710Protocol()
/**
 * Destructor.
 */
	{
	_LOG_L4C1("CMux0710Protocol::~CMux0710Protocol");

	// Release  Free Frame Memory 
	CCsyMsgBufBpFrame* aBpFrame;
    iFreeFrameBufIter.SetToFirst();
	while ((aBpFrame = iFreeFrameBufIter++) != NULL)
		{
		iFreeFrameBufList.Remove(*aBpFrame);
		delete  aBpFrame;
	 	}  

	delete iTimeouter;
	}

CMux0710Protocol::CMux0710Protocol(CPortFactory& aPortFactory)
/**
 * Constructor.
 *
 */
 : iPortFactory(aPortFactory),
   iMuxMgrState(ECsyWaitingForFlushResp),
   iFreeFrameBufList(_FOFF(CCsyMsgBufBpFrame, iMsgLink)),
   iFreeFrameBufIter(iFreeFrameBufList),
   iRetryCount(KRetryCount)
   	{}

void CMux0710Protocol::ConstructL()
/**
 * Allocate the memory for the free frames list.
 */
	{
	_LOG_L4C1("CMux0710Protocol::ConstructL");

	iTimeouter = CActiveTimeouter::NewL(*this);

	// allocate memory for the free frame buffer
	for (TUint i=0; i < KMaxFreeFrames; i++)
		{
		CCsyMsgBufBpFrame* aBpFrame = CCsyMsgBufBpFrame::NewL();
		iFreeFrameBufList.AddLast(*aBpFrame);
		}
	iFreeFrameCount = KMaxFreeFrames;
	}

CCsyMsgBufBpFrame* CMux0710Protocol::GetFreeFrameBuf(TBool aCheckFlowControlThresholds)
/**
 * Get a free frame buffer and then remove the buffer from
 * the free list.
 *
 * @return - Pointer to a frame buffer or NULL 
 */
	{
	_LOG_L4C2(">>CMux0710Protocol::GetFreeFrameBuf [aCheckFlowControlThresholds=%d]",
		aCheckFlowControlThresholds);

	CCsyMsgBufBpFrame* frame = NULL;
	if (!iFreeFrameBufList.IsEmpty())
		{
		frame = iFreeFrameBufList.First();
		if (frame)
			{
			iFreeFrameBufList.Remove(*frame);
			iFreeFrameCount--;
			_LOG_L4C2("iFreeFrameCount=%d",iFreeFrameCount);

			if ((aCheckFlowControlThresholds)&&(iFreeFrameCount < KStopDataDlcsThreshold))
				{
				// Need to enforce some flow control
				iEnforcingFlowControl = ETrue;

				if (iFreeFrameCount == 1)
					{
					// Only one frame free - that can be used to send the MSC command
					_LOG_L4C1("Drastic action! - 1 frame free");
					iPortFactory.StopAnyDlc();
					}
				else
					{
					_LOG_L4C2("Less than %d frames free",KStopDataDlcsThreshold);
					iPortFactory.FindActiveDataDlcToStop();
					}
				}

			frame->iMsg.Zero();
			frame->MsgDlc() = KCtrlChannelDlcNum;
			frame->CompleteWhenSent() = ETrue;
			}
		}

	_LOG_L4C2("<<CMux0710Protocol::GetFreeFrameBuf [frame=0x%x]",frame);
	return frame;
	}

void CMux0710Protocol::AddFrameFreeQ(CCsyMsgBufBpFrame* aBpFrame)
/**
 * This method is called to add the specified frame buffer to the
 * free frame queue.
 *
 * @param aBpFrame - Pointer to the frame buffer
 */
	{
	_LOG_L4C2(">>CMux0710Protocol::AddFrameFreeQ [aBpFrame=0x%x]",aBpFrame);

	iFreeFrameBufList.AddLast(*aBpFrame);

	if (iEnforcingFlowControl)
		{
		if (iFreeFrameCount >= KStartDataDlcsThreshold)
			{
			// Release flow control
			_LOG_L4C2("More than %d frames free",
				KStartDataDlcsThreshold);

			// re-enable a data dlc (note only re-enable one dlc at a time)
			iEnforcingFlowControl = iPortFactory.FindDlcToEnable();
			_LOG_L4C2("iEnforcingFlowControl=%d",iEnforcingFlowControl);
			}
		}

	iFreeFrameCount++;
	//MAF __ASSERT_DEBUG((iFreeFrameCount <= KMaxFreeFrames),PANIC(KPanicIllegalState));

	_LOG_L4C2("<<CMux0710Protocol::AddFrameFreeQ iFreeFrameCount=%d",
		iFreeFrameCount);
	}
	
TUint8 CMux0710Protocol::CalcFCS(TUint8* aBuffer, TInt aLen)
/**
 * This method calculates the checkSum for the specified buffer
 * with the specified length.
 *
 * @param aBuffer - Pointer to the data buffer
 * @param aLen    - Length of Number of bytes in the Buffer for which CheckSum to be calculated
 * @return checksum
 */
	{
    TUint8 frameCheckSum = 0xFF;
	aBuffer++;
	for (TInt i=0; i < aLen; i++ )
		{
		frameCheckSum = crctable[(frameCheckSum ^ (*aBuffer++))];
		}

	// One's Complement
	frameCheckSum = (TUint8) ((TUint8)(0xFF) - frameCheckSum);

	return frameCheckSum;
	}

TInt CMux0710Protocol::Create0710DataFrames(TDesC8& aMsgBuf, 
											   const TUint8 aDlcNum)
/**
 * This method is called by channel manager objects to fragment
 * a client message into frames for transmission to the BP.
 * The length of the client message is determined, the client message is
 * fragment into 27.010 format, and the formatted frames are added to the
 * write frame queue.
 *
 * @param aMsgBuf - Pointer to the client message
 * @param aDlcNum - DLC number
 * @return KErrNone, KErrArgument, or KErrNoMemory
 */
	{
	_LOG_L4C2(">>CMux0710Protocol::Create0710DataFrames [aDlcNum=%d]",
		aDlcNum);

	TInt ret = KErrNone;

	TInt msgLength   = aMsgBuf.Length();
	TInt offset      = 0;
	TInt frameLength = 0;
	TInt maxFrameLength = KMaxFrameSize;

	TBool beginFlag = ETrue;
	TBool endFlag = EFalse;

	_LOG_L4C2("msgLength = %d", msgLength);
	if (msgLength > 0)
		{
		while ((msgLength > 0)&&(ret == KErrNone))
			{
			CCsyMsgBufBpFrame* bpFrame = GetFreeFrameBuf();
			if (bpFrame)
				{
				// Set up the frame
				bpFrame->MsgDlc() = aDlcNum;

				if (msgLength > maxFrameLength)
					{
					_LOG_L4C3("Fragmenting (%d > %d)",msgLength,maxFrameLength);

					frameLength = maxFrameLength;
					bpFrame->CompleteWhenSent() = EFalse;
					}
				else
					{
					_LOG_L4C1("No Fragment");

					frameLength = msgLength;
					bpFrame->CompleteWhenSent() = ETrue;
					endFlag = ETrue;
					}
			
				// Test for convergence layer 4
				CPortC32InterfaceBase* port = 
					iPortFactory.FindPortC32Interface(aDlcNum);
				if (port)
					{
					if (port->GetClientType() == CPortFactory::EC32ClientIpNif)
						{
						_LOG_L4C1("Layer 4 frame");

						Create0710UIHLayer4FrameFromMsg(aMsgBuf, offset, 
							bpFrame, frameLength, aDlcNum, beginFlag, endFlag);

						// No longer the beginning
						beginFlag = EFalse;
						}
					else
						{
						_LOG_L4C1("Normal frame");

						Create0710UIHFrameFromMsg(
							aMsgBuf, offset, bpFrame, frameLength, aDlcNum);
						}

					_LOG_L4C1("Writing the frame");
					ret = iCommWriter->Write(bpFrame);
					}
				else
					{
					_LOG_L1C2("** No port defined for aDlcNum=%d",
						aDlcNum);
					}
				}
			else
				{
				// error no free frame available
				_LOG_L1C1("** No free frame buffer **");
				ret = KErrNoMemory;
				}

			msgLength -= frameLength;
			offset += maxFrameLength;
			}
		}
	else
		{
		ret = KErrArgument;
		}

	_LOG_L4C2("<<CMux0710Protocol::Create0710DataFrames [ret=%d]",ret);
	return ret;
	}

void CMux0710Protocol::Create0710UIHFrameFromMsg(TDesC8& aMsgBuf,
												 TInt aOffset,
												 CCsyMsgBufBpFrame* aFrameBuf,
												 TInt aLength,
												 TUint8 aDlcNum)
/**
 * This method creates an 27.010 UIH frame from a message.
 * NOTE: This method assumes that a free frame buffer has been allocated.
 *
 * @param aMsgBuf - Pointer to a CSY memory element pointing to a Msg
 * @param aOffset - Offset from the start of the message to be copied to the frame.
 *                  This is needed if the calling method needs to defragment a long message.
 * @param aFrameBuf - Pointer to a CSY memory elelemt pointing to a Frame
 * @param aLength   - Length of the payload to be copied.
 * @param aDlcNum   - DLC channel number.
 */
    {
	_LOG_L4C3(">>CMux0710Protocol::Create0710UIHFrameFromMsg [aOffset=%d,aLength=%d]",aOffset,aLength);

	aFrameBuf->iMsg.Zero();

	// set initial length for headers
#ifdef _27010ADVANCEOPTION
	aFrameBuf->iMsg.SetLength(3);
#else
	aFrameBuf->iMsg.SetLength(4);
#endif

	// Octet 0 = Start Flag
	aFrameBuf->iMsg[0] = KCsy0710StartEndFlag;

	// Octet 1 = Non-extended Address, Command/Response, DLCI number
	aFrameBuf->iMsg[1] = (TUint8) ((aDlcNum << 2) | 0x03);  // Set the DLCI, EA, C/R

	// Octet 2 = Control Field = Frame Type (UIH)
	aFrameBuf->iMsg[2] = (TUint8) KCsy0710CTLUIH;
	

#ifdef _27010ADVANCEOPTION
	TInt checksumLength = 2;
#else
	// Octet 3 = Length indicator
	// ASSUME only 1 byte needed for length size
	//    27.010 supports 7 bits in Octet 3 to indicate a length up to 128 bytes long
	aFrameBuf->iMsg[3] = (TUint8) ((aLength << 1) | KCsy0710EA);


	// CRC Frame check : Basic Option  -> for Addr, Control, Length Fields only
	// length = 3 bytes  [Addr(1) + Control(1) + Length (1)]
	TInt checksumLength = 3;
#endif
	TUint8 checksum;
	checksum = (TUint8) CalcFCS(&aFrameBuf->iMsg[0], checksumLength);

	// Octet 5-x
	//CCsyMsgBufClient* csyMsgBufClient = STATIC_CAST(CCsyMsgBufClient*, aMsgBuf);
	const TUint8* temp = &aMsgBuf[aOffset];
	aFrameBuf->iMsg.Append(temp, aLength);
	TInt tempLength = aFrameBuf->iMsg.Length();
	aFrameBuf->iMsg.SetLength(tempLength + 2);
	aFrameBuf->iMsg[tempLength] = checksum;
	aFrameBuf->iMsg[tempLength+1] = KCsy0710StartEndFlag;

 	_LOG_L4C1("<<CMux0710Protocol::Create0710UIHFrameFromMsg");
	}

void CMux0710Protocol::Create0710UIHLayer4FrameFromMsg(TDesC8& aMsgBuf, 
													   TInt aOffset, 
													   CCsyMsgBufBpFrame* aFrameBuf, 
													   TInt aLength, 
													   TUint8 aDlcNum, 
													   TBool aLayer4Begin, 
													   TBool aLayer4End)
/**
 * This method creates an 27.010 UIH frame from a message.
 * NOTE: This method assumes that a free frame buffer has been allocated.
 *
 * @param aMsgBuf - Pointer to a CSY memory element pointing to a Msg
 * @param aOffset - Offset from the start of the message to be copied to the frame.
 *                  This is needed if the calling method needs to defragment a long message.
 *                  This method builds the message as a Convergence Layer 4 type, so
 *                  only Advanced Option.
 * @param aFrameBuf - Pointer to a CSY memory elelemt pointing to a Frame
 * @param aLength   - Length of the payload to be copied.
 * @param aDlcNum   - DLC channel number.
 * @param aLayer4Begin - Beginning of Convergence Layer 4.
 * @param aLayer4End   - End of Convergence Layer 4.
 */
    {
	_LOG_L4C3(">>CMux0710Protocol::Create0710UIHLayer4FrameFromMsg [aOffset=%d,aLength=%d]",aOffset,aLength);
	_LOG_L4C3("[aLayer4Begin=%d,aLayer4End=%d]",aLayer4Begin,aLayer4End);

	//aFrameBuf->iMsg.Zero(); - this is done in GetFreeFrameBuf

	// set initial length for headers
	aFrameBuf->iMsg.SetLength(3);

	// Octet 0 = Start Flag
	aFrameBuf->iMsg[0] = KCsy0710StartEndFlag;

	// Octet 1 = Non-extended Address, Command/Response, DLCI number
	aFrameBuf->iMsg[1] = (TUint8) ((aDlcNum << 2) | 0x03);  // Set the DLCI, EA, C/R

	// Octet 2 = Control Field = Frame Type (UIH)
	aFrameBuf->iMsg[2] = (TUint8) KCsy0710CTLUIH;

	TInt checksumLength = 2;
	TUint8 checksum;
	checksum = (TUint8) CalcFCS(&aFrameBuf->iMsg[0], checksumLength);

	// Octet 5-x	
	// Build the Convergence Layer 4 byte
	TUint8 tempLayer4Byte = 0x01;	// Default Middle Frame fragment 
	if (aLayer4Begin && aLayer4End)	// Begin and End - Single Frame Message
		{
		tempLayer4Byte = 0xC1; // MAF magic numbers
		}
	else if (aLayer4Begin)	// Begin Frame
		{
		tempLayer4Byte = 0x41;
		}
	else if (aLayer4End)	// End Frame
		{
		tempLayer4Byte = 0x81;
		}
	aFrameBuf->iMsg.Append(tempLayer4Byte);

	const TUint8* temp = &aMsgBuf[aOffset];
	aFrameBuf->iMsg.Append(temp, aLength);
	TInt tempLength = aFrameBuf->iMsg.Length();

	aFrameBuf->iMsg.SetLength(tempLength + 2);
	aFrameBuf->iMsg[tempLength] = checksum;
	aFrameBuf->iMsg[tempLength+1] = KCsy0710StartEndFlag;

	DumpFrame(aFrameBuf);

	_LOG_L4C1("<<CMux0710Protocol::Create0710UIHLayer4FrameFromMsg");
    }

TInt CMux0710Protocol::Create0710ControlFrame(TCsyFrameType aFrameType,
											  TUint8 aDlcNum,
											  TCsyCtrlCommand aType,
											  TUint8 aV24Signals)
/**
 * This method creates an 27.010 control frame using the specified frame type
 * and DLC number.
 * NOTE: This method assumes that a free frame buffer has been allocated.
 *
 * @param aFrameType - Frame type of the control frame
 * @param aDlcNum - DLC number of this frame
 * @return KErrNone or KErrGeneral
 */
	{
	_LOG_L4C2(">>CMux0710Protocol::Create0710ControlFrame [aDlcNum=%d]",aDlcNum);

	TInt ret = KErrNone;

	if (iMaxRetriesReached == EFalse)
		{
	
		#ifdef _27010ADVANCEOPTION
			TUint8 tempArray[15]; // MAF magic numbers
		#else
			TUint8 tempArray[8];
		#endif

		// Octet 0 = Start Flag
		tempArray[0] = KCsy0710StartEndFlag;

		// Octet 1 = Non-extended Address, initiating end (Command/Response), DLCI number
		tempArray[1] = (TUint8) ((aDlcNum << 2) | 0x03);  // Set the DLCI, EA, C/R

		// Octet 2 = Frame Type
		switch (aFrameType)
			{
		case ESABM:
			_LOG_L3C1("ESABM");
			tempArray[2] = (TUint8) KCsy0710CTLSABM;
			tempArray[2] |= KCsy0710PollFinal;
			break;
		case EUA:
			_LOG_L3C1("EUA");
			tempArray[1] = (TUint8) ((aDlcNum << 2) | 0x01);  // Set the DLCI, EA
			tempArray[2] = (TUint8) KCsy0710CTLUA;
			break;
		case EDM:
			_LOG_L3C1("EDM");
			tempArray[1] = (TUint8) ((aDlcNum << 2) | 0x01);  // Set the DLCI, EA
			tempArray[2] = (TUint8) KCsy0710CTLDM;
			break;
		case EDISC:
			_LOG_L3C1("EDISC");
			tempArray[2] = (TUint8) KCsy0710CTLDISC;
			tempArray[2] |= KCsy0710PollFinal;
			break;
		case EUIH:
			_LOG_L3C1("EUIH");
			tempArray[2] = (TUint8) KCsy0710CTLUIH;
			tempArray[2] |= KCsy0710PollFinal; // no pollbit
			break;
		case EUI:
			_LOG_L3C1("EUI");
			tempArray[2] = (TUint8) KCsy0710CTLUI;
			break;
		default:
			_LOG_L1C2("** Unknown FrameType=%d **",aFrameType);
			ret = KErrGeneral;
			break;
			}

	#ifndef _27010ADVANCEOPTION

		(void) aType;	//to hide warning.
		(void) aV24Signals; //to hide warning.

		// Octet 3 = Length1 indicator octet1
		tempArray[3] = (TUint8) 0x01;  // zero length and set EA bit

		// CRC Frame check : Basic Option  -> for Addr, Control, Length Fields only
		// length = 4 bytes  [Addr(1) + Control(1) + Length (2)]
		TInt length = 3;  
		tempArray[4] = (TUint8) CalcFCS(tempArray, length);
		tempArray[5] = KCsy0710StartEndFlag;

		// For this call of GetFreeFrameBuf do not check the low frame threshold
		// since control frame and it could be related to an already occurring flow
		// control action anyway.
		CCsyMsgBufBpFrame* ctrlFrame = GetFreeFrameBuf(EFalse);
		if (ctrlFrame)
			{
			ctrlFrame->iMsg.Copy(tempArray, 6);
			iCommWriter->Write(ctrlFrame); // MAF should be high priority frame
			}
	#else

		if (aFrameType == EUIH)
			{
			// Set the DLCI to 0
			tempArray[1]  = 0x03; // Set the DLCI to 0, EA, C/R
			}

		TUint8 checkSum; 
		TInt length = 2;
		checkSum = (TUint8) CalcFCS(tempArray, length);

		if (aFrameType == EUIH)
			{
			// Only two types of control UIH supported
			if (aType == EParamNeg)
				{
				_LOG_L3C1("Param Negotiate");

				tempArray[3]  = KCsy0710CTLUIH_DlcParamNegotiate;
				tempArray[4]  = 0x11; // i.e 8 bytes, EA
				tempArray[5]  = aDlcNum;
				tempArray[6]  = (KPNClBits << 4) + KPNFrameType;
				tempArray[7]  = KPNDlcPriority;
				tempArray[8]  = KPNAckTimer;
				tempArray[9]  = KPNMaxFrameSize & 0x00ff;
				tempArray[10] = KPNMaxFrameSize >> 8;
				tempArray[11] = KPNMaxRetransmissions;
				tempArray[12] = KPNWindowSize;
				tempArray[13] = checkSum;
				tempArray[14] = KCsy0710StartEndFlag;

				iParamNegotiateDlcNum = aDlcNum;
				}
			else
				{
				_LOG_L3C2("MSC aV24Signals=0x%x", aV24Signals);

				tempArray[3]  = KCsy0710CTLUIH_ModemStatusCmd;
				tempArray[4]  = 0x05; // i.e 2 bytes, EA
				tempArray[5]  = (TUint8) ((aDlcNum << 2) | 0x03);  // Set the DLCI, EA, 1
				tempArray[6]  = aV24Signals;
				tempArray[7] = checkSum;
				tempArray[8] = KCsy0710StartEndFlag;
				}
			}
		else
			{
			tempArray[3] = checkSum;
			tempArray[4] = KCsy0710StartEndFlag;
			}

		CCsyMsgBufBpFrame* ctrlFrame = GetFreeFrameBuf(EFalse);
		if (ctrlFrame)
			{
	#ifdef _27010ADVANCEOPTION
			if (aFrameType == EUIH)
				{
				if (aType == EParamNeg)
					{
					ctrlFrame->iMsg.Copy(tempArray, 15);
					}
				else
					{
					ctrlFrame->iMsg.Copy(tempArray, 9);
					}
				}
			else
				{
				ctrlFrame->iMsg.Copy(tempArray, 5);
				}
	#else
			ctrlFrame->iMsg.Copy(tempArray, 5);
	#endif

			_LOG_L3C1("Write the frame");
			iCommWriter->Write(ctrlFrame); // MAF should be high priority frame
			}
	#endif
		else
			{		// no memory available
			_LOG_L1C1("** No Memory Available **");
			ret = KErrNoMemory;
			}
		}
	else
		{	
		_LOG_L1C1("** Max Retries Reached **");
		ret = KErrTimedOut;
		}
	_LOG_L4C2("<<CMux0710Protocol::Create0710ControlFrame [ret=%d]",ret);
	return ret;
	}

TInt CMux0710Protocol::SwitchToMuxMode()
/**
 * This method is called to switch the baseband to multiplexer mode.
 * This method formats the AT command the buffer with the AT+CMUX command
 * and writes to the write FrameBuf List.
 *
 * @return KErrNone or KErrNoMemory
 */
	{
	_LOG_L4C1(">>CMux0710Protocol::SwitchToMuxMode");

#ifdef __CSY_PROTOTYPE__
	iMuxMgrState = ECsyMuxEnabled;
#else
	
	if(iMuxMgrState == ECsyWaitingForFlushResp)
		{
		//flush the read queue.
		iTimeouter->Start(KOneSecond);
		return KErrNone;
		}
	else
		{
		iTimeouter->Start(KTwoSeconds);
		}
		
	TInt ret = KErrNone;

	CCsyMsgBufBpFrame* initBuf = GetFreeFrameBuf();
	if (initBuf)
		{
		
#ifdef USE_TI_CONDAT_STACK
		if(iMuxMgrState == ECsyWaitingForResetResp)
			{
			//when using the TI stack we first attempt to reset the board using at%b.  
			_LOG_L3C1("Sending AT reset command");
			TBuf8<16> aBuf(KATCmdReset);
			initBuf->iMsg.Copy(aBuf);
			iMuxMgrState = ECsyWaitingForResetResp;
			}
		else if(iMuxMgrState == ECsyWaitingForSleepResp)
			{
			//Need to ensure that the TI stack will not timeout in the lull between commands
			_LOG_L3C1("Sending AT sleep command");
			TBuf8<16> aBuf(KATCmdSleep);
			initBuf->iMsg.Copy(aBuf);
			iMuxMgrState = ECsyWaitingForSleepResp;
			}
		else
			{
#endif
		_LOG_L3C1("Sending AT");
		TBuf8<16> aBuf(KATCmdAttention);
		initBuf->iMsg.Copy(aBuf);
		iMuxMgrState = ECsyWaitingForAttentionResp;

#ifdef USE_TI_CONDAT_STACK
			}
#endif
		iResponseStr.Zero();
		
		// call comm write to xmit frame
		// comm writer's RunL will run when frame has been xmitted by LDD
		// and return the frame buffer to the free list
		ret = iCommWriter->Write(initBuf);
		}
	else
		{
		_LOG_L1C1("** No Memory Available **");
		ret = KErrNoMemory;
		}
#endif

	_LOG_L4C2("<<CMux0710Protocol::SwitchToMuxMode [ret=%d]",ret);
	return ret;
	}

TInt CMux0710Protocol::ParseATResponse(TDes8& aResp)
/**
 * This method parses the specified AT ascii response for the
 * expected ascii "OK" string.  If the string is found then set
 * the Mux Mode to True.
 *
 * @param aResp - Reference to the AT response string to parse
 * @return KErrNone, KErrGeneral or KErrNotFound
 */
	{
	_LOG_L4C3(">>CMux0710Protocol::ParseATResponse [iMuxMgrState = %d, aResp=%S]",
		iMuxMgrState,&aResp);
		
	//Response string not always read in one go so concatonate it together to form full response string.
	TInt appendsize = iResponseStr.MaxSize() - iResponseStr.Length();
	iResponseStr.Append(aResp.LeftTPtr(appendsize));
	_LOG_L4C2("Result string concatenated = %S",&iResponseStr);
	
	if(iMuxMgrState == ECsyWaitingForFlushResp)
		{
		_LOG_L4C2("Flushed comm port on opening = %S",&iResponseStr);
		return KErrNotFound;
		}
		
#ifdef USE_TI_CONDAT_STACK
	if(iMuxMgrState == ECsyWaitingForResetResp)
		{
		if(iResponseStr.Find(KATInitialised) != KErrNotFound)
			{
			//got match - reset Rsp string to enter normal loop handle
			_LOG_L3C1("Board has successfully been reset.");
			iResponseStr = _L8("OK");
			iRetryCount = 0; // stop retries of the reset
			}
		}

	if(iMuxMgrState == ECsyWaitingForSleepResp)
		{
		if(iResponseStr.Find(_L8("OK")) != KErrNotFound)
			{
			//got match - sleep acknowledged
			_LOG_L3C1("Board has had sleep timeout disabled.");
			iRetryCount = 0; //stop retries of the sleep;
			}
		}				
#endif

	TInt ret = KErrNone;

	// all AT responses should contain an "OK" keyword
	if ((iResponseStr.Find(_L8("OK")) != KErrNotFound) ||
		(iResponseStr.Find(_L8("ERROR")) != KErrNotFound))
		{
		if (iResponseStr.Find(_L8("ERROR")) != KErrNotFound)
			{
#ifdef USE_TI_CONDAT_STACK
			if(iMuxMgrState == ECsyWaitingForResetResp)
				{
				_LOG_L3C1("*******TI Board failed to reset (AT%b) - continue anyway as may already be in a stable state.*******");
				iRetryCount--;
				}
			else if (iMuxMgrState == ECsyWaitingForSleepResp)
				{
				_LOG_L3C1("*******TI Board failed to acknowledge sleep disable (AT%SLEEP=0) - continue anyway as may already be in a stable state.*******");
				iRetryCount--;
				}
			else
				{
				_LOG_L3C1("****************** Received ERROR back ****************");
				}
#else
			_LOG_L3C1("****************** Received ERROR back ****************");
#endif
			}
		iTimeouter->Stop();

		switch (iMuxMgrState)
			{
#ifdef USE_TI_CONDAT_STACK
		case ECsyWaitingForResetResp:
			{			
			_LOG_L3C1("ECsyWaitingForResetResp - Start proper init sequence.");
			if(iRetryCount>0) 
				iMuxMgrState = ECsyWaitingForResetResp; //try reset command again
			else
				{
				iMuxMgrState = ECsyWaitingForSleepResp;
				iRetryCount = KRetryCount;
				}				
			SwitchToMuxMode();
			}
			break;
			
		case ECsyWaitingForSleepResp:
			{			
			iResponseStr.Zero();
			_LOG_L3C1("ECsyWaitingForSleepResp");
			if(iRetryCount>0)
				iMuxMgrState = ECsyWaitingForSleepResp;
			else
				iMuxMgrState = ECsyWaitingForAttentionResp;
			SwitchToMuxMode();
			}
			break;			
#endif			
		case ECsyWaitingForAttentionResp:
			{
			iResponseStr.Zero();
			CCsyMsgBufBpFrame* atCmdBuf = GetFreeFrameBuf();
			if (atCmdBuf)
				{
				_LOG_L3C1("Writing ATE0 command");
				atCmdBuf->iMsg.Copy(KATCmdDisableEcho);
				iCommWriter->Write(atCmdBuf);
				iMuxMgrState = ECsyWaitingForEchoDisableResp;
				}

			iTimeouter->Start(KTwoSeconds);
			}
			break;

		case ECsyWaitingForEchoDisableResp:
			{
			iResponseStr.Zero();
			CCsyMsgBufBpFrame* atCmdBuf = GetFreeFrameBuf();
			if (atCmdBuf)
				{
				_LOG_L3C1("Writing CMUX cmd");
				atCmdBuf->iMsg.Copy(KCsyDefaultATEnterMuxModeCmd);
				iCommWriter->Write(atCmdBuf);
				iMuxMgrState = ECsyWaitingForCmuxResp;
				}

			iTimeouter->Start(KTwoSeconds);
			}
			break;

		case ECsyWaitingForCmuxResp:
			{
			iResponseStr.Zero();
			_LOG_L3C1("Got CMUX response");
			iMuxMgrState = ECsyMuxEnabled;

			// start the connect process for the control, command & data channels
			iPortFactory.ConnectControlChannel();
			}
			break;

		default:
			_LOG_L3C2("** Invalid Mux Mgr State = %d **", iMuxMgrState);
			ret = KErrGeneral;
			break;
			}
		}
	else
		{
		ret = KErrNotFound;
		}

	_LOG_L4C2("<<CMux0710Protocol::ParseATResponse [ret = %d]",ret);
	return ret;
	}

void CMux0710Protocol::TimedOut()
/**
 * This resends the AT command required.
 */
	{
	_LOG_L4C2(">>CMux0710Protocol::TimedOut [iMuxMgrState=%d]",iMuxMgrState);

	switch (iMuxMgrState)
		{

	case ECsyWaitingForFlushResp:
		{
		//read queue has just been flushed, now start switch to MUX mode
		_LOG_L3C1("ECsyWaitingForFlushResp");
#ifdef USE_TI_CONDAT_STACK
		iMuxMgrState = ECsyWaitingForResetResp;
#else
		iMuxMgrState = ECsyWaitingForAttentionResp;
#endif
		SwitchToMuxMode();
		}
		break;
#ifdef USE_TI_CONDAT_STACK
	case ECsyWaitingForResetResp:
		{
		_LOG_L3C1("ECsyWaitingForResetResp");
		if(--iRetryCount>0) 
			iMuxMgrState = ECsyWaitingForResetResp; //try reset command again
		else
			iMuxMgrState = ECsyWaitingForSleepResp;	
		SwitchToMuxMode();
		}
		break;

	case ECsyWaitingForSleepResp:
		{
		_LOG_L3C1("ECsyWaitingForSleepResp");
		if(--iRetryCount>0)
			iMuxMgrState = ECsyWaitingForSleepResp; //try sleep command again
		else
			iMuxMgrState = ECsyWaitingForAttentionResp;
		SwitchToMuxMode();	
		}
		break;
#endif
	case ECsyWaitingForAttentionResp:
		{
		if (++iAtResponseTimeout < KAtResponseTimeoutLimit)
			{
			_LOG_L3C2("ECsyWaitingForAttentionResp [timeout:%d]",iAtResponseTimeout);
			SwitchToMuxMode();	
			}
		else
			{
			_LOG_L3C2("Max retries [%d] reached for At Response", KAtResponseTimeoutLimit);

			iMaxRetriesReached = ETrue;
			//call to scheduler DoCancel required here
			iPortFactory.ChannelCtrlDoCancel();
			}
		}
		break;

	case ECsyWaitingForEchoDisableResp:
		{
		_LOG_L3C1("ECsyWaitingForEchoDisableResp");
		iResponseStr.Zero();
		CCsyMsgBufBpFrame* atCmdBuf = NULL;
		atCmdBuf = GetFreeFrameBuf();
		if (atCmdBuf)
			{
			TBuf8<16> tempBuf(KATCmdDisableEcho);
			atCmdBuf->iMsg.Copy(tempBuf);
			iCommWriter->Write(atCmdBuf);
			}

		iTimeouter->Start(KOneSecond);
		}
		break;

	case ECsyWaitingForCmuxResp:
		{
		_LOG_L3C1("ECsyWaitingForCmuxResp");
		iResponseStr.Zero();
		CCsyMsgBufBpFrame* atCmdBuf = GetFreeFrameBuf();
		if (atCmdBuf)
			{
			_LOG_L3C1("Writing CMUX cmd");
			atCmdBuf->iMsg.Copy(KCsyDefaultATEnterMuxModeCmd);
			iCommWriter->Write(atCmdBuf);
			iMuxMgrState = ECsyWaitingForCmuxResp;
			}
		iTimeouter->Start(KTwoSeconds);
		}
		break;

	default:
		_LOG_L1C2("** Invalid Mux Mgr State = %d **", iMuxMgrState);
		break;
		}

	_LOG_L4C1("<<CMux0710Protocol::TimedOut");
	}

void CMux0710Protocol::DumpFrame(CCsyMsgBufBpFrame* aFrameBuf)
/**
 * Logs the packet into the log file.
 *
 * @param aPacket The incoming packet
 */
	{
	_LOG_L4C1(">>CMux0710Protocol::DumpFrame");

	TInt packetLen = aFrameBuf->iMsg.Length();
	_LOG_L4C2("frame length %d",packetLen);

	TBuf8<256> logBuf;
	TBuf8<256> logBuf2;

	logBuf.Copy(_L8("    "));
	logBuf2.Copy(_L8(" "));
	_LOG_L4C1("");

	for (TInt i = 0; i <= packetLen; i++)
		{
		if (i >= packetLen)
			{
			logBuf.Append(logBuf2);
			_LOG_L4C2("%S",&logBuf);
			break;
			}
		if (((i % 16) == 0) && (i > 0))
			{
			logBuf.Append(logBuf2);
			_LOG_L4C2("%S",&logBuf);
			logBuf.Copy(_L8("    "));
			logBuf2.Copy(_L8(" "));
			}
		logBuf.AppendFormat(_L8("%02X "), aFrameBuf->iMsg[i]);
		if (TChar(aFrameBuf->iMsg[i]).IsPrint())
			logBuf2.AppendFormat(_L8("%c"), aFrameBuf->iMsg[i]);
		else
			logBuf2.Append(_L8("."));
		}

	_LOG_L4C1("<<CMux0710Protocol::DumpFrame");
	_LOG_L4C1("");
	}
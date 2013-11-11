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
// * Description:  This file contains the implementation for class CCommFrameReaderAo.
// *               Class CCommFrameReaderAo is used to request a read from the serial port LDD.
// *               Class CommFrameReaderAo is an active object and is run by the active
// *               scheduler when the LDD has completed the read request.
//

// CommFrameReaderAo.cpp

/** @file CommFrameReaderAo.cpp
 *
 * The file contains the implementation for class CCommFrameReaderAo.
 * Class CCommFrameReaderAo is used to request a read from the serial port LDD.
 * Class CommFrameReaderAo is an active object and is run by the active
 * scheduler when the LDD has completed the read request
 */

// Need to add code to package together a frame and then send the
// complete frame to the Mux object for defragmentation into a client msg.

#include "CommFrameReaderAo.h"
#include "Portfactory.h"
#include "CsyMsgBufBPFrame.h"
#include "Mux0710Protocol.h"
#include "CsyDebugLogger.h"
#include "ChannelMgrCtrl.h"
#include "ChannelMgrCmdData.h"

CCommFrameReaderAo* CCommFrameReaderAo::NewL(CPortFactory* aParent, CMux0710Protocol* aMux0710Protocol)
/**
 * This methods uses two phase construction and the cleanup stack to create
 * an instance of class CCommFrameReaderAo.
 * @param aParent - Pointer to parent
 * @param aMux0710Protocol - Pointer to Mux 27.010 object
 * @return Pointer to the created instance
 */
	{
	_LOG_L4C1("CCommFrameReaderAo::NewL");

	CCommFrameReaderAo* c = new (ELeave) CCommFrameReaderAo(aParent, aMux0710Protocol);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}

CCommFrameReaderAo::~CCommFrameReaderAo()
/**
 * Destructor. Cancel any outstanding request.
 */
	{
	_LOG_L4C1("CCommFrameReaderAo::~CCommFrameReaderAo");
	Cancel();
	}

CCommFrameReaderAo::CCommFrameReaderAo(CPortFactory* aParent, CMux0710Protocol* aMux0710Protocol)
/**
 * Constructor.  Pass priority of active object to base class.
 * @param aParent - Pointer to parent
 * @param aMux0710Protocol - Pointer to Mux 27.010 object
 */
: CCommReadWriteBaseAo(aParent, aMux0710Protocol, KFrameReaderAoPriority),
  iFrameReaderState(ECsyWaitingForStartFlag)
	{
	}

void CCommFrameReaderAo::ConstructL()
/**
 * Retrieve a pointer to the LDD comm port.
 */
    {
	_LOG_L4C1("CCommFrameReaderAo::ConstructL");
	SetBuffersL();
	}

void CCommFrameReaderAo::DoCancel()
/**
 * Cancel an outstanding request.
 * @param void
 */
	{
	_LOG_L4C1("CCommFrameReaderAo::DoCancel");

	iCommPort->ReadCancel();
	}

void CCommFrameReaderAo::BasicOption()
/**
 * Basic option frames
 * Note this has not been tested since DSample/P2 do not use basic option
 */
	{
	TUint octet;
	TBool frameData = EFalse;

	TInt len = iBuf->Length();
	TPtr8 ptr = iBuffer->Des();
	_LOG_L3C2("Received %d bytes",len);

	for (TInt loop = 0; loop < len; loop++)
		{
		octet = ptr[loop];

		if (octet == KCsy0710StartEndFlag)
			{
			// Control bit
			switch(iFrameReaderState)
				{
			case ECsyWaitingForStartFlag:
				{
				_LOG_L3C2("[0x%02x] Got start flag",octet);
				iFrameReaderState = ECsyWaitingForFrameStart;

				if (iFrameBuf == NULL)
					{
					// There is data to process - get a new frame
					iFrameBuf = iMux0710Protocol->GetFreeFrameBuf();
					if (iFrameBuf == NULL)
						{
						_LOG_L1C1("** No free frame buffer ** - ignoring frame");
						break;
						}
					}
				}
				break;
			case ECsyWaitingForFrameStart:
				{
				// disregard additional flag
				}
				break;
			case ECsyWaitingForFrameData:
				{
				iFrameReaderState = ECsyWaitingForStartFlag;

				// must be the end flag
				_LOG_L3C2("[0x%02x] End of frame",octet);
				if (iFrameBuf)
					{
					if(iFrameBuf->iMsg.Length())
						{
						// Frame holds something
						AddToReceivedFramesList(iFrameBuf);
						iFrameBuf = NULL;
						}
					else
						{
						_LOG_L1C1("** zero length frame **");
						// MAF __ASSERT_DEBUG(EFalse, PANIC(KPanicIllegalState));
						}
					}
				else
					{
					_LOG_L3C1("** no frame buffer! **");
					}
				}
				break;
			default:
				// MAF __ASSERT_DEBUG(EFalse, PANIC(KPanicIllegalState));
				_LOG_L1C2("** unexpected flag ** [iFrameReaderState=%d]",iFrameReaderState);
				break;
				}
			}
		else
			{
			if (iFrameReaderState == ECsyWaitingForFrameStart)
				{
				iFrameReaderState = ECsyWaitingForFrameData;
				iFrameBuf->iMsg.Append(octet);
				
				#ifdef _DEBUG
				// This variable should only be set if it debug
				// Otherwise we get warnings when compiling with ARM v5
				TUint dlcNum = octet >> 2;
				#endif

				_LOG_L3C3E("[0x%02x] dlcNum=%d", octet, dlcNum);
				}
			else
				{
				_LOG_L3C2("[0x%02x]",octet);
				frameData = ETrue;
				}
			}

		if (frameData)
			{
			// MAF __ASSERT_DEBUG((iFrameReaderState == ECsyWaitingForFrameData),PANIC(KPanicIllegalState));

			TUint frameLen = (TUint) (iFrameBuf->iMsg.Length() + 1);
			if (frameLen > KMaxAdvFrameSize)
				{
				_LOG_L1C3("** length %d will exceed max length %d **",
					frameLen,KMaxFrameSize);
				iFrameBuf->iMsg.Zero();		
				iFrameReaderState = ECsyWaitingForStartFlag;
				break;
				}

			frameData = EFalse;
			iFrameBuf->iMsg.Append(octet);
			}
		}
	}

void CCommFrameReaderAo::AdvancedOption()
/**
 * Advanced option frames
 */
	{
	TUint octet = 0;
	TBool frameData = EFalse;

	TInt len = iBuf->Length();
	TPtr8 ptr = iBuffer->Des();
	_LOG_L3C2("Received %d bytes",len);

	TInt startVal = 0;

	// Check processing (helps when we have lost sync with data due to read error)
	if ((iFrameReaderState == ECsyWaitingForStartFlag)&&(len))
		{
		// We are on the hunt for the start flag
		if (iESCRecved)
			{
			_LOG_L2C1("Escape flag set - ignore first octet");
			iESCRecved = EFalse;
			}
		else
			octet = ptr[0];

		if (octet != KCsy0710StartEndFlag)
			{
			_LOG_L3C2("[0x%02x]",octet);
			_LOG_L2C1("* Looking for the start flag *");
			do
				{
				startVal++;
				if (startVal == len)
					break;

				octet = ptr[startVal];

				if (iESCRecved)
					{
					iESCRecved = EFalse;
					continue;
					}

				if (octet == KCsy0710EscapeByte)
					iESCRecved = ETrue;
				}
			while (octet != KCsy0710StartEndFlag);

			_LOG_L2C2("Disregarded %d bytes",startVal);
			}
		}

	// Do processing of the received buffer
	for (TInt loop = startVal; loop < len; loop++)
		{
		octet = ptr[loop];

		if (iESCRecved)
			{
			_LOG_L3C2("[0x%02x] - Recv ESC before, change this char",octet);
			octet = (TUint8) (octet^(1<<5));
			_LOG_L3C2("Changed to [0x%02x]",octet);
			iESCRecved = EFalse;
			frameData = ETrue;
			}
		else
			{
			if (octet == KCsy0710EscapeByte)
				{
				_LOG_L3C1("KCsy0710EscapeByte");
				iESCRecved = ETrue;
				}
			else if (octet == KCsy0710StartEndFlag)
				{
				// Control bit
				switch(iFrameReaderState)
					{
				case ECsyWaitingForFrameData:
					{
					iFrameReaderState = ECsyWaitingForStartFlag;
					
					if (iFrameBuf)
						{
						TInt frameLen = iFrameBuf->iMsg.Length();
						if(frameLen > KAdvOptionHeaderSize)
							{
							TUint8 checkSum = iFrameBuf->iMsg[frameLen-1];
							if (CheckFCS(iFrameBuf->iMsg,KAdvOptionHeaderSize,checkSum))
								{
								// must be the end flag
								_LOG_L3C2("[0x%02x] End of frame",octet);
								// Frame holds something
								AddToReceivedFramesList(iFrameBuf);
								iFrameBuf = NULL;
								}
							else
								{
								_LOG_L1C1("** checksum is incorrect - rejecting frame **");
								iFrameBuf->iMsg.Zero();		
								}
							}
						else
							{
							_LOG_L1C2("** Rejected incorrect length frame %d **",frameLen);
							iFrameBuf->iMsg.Zero();		
							}
						}
					else
						{
						_LOG_L3C1("** no frame buffer! **");
						}

					TInt check = loop+1;
					if (check == len)
						break;

					// There are more bytes in this buffer to process
					octet = ptr[check];
					if (octet != KCsy0710StartEndFlag)
						{
						// Frames are back to back
						_LOG_L2C1("* No start flag - assuming frames back to back *");
						octet = KCsy0710StartEndFlag;
						}
					}
					// deliberate fall through
				case ECsyWaitingForStartFlag:
					{
					_LOG_L3C2("[0x%02x] Got start flag",octet);
					iFrameReaderState = ECsyWaitingForFrameStart;

					if (iFrameBuf == NULL)
						{
						// There is data to process - get a new frame
						iFrameBuf = iMux0710Protocol->GetFreeFrameBuf();
						if (iFrameBuf == NULL)
							{
							_LOG_L1C1("** No free frame buffer ** - ignoring frame");
							// Note: GetFreeFrameBuf handles the flow control
							break;
							}
						}
					}
					break;
				case ECsyWaitingForFrameStart:
					{
					// disregard additional flag
					}
					break;
				default:
					_LOG_L1C2("** unexpected flag ** [iFrameReaderState=%d]",iFrameReaderState);
					// MAF __ASSERT_DEBUG(EFalse, PANIC(KPanicIllegalState));
					break;
					}
				}
			else
				{
				if (iFrameReaderState == ECsyWaitingForFrameStart)
					iFrameReaderState = ECsyWaitingForFrameData;

				_LOG_L3C2("[0x%02x]",octet);
				frameData = ETrue;
				}
			}

		if (frameData)
			{
			frameData = EFalse;

			// MAF __ASSERT_DEBUG((iFrameReaderState == ECsyWaitingForFrameData),PANIC(KPanicIllegalState));

			if (iFrameBuf)
				{
				TUint frameLen = (TUint) (iFrameBuf->iMsg.Length() + 1);
				if (frameLen > KMaxAdvFrameSize)
					{
					_LOG_L1C2("** length will exceed max length %d **",
						KMaxFrameSize);
					iFrameBuf->iMsg.Zero();		
					iFrameReaderState = ECsyWaitingForStartFlag;
					break;
					}

				iFrameBuf->iMsg.Append(octet);
				}
			else
				{
				// it should not be possible to enter this condition

				_LOG_L1C2("** No frame buffer allocated for frame ** [iFrameReaderState=%d]",iFrameReaderState);
				// MAF __ASSERT_DEBUG(EFalse,PANIC(KPanicIllegalState));
				}
			}
		}
	}

void CCommFrameReaderAo::NonMuxed()
	{
	if (iMux0710Protocol->ParseATResponse(*iBuf) == KErrNone)
		{
		iFrameReaderState = ECsyWaitingForStartFlag;
		_LOG_L3C1("ParseATResponse returned KErrNone");
		}
	}

void CCommFrameReaderAo::RunL()
/**
 * This method is invoked by the active scheduler when the read request
 * to the LDD has completed.
 */
    {
	if (iStatus.Int())
		{
		_LOG_L3C1(" "); // please leave separator in
		_LOG_L1C2("** CCommFrameReaderAo::RunL [iStatus %d] **",iStatus.Int());

		// assumption here, whatever the error we will be able to repost
		// another request to readoneormore from the serial device driver.

		if (iFrameBuf)
			{
			iMux0710Protocol->AddFrameFreeQ(iFrameBuf);
			iFrameBuf = NULL;
			}

		//reset state
		iESCRecved = EFalse;
		iFrameReaderState = ECsyWaitingForStartFlag;

		iStatus = KRequestPending;
 		SetActive();
		iCommPort->ReadOneOrMore(iStatus, *iBuf);
		return;
		}

	if (iFrameBuf == NULL)
		{
		// Only show this log if start of a new frame
		_LOG_L3C1(" "); // please leave separator in
		_LOG_L3C1("CCommFrameReaderAo::RunL - start of read");
		}

	if (!iMux0710Protocol->IsMuxModeEnabled())
		{
		NonMuxed();
		}
	else
		{
		// mux mode is enabled
		// process recv data according to our current state

#ifndef _27010ADVANCEOPTION

		BasicOption();

#else
		AdvancedOption();

#endif
		}

	if (!IsActive())
		{
		iStatus = KRequestPending;
 		SetActive();
		iCommPort->ReadOneOrMore(iStatus, *iBuf);
		}
	}

void CCommFrameReaderAo::Read()
/**
 * This method is called to start the process to read an ascii string
 * (e.g. response to initial AT+CMUX=0,0,5 command) or a frame from 
 * the serial port LDD. 
 * Note that this method will only be called once and then the RunL
 * will handle the reading of the comm port.
 */
	{
	_LOG_L4C1(">>CCommFrameReaderAo::Read");

	if (!IsActive())
		{
		_LOG_L3C1("Read not active");

		// Set the object active
		iStatus = KRequestPending;
 		SetActive();
		iCommPort->ReadOneOrMore(iStatus, *iBuf);
		}
	else
		{
		_LOG_L2C1("** Already active **");
		}

	_LOG_L4C1("<<CCommFrameReaderAo::Read");
    }

void CCommFrameReaderAo::ReadCancel()
/**
 * Cancel a read request.
 */
	{
	_LOG_L4C1(">>CCommFrameReaderAo::ReadCancel");

	iCommPort->ReadCancel();
	if (iFrameBuf)
		{
		// return buffer to free frame queue
		iMux0710Protocol->AddFrameFreeQ(iFrameBuf);
		iFrameBuf = NULL;
		}

	_LOG_L4C1("<<CCommFrameReaderAo::ReadCancel");
	}

TBool CCommFrameReaderAo::CheckFCS(TDes8& aBuffer, TInt aLen, TUint8 aSuppliedChecksum)
/**
 * This method checks the checksum for the specified buffer with the 
 * specified length.
 *
 * @param aBuffer - Data 
 * @param aLen    - Number of bytes in the Buffer for which CheckSum to be calculated
 * @return ETrue if checksum is okay
 */
	{
	TUint8 frameCheckSum = 0xFF;

	for (TInt i=0; i < aLen; i++)
		{
		frameCheckSum = crctable[(frameCheckSum ^ aBuffer[i])];
		}

	// One's Complement
	frameCheckSum = crctable[(frameCheckSum ^ aSuppliedChecksum)];

	if (frameCheckSum == 0xCF)
		return ETrue;

	return EFalse;
	}

void CCommFrameReaderAo::AddToReceivedFramesList(CCsyMsgBufBpFrame* aBpFrame)
/**
 * This method is called by the comm frame reader to add the specified
 * received frame to a c32 port interface queue. The specific C32 port 
 * interface object is determined from the DLC number embedded in the frame.
 *
 * @param aBpFrame - Pointer to the received frame
 */
	{
	_LOG_L4C1(">>CCommFrameReaderAo::AddToReceivedFramesList");

	TUint8 dlcNum = aBpFrame->GetDlcNum();

	_LOG_L4C2("dlcNum=%d", dlcNum);
	_LOG_L4C2("frameType=0x%x", aBpFrame->GetFrameType());

	// check for a message for the control channel
	if (dlcNum == 0)
		{
		iParent->GetControlChannel()->ProcessRecvFrame(aBpFrame);
		}
	else
		{
		CChannelMgrCmdData* channel = iParent->FindChannelMgrByDlcNum(dlcNum);
		if (channel != NULL)
			channel->ProcessRecvFrame(aBpFrame);
		else
			{
			_LOG_L1C2("** No port defined for dlcNum=%d **", dlcNum);
			iMux0710Protocol->AddFrameFreeQ(aBpFrame);
			}
		}

	_LOG_L4C1("<<CCommFrameReaderAo::AddToReceivedFramesList");
	}


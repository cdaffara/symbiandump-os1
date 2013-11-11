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
// * Description:  This file contains the implementation for the CCsyMsgBufBpFrame class.
// *               This class is used to hold messages that the CSY sends to and receives
// *               from the BP multiplexer.
//

/** @file
 * This file contains the implementation for the CCsyMsgBufBpFrame class.
 * This class is used to hold messages that the CSY sends to and receives
 * from the baseband multiplexer.
 */

#include "CsyMsgBufBPFrame.h"
#include "CsyGlobals.h"

CCsyMsgBufBpFrame* CCsyMsgBufBpFrame::NewL()
/**
 * This static method uses 2-phase construction to create an instance of
 * class CCsyMsgBufBpFrame.
 *
 * @return Pointer to the created object
 */
	{
	CCsyMsgBufBpFrame* p = new(ELeave) CCsyMsgBufBpFrame();
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop(p);
	return p;
 	}

CCsyMsgBufBpFrame::~CCsyMsgBufBpFrame()
/**
 * Destructor.
 */
	{}

CCsyMsgBufBpFrame::CCsyMsgBufBpFrame()
/**
 * Constructor.
 */
	{}

void CCsyMsgBufBpFrame::ConstructL()
/**
 * Set the buffer type to Frame and then zero the buffer's contents.
 */
	{
	iMsg.FillZ();
	}

TUint8 CCsyMsgBufBpFrame::GetDlcNum() const
/**
 * The is method extracts the DLC number from the frame.
 *
 * @return DLC number
 */
	{
	return (TUint8) (iMsg[KAdvOptionAddress] >> 2); // remove EA and CR bits
	}

TUint8 CCsyMsgBufBpFrame::GetFrameType() const
/**
 * The is method extracts the frame type from the frame.
 *
 * @return frame type
 */
	{
	return ((TUint8) (iMsg[KAdvOptionControl] & 0xEF));  // remove poll/final bit
	}

TUint8 CCsyMsgBufBpFrame::GetType4FrameSequence() const
/**
 * The is method extracts the Convergence Layer Type 4 sequence from the frame.
 *
 * @return Frame Sequence
 */
	{
	return (TUint8) (iMsg[KAdvOptionType4FrameControl] >> 6);
	}


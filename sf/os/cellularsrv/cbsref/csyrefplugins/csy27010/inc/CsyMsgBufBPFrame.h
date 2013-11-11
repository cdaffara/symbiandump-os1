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
// * Description: This file defines the CCsyMsgBufBpFrame class.
// *              This class is used to hold frame messages that the CSY sends to and 
// *              receives from the BP multiplexer.
//

// CsyMsgBufBpFrame.h

/** @file CsyMsgBufBpFrame.h
 *
 */

#ifndef _CSYMSGBUFBPFRAME_H_
#define _CSYMSGBUFBPFRAME_H_

#include <e32base.h>
#include "CsyGlobals.h"

/** @class CCsyMsgBufBpFrame CsyMsgBufBpFrame.h "CsyMsgBufBpFrame.h"
 *  @brief Class CCsyMsgBufBpFrame is used to hold frame messages 
 *  that the CSY sends to and receives from the BP multiplexer.
 */
class CCsyMsgBufBpFrame : public CBase
	{
public:
	static CCsyMsgBufBpFrame* NewL();
	virtual ~CCsyMsgBufBpFrame();

	TUint8 GetDlcNum() const;
	TUint8 GetFrameType() const;
	TUint8 GetType4FrameSequence() const;

	TBuf8<KMaxAdvFrameSize> iMsg;

	inline TUint8& CCsyMsgBufBpFrame::MsgDlc();
	inline TBool& CCsyMsgBufBpFrame::CompleteWhenSent();

public:
	TSglQueLink iMsgLink;

private:
	CCsyMsgBufBpFrame();
	void ConstructL();

private:
	TUint8	iCsyMsgDlc;
	TBool	iCompleteWhenSent;

	};

inline TUint8& CCsyMsgBufBpFrame::MsgDlc()
	{
	return iCsyMsgDlc;
	}

inline TBool& CCsyMsgBufBpFrame::CompleteWhenSent()
	{
	return iCompleteWhenSent;
	}

#endif  //_CSYMSGBUFBPFRAME_H_
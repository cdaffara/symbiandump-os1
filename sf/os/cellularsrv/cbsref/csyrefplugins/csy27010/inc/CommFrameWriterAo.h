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
// * Description: This file contains the declaration for class CCommFrameWriterAo.
// *              Class CCommFrameWriterAo is used to perform write operations to
// *              the serial port logical device driver. Class CommFrameWriteterAo
// *              is an active object and is run by the active scheduler when the
// *              LDD has completed the CSY's write request.
// *
//

// CommFrameWriterAo.h

/** @file CommFrameWriterAo.h
 *
 * 
 */

#ifndef _COMMFRAMEWRITERAO_H_
#define _COMMFRAMEWRITERAO_H_

#include "CommReadWriteBaseAo.h"

class CPortFactory;
class CCsyMsgBufBpFrame;

/** @class CCommFrameWriterAo CommFrameWriterAo.h "CommFrameWriterAo.h"
 *  @brief Class CCommFrameWriterAo is used to perform write operations
 *  to the serial port logical device driver. Class CommFrameWriteterAo
 *  is an active object and is run by the active scheduler when the
 *  LDD has completed the CSY's write request. There is one instance 
 *  of CCommFrameWriterAo.
 */
class CCommFrameWriterAo : public CCommReadWriteBaseAo
    {
public:
    static CCommFrameWriterAo* NewL(CPortFactory* aParent, CMux0710Protocol* aMux0710Protocol);
    ~CCommFrameWriterAo();

    TInt Write(CCsyMsgBufBpFrame* aBpFrame,
		TBool aHighPriority = EFalse);
    void WriteCancel();

	void RemoveAnyDlcFramesOnWriteList(TUint8 aDlcNum, TBool aPlaceOnWaitList = ETrue);
	void RemoveAnyDlcFramesFromWaitList(TUint8 aDlcNum, TBool aPlaceOnWriteList = ETrue);

public:// from CActive
    virtual void DoCancel();
    virtual void RunL();

private:
    CCommFrameWriterAo(CPortFactory* aParent, CMux0710Protocol* aMux0710Protocol);
    void ConstructL();

	TInt WriteFrame(CCsyMsgBufBpFrame* aBpFrame);
	void CompleteWrite(const TUint8 aDlcNum, TInt aStatus);

	CCsyMsgBufBpFrame* GetFrameToWrite();
	void AddToWaitingToSendList(CCsyMsgBufBpFrame* aBpFrame, TBool aHighPriority = EFalse);

private:
	TUint8	iDlcNum;
	TBool	iCompleteWhenSent;

	// Write Frame Buffer Lists
	TSglQue<CCsyMsgBufBpFrame>		iWriteFrameBufList;
	TSglQueIter<CCsyMsgBufBpFrame>	iWriteFrameBufIter;

	TSglQue<CCsyMsgBufBpFrame>		iWaitingForFcOffList;
	TSglQueIter<CCsyMsgBufBpFrame>	iWaitingForFcOffIter;

	};

#endif  //_COMMFRAMEWRITERAO_H_
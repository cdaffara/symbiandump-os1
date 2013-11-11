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
// * Description: This file contains the declaration for class CCommFrameReaderAo.
// *              Class CCommFrameReaderAo is used to request a read from the serial port LDD.
// *              Class CommFrameReaderAo is an active object and is run by the active
// *              scheduler when the LDD has completed the CSY's read request.
// *
//

// CommFrameReader.h

/** @file CommFrameReaderAo.h
 *
 * 
 */

#ifndef _CCOMMFRAMEREADERAO_H_
#define _CCOMMFRAMEREADERAO_H_

#include "CommReadWriteBaseAo.h"
#include "CsyGlobals.h"

class CCsyMsgBufBpFrame;

/** @class CCommFrameReaderAo CommFrameReaderAo.h "CommFrameReaderAo.h"
 *  @brief Class CCommFrameReaderAo is used to request a read from 
 *  the serial port LDD. Class CommFrameReaderAo is an active object
 *  and is run by the active scheduler when the LDD has completed
 *  the CSY's read request. There is one instance of CCommFrameReaderAo.
 */
class CCommFrameReaderAo : public CCommReadWriteBaseAo
	{
public:
	enum TCsyFrameReaderState
		{
		ECsyWaitingForStartFlag  = 0,
		ECsyWaitingForFrameStart = 1,
		ECsyWaitingForFrameData  = 2,
		ECsyWaitingForEndOfFrame = 3
		};

	static CCommFrameReaderAo* NewL(CPortFactory* aParent, CMux0710Protocol* aMux0710Protocol);
	~CCommFrameReaderAo();

	void Read();
	void ReadCancel();

public:// from CActive
	virtual void DoCancel();
	virtual void RunL();

private:
	CCommFrameReaderAo(CPortFactory* aParent, CMux0710Protocol* aMux0710Protocol);
	void ConstructL();

	void NonMuxed();
	void BasicOption();
	void AdvancedOption();
 
	TBool CheckFCS(TDes8& aBuffer, TInt aLen, TUint8 aSuppliedChecksum);

	void AddToReceivedFramesList(CCsyMsgBufBpFrame* aBpFrame);

private:
	TCsyFrameReaderState	iFrameReaderState;
	CCsyMsgBufBpFrame*		iFrameBuf;

	TBool iESCRecved;

	};

#endif  //_CCOMMFRAMEREADERAO_H_
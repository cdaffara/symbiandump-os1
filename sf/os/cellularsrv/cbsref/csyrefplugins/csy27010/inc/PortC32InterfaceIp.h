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
// * Description:  This file defines the CPortC32InterfaceIp class.  This class
// *               is equivalent to client RComm subsessions. This is derived 
// *               from the C32 base class for ports CPort.
//

// CPortC32InterfaceIp.h

/** @file CPortC32InterfaceIp.h
*/
#ifndef _PORTC32INTERFACEIP_H__
#define _PORTC32INTERFACEIP_H__

#include <cs_port.h>
#include "PortC32InterfaceBase.h"

class CChannelMgrCmdData;
class CCsyMsgBufBpFrame;

class CPortC32InterfaceIp : public CPortC32InterfaceBase
	{
public:
	static CPortC32InterfaceIp* NewL(CPortFactory& aPortFactory, 
								   CPortFactory::TC32PortInfo& aPortInfo);
	virtual ~CPortC32InterfaceIp();

	void SendFrameToClient(CCsyMsgBufBpFrame* aFrame);

protected:
	virtual TBool ReadFromBufOrQueue();

private:
	CPortC32InterfaceIp(CPortFactory& aPortFactory,
					  CPortFactory::TC32PortInfo& aPortInfo);
	void ConstructL();

	virtual TInt QueryReceiveBuffer(TInt& aLength) const;
	virtual void ResetBuffers(TUint aFlags);
	
	void PlaceIpPacketFramesOntoFreeList();
	void PlaceIpPacketFramesOntoReadList();
	TBool CreateIpPacketFromFrames();

private:
	//TBuf8<KMaxIpPacketSize>	iPartialReadBuf;

	TUint							iNumberOfPacketsWaiting;

	TSglQue<CCsyMsgBufBpFrame>		iFramesReceivedForIpPacket;
	TSglQueIter<CCsyMsgBufBpFrame>	iFramesReceivedForIpPacketIter;
	TUint							iCurrentIpPacketLength;

	TBuf8<KMaxIpPacketSize>			iIpPacket;
	};


#endif  // _PORTC32INTERFACEIP_H__

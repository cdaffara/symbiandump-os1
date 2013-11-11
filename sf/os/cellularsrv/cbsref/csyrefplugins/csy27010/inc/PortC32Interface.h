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
// * Description:  This file defines the CPortC32Interface class.  This class
// *               is equivalent to client RComm subsessions. This is derived 
// *               from the C32 base class for ports CPort.
//

// CPortC32Interface.h

/** @file CPortC32Interface.h
 *
 * PortC32Interface:
 * - Equivalent to client RComm subsessions. This is derived 
 *   from the C32 base class for ports CPort.
 */

#ifndef _PORTC32INTERFACE_H__
#define _PORTC32INTERFACE_H__

#include <cs_port.h>
#include "PortC32InterfaceBase.h"

class CPortC32Interface : public CPortC32InterfaceBase
	{
public:
	static CPortC32Interface* NewL(CPortFactory& aPortFactory, 
								   CPortFactory::TC32PortInfo& aPortInfo);

	virtual ~CPortC32Interface();
	void SendFrameToClient(CCsyMsgBufBpFrame* aFrame);

protected:
	virtual TBool ReadFromBufOrQueue();

private:
	CPortC32Interface(CPortFactory& aPortFactory,
					  CPortFactory::TC32PortInfo& aPortInfo);
	void ConstructL();

	virtual TInt QueryReceiveBuffer(TInt& aLength) const;
	virtual void ResetBuffers(TUint aFlags);

private:
	TBuf8<KMaxAdvFrameSize>	iPartialReadBuf;

	CCsyMsgBufBpFrame* iReadBuf;
	};

#endif  // _PORTC32INTERFACE_H__
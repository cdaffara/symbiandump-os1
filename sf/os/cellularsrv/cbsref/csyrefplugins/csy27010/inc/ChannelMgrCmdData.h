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
// * Description: This file contains the declaration for class CChannelMgrCmdData.
// *              This class contains the behavior for a single 3GPP 27.010 logical
// *              channel (DLC) for either a command or and data channel.
// *
//

// ChannelMgrCmdData.h

/** @file CChannelMgrCmdData.h
 *
 * .
 */

#ifndef _CHANNELMGRCMDDATA_H_
#define _CHANNELMGRCMDDATA_H_

#include "CsyGlobals.h"
#include "ChannelMgrBase.h"
#include "Mux0710Protocol.h"

class CPortFactory;
class CCsyMsgBufBpFrame;

/** @class CChannelMgrCmdData ChannelMgrCmdData.h "ChannelMgrCmdData.h"
 *  @brief Class CChannelMgrCmdData contains the behavior for a single
 *  3GPP 27.010 logical channel (DLC) for a command/data channel.
 *  This includes methods to open, close, initialize, configure
 *  and use the channel to transmit frames.
 */
class CChannelMgrCmdData : public CChannelMgrBase
	{
public:
	static CChannelMgrCmdData* NewL(const TUint8 aDlcNum,
									CPortFactory& aPortFactory,
									CMux0710Protocol& aMux0710Protocol);
    virtual ~CChannelMgrCmdData();

	void Open(CPortC32InterfaceBase* aPort);
	void Close(CPortC32InterfaceBase* aPort);
	void SetOwner(CPortC32InterfaceBase* aPort);

	// link for our parent's (port factory) single linked list
	TSglQueLink iLink;

private:
    CChannelMgrCmdData(const TUint8 aDlcNum, 
					   CPortFactory& aPortFactory,
					   CMux0710Protocol& aMux0710Protocol);
	void ConstructL();

	void ProcessRecvUihFrame(CCsyMsgBufBpFrame* aBpFrame);
	void SetupCmdChannelsForATCmdsL();

private:

	TUint8 iCount; 
	RArray<CPortC32InterfaceBase*> iPortArray;

	};


#endif  // _CHANNELMGRCMDDATA_H_
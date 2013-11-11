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
// * Description: This file contains the declaration for class CChannelMgrCtrl.
// *              Class CChannelMgrCtrl defines a 3GPP 27.010 control channel.
// *              A control channel is used to initialize the multiplexer and to
// *              create/monitor DLC's. There is one instance of the control channel.
// *
//

// ChannelMgrCtrl.h

/** @file ChannelMgrCtrl.h
 *
 * 
 */

#ifndef _CCHANNELMGRCTRL_H_
#define _CCHANNELMGRCTRL_H_

#include "ChannelMgrBase.h"

class CPortFactory;
class CMux0710Protocol;
class CCsyMsgBufBpFrame;

/** @class CChannelMgrCtrl ChannelMgrCtrl.h "ChannelMgrCtrl.h"
 *  @brief Class CChannelMgrCtrl defines a 3GPP 27.010 control channel.
 *  A control channel is used to initialize the multiplexer and to
 *  create/monitor DLC's. There is one instance of the control channel.
 */
class CChannelMgrCtrl : public CChannelMgrBase
    {
public:
    static CChannelMgrCtrl* NewL(CPortFactory& aPortFactory,
								 CMux0710Protocol& iMux0710Protocol);
    virtual ~CChannelMgrCtrl();
	inline TBool GetChannelReady() const;

private:
    CChannelMgrCtrl(CPortFactory& aPortFactory,
					CMux0710Protocol& aMux0710Protocol);

    void ConstructL();

	void ProcessRecvUihFrame(CCsyMsgBufBpFrame* aBpFrame);
	void ConfirmCtrlFrame(CCsyMsgBufBpFrame* aBpFrame);
	};


inline TBool CChannelMgrCtrl::GetChannelReady() const
/**
 * Return the channel ready
 *
 * @return Channel ready
 */
	{
	return iChannelReady;
	}

#endif // _CCHANNELMGRCTRL_H_
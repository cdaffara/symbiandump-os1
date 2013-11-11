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
// * Description: This file contains the declaration for class CChannelMgrBase.
// *              Class CChannelMgrBase is an abstract class that is inherted by
// *              classes that define a single 3GPP 27.010 based logical channel (DLC).
// *              Derived classes are either control, command and data channels.
//

// ChannelMgrBase.h

/** @file ChannelMgrBase.h
 *
 * This file contains the declaration for class CChannelMgrBase.
 * Class CChannelMgrBase is an abstract class that is inherted by
 * classes that define a single 3GPP 27.010 based logical channel (DLC).
 * Derived classes are either control, command and data channels.
 */

#ifndef _CHANNELMGRBASE_H_
#define _CHANNELMGRBASE_H_

#include <e32base.h>
#include <c32comm.h>
#include "CsyGlobals.h"
#include "timeouter.h"

class CPortFactory;
class CMux0710Protocol;
class CPortC32InterfaceBase;
class CPortC32Interface;
class CFrameToBPQueue;
class CFrameToClientQueue;
class CChannelObserverAo;
class CCsyMsgBufBpFrame;

/** 
 *  CChannelMgrBase is an abstract class that is inherted by
 *  classes that define a single 3GPP 27.010 based logical channel (DLC).
 *  Derived classes are either control or cmd/data channels.
 */
class CChannelMgrBase : public CBase, public MTimeoutObserver
	{
public:
	enum TCsyChannelState
		{
		ECsyChannelStateDisconnected,    // Channel is Disconnected
		ECsyChannelStateParameterNegotiating, // Channel was disconnected and has sent Parameter Negotiation
		ECsyChannelStateConnecting,      // Channel was disconnected and has sent a SABM to be connected
		ECsyChannelStateMSCsent,         
		ECsyChannelStateWaitingForChannelReady, 
		ECsyChannelStateConnected,       // Channel is Connected, Data can be transmitted
		ECsyChannelStateDisconnecting,   // Channel was connected and is now waiting to be disconnected
		ECsyChannelStateFlowControlOff,  // Channel has been Xoff'd by the baseband
		ECsyChannelStateTransmitError    // Not receive after timeout and retransmite, Link error
		};

public:
    CChannelMgrBase(const TUint8 aDlcNum,
					CPortFactory& aPortFactory, 
					CMux0710Protocol& aMux0710Protocol);

    virtual ~CChannelMgrBase();

    TInt Connect();
    TInt Disconnect();
	TInt ParameterNegotiate();

	void ProcessRecvFrame(CCsyMsgBufBpFrame* aBpFrame);
	
	TDes8& RefToMsgBuffer();

	TUint8 GetDlcNumber() const;
	TCsyChannelState GetChannelState() const;

	void WaitForChannelReady();
	void NotifyChannelReady();

	CPortC32InterfaceBase* GetC32Port() const;

	void MscReceived(TUint8 aV24Signals);
	TInt SendMscCommand(TUint8 aV24Signals);

	void ModemAndCsyToClientFlowCtrl(const TFlowControl aFlowControl);
	void ReceivedV24Signals(const TUint8 aV24Signals);

	inline TFlowControl CsyToModemFlowControl();
	TInt SetCsyToModemFlowControl(TFlowControl aFlowControl);

	TInt PlaceOnOutboundQueue();
	void WriteCancel();

	inline TFlowControl GetModemAndCsyToClientFlowControl();

public://From MTimeoutObserver
	void TimedOut();

protected:
	void ConstructL();

	void IssueNotificationRequest();

	virtual void ProcessRecvUihFrame(CCsyMsgBufBpFrame* aBpFrame) = 0;
	void ProcessNonUihRecvFrame(CCsyMsgBufBpFrame* aBpFrame);

protected:

	TUint8					iV24Signals;

    const TUint8			iDlcNum;

    TCsyChannelState		iChannelState;
    CPortFactory&			iPortFactory;
	CPortC32InterfaceBase*	iPortC32Interface;
	CMux0710Protocol&		iMux0710Protocol;

    TBuf8<KMaxIpPacketSize>	iDataToSendToModem;

	TBool					iChannelReady;
	TBool					iInitFinished;

	TInt					iTimeoutVal;
	TInt					iTxCountLimit;
	TInt					iTxCount;

	CChannelObserverAo*		iChannelObserverAo;
	CActiveTimeouter*		iTimeouter;

	TBool					iMscReplyExpected;

	// modem -> csy flow control - controls the CSY sending frames to the modem
	TFlowControl			iCsyAllowedToSendFrames;

	// csy -> modem flow control - controls the modem sending frames to the CSY
	TFlowControl			iModemAllowedToSendFrames;
	TBool					iDelayedWriteToModem;

	};

// Inline methods

inline TFlowControl CChannelMgrBase::CsyToModemFlowControl()
/**
 * Return the current CSY to modem flow control status for this channel.
 */
	{
	return iModemAllowedToSendFrames;
	}

inline TUint8 CChannelMgrBase::GetDlcNumber() const
/**
 * Return the DLC number of the channel manager object.
 *
 * @return DLC number
 */
	{
	return iDlcNum;
	}

inline CChannelMgrBase::TCsyChannelState CChannelMgrBase::GetChannelState() const
/**
 * Return the channel state.
 *
 * @return Channel state
 */
	{
	return iChannelState;
	}

inline CPortC32InterfaceBase* CChannelMgrBase::GetC32Port() const
/**
 * Return a pointer to the C32 port associated with this channel manager object.
 *
 * @return Pointer to the C32 port associated with this channel manager object
 */
	{
	return iPortC32Interface;
	}

inline TFlowControl CChannelMgrBase::GetModemAndCsyToClientFlowControl()
/**
 * Return the current flow control state of the channel.
 *
 * @return Flow control state
 */
	{
	return iCsyAllowedToSendFrames;
	}

/* CChannelObserverAo use to block until channel init is finished
 */
class CChannelObserverAo :public CActive
	{
public:
	static CChannelObserverAo* NewL();
	~CChannelObserverAo();

 	void StartWait();
	void ChannelReady();

protected:// from CActive
    void RunL();
    void DoCancel();

private:
	CChannelObserverAo();
	};

#endif  // _CHANNELMGRBASE_H_
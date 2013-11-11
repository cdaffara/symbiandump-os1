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
// * Description:  This file defines the CMux0710Protocol class.  This class
//                 performs the 3GPP2 protocol specific behavior for 27.010.
//                 This file also includes CRC table.
//

// CMux0710Protocol.h

/** @file CMux0710Protocol.h
 *
 */

#ifndef _MUX0710PROTOCOL_H_
#define _MUX0710PROTOCOL_H_

#include <e32base.h>
#include "CsyGlobals.h"
#include "timeouter.h"


class CCsyMsgBufBase;
class CCsyMsgBufFrame;
class CPortFactory;
class CCsyMsgBufBpFrame;
class CCommFrameWriterAo;
class CCommFrameReaderAo;

class CMux0710Protocol : public CBase, public MTimeoutObserver
	{
public:

	// Frame Types used in 27.010 Protocol
	enum TCsyFrameType
		{
		ESABM, // Set asynchronous balanced mode
		EUA,   // Unnumbered Acknowledgement
		EDM,   // Disconnected Mode
		EDISC, // Disconnect
		EUIH,  // Unnumbered information with header check
		EUI,   // Unnumberd information
		EUNKNOWN 
		};

	enum TCsyCtrlCommand
		{
		EParamNeg, // Parameter Negotiation
		EMSC	   // Modem status command
		};

public:
	static CMux0710Protocol* NewL(CPortFactory& aPortFactory);
	virtual ~CMux0710Protocol();

	void SetCommWriter(CCommFrameWriterAo* aCommWriter);

	TBool IsMuxModeEnabled() const;
	TInt SwitchToMuxMode();
	TInt ParseATResponse(TDes8& aResp);

	CCsyMsgBufBpFrame* GetFreeFrameBuf(TBool aCheckFlowControlThresholds = ETrue);
	void AddFrameFreeQ(CCsyMsgBufBpFrame* aBpFrame);

    TInt Create0710DataFrames(TDesC8& aMsgBuf, const TUint8 aDlcNum);
	TInt Create0710ControlFrame(TCsyFrameType aFrameType, TUint8 aDlcNum,
		TCsyCtrlCommand aType = EParamNeg, TUint8 aV24Signals = 0);

	inline TUint8 PreviousNegotiateDlcNum() const;
    inline TBool MaxRetriesReached() const;
    
public:// from MTimeoutObserver
	void TimedOut();

private:
	enum TCsyMuxMgrState
		{
		ECsyWaitingForAttentionResp,
		ECsyWaitingForEchoDisableResp,
		ECsyWaitingForCmuxResp,
		ECsyMuxEnabled,
		ECsyWaitingForFlushResp,
#ifdef USE_TI_CONDAT_STACK
		ECsyWaitingForResetResp,
		ECsyWaitingForSleepResp
#endif
		};
	
private:
	CMux0710Protocol(CPortFactory& aPortFactory);
	void ConstructL();

	void Create0710UIHFrameFromMsg(TDesC8& aMsgBuf, TInt aOffset,
								   CCsyMsgBufBpFrame* aFrameBuf,
								   TInt aLength, TUint8 aDlcNum);

	void Create0710UIHLayer4FrameFromMsg(TDesC8& aMsgBuf, 
										 TInt aOffset, 
										 CCsyMsgBufBpFrame* aFrameBuf, 
										 TInt aLength, TUint8 aDlcNum, 
										 TBool aLayer4Begin, 
										 TBool aLayer4End);

	void ProcessControlChannelRecvUihFrame(CCsyMsgBufBpFrame* aBpFrame);

	TUint8 CalcFCS(TUint8* aBuffer, TInt aLen);

	void DumpFrame(CCsyMsgBufBpFrame* aFrameBuf);

private:
	CActiveTimeouter*               iTimeouter;

	CPortFactory&                   iPortFactory;

	TCsyMuxMgrState                 iMuxMgrState;

	CCommFrameWriterAo*             iCommWriter;

	// Free Frame Buf List
	TSglQue<CCsyMsgBufBpFrame>      iFreeFrameBufList;
	TSglQueIter<CCsyMsgBufBpFrame>  iFreeFrameBufIter;
	TUint                           iFreeFrameCount;

	TBool iEnforcingFlowControl;

	TBool iModemRequestedToStop;

	TUint8 iParamNegotiateDlcNum;

	TUint8 iAtResponseTimeout;

	TBool iMaxRetriesReached;
	TBuf8<100> iResponseStr; 
	TInt iRetryCount;
	};

// inline methods

/**
Set the comm writer to the specified object pointer.

@param aCommWriter Pointer to the comm writer
*/
inline void CMux0710Protocol::SetCommWriter(CCommFrameWriterAo* aCommWriter)
	{
	iCommWriter = aCommWriter;
	}

/**
Return True if multiplexing mode is enabled, else return False.
*/
inline TBool CMux0710Protocol::IsMuxModeEnabled() const
	{
	if (iMuxMgrState == ECsyMuxEnabled)
		{
		return (ETrue);
		}
	else
		{
		return (EFalse);
		}
	}

/**
Return dlc that parameter negotiate command was sent for.
*/
inline TUint8 CMux0710Protocol::PreviousNegotiateDlcNum() const
	{
	return iParamNegotiateDlcNum;
	}

/**
Return whether the max connection retries has been reached.
*/
inline TBool CMux0710Protocol::MaxRetriesReached() const
	{
	return iMaxRetriesReached;
	}

#endif // _MUX0710PROTOCOL_H_

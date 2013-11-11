// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Defines the avdtp signalling transaction
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVDTPSIGNALLINGTRANSACTION_H
#define AVDTPSIGNALLINGTRANSACTION_H

#include "avdtpSignallingMessages.h"

/**
NB the value here is specified by GAVDP to be 0.5~3.0 seconds.
It also only applies to certain signalling operations (see GAVDP 4.1.8)

This value is in microseconds.
*/
const TInt KAvdtpSigRTXTimeout = 3000000;

class CSignallingChannel;
class XAvdtpSignalReceiver;

NONSHARABLE_CLASS(CSignallingTransaction) : public CBase
/**
Represents data pertaining to an actual transaction: the message, the label,
a pointer to useful data that is not received in AVDTP responses etc.

It also manages the response timer for commands.
*/
	{
public:
	static CSignallingTransaction* New(CSignallingChannel& aSignallingChannel,
												TAvdtpMessage aSignal,
												TAvdtpMessageType aMessageType);
	~CSignallingTransaction();
	
	inline void SetUser(XAvdtpSignalReceiver* aUser);
	inline void SetLabel(TAvdtpTransactionLabel aLabel);
	inline void SetCookie(TAny* aCookie);
	void SetSentAction();

	inline XAvdtpSignalReceiver* User() const;
	inline TAvdtpTransactionLabel Label() const;
	inline TAny* Cookie() const;
	inline TPostSendAction SentAction() const;
	inline CSignallingChannel& SignallingChannel() const;
	inline TAvdtpMessage Signal() const;
	inline CAvdtpOutboundSignallingMessage& Message() const;
	
	void StartTimer();
	void CancelTimer();
		
	static TInt TimerExpired(TAny* aTransaction);
	void Error(TInt aError);
public:
	TDblQueLink iLink;			// to que

private:
	CSignallingTransaction(CSignallingChannel& aSignallingChannel,
							TAvdtpMessage aSignal);
	TInt Construct(TAvdtpMessageType aMessageType);
private:		
// not-owned, NULL if resp, or command to which issues does not need reply (eg Abort and Release)
	XAvdtpSignalReceiver*		iUser;
// used in packet && by us to mux
	TAvdtpTransactionLabel		iLabel;

// the signal represented by the transaction - this is effectively a "cache" of the message signal
// do reduce RAM footprint the message is destroyed upon sending as AVDTP signalling
// does not retransmit
	TAvdtpMessage				iSignal;
// represents what actually is transmitted
 	CAvdtpOutboundSignallingMessage*	iMessage;
// e.g to remember useful stuff that the protocol specification doesnt give back in responses
	TAny*						iCookie;
// the object that will perform this transaction	
	CSignallingChannel&			iSigCh;

	TDeltaTimerEntry			iTimerEntry;	// for pending messages
	TBool						iTimerSet;
	TPostSendAction				iPostSendAction;
	};


inline void CSignallingTransaction::SetUser(XAvdtpSignalReceiver* aUser)
	{
	iUser = aUser;
	}
	
inline void CSignallingTransaction::SetLabel(TAvdtpTransactionLabel aLabel)
	{
	iLabel = aLabel;
	}
	
inline void CSignallingTransaction::SetCookie(TAny* aCookie)
	{
	iCookie = aCookie;
	}
	
inline XAvdtpSignalReceiver* CSignallingTransaction::User() const
	{
	return iUser;
	}
	
inline TAvdtpTransactionLabel CSignallingTransaction::Label() const
	{
	return iLabel;
	}
	
inline TAny* CSignallingTransaction::Cookie() const
	{
	return iCookie;
	}
	
inline TPostSendAction CSignallingTransaction::SentAction() const
	{
	return iPostSendAction;
	}
	
inline CSignallingChannel& CSignallingTransaction::SignallingChannel() const
	{
	return iSigCh;
	}

inline TAvdtpMessage CSignallingTransaction::Signal() const
	{
	return iSignal;
	}

inline CAvdtpOutboundSignallingMessage& CSignallingTransaction::Message() const
	{
	__ASSERT_DEBUG(iMessage, User::Panic(_L("avdtp sig tr h"), __LINE__));
	return *iMessage;
	}
	
	
#endif //AVDTPSIGNALLINGTRANSACTION


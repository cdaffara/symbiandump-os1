/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file lbsnrhngchannel.h
*/
#ifndef LBSNRHNGCHANNEL_H_
#define LBSNRHNGCHANNEL_H_

#include "lbsnetinternalapi.h"
#include "lbsreffnpint.h"


class CTestExecuteLogger;

/* Class that handles sending messages to the NRH
   over the NG-NRH interface.
   
   It is an active object because it needs to wait for the
   acknowledgement signal to ensure the message has been
   read by the NRH.
*/
class CNgChannel : public CActive,
				   public MLbsNetChannelObserver
	{
public:
	static CNgChannel* NewL(CTestExecuteLogger& aLogger);
	~CNgChannel();
	
	void RunL();
	void DoCancel();
	
	void SendMessageAndWait(const TLbsNetInternalMsgBase& aMessage, 
							TTimeIntervalSeconds aTimeout);
	
	void WaitForResponseL(TTimeIntervalSeconds aTimeout);
	TLbsNetInternalMsgBase::TLbsNetInternalMsgType GetNextResponseType();
	void GetNextResponseL(TLbsNetInternalMsgBase& aMessage);
	
private:
	CNgChannel();
	CNgChannel(CTestExecuteLogger& aLogger);
	void ConstructL();

	// from MLbsNetChannelObserver - receives messages from NRH
	void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, 
								  const TLbsNetInternalMsgBase& aMessage);
	void BufferMessageL(const TLbsNetInternalMsgBase& aMessage);
	
	CTestExecuteLogger& Logger() { return iLogger; }
	void LogIncomingMessage(const TLbsNetInternalMsgBase& aMessage);
	void LogOutgoingMessage(const TLbsNetInternalMsgBase& aMessage);

private:
	CTestExecuteLogger& iLogger;
	CActiveSchedulerWait iSendWait;
	CActiveSchedulerWait iReceiveWait;
	RLbsNetChannel iNrhChannel;
	RPointerArray<TLbsNetInternalMsgBase> iResponseBuffer;
	
	// TODO: remove these when the reference and final network positions
	//       are sent directly over the NG interface
	RLbsNetworkPositionUpdates iRefPosUpdates;
	RLbsNetworkPositionUpdates iFinalPosUpdates;
	};

#endif /*LBSNRHNGCHANNEL_H_*/

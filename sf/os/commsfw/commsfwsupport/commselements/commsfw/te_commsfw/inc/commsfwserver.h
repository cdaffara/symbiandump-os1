/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file CommsFWServer.h
*/
#if (!defined __COMMSFWSERVER_H__)
#define __COMMSFWSERVER_H__
#include <test/testexecuteserverbase.h>

#include <commschan.h>
#include <cfmsgs.h>
#include <cflog.h>

using namespace CommsFW;

class TMsg : public TCFCommsMessageId
	{
public:
	inline TMsg(TId aIdentifier = 0)
		: TCFCommsMessageId(TCFCommsMessage::ECodeDiscover, aIdentifier)
		{ }
	operator TUint () const { return Identifier(); }
	};

typedef RCFChannelBase<TMsg, 1> RChanUDSP;		// UniDirectionalSinglePriority
typedef RCFChannelBase<TMsg, 3> RChanUDTP;		// UniDirectionalTriplePriority
typedef RCFChannelPairBase<TMsg, 1> RChanBDSP;	// BiDirectionalSinglePriority


class CCommsFWServer : public CTestServer
	{
public:
	static CCommsFWServer* NewL();
	void ConstructL(const TDesC& aName);
	~CCommsFWServer();

	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

private:
	RFs iFs;
	__CFLOG_DECLARATION_MEMBER;
	};

#endif	// __COMMSFWSERVER_H__


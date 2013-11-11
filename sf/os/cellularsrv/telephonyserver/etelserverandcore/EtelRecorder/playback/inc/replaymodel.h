// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class definitions for replayer model.
// 
//

/**
 @file
*/

#ifndef __REPLAYMODEL_H__
#define __REPLAYMODEL_H__

#include <e32base.h>
#include "replaylogger.h"
#include "replaysession.h"

class CDispatchMessageBaseAO;
class MModelCallback
	{
public:
	virtual void DispatchFinished(CDispatchMessageBaseAO* aWho) = 0;
	};

class CReplayModel : public CTimer, public MModelCallback, public MSessionManager 
	{
public:
	static CReplayModel* NewLC(CConsoleBase* aConsole);
	virtual ~CReplayModel();
	void AddMessageToModelL(CRecordMessage* aMessage);
	void StartL(TBool aInPreviewMode);
	
public: //from CActive
	void DoCancel();
	
protected: //from CActive
	void RunL();
	
public: //from MSessionManager
	virtual TInt GetSessionFromSessionId(CRecordMessage::TSession aId, RReplaySession*& aSessionHandle);
	virtual TInt GetSubSessionFromSubSessionHandle(CRecordMessage::TSession aSessionId, CRecordMessage::TSubSession aSubsessionId, CRecordMessage::TSubSessionHandle ahandle, RReplaySubSession*& aSessionHandle);
	virtual RReplaySession& GetNewSessionL(CRecordMessage::TSession aId);
	virtual RReplaySubSession& GetNewSubSessionL(CRecordMessage::TSubSession aId, CRecordMessage::TSubSessionHandle aHandle, const RReplaySession& aSessionHandle);
	virtual TInt CloseAndRemoveSession(CRecordMessage::TSession aId);
	virtual TInt CloseAndRemoveSubSession(CRecordMessage::TSession aSessionId, CRecordMessage::TSubSession aSubsessionId, CRecordMessage::TSubSessionHandle aHandle);
	
public:
	void DispatchFinished(CDispatchMessageBaseAO* aWho);
	
protected:
	CReplayModel(CConsoleBase* aConsole);
	void DispatchNextRequestL();
	CDispatchMessageBaseAO* FactoryCreateDispatcherAOL(CRecordMessage::TMessageType aType, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager);
	TBool ContinueOrStopModel();
	void PrintOutDispatchInfo(CLogger* aHtmlLogger, TInt aId, CRecordMessage& aMessage, TBool aIsFirstMessageOfSession);
	void RemoveSubSessionsAssociatedWithSession(CRecordMessage::TSession aSessionId);
	
private:
	CConsoleBase* iConsole;
	RPointerArray<CRecordMessage> iAllMessages;
	RPointerArray<CDispatchMessageBaseAO> iActiveRequests;
	TInt iNextRequestToActivateIndex;
	RArray<RReplaySession> iLiveSessions;
	RArray<RReplaySubSession> iLiveSubSessions;
	TBool iRunInPreviewMode;
	};

	

#endif

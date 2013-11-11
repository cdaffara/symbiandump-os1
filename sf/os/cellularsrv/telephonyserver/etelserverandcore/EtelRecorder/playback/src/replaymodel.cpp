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
// Function definitions for replayer model.
// 
//

/**
 @file
*/

#include "replaymodel.h"
#include "replaymain.h"
#include "replaydispatchmessageao.h"
#include "tipclookup.h"

_LIT8(KGreen, "00AF00");
_LIT8(KRed, "FF0000");
_LIT8(KBlue, "0000FF");


_LIT8(KEtelSequenceHtmlNewLogSession, "<font size=2 color=%S>//-------Start of new log session(time=%lf)--------//</font>\r\r");
_LIT8(KEtelSequenceHtmlConnectSession, ">>---------Connect Session(time=%lf ipc=%S(%d), session=0x%x)--------->>\r\r");
_LIT8(KEtelSequenceHtmlCloseSession, ">>---------Close Session(time=%lf, ipc=%S(%d), session=0x%x)--------->>\r\r");
_LIT8(KEtelSequenceHtmlOpenSubSession, ">>---------Open SubSession(time=%lf, ipc=%S(%d), session=0x%x, subsession=0x%x, subsessionhandle=%d)--------->>\r\r");
_LIT8(KEtelSequenceHtmlCloseSubSession, ">>---------Close SubSession(time=%lf, ipc=%S(%d), session=0x%x, subsession=0x%x, subsessionhandle=%d)--------->>\r\r");
_LIT8(KEtelSequenceHtmlMessageComplete, "<font size=2 color=%S><<---------Message Complete(time=%lf, ipc=%S(%d), ret=%d, session=0x%x, subsession=0x%x, subsessionhandle=%d)---------<<</font>\r\r");
_LIT8(KEtelSequenceHtmlSessionIpc, ">>---------Session IPC(time=%lf, ipc=%S(%d), session=0x%x)--------->>\r\r");
_LIT8(KEtelSequenceHtmlSubSessionIpc, ">>---------SubSession IPC(time=%lf, ipc=%S(%d), session=0x%x, subsession=0x%x, subsessionhandle=%d)--------->>\r\r");
_LIT8(KUnknownIPC, "UnknownIPC");


CReplayModel* CReplayModel::NewLC(CConsoleBase* aConsole)
	{
	CReplayModel* p = new(ELeave) CReplayModel(aConsole);
	CleanupStack::PushL(p);
	p->ConstructL(); //CTimer::ConstructL()
	return p;
	}
	
CReplayModel::CReplayModel(CConsoleBase* aConsole)
: CTimer(EPriorityStandard), iConsole(aConsole)
	{
	CActiveScheduler::Add(this);
	}
	
void CReplayModel::RunL()
	{
	DispatchNextRequestL();
	}
	
void CReplayModel::DoCancel()
	{
	
	}
	
CReplayModel::~CReplayModel()
	{
	
	Cancel(); //cancel timer
	TInt sz = iAllMessages.Count();
	CRecordMessage* message;
	for(TInt i = sz-1; i>=0;i--)
		{
		message = iAllMessages[i];
		iAllMessages.Remove(i);
		delete message;
		}
	iAllMessages.Reset();
	iAllMessages.Close();
	
	sz = iActiveRequests.Count();
	CDispatchMessageBaseAO* reqAO;
	for(TInt i = sz-1; i>=0;i--)
		{
		reqAO = iActiveRequests[i];
		iActiveRequests.Remove(i);
		delete reqAO;
		}
	iActiveRequests.Reset();
	iActiveRequests.Close();
	
	sz = iLiveSubSessions.Count();
	for(TInt i = sz-1; i>=0;i--)
		{
		iLiveSubSessions[i].Close();
		iLiveSubSessions.Remove(i);
		}
	iLiveSubSessions.Close();
	
	//close all live sessions. We are using an RArray so need to close the session before removing (destroying) from array
	sz = iLiveSessions.Count();
	for(TInt i = sz-1; i>=0;i--)
		{
		iLiveSessions[i].Close();
		iLiveSessions.Remove(i);
		}
	iLiveSessions.Close();			
	}
	
void CReplayModel::AddMessageToModelL(CRecordMessage* aMessage)
	{
	//becomes owner
	iAllMessages.AppendL(aMessage);
	}
	
void CReplayModel::PrintOutDispatchInfo(CLogger* aHtmlLogger, TInt aId, CRecordMessage& aMessage, TBool aIsFirstMessageOfSession)
	{
	CRecordMessage::TMessageHeader& aHdr = *aMessage.GetMessageHeader();
	CRecordMessage::TSessionInfo& aSess = *aMessage.GetSessionInfo();
	
	if(aIsFirstMessageOfSession)
		{
		LOGLINE1(_L(""));
		LOGLINE1(_L("Start of new log session - this is the first message of new log session."));
		LOGLINE1(_L(""));
		if(aHtmlLogger)
			{
			aHtmlLogger->Write(KEtelSequenceHtmlNewLogSession, &KBlue, aHdr.iTimeStamp/1000.0);
			}		
		}

	LOGLINE5(_L(">>> At time=%lf dispatch message ID=%d. Type=%d, ipc=%d"), aHdr.iTimeStamp/1000.0, aId, aHdr.iMessageType, aHdr.iIpc);
	LOGLINE4(_L("    arg0(valid=%d, argtype=%d, len=%d)"), aHdr.iArg0Info.iArgFlags & CRecordMessage::KArgValid, aHdr.iArg0Info.iArgFlags & ~CRecordMessage::KArgValid,aHdr.iArg0Info.iArgLength);
	LOGLINE4(_L("    arg1(valid=%d, argtype=%d, len=%d)"), aHdr.iArg1Info.iArgFlags & CRecordMessage::KArgValid, aHdr.iArg1Info.iArgFlags & ~CRecordMessage::KArgValid,aHdr.iArg1Info.iArgLength);
	LOGLINE4(_L("    arg2(valid=%d, argtype=%d, len=%d)"), aHdr.iArg2Info.iArgFlags & CRecordMessage::KArgValid, aHdr.iArg2Info.iArgFlags & ~CRecordMessage::KArgValid,aHdr.iArg2Info.iArgLength);
	LOGLINE4(_L("    Session handles. Session=0x%x,SubSession=0x%x, SubSessionHandle=0x%x"), aSess.iSession, aSess.iSubSession, aSess.iSubSessionHandle);
	
	//find name for the ipc in lookup table
	TBuf8<50> unknownIPC;
	TPtrC8 ipcname;
	TInt count = sizeof(TIpcLookup)/sizeof(TIpcToName);
	TInt i;
	for(i =0; i<count;i++)
		{
		if(TIpcLookup[i].iIpc == aHdr.iIpc)
			{
			ipcname.Set(TIpcLookup[i].iName);
			break;
			}						
		}
	if(i>=count) //could not find ipc
		{
	//	unknownIPC.Format(KUnknownIPC, aHdr.iIpc);
		ipcname.Set(KUnknownIPC);
		LOGLINE2(_L("WARNING: Unable to match ipc=%d in TIpcLookup"), aHdr.iIpc);
		}
						
	if(aHtmlLogger)
		{
		switch(aHdr.iMessageType)
			{
			case CRecordMessage::EConnectSession:
			aHtmlLogger->Write(KEtelSequenceHtmlConnectSession, aHdr.iTimeStamp/1000.0, &ipcname, aHdr.iIpc, aSess.iSession);			
			break;
			case CRecordMessage::ECloseSession:
			ipcname.Set(KCloseSessionIpc);
			aHtmlLogger->Write(KEtelSequenceHtmlCloseSession, aHdr.iTimeStamp/1000.0, &ipcname, aHdr.iIpc, aSess.iSession);			
			break;
			case CRecordMessage::EOpenSubSession:
			aHtmlLogger->Write(KEtelSequenceHtmlOpenSubSession, aHdr.iTimeStamp/1000.0, &ipcname, aHdr.iIpc, aSess.iSession, aSess.iSubSession, aSess.iSubSessionHandle);			
			break;
			case CRecordMessage::ECloseSubSession:
			aHtmlLogger->Write(KEtelSequenceHtmlCloseSubSession, aHdr.iTimeStamp/1000.0, &ipcname, aHdr.iIpc, aSess.iSession, aSess.iSubSession, aSess.iSubSessionHandle);						
			break;
			case CRecordMessage::ESessionIpc:
			aHtmlLogger->Write(KEtelSequenceHtmlSessionIpc, aHdr.iTimeStamp/1000.0, &ipcname, aHdr.iIpc, aSess.iSession);						
			break;
			case CRecordMessage::ESubSessionIpc:
			aHtmlLogger->Write(KEtelSequenceHtmlSubSessionIpc, aHdr.iTimeStamp/1000.0, &ipcname, aHdr.iIpc, aSess.iSession, aSess.iSubSession, aSess.iSubSessionHandle);						
			break;
			case CRecordMessage::EMessageComplete:
				{
				if(aHdr.iArg0Info.iArgFlags & CRecordMessage::KArgValid)
					{
					TPtrC8 resBufd;
					aMessage.GetArg(0, resBufd);
					TInt res = *reinterpret_cast<TInt*>(const_cast<TUint8*>(resBufd.Ptr()));
					
					if(res == KErrNone)
						{
						aHtmlLogger->Write(KEtelSequenceHtmlMessageComplete, &KGreen, aHdr.iTimeStamp/1000.0, &ipcname, aHdr.iIpc, res, aSess.iSession, aSess.iSubSession, aSess.iSubSessionHandle);							
						}
					else
						{
						aHtmlLogger->Write(KEtelSequenceHtmlMessageComplete, &KRed, aHdr.iTimeStamp/1000.0, &ipcname, aHdr.iIpc, res, aSess.iSession, aSess.iSubSession, aSess.iSubSessionHandle);								
						}
					
						
					}
				}
		
			break;			
			};
		}
	}
	
void CReplayModel::StartL(TBool aRunInPreviewMode)
	{
	iRunInPreviewMode = aRunInPreviewMode;
	TInt noMsgs = iAllMessages.Count();
	LOGLINE2(_L("Number of messages in model = %d"), noMsgs);	
	
	//remove UTC offsets from the time stamps
	TReal utcOffset=0.0;
	TReal timeOfLastMsgInLogSession = 0.0;
	for(TInt i = 0; i<noMsgs;i++)
		{
		CRecordMessage::TMessageHeader* msgHdr = iAllMessages[i]->GetMessageHeader();
		if(iAllMessages[i]->IsFirstMessage())
			{
			//we get the offset at each message which is the first message for a new log session
			utcOffset = msgHdr->iTimeStamp;
			if(utcOffset < 0.0)
				{
				utcOffset=0.0;
				}		
			if(i>0)
				{
				timeOfLastMsgInLogSession = iAllMessages[i-1]->GetMessageHeader()->iTimeStamp;
				timeOfLastMsgInLogSession += 1000.0; //add another second
				}
			}
		msgHdr->iTimeStamp = msgHdr->iTimeStamp - utcOffset + timeOfLastMsgInLogSession;		
		}
	
	if(aRunInPreviewMode)
		{
		LOGLINE1(_L("Executing model in preview mode.  This will just log out all the messages that were recorder"));
		
		CLogger* htmlLogger = CLogger::NewL();
		CleanupStack::PushL(htmlLogger);		
		
		for(TInt i = 0; i<noMsgs;i++)
			{
			CRecordMessage* message = iAllMessages[i];
			PrintOutDispatchInfo(htmlLogger, i, *message, message->IsFirstMessage());
			}
			
		CleanupStack::PopAndDestroy(htmlLogger);
		//frig the async complete
		TRequestStatus *pS=(&iStatus);
		iStatus = KRequestPending;
		SetActive();
		User::RequestComplete(pS,KErrNone);
		}
	else
		{
		DispatchNextRequestL();
		}
	}
	
void CReplayModel::DispatchNextRequestL()
	{
	if(ContinueOrStopModel()) //this tells us whether to continue or stop
		{
		//get current message and next message to work out delay between them
		CRecordMessage* messageToDispatch = iAllMessages[iNextRequestToActivateIndex];
		CRecordMessage::TMessageHeader* messageToDispatchHdr = messageToDispatch->GetMessageHeader();
		
		PrintOutDispatchInfo(NULL, iNextRequestToActivateIndex, *messageToDispatch, messageToDispatch->IsFirstMessage());
		
		//dispatch the message
		CDispatchMessageBaseAO* dispatcher = FactoryCreateDispatcherAOL(messageToDispatchHdr->iMessageType, iNextRequestToActivateIndex, messageToDispatch, *this, *this);
		if(dispatcher) //dispatcher can be null. If it is just kick off next timer
			dispatcher->StartL();
			
		//start timer
		if(iNextRequestToActivateIndex+1 >= iAllMessages.Count())
			{
			LOGLINE1(_L("This is the last message to dispatch"));
			}
		else
			{
			CRecordMessage* nextMessageToDispatch = iAllMessages[iNextRequestToActivateIndex+1];
			CRecordMessage::TMessageHeader* nextMessageToDispatchHdr = nextMessageToDispatch->GetMessageHeader();
			TReal delay = nextMessageToDispatchHdr->iTimeStamp - messageToDispatchHdr->iTimeStamp;		
			
			TTimeIntervalMicroSeconds32 delayMS32 = delay*1000.0;
			After(delayMS32);
			//no need to call SetActive() as After() does it for us
			}
			
		iNextRequestToActivateIndex++; //increment for next message		
		}
	}
	
void CReplayModel::DispatchFinished(CDispatchMessageBaseAO* aWho)
	{
	//remove disptacher from active list
	TInt idx = iActiveRequests.Find(aWho);
	if(idx == KErrNotFound)
		{
		ReplaySession::Panic(ReplaySession::EUnknownDispatcher);
		}
	else
		{
		CDispatchMessageBaseAO* dispatcher = iActiveRequests[idx];
		iActiveRequests.Remove(idx);
		delete dispatcher;
		}
	ContinueOrStopModel();
	}
	
TBool CReplayModel::ContinueOrStopModel()
	{
	TBool bContinue = ETrue;;
	//get next message to dispatch
	if(iRunInPreviewMode)
		{
		LOGLINE1(_L("Preview mode finished - stopping model"));
		bContinue = EFalse;
		}
	else if(iNextRequestToActivateIndex >= iAllMessages.Count() && iActiveRequests.Count() > 0)
		{
		LOGLINE2(_L("No more messages to dispatch - waiting on %d active requests to complete before stopping"), iActiveRequests.Count());
		}
	else if(iNextRequestToActivateIndex >= iAllMessages.Count() && iActiveRequests.Count() == 0)
		{
		LOGLINE1(_L("No more messages to dispatch and no active requests.  Model will now be stopped"));
		bContinue = EFalse;
		}
	
	if(!bContinue)
		{
		CActiveScheduler::Stop();
		}
	//continue model
	return bContinue;
	}
	
CDispatchMessageBaseAO* CReplayModel::FactoryCreateDispatcherAOL(CRecordMessage::TMessageType aType, TInt aMessageId, CRecordMessage* aMessage,  MModelCallback& aObserver, MSessionManager& aSessionManager)
	{
	CDispatchMessageBaseAO* dispatcher = NULL;
	switch(aType)
		{
	case CRecordMessage::EConnectSession:
		dispatcher = CDispatchMessageSessionConnectAO::NewL(iConsole, aMessageId, aMessage, aObserver, aSessionManager);
		break;
	case CRecordMessage::EOpenSubSession:
		dispatcher = CDispatchMessageSubSessionOpenAO::NewL(iConsole, aMessageId, aMessage, aObserver, aSessionManager);
		break;
	case CRecordMessage::ESessionIpc:
		dispatcher = CDispatchMessageSessionIpcAO::NewL(iConsole, aMessageId, aMessage, aObserver, aSessionManager);
		break;
	case CRecordMessage::ESubSessionIpc:
		dispatcher = CDispatchMessageSubSessionIpcAO::NewL(iConsole, aMessageId, aMessage, aObserver, aSessionManager);
		break;
	case CRecordMessage::ECloseSession:
		dispatcher = CDispatchMessageSessionCloseAO::NewL(iConsole, aMessageId, aMessage, aObserver, aSessionManager);	
		break;
	case CRecordMessage::ECloseSubSession:
		dispatcher = CDispatchMessageSubSessionCloseAO::NewL(iConsole, aMessageId, aMessage, aObserver, aSessionManager);	
		break;
	case CRecordMessage::EMessageComplete:
		//do nothing here as message tye only used in preview mode, not needed for dispatching
		break;
	default:
		LOGLINE1(_L("Unrecognised message type."));
		Panic(ReplaySession::EUnknownMessageType);	
		};
		//add to the queue of active dispatchers
	if(dispatcher) //dispatcher can be null.
		iActiveRequests.AppendL(dispatcher);
	return dispatcher;
	}
	
TInt CReplayModel::GetSessionFromSessionId(CRecordMessage::TSession aId, RReplaySession*& aSessionHandle)
	{
	TInt ret = KErrNotFound;
	for(TInt i = 0; i<iLiveSessions.Count();i++)
		{
		if(iLiveSessions[i].Id() == aId)
			{
			//found Id
			aSessionHandle = &iLiveSessions[i];
			LOGLINE3(_L("Found session (ID=0x%x) at index = %d"), aSessionHandle->Id(),i);
			ret = KErrNone;
			break;
			}
		}
	return ret;
	}
	
	
TInt CReplayModel::GetSubSessionFromSubSessionHandle(CRecordMessage::TSession aSessionId, CRecordMessage::TSubSession aSubsessionId,CRecordMessage::TSubSessionHandle aHandle, RReplaySubSession*& aSessionHandle)
	{
	/*to find a subsession we have the 
	TSubsession iId = location of CTelObject at point log wass created
	TSubsessionHandle iHandleId = client side handle allocated to the subsession (this is what gets passed as iMessage.Int3())
	RReplaySession iSession - the link to the parent session of this subsession
	
	It is possible that two subsessions may have the same iId and iHandleId but have different parents (CSession) so when searching for a 
	subsession we need match on all three identifiers.
	
	*/
	TInt ret = KErrNotFound;
	for(TInt i = 0; i<iLiveSubSessions.Count();i++)
		{
		if(iLiveSubSessions[i].HandleId() == aHandle && iLiveSubSessions[i].Id() == aSubsessionId && iLiveSubSessions[i].SessionId() == aSessionId)
			{
			//found Id
			aSessionHandle = &iLiveSubSessions[i];
			LOGLINE5(_L("Found subsession (ID=0x%x, HandleId=%d, sessionId=0x%x) at index = %d"), aSessionHandle->Id(), aSessionHandle->HandleId(), aSessionId, i);
			ret = KErrNone;
			break;
			}
		}
	return ret;
	}
	
RReplaySession& CReplayModel::GetNewSessionL(CRecordMessage::TSession aId)
	{
	LOGLINE3(_L("Creating new session with ID = 0x%x at index = %d"), aId, iLiveSessions.Count());
	RReplaySession sess(aId);
	iLiveSessions.AppendL(sess);
	return iLiveSessions[iLiveSessions.Count()-1];//get last one we just added
	}
	
RReplaySubSession& CReplayModel::GetNewSubSessionL(CRecordMessage::TSubSession aId, CRecordMessage::TSubSessionHandle aHandleId, const RReplaySession& aSession)
	{
	LOGLINE4(_L("Creating new subsession with ID = 0x%x, HandleId=%d at index = %d"), aId, aHandleId, iLiveSubSessions.Count());
	RReplaySubSession sess(aId, aHandleId, aSession);
	iLiveSubSessions.AppendL(sess);
	return iLiveSubSessions[iLiveSubSessions.Count()-1];//get last one we just added
	}
	
TInt CReplayModel::CloseAndRemoveSession(CRecordMessage::TSession aId)
	{
	TInt ret = KErrNotFound;
	for(TInt i = 0; i<iLiveSessions.Count();i++)
		{
		if(iLiveSessions[i].Id() == aId)
			{
			//remove all subsessions associated with this session from iLiveSubSessions.  There should not be any
			//but it could the client has closed its session without first calling close on all its subsession objects
			//we do not actually close them but just remove them - is this a memory leak????
			RemoveSubSessionsAssociatedWithSession(aId);
			//found Id
			iLiveSessions[i].Close();
			iLiveSessions.Remove(i);
			LOGLINE3(_L("Removed session at index = %d (count=%d)"), i, iLiveSessions.Count());
			ret = KErrNone;
			break;
			}
		}
	return ret;
	}
	
void CReplayModel::RemoveSubSessionsAssociatedWithSession(CRecordMessage::TSession aSessionId)
	{
	for(TInt i = iLiveSubSessions.Count()-1; i>=0;i--)
		{
		if(iLiveSubSessions[i].SessionId() == aSessionId)
			{
			//found Id
			iLiveSubSessions.Remove(i);
			LOGLINE1(_L("WARNING: Subsession associated with closed session found."));
			LOGLINE3(_L("Removed subsession at index = %d (count=%d)"), i, iLiveSubSessions.Count());
			}
		}
	
	}
	
TInt CReplayModel::CloseAndRemoveSubSession(CRecordMessage::TSession aSessionId, CRecordMessage::TSubSession aSubsessionId, CRecordMessage::TSubSessionHandle aHandle)
	{
	TInt ret = KErrNotFound;
	for(TInt i = 0; i<iLiveSubSessions.Count();i++)
		{
		if(iLiveSubSessions[i].HandleId() == aHandle && iLiveSubSessions[i].Id() == aSubsessionId && iLiveSubSessions[i].SessionId() == aSessionId)
			{
			//found Id
			iLiveSubSessions[i].Close();
			iLiveSubSessions.Remove(i);
			LOGLINE3(_L("Removed subsession at index = %d (count=%d)"), i, iLiveSubSessions.Count());
			ret = KErrNone;
			break;
			}
		}
	return ret;
	}

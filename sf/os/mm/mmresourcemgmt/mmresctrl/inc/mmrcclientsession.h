// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef MMRCCLIENTSESSION_H__
#define MMRCCLIENTSESSION_H__

#include "mmrcclientserver.h"

#include <e32std.h>
#include <e32base.h>
#include <a3f/a3fbase.h>
#include "mmrcclientserver.h"

class MLogicalChain;
class MAudioContext;
class MMultimediaResourceControlObserver;
class CMMRCClientSideServerRequestToAdaptationAO;
class CAudioContext;

NONSHARABLE_CLASS(RMMRCClientSession) : public RSessionBase
	{
public:
	RMMRCClientSession();
	RMMRCClientSession::~RMMRCClientSession();

	TInt StartServer();
	TInt Open(MMultimediaResourceControlObserver& aCallback);
	TUint64 LogOn(TProcessId aProcessId);
	void Close();

	void InitializeServerL(RHandleBase& aMsgQueueHandle);
	TInt SendResourceRequest(MLogicalChain* aCurrent, MLogicalChain* aDesired, CAudioContext* aContext);
	void ResourceRequestResponse(MLogicalChain* aChainResponse, TMMRCServerToClientMessageResults aResult, TInt aError);
	
	//for notification
	TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData);
	TInt CancelRegisterAsClient(TUid aEventType);
	TInt WillResumePlay();

	void ResetMessages();
private:
	//void SendRequestResourceDataL();

private:
	RThread iServerThread;
	TRequestStatus *iCallingStatus;

	//owned
	MLogicalChain* iLogicalChainLastCommited;
	MLogicalChain* iLogicalChainRequested;
	CMMRCClientSideServerRequestToAdaptationAO* iMMRCClientSideServerRequestToAdaptationAO;

	//not owned
	MMultimediaResourceControlObserver* iObserver;

	TInt iCurrentRequestId;
	};

#endif //__MMRCCLIENTSESSION_H__

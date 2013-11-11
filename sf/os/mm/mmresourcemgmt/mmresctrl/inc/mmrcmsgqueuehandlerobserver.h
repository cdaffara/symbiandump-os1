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
//


#ifndef MMRCMSGQUEUEHANDLEROBSERVER_H
#define MMRCMSGQUEUEHANDLEROBSERVER_H
	
class MMMRCMsgQueueHandlerObserver
	{
public:
	virtual void HandleMessage(TMMRCQueueItem& aMessage) = 0;
	};

#endif //__MMRCMSGQUEUEHANDLEROBSERVER_H

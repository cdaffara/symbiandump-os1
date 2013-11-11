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
//
 
#include "PsdDialogServer.h"

CDialogServer::CDialogServer() : CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

CDialogServer::~CDialogServer()
	{
	}

TBool CDialogServer::Open()
	{
	TInt err = iDlgSvr.Connect();
	if (err != KErrNone)
		return EFalse;
	
	return ETrue;
	}

void CDialogServer::Close()
	{
	Cancel();
	iDlgSvr.Close();
	}

void CDialogServer::RequestDialogAppearanceNotification(MObserver* aObserver)
	{
	iObserver = aObserver;
	iDlgSvr.RequestDialogAppearanceNotification(iStatus);
	SetActive();
	}

void CDialogServer::SetReconnectResponse(TBool aResponse)
	{
	iDlgSvr.SetReconnectDialogResponse(aResponse);
	}

void CDialogServer::SetQoSWarnResponse(TBool aResponse)
	{
	iDlgSvr.SetQoSWarnDialogResponse(aResponse);
	}

void CDialogServer::RunL()
	{
	iObserver->DialogHasAppeared();
	}

void CDialogServer::DoCancel()
	{
	iDlgSvr.CancelDialogAppearanceNotification();
	}

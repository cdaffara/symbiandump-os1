// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tredrawhandler.h"
#include "twindows.h"
#include <w32std.h>

CTRedrawHandler::CTRedrawHandler(RWsSession& aWs) : CActive(EPriorityStandard),
		iWs(aWs)
	{
	}
	
CTRedrawHandler::~CTRedrawHandler()
	{
	Cancel();
	}

void CTRedrawHandler::Start()
	{
	CActiveScheduler::Add(this);
	iWs.RedrawReady(&iStatus);
	SetActive();
	}

void CTRedrawHandler::RunL()
	{
	TWsRedrawEvent redraw;
	iWs.GetRedraw(redraw);
	
	if (redraw.Handle())
		{
		if(redraw.Rect().IsEmpty())
			{
			User::Leave(KErrGeneral);
			}
		CTWindowTreeNode* node = reinterpret_cast<CTWindowTreeNode *>(redraw.Handle());
		node->Redraw(redraw);
		}
		
	iWs.RedrawReady(&iStatus);
	SetActive();
	}

void CTRedrawHandler::DoCancel()
	{
	iWs.RedrawReadyCancel();
	}

TInt CTRedrawHandler::RunError(TInt aError)
	{
	return(aError);
	}


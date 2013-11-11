// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include "captestinfoserver.h"
#include "captestinfosession.h"

RCapTestInfoSession::RCapTestInfoSession()
	{
	}

// Create the server, if one with this name does not already exist.
TInt RCapTestInfoSession::Connect()
	{
	TFindServer findCountServer(KInfoServerName);
	TFullName name;
	TInt err = KErrNone;
	
	 // Need to check that the server exists.
    if (findCountServer.Next(name)==KErrNone)
    	{
    	err = CreateSession(KInfoServerName, TVersion());
    	}
    else
    	{
    	err = KErrNotReady;
    	}
    
	return err;
    }

void RCapTestInfoSession::Close()
	{
	RSessionBase::Close();
	}

TThreadId RCapTestInfoSession::InfoServerTidL()
	{
	TPckgBuf<TThreadId> tid;
	TIpcArgs args(&tid);
    User::LeaveIfError(SendReceive(ECapTestInfoThreadId, args));
    return tid();
	}

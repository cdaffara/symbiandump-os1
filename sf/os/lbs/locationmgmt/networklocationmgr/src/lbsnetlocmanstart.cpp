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

#include <e32std.h>
#include <e32base.h>
#include "LbsInternalInterface.h"

#include "lbsnetlocmanlogic.h"
#include "cprocesslaunch.h"
#include "csecureprocessasbase.h"


CBase* CSecureProcessASBase::CreateRootObjectL()
	{
	CNetLocManLogic* server = CNetLocManLogic::NewL();
	TVersion version;
	version = server->Version();
	return server;
	}

TInt CSecureProcessASBase::EntryPointL(CBase* /*aBaseObj*/)
	{
	// EntryPointL not used by manager
	return KErrNone;
	}
	
// must be static	
void CSecureProcessASBase::Final()
	{
	}


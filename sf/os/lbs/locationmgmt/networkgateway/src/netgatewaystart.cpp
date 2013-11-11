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
// Global entry point for the LBS Network Gateway executable
// 
//

#include <e32base.h>
#include "csecureprocessasbase.h"
#include "netgateway.h"

CBase* CSecureProcessASBase::CreateRootObjectL()
	{	
	CNetworkGateway* server = CNetworkGateway::NewL();
	return server;
	}

TInt CSecureProcessASBase::EntryPointL(CBase* /*aBaseObj*/)
	{
	// EntryPointL not used by CNetGateway
	return KErrNone;
	}

void CSecureProcessASBase::Final()
	{
	}

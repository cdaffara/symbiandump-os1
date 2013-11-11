// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "initapparcserver2.h"
#include "startuputilprovider.h"


//
// Standard Symbian factory functions/destructor
//

CInitAppArcServer* CInitAppArcServer::NewL(CStartupUtilProvider& aProvider)
	{
	CInitAppArcServer* self = NewLC(aProvider);
	CleanupStack::Pop(self);
	return self;
	}

CInitAppArcServer* CInitAppArcServer::NewLC(CStartupUtilProvider& aProvider)
	{
	CInitAppArcServer* self = new (ELeave) CInitAppArcServer(aProvider);	
	CleanupStack::PushL(self);
	return self;
	}

CInitAppArcServer::CInitAppArcServer(CStartupUtilProvider& aProvider)
:iProvider(aProvider) 
	{
	}

//
// Public member functions
//


/** Implementation of MStartupCommand interface.

@see MStartupCommand.
*/
void CInitAppArcServer::Execute(TRequestStatus& aStatus)
	{
	iProvider.StartSafe().InitAppArcServer(aStatus);
	}

void CInitAppArcServer::Release() 
	{
	delete this;
	}

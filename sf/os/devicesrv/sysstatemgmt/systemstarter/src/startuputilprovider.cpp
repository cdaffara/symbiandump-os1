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

#include "startuputilprovider.h"

CStartupUtilProvider* CStartupUtilProvider::NewL()
	{
	CStartupUtilProvider* provider = new(ELeave) CStartupUtilProvider();
	CleanupStack::PushL(provider);

	provider->ConstructL();

	CleanupStack::Pop(provider);	
	return provider;	
	}

		

CStartSafe& CStartupUtilProvider::StartSafe()
	{
	return *iStartSafe; // returns CStartSafe object
	}


CStartupUtilProvider::~CStartupUtilProvider()
	{
	iSysMonSession.Close();
	delete iStartSafe;	
	}
		
CStartupUtilProvider::CStartupUtilProvider()
	{

	}

RSysMonSession& CStartupUtilProvider::SysMonSessionL()
	{
	if (iSysMonSession.Handle() == NULL)
		{	
		iSysMonSession.OpenL();		// if not already connected then connect 
		}
	return iSysMonSession;
	}

void CStartupUtilProvider::ConstructL()
	{
	iStartSafe = CStartSafe::NewL(); // Creates CStartSafe object.
	}

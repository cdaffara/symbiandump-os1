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
// Responder.cpp - all the active responder code
// 
//

#include <cs_port.h>
#include "btcomm.h"

#ifdef __GUFF__

CBtConnectResponder *CBtConnectResponder::NewL(CBTPort *aParent)
	{
	CBtConnectResponder *cr;
	cr=new (ELeave) CBtConnectResponder();
	CleanupStack::PushL(cr);
	cr->InitL(aParent);
	CleanupStack::Pop();
	return cr;
	}

void CBtConnectResponder::InitL(CBTPort *aParent)
	{
	iPort=aParent;
	}

CBtConnectResponder::~CBtConnectResponder()
	{
	}

void CBtConnectResponder::RunL()
// active status cleared.
	{
	}

void CBtConnectResponder::WaitForConnect()
	{
	// wait on something using iStatus
	SetActive();
	}

void CBtConnectResponder::DoCancel()
	{
	}

CBtConnectResponder::CBtConnectResponder() 
	: CActive(EPriorityStandard)
// private c'tor
	{
	}

#endif

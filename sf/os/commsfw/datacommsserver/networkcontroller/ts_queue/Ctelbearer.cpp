// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CTelBearer.h"

CTelBearer* CTelBearer::NewL(MBearerObserver* aObserver)
	{
	CTelBearer* self = CTelBearer::NewLC(aObserver);
	CleanupStack::Pop(); // self
	return self;
	}

CTelBearer* CTelBearer::NewLC(MBearerObserver* aObserver)
	{
	CTelBearer* self = new(ELeave) CTelBearer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTelBearer::CTelBearer(MBearerObserver* aObserver)
:CBearerBase(aObserver)
	{
	}

void CTelBearer::ConstructL()
	{
	}

void CTelBearer::AssignSupportedBearerSet()
	{
	}

CTelBearer::~CTelBearer()
	{
	}

TBool CTelBearer::StartChecking()
	{

	return ETrue;
	}

TBool CTelBearer::StartChecking(TBool)
	{

	return ETrue;
	}

TBool CTelBearer::StopChecking()
	{

	return ETrue;
	}

void CTelBearer::Disconnect()
	{
	}

TInt CTelBearer::SecondPhaseAvailability()
	{

	return KErrNone;
	}



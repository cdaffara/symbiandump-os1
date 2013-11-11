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

#include "ReconnectTests.h"

//
//  Test case NC-3-1
//

CNetConTest0301* CNetConTest0301::NewLC()
	{
	CNetConTest0301* self = new(ELeave) CNetConTest0301();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0301::CNetConTest0301()
	{
	
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	}

const TDesC& CNetConTest0301::Name() const
	{

	_LIT(KTestName, "Test Case NC-3-1");
	return KTestName();
	}


//
//  Test case NC-3-2
//

CNetConTest0302* CNetConTest0302::NewLC()
	{
	CNetConTest0302* self = new(ELeave) CNetConTest0302();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0302::CNetConTest0302()
	{
	
	iAvailableBearers = KCommDbBearerUnknown;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrCancel;
	}

const TDesC& CNetConTest0302::Name() const
	{

	_LIT(KTestName, "Test Case NC-3-2");
	return KTestName();
	}


//
//  Test case NC-3-3
//

CNetConTest0303* CNetConTest0303::NewLC()
	{
	CNetConTest0303* self = new(ELeave) CNetConTest0303();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0303::CNetConTest0303()
	{
	
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxError = KErrNone;
	iExpectedError = KErrNone;
	}

const TDesC& CNetConTest0303::Name() const
	{

	_LIT(KTestName, "Test Case NC-3-3");
	return KTestName();
	}


//
//  Test case NC-3-4
//

CNetConTest0304* CNetConTest0304::NewLC()
	{
	CNetConTest0304* self = new(ELeave) CNetConTest0304();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0304::CNetConTest0304()
	{
	
	iAvailableBearers = KCommDbBearerCSD;
	iDialogBoxError = KErrCancel;
	iExpectedError = KErrCancel;
	}

const TDesC& CNetConTest0304::Name() const
	{

	_LIT(KTestName, "Test Case NC-3-4");
	return KTestName();
	}


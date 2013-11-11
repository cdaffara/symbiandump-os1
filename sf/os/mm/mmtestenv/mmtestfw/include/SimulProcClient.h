// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTCLIENT__
#define __TESTCLIENT__

#include <e32base.h>
#include <testframework.h>

#define KMaxServerNameLength 256


class RTestServ : public RSessionBase
/**
@publishedPartner
@test
*/
	{
public:
	IMPORT_C TInt Connect(const TDesC& aServerName);
	IMPORT_C const TDesC& ServerName() const;
protected:
private:
	RLibrary iLibrary;
	TBuf<KMaxServerNameLength> iServerName;
	TVersion Version() const;
	};


class RTestSession : public RSubSessionBase
/**
@publishedPartner
@test
*/
	{
public:
	IMPORT_C TInt Open(RTestServ& aServ, const TDesC& aStepName, TBool aSharedData = EFalse);
	IMPORT_C void StartProcessing(TRequestStatus& aStatus);
	IMPORT_C TVerdict EndProcessingAndReturnResult(TDes8& aMessage);
	IMPORT_C void Close();
protected:
	};


#endif

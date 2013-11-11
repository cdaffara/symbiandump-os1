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

#ifndef LASROOT_H
#define LASROOT_H

#include <e32test.h>

//************************************************************************************************************
//   CLocationManagerRoot
//************************************************************************************************************
class CTestAppRoot : public CTimer//, MSuplPushObserver
	{
public:
	static CTestAppRoot* NewL(RTest& aTest, TDesC& aFrom, TDesC& aTo);
	~CTestAppRoot();
	

	void RunL();
	void DoCancel();
	
	void CompleteSelf(TInt aReason = KErrNone);

private:
	CTestAppRoot(RTest& aTest, TDesC& aFrom, TDesC& aTo);
	void ConstructL();


private:
	RTest& 	iTest;
	TDesC&  iFrom;
	TDesC&  iTo;
	};


#endif //LASROOT_H
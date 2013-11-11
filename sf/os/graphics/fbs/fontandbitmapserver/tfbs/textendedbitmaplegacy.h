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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef TEXTENDEDBITMAPLEGACY_H
#define TEXTENDEDBITMAPLEGACY_H

#include "tfbsbase.h"

//The main test class. Add test methods here.
class CTExtendedBitmapLegacy : public CTFbsBase
	{
public:
	CTExtendedBitmapLegacy(CTestStep* aStep);
	~CTExtendedBitmapLegacy();
protected:
	// From CTFbsBase
	virtual void RunFbsTestL(TInt aCurTestCase);

private:
	void TestDuplicateL();
	void TestAccessDeniedL();
	void TestNotSupportedL();
	void TestTwipsL();
	void TestGettersL();
	void TestGetScanLineL();
	void TestGetPixelL();
	void TestApisThatCallResetL();
	void TestApisThatLeaveL();
	void TestNotSupportedNoReturnL();
	};


class CTExtendedBitmapLegacyStep : public CTGraphicsStep
	{
public:
	CTExtendedBitmapLegacyStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();	
	virtual void TestSetupL();	
	virtual void TestClose();
	};

_LIT(KTExtendedBitmapLegacyStep,"TExtendedBitmapLegacy");

#endif // TEXTENDEDBITMAPLEGACY_H

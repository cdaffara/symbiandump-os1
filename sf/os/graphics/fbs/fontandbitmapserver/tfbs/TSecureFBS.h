// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSECUREFBS_H__
#define __TSECUREFBS_H__

#include <fbs.h>
#include "test/TGraphicsHarness.h"

class CTFbsSecure : public CTGraphicsBase
	{
public:
	CTFbsSecure(CTestStep* aStep);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);

	TInt FileSizeL(const TDesC& aFileName);
	void TestCase1();
	void TestCase2();
	void TestCase3();
	void TestSecureBitmapL();
	};


class CTFbsSecureStep : public CTGraphicsStep
	{
public:
	CTFbsSecureStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
private:
//This function is here to delete the bitmaps in C after finished with it
	void DeleteDataFile(const TDesC& aFullName);
	};

_LIT(KTFbsSecureStep,"TFbsSecure");



#endif

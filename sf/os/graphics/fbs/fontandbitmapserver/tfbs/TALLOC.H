// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TALLOC_H__
#define __TALLOC_H__

#include <fbs.h>
#include "test/TGraphicsHarness.h"

class CTAlloc : public CTGraphicsBase
	{
public:
	CTAlloc(CTestStep* aStep);
	~CTAlloc();

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	void ConstructL();
private:
	void Session();
	void BitmapsL();
	void BitmapHandlerL();
	void TypefaceStoreL();
	void FontStoreL();

private:
	RFbsSession* iFbs;
	CFbsTypefaceStore* iStore;
	CFbsBitmap *iBitmap;
	CFbsBitmap *iBitmap2;
	TFileName iTestBitmapName;
	TBool iShutdownFbs;
	};


class CTAllocStep : public CTGraphicsStep
	{
public:
	CTAllocStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTAllocStep,"TAlloc");
_LIT(KDefaultSectionName,"default");
_LIT(KFbsHeapMarkCheckFlag, "FbsHeapMarkCheckFlag");

#endif

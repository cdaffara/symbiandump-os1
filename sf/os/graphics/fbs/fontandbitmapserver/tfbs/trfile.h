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
// TRFILE.CPP
// 
//

#ifndef __TRFILE_H__
#define __TRFILE_H__

#include <fbs.h>
#include <bitstd.h>
#include "test/TGraphicsHarness.h"

class CTFile : public CTGraphicsBase
	{
public:
	CTFile(CTestStep* aStep);
	~CTFile();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();
private:
	void CreateL();
	void SaveL();
	void StoreL();
	void LoadL();
	void LoadAndCompressL();
	void Draw(CFbsBitGc* aGc,const TSize& aSize);
	void CleanUp();
	void BlitAndCompareL(CFbsBitmap* aBmpOrig,CFbsBitmap* aBmpTest,const TSize& aBmpSize);

private:
	RFbsSession* iFbs;
	RPointerArray<CFbsBitmap> iSource;
	RFs iFs;
	TBool iShutdownFbs;
	TBool iSkipRomBitmapTests;
	};


class CTFileStep : public CTGraphicsStep
	{
public:
	CTFileStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTFileStep,"TFile");

#endif

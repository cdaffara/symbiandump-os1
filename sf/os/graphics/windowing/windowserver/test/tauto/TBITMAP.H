
// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TBITMAP_H__
#define __TBITMAP_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

#define SAVE_BITMAP_NAME _L("C:\\WSTEST\\SAVE.PBM")

class CTBitmap : public CTWsGraphicsBase
	{
public:
	CTBitmap(CTestStep* aStep);
	~CTBitmap();
	void ConstructL();
	void BasicTestsL();
	void TestFixForPDEF098889L();
private:
	struct TThreadParams
		{
		TInt iScreenNr;
		void (*iFunction)(RWsSession& aSession, CWsScreenDevice& aDevice);
		};
	static TInt ThreadFunction(TAny* aParams);
	static void Function1PDEF098889L(RWsSession& aSession, CWsScreenDevice& aDevice);
	static void Function2PDEF098889L(RWsSession& aSession, CWsScreenDevice& aDevice); 
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	CWsBitmap *iBitmap1;
	CWsBitmap *iBitmap2;
	};

class CTBitmapStep : public CTGraphicsStep
	{
public:
	CTBitmapStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTBitmapStep,"TBitmap");


#endif

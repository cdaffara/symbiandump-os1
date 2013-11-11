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

#ifndef TEXTENDEDBITMAPPANIC_H
#define TEXTENDEDBITMAPPANIC_H

#include "TFBS.H"
#include "tfbsbase.h"
#include <gdi.h>

//The main test class. Add test methods here.
class CTExtendedBitmapPanic : public CTFbsBase
	{
public:
	CTExtendedBitmapPanic(CTestStep* aStep);
	~CTExtendedBitmapPanic();	
	
protected:
	// From CTFbsBase
	void RunFbsTestL(TInt aCurTestCase);
	void ConstructL();

private:
	void TestSetScanLine();		
	void TestIsMonochrome();
	
private: 
	CFbsBitmap* iExtendedBitmap;
	};


class CTExtendedBitmapPanicStep : public CTGraphicsStep
	{
public:
	CTExtendedBitmapPanicStep();
	
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();	
	};

_LIT(KTExtendedBitmapPanicStep,"TExtendedBitmapPanic");

#endif // TEXTENDEDBITMAPPANIC_H

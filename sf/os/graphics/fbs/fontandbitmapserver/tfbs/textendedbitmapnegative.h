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

#ifndef TEXTENDEDBITMAPNEGATIVE_H
#define TEXTENDEDBITMAPNEGATIVE_H

#include "tfbsbase.h"

/** Class containing negative tests for extended bitmaps. Negative tests include
passing bad parameters to methods used with extended bitmaps, and creating extended
bitmaps with no FbsSession.
 */
class CTExtendedBitmapNegative : public CTFbsBase
	{
public:
	CTExtendedBitmapNegative(CTestStep* aStep);
	~CTExtendedBitmapNegative();
	
protected:
	// From CTFbsBase
	virtual void RunFbsTestL(TInt aCurTestCase);

private:
	static TInt CreateWithoutFbsSessionStart(TAny* aInfo);

	// CreateExtendedBitmap() tests
	void CreateWithoutFbsSessionL();
	void CreateBadPixelSizeL();
	void CreateBadDisplayModeL();
	void CreateRestrictedUidL();
	void CreateNullDataL();
	void CreateBadDataSizeL();

	// ExtendedBitmapType() tests
	void TypeUninitialisedBitmapL();
	void TypeStandardBitmapL();
	void TypeHardwareBitmapL();

	// DataSize() tests
	void SizeUninitialisedBitmapL();
	};


class CTExtendedBitmapNegativeStep : public CTGraphicsStep
	{
public:
	CTExtendedBitmapNegativeStep();
	
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();	
	};

_LIT(KTExtendedBitmapNegativeStep,"TExtendedBitmapNegative");

#endif // TEXTENDEDBITMAPNEGATIVE_H

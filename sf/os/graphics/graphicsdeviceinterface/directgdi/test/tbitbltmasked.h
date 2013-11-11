// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test BitBltMasked() function
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TBITBLTMASKED_H__
#define __TBITBLTMASKED_H__

#include "tbitbltmaskedbase.h"


/**
BitBltMasked() test class.
*/
class CTBitBltMasked : public CTBitBltMaskedBase
	{
public:
	CTBitBltMasked();
	~CTBitBltMasked();

private:
	void TestBasicL();
	void TestPositioningL();
	void TestInvalidParametersL();
	void TestSourceBitmapCloningL();
	void TestMaskCloningL();
	void TestDrawModeL();
	void TestWithSameBitmapsL();
	void TestSetOriginL();
	
	// from CTDirectGdiStepBase
	void RunTestsL();
	};
	
_LIT(KTDirectGdiBitBltMaskedStep, "TDirectGDIBitBltMasked");

#endif

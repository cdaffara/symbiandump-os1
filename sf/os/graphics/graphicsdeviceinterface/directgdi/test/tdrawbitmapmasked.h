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
 @test DrawBitmapMasked() function
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TDRAWBITMAPMASKED_H__
#define __TDRAWBITMAPMASKED_H__

#include "tbitbltmaskedbase.h"

/**
DrawBitmapMasked() test class.
*/
class CTDrawBitmapMasked : public CTBitBltMaskedBase
	{
public:
	CTDrawBitmapMasked();
	~CTDrawBitmapMasked();

private:
	void TestBasicL();
	void TestPositioningL();
	void TestInvalidParametersL();
	void TestSourceBitmapCloningL();	
	void TestScalingL();
	void TestMaskCloningL();
	void TestDrawModeL();
	void TestWithSameBitmapsL();
	void TestSetOriginL();
	
	// from CTDirectGdiStepBase
	void RunTestsL();
	};
	
_LIT(KTDirectGdiDrawBitmapMaskedStep, "TDirectGDIDrawBitmapMasked");

#endif

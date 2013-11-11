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
 @test BitBlt() function
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TBITBLT_H__
#define __TBITBLT_H__

#include "tbitbltbase.h"

/**
BitBlt() test class.
*/
class CTBitBlt : public CTBitBltBase
	{
public:
	CTBitBlt();
	~CTBitBlt();

private:
	void TestBasicL();
	void TestPositioningL();
	void TestInvalidParametersL();
	void TestSourceBitmapCloningL();
	void TestDrawModeL();
	void TestSetOriginL();

	// from CTDirectGdiStepBase
	void RunTestsL();
	
private:
	TBool iUseLargeTarget;
	};
	
_LIT(KTDirectGdiBitBltStep, "TDirectGDIBitBlt");

#endif

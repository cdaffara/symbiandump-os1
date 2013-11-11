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
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef TEXTENDEDBITMAP_H
#define TEXTENDEDBITMAP_H

#include "tdirectgdi_test_step_base.h"


/** Class for testing CDirectGdiContext APIs when used with extended bitmaps, that is
CFbsBitmap bitmaps that have been created using CFbsBitmap::CreateExtendedBitmap().
*/
class CTExtendedBitmap : public CTDirectGdiStepBase
	{
public:
	CTExtendedBitmap();
	~CTExtendedBitmap();
	
private:
	// Main test methods 
 	void TestErrorCodesL(); 	
 	
	// From CStepStep	
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();	
	
	// from CTDirectGdiStepBase
	void RunTestsL();
	
	};		
	
_LIT(KTDirectGdiExtendedBitmapStep,"TDirectGDIExtendedBitmap");

#endif // TEXTENDEDBITMAP_H

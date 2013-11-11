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

#ifndef __TDIRECTGDITESTBASE_H__
#define __TDIRECTGDITESTBASE_H__

#include "te_graphicsperformanceSuiteStepBase.h"

#include <pixelformats.h>
#include <graphics/directgdicontext.h>
#include <graphics/directgdiimagetarget.h>
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <graphics/sgimage.h>
#endif
_LIT(KFontTypeface, "DejaVu Sans Condensed");
const TInt KFontSize = 14;

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTDirectGdiTestBase : public CTe_graphicsperformanceSuiteStepBase
	{
public:	
	CTDirectGdiTestBase();
	virtual ~CTDirectGdiTestBase();	
	
	virtual CFbsBitmap* GetTargetAsBitmapL();
		
protected:	
	// From CTe_graphicsperformanceSuiteStepBase
	virtual enum TVerdict doTestStepL() { return ETestSuiteError; }
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	virtual TVerdict doTestStepPreambleL(); 	
	virtual TVerdict doTestStepPostambleL();
#endif	
	TInt SetTargetPixelFormatL(TDisplayMode aDisplayMode);
	
	
private:	
	void InitializeDirectGdiL();
	void UninitializeDirectGdi();

protected:
	CDirectGdiContext* iContext;
	CDirectGdiDriver* iDGdiDriver;
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	RSgImage iSgImageTarget;
	TSgImageInfo	iImageInfo;	// image info for RSgImage
#endif
	RDirectGdiImageTarget* iDGdiImageTarget;
	CDirectGdiFont* iFont;
	CDirectGdiFont* iLargeFont;
	CFbsTypefaceStore* iTs;
	
	RArray<TUidPixelFormat> iTargetPixelFormatArray; // target pixel format(s) for testing
	RArray<TUidPixelFormat> iSourcePixelFormatArray; // source pixel format(s) for testing
	};
	
#endif

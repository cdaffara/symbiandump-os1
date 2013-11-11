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
 @internalComponent 
 @test
*/

#ifndef TDIRECTGDIRESOURCE_H
#define TDIRECTGDIRESOURCE_H


#include "tdirectgdi_test_step_base.h"
#include <bitmaptransforms.h> 

class RDirectGdiDrawableSource;

_LIT(KBitmapRotatorSemaphore, "CTBitmapRotator_Sempaphore");

/**
This class contains all the test cases related to DirectGDI DrawResource() APIs.

@internalComponent
@test
*/
class CTDirectGdiResource : public CTDirectGdiStepBase, public CActive
	{
public:
	CTDirectGdiResource();
	~CTDirectGdiResource();
	
protected:
	// From CActive
	void RunL();
	void DoCancel();
	
private:
	void TestDrawResource_ImageAsDrawableL();
	void TestDrawResource_PosL();
	void TestDrawResource_DestRectL();
	void TestDrawResource_DestRectSrcRectL();

	void TestDrawResource_NullHandleL();
	void TestDrawResource_InvalidParameterL();
	void TestDrawResource_NonZeroOriginL();

	void TestDrawTargetAsSourceL();
	void TestShareEGLImageBetweenSourcesL();	
	void TestDrawResourceSourceNotCreatedL();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	// from CTDirectGdiStepBase
	void RunTestsL();
	
	void CreateCommonResourceL();
	void DestroyCommonResource();
	void SaveOutput(const TDesC& aName);

	void DrawTestPattern1(const TPoint&);
	void DrawTestPattern1(const TRect&, const TDrawableSourceAndEquivRotatedBmps&, const TDrawableSourceAndEquivRotatedBmps&);
	void DrawTestPattern2(const TRect&, const TDrawableSourceAndEquivRotatedBmps&, const TDrawableSourceAndEquivRotatedBmps&);
	void DrawTestPattern1(const TRect&, const TRect* = NULL);
	void DrawTestPattern2(const TRect&, const TRect* = NULL);
	void DrawTestPattern3(const TRect&, const TRect* = NULL);

	void DoDrawTestL();
	TBool SourceResourceHasAlpha() const;
	TBool OneTimeTestEnabled() const;

private:
	TBool iUseWriteAlpha;
	TBool iEnableClipRegion;
	RRegion iClip;
	TDrawableSourceAndEquivRotatedBmps iImgSrc;
	TDrawableSourceAndEquivRotatedBmps iDwbSrc[2];
	TSize iImgSz;
	CBitmapRotator* iBitmapRotator;	
	
	enum TActiveState
		{
		EStartup,
		ERotatingBitmap90,
		ERotatingBitmap180,
		ERotatingBitmap270,
		EDone
		};
	
	TActiveState iCurrentState;
	};		
	
_LIT(KTDirectGdiResourceStep,"TDirectGDIResource");

#endif // TDIRECTGDIRESOURCE_H

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

#ifndef __TGC_H__
#define __TGC_H__

#include "AUTO.H"
#include "TGraphicsHarness.h"
#include <graphics/sgimage.h>
#include <graphics/sgimagecollection.h>

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
/*
 * The following classes are only required for the RemoteGc DrawText tests: .
 * CDrawTextInContextTest : is the Abstract base class
 * CDrawTextInContextTestPoint : Test DrawText(const TDesC&,const TTextParameters*,const TPoint&)
 * CDrawTextInContextTestBox : Test DrawText(const TDesC&,const TTextParameters*,const TRect&,TInt,TTextAlign,TInt)
 * CDrawTextInContextTestPointVertical : Test DrawTextVertical(const TDesC&,const TTextParameters*,const TPoint&)
 * CDrawTextInContextTestBoxVertical : Test DrawTextVertical(const TDesC&,const TTextParameters*,const TRect&,TInt,TTextAlign,TInt)
 */
class CDirectGdiGcWrapper;
class CRemoteGc;
class CCommandBuffer;
class CWSGraphicsRes;
class RDirectGdiImageTarget;

const TSize KBitmapSize(620, 240);
const TRect KBitmapRect(TPoint(0,0),TSize(620, 240));
const TRegionFix<1> KBitmapRegion(KBitmapRect);

//CDrawTextInContextTest
class CDrawTextInContextTest : public CBase
	{
public:
	void Test();
	TBool HasPassedTest();
protected:
	CDrawTextInContextTest();
	~CDrawTextInContextTest();
	void BaseConstructL();
	virtual void DoDrawTextBitGc() = 0;
	virtual void DoDrawTextRemoteGc() = 0;
protected:
	CFont *iFont;
	CGraphicsContext::TTextParameters iParam;
	TPtrC iText;
	
	//For reference bitmap
	CFbsBitmap* iRefBitmap;
	CFbsBitmapDevice* iRefDevice;
	CFbsBitGc* iRefBitGc;
	
	//For CDirectGdiGcWrapper
	RSgImage iWrapperImage;
	RDirectGdiImageTarget* iWrapperImageTarget;
	RSgImageCollection iWrapperImageCollection;
	CDirectGdiGcWrapper* iDirectGdiGcWrapper;
	
	
	//For CRemoteGc
	CRemoteGc* iRemoteGc;
	RWsGraphicMsgBuf iMsgBuf;
	CCommandBuffer* iCommandBuffer;
	CWSGraphicsRes* iWsGraphicRes;
	TPoint iOffset;
	
	//Test result
	TBool iHasPassedTest;
	};

//CDrawTextInContextTestPoint
class CDrawTextInContextTestPoint : public CDrawTextInContextTest
	{
public:
	static CDrawTextInContextTestPoint* NewL();
	~CDrawTextInContextTestPoint();
	void DoDrawTextBitGc();
	void DoDrawTextRemoteGc();
private:
	CDrawTextInContextTestPoint();
	void ConstructL();
private:
	TPoint iPosition;
	};

//CDrawTextInContextTestBox
class CDrawTextInContextTestBox : public CDrawTextInContextTest
	{
public:
	static CDrawTextInContextTestBox* NewL();
	~CDrawTextInContextTestBox();
	void DoDrawTextBitGc();
	void DoDrawTextRemoteGc();
private:
	CDrawTextInContextTestBox();
	void ConstructL();
private:
	TRect iClipFillRect;
	TInt iBaselineOffset;
	CGraphicsContext::TTextAlign iTTextAlign;
	};

//CDrawTextInContextTestPointVertical
class CDrawTextInContextTestPointVertical : public CDrawTextInContextTest
	{
public:
	static CDrawTextInContextTestPointVertical* NewL();
	~CDrawTextInContextTestPointVertical();
	void DoDrawTextBitGc();
	void DoDrawTextRemoteGc();
private:
	CDrawTextInContextTestPointVertical();
	void ConstructL();
private:
	TPoint iPosition;
	TBool iUp;
	};

//CDrawTextInContextTestBoxVertical
class CDrawTextInContextTestBoxVertical : public CDrawTextInContextTest
	{
public:
	static CDrawTextInContextTestBoxVertical* NewL();
	~CDrawTextInContextTestBoxVertical();
	void DoDrawTextBitGc();
	void DoDrawTextRemoteGc();
private:
	CDrawTextInContextTestBoxVertical();
	void ConstructL();
private:
	TRect iClipFillRect;
	TInt iBaselineOffset;
	TBool iUp;
	CGraphicsContext::TTextAlign iTTextAlign;
	};

#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA

class CTGc : public CTWsGraphicsBase
	{
public:
	CTGc(CTestStep* aStep);
	~CTGc();
	void ConstructL();
	void TestOutlineAndShadowL();
	void TestGcClipRectOrigin();
	void TestResetWithBackgroundColorL();
	void TestCommandBufferL();
	void TestEmptyCommandBufferL();
	void TestGcSetBrushPatternL();
	void TestGcDeleteBitmap1L();
	void TestGcDeleteBitmap2L();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	void TestCRemoteGcAndMWsGraphicsContextClippingRectL();
	void TestCRemoteGcDrawTextInContextPointL();
	void TestCRemoteGcDrawTextInContextBoxL();
	void TestCRemoteGcDrawTextInContextPointVerticalL();
	void TestCRemoteGcDrawTextInContextBoxVerticalL();
#endif

protected:
	void TestGcClipRectOrigin_DrawContent(TestWindow& aWindow, TBool bActivateBeforeRedraw = ETrue);
	
//from 	CTWsGraphicsBase
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	CTestBase *iTest;
	};

class CTGcStep : public CTGraphicsStep
	{
public:
	CTGcStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTGcStep,"TGc");

#endif

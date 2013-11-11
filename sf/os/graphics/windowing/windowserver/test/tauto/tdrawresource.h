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

#ifndef TDRAWRESOURCE_H_
#define TDRAWRESOURCE_H_

#include "AUTO.H"
#include <graphics/directgdidriver.h>
#include <graphics/wsgraphicscontext.h>
#include <graphics/directgdicontext.h>
#include <graphics/wsdrawablesourceprovider.h>
#include <graphics/sgimagecollection.h>
#include <graphics/directgdiimagetarget.h>
#include <graphics/wsdrawresource.h>
#include "RemoteGc.h"
#include "CommandBuffer.h"

class TSgImageInfo;

_LIT(KTDrawResourceStep, "TDrawResource");

const TSize KSourceSize(200,200); //The size of the rect to copy from the screen
const TPoint KDestPoint(0,0);
const TRect KCopyTestRect(TPoint(0,0), TSize(200,200)); //The test rect to copy from the screen
const TRect KSourceRect(TPoint(0,0), TSize(50,50));//Rect of the source image to be drawn
const TRect KDestRect(TPoint(10,10), TSize(60,60));//Rect of the destination to draw to
const CWindowGc::TGraphicsRotation KTestRotation= CWindowGc::EGraphicsRotation90;
const TPoint KPlayOffset(0,0);


/**
 * Dummy class used to check the values received by MWsGraphicsContext.
 * The only funtions overridden with some functionality are the DrawResource functions.
 * These simply receive the values and then store them in public member variables.
 */
class CTestWsGraphicsContext : public CBase, public MWsGraphicsContext, public MWsDrawableSourceProvider
	{
public:
	static CTestWsGraphicsContext* NewL(RDirectGdiImageTarget& aTarget);
	~CTestWsGraphicsContext();
	
public: //from MWsGraphicsContext
	void BitBlt(const TPoint&, const CFbsBitmap&){}
	void BitBlt(const TPoint&, const CFbsBitmap&, const TRect&){}
	void BitBltMasked(const TPoint&, const CFbsBitmap&, const TRect&, const CFbsBitmap&, TBool){}
	void BitBltMasked(const TPoint&, const CFbsBitmap&, const TRect&, const CFbsBitmap&, const TPoint&){}
	void ResetClippingRegion(){}
	void Clear();
	void Clear(const TRect&){}
	void ResetBrushPattern(){}
	void ResetFont(){}
	void DrawArc(const TRect&, const TPoint&, const TPoint&){}
	void DrawPie(const TRect&, const TPoint&, const TPoint&){}
	void DrawBitmap(const TRect&, const CFbsBitmap&){}
	void DrawBitmap(const TRect&,	const CFbsBitmap&, const TRect&){}
	void DrawBitmapMasked(const TRect&, const CFbsBitmap&,const TRect&, const CFbsBitmap&, TBool){}
	void DrawRoundRect(const TRect&, const TSize&){}
	void DrawPolyLine(const TArray<TPoint>&){}
	void DrawPolyLineNoEndPoint(const TArray<TPoint>&){}
	void DrawPolygon(const TArray<TPoint>& , TFillRule ){}
	void DrawEllipse(const TRect&){}
	void DrawLine(const TPoint&, const TPoint&){}
	void DrawLineTo(const TPoint&){}
	void DrawLineBy(const TPoint&){}
	void DrawRect(const TRect&){}
	void DrawText(const TDesC& ,const TTextParameters* ){}
	void DrawText(const TDesC& ,const TTextParameters* ,const TPoint& ){}
	void DrawText(const TDesC& ,const TTextParameters* ,const TRect& ){}
	void DrawText(const TDesC& ,const TTextParameters* ,const TRect& ,TInt ,TTextAlign ,TInt ){}
	void DrawTextVertical(const TDesC& ,const TTextParameters* ,TBool ) {}
	void DrawTextVertical(const TDesC& ,const TTextParameters* ,const TPoint& ,TBool ){}
	void DrawTextVertical(const TDesC& ,const TTextParameters* ,const TRect& ,TBool ){}
	void DrawTextVertical(const TDesC& ,const TTextParameters* ,const TRect& ,TInt ,TBool ,TTextAlign ,TInt ){}
	void DrawTextVertical(const TDesC& ,const TTextParameters* ,const TRect& ,TInt ,TInt ,TBool ,TTextAlign ,TInt ){}
	void MoveTo(const TPoint&){}
	void MoveBy(const TPoint&){}
	TPoint Origin() const {return TPoint(0,0);}
	void Plot(const TPoint&){}
	void Reset(){}
	void SetBrushColor(const TRgb&){}
	void SetBrushOrigin(const TPoint&){}
	void SetBrushStyle(TBrushStyle ){}
	void SetClippingRegion(const TRegion&){}
	void SetDrawMode(TDrawMode){}
	void SetOrigin(const TPoint&){}
	void SetPenColor(const TRgb&){}
	void SetPenStyle(TPenStyle){}
	void SetPenSize(const TSize& ){}
	void SetTextShadowColor(const TRgb&){}
	void SetCharJustification(TInt, TInt){}
	void SetWordJustification(TInt, TInt){}
	void SetUnderlineStyle(TFontUnderline){}
	void SetStrikethroughStyle(TFontStrikethrough){}
	void SetBrushPattern(const CFbsBitmap&){}
	void SetBrushPattern(TInt){}
	void SetFont(const CFont*){}
	void CopyRect(const TPoint&, const TRect&){}
	void RectDrawnTo(TRect&){}
	void UpdateJustification(const TDesC& ,const TTextParameters* ){}
	void UpdateJustificationVertical(const TDesC& ,const TTextParameters* ,TBool ){}
	void SetFontNoDuplicate(const CFont* ){}
	TBool HasBrushPattern() const{return EFalse;}
	TBool HasFont() const{return EFalse;}
	void InternalizeL(RReadStream&){}
	void ExternalizeL(RWriteStream&){}
	TRgb BrushColor() const {return iContext->BrushColor();}
	TRgb PenColor() const {return iContext->PenColor();}
	TRgb TextShadowColor() const {return iContext->TextShadowColor ();}
	
	const TRegion& ClippingRegion(){return iRegion;}
	TInt GetError(){return KErrNone;}
	TInt Push(){return KErrNone;}
	void Pop(){}
	//from MWsObjectProvider
	TAny* ResolveObjectInterface(TUint aTypeId);
	
	//from MWsDrawableSourceProvider
	TInt CreateDrawableSource(const TSgDrawableId& aDrawableId, TAny*& aSource);
	void CloseDrawableSource(TAny* aSource);
	void DrawResource(const TAny* aSource, const TPoint& aPos, CWindowGc::TGraphicsRotation aRotation = CWindowGc::EGraphicsRotationNone);
	void DrawResource(const TAny* aSource, const TRect& aRect, CWindowGc::TGraphicsRotation aRotation = CWindowGc::EGraphicsRotationNone);
	void DrawResource(const TAny* aSource, const TRect& aRectDest, const TRect& aRectSrc, CWindowGc::TGraphicsRotation aRotation = CWindowGc::EGraphicsRotationNone);
	void DrawResource(const TAny* aSource, const TRect& aRect, const TDesC8& aParam);
	
private:
	void ConstructL(RDirectGdiImageTarget& aTarget);
	void DoCreateDrawableSourceL(const TSgDrawableId& aDrawableId, TAny*& aSource);
public:
	TPoint iPos;
	TRect iDestRect;
	TRect iSrcRect;
	DirectGdi::TGraphicsRotation iRotation;
	RRegion iRegion;
private:
	CDirectGdiContext* iContext;
	};

/**
 * Class derived from MWsGraphicResolver.  This a dummy implmentation which has no functionality.
 * An instance of it is passed to CCommandBuffer::Play to make it a valid call.  
 */
class CWSGraphicsRes: public CBase, public MWsGraphicResolver
	{
public:
	void DrawWsGraphic(TInt, TBool, const TRect&, const TDesC8&) const{}
	};
	
class CTDrawResource : public CTWsGraphicsBase
	{
public:
	CTDrawResource(CTestStep* aStep);
	~CTDrawResource();
	void ConstructL();
protected:
	//from 	CTGraphicsStep
	void RunTestCaseL(TInt aCurTestCase);
private:
	void TestDrawResourcePos();
	void TestDrawResourceRect();
	void TestDrawResourceScale();
	void TestRWsDrawableSourceReferenceCountingL();
	void TestRemoteGcDrawResourcePosL();
	void TestRemoteGcDrawResourceRectL();
	void TestRemoteGcDrawResourceScaleL();
	void TestDrawResourceScreensL();
	void TestCopyScreenToBitmapWithDifferentDisplayModesL();
	//Helper functions
	TBool CompareBitmapsByPixel(CFbsBitmap* aCandidateBitmap, CFbsBitmap* aReferenceBitmap);
	void CreateReferenceAndCopyBitmapsL();
	void CreateSgImageFromBitmapL(RSgImage& aImage);
	void BitmapLC(CFbsBitmap*& aBmp); //helper function to extract bitmap from the image
	void CreateReferenceBitmapLC(CFbsBitmap*& aBmpTarget, CFbsBitmap* aBmpSrc, TDisplayMode aDestMode); 
	void CreateBitmapLC(CFbsBitmap*& aBmpTarget, const TSize& aSize, TDisplayMode aDispMode) const;
private:
	RWindow iWindow;
	CFbsBitmap* iRefBitmap;
	CFbsBitmap* iRotatedRefBitmap;
	CFbsBitmap* iScaledBitmap;
	CFbsBitmap *iScaledCroppedBitmap;
	CFbsBitmap* iCopyBitmap;
	CFbsBitmap* iBitmapWrongScreenNumber;
	CDirectGdiDriver* iDGdiDriver;
	
	//Required for RemoteGc testing
	RSgImageCollection iImageCollection;
	RSgImage iImage;
	RDirectGdiImageTarget iImageTarget;
	RWsDrawableSource iWsDrawableSource;
	CTestWsGraphicsContext* iGraphicsCon;
	CWSGraphicsRes* iWsGrapicResolver;
	};

class CTDrawResourceStep : public CTGraphicsStep
	{
public:
	CTDrawResourceStep();
protected:	
	//from CTGraphicsStep
	CTGraphicsBase* CreateTestL();
	};


#endif /*TDRAWRESOURCE_H_*/

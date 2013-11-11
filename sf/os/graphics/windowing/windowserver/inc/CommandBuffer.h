// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __COMMANDBUFFER_H__
#define __COMMANDBUFFER_H__

#include <w32std.h>
#include <s32mem.h>
#include <graphics/remotegcutils.h>

class CBitmapCache;
class CFontsCache;
class CDrawableCacheBase;
class CDrawSection;
class MWsGraphicsContext;

/**
MWsGraphicResolver should be implemented by CWsGraphicDrawers that want to be able to play DrawWsGraphic drawcommands.
@publishedPartner
@released*/
class MWsGraphicResolver
	{
public:
	/**
	This method is called when a DrawWsGraphics drawcommand is played by the commandbuffer. The CWsGraphicDrawer that
	implements this method needs to resolve the WsGraphics from the MWsGraphicDrawerEnvironment and then draw it.
	
	@param aId The id of the WsGraphic.
	@param aIsUid If ETrue the WsGraphic has a uid.
	@param aRect The rect to draw the WsGraphic in.
	@param aData Data passed along with the DrawWsGraphic command.
	*/
	virtual void DrawWsGraphic(TInt aId, TBool aIsUid, const TRect& aRect, const TDesC8& aData) const = 0;
	};

/**
The commandbuffer can record and play drawcommands.
It has an array of CDrawSections which contains information about every redraw that has been recorded.
@publishedPartner
@released*/
NONSHARABLE_CLASS(CCommandBuffer): public CBase
	{
public:
	IMPORT_C static CCommandBuffer* NewL();	
	IMPORT_C ~CCommandBuffer();	
	IMPORT_C void InternalizeL(const TDesC8& aBuf);
	IMPORT_C void InternalizeAppendL(const TDesC8& aBuf);
	IMPORT_C const TRegion& ClippingRegion() const;
	IMPORT_C TInt Play(const TPoint& aPosition, const TRect& aSourceRect, const MWsGraphicResolver& aWsGraphicResolver, CBitmapContext& aBitmapContext);
	IMPORT_C TInt Play(const TPoint& aMasterOrigin, const TRegion * aMasterClippingRegion, const TRect& aMasterClippingRect, const MWsGraphicResolver& aWsGraphicResolver, CBitmapContext& aContext);
	IMPORT_C TInt Play(const TPoint& aOffset, const TRegion* aClippingRegion, const TRect& aSourceRect, const MWsGraphicResolver& aWsGraphicResolver, MWsGraphicsContext& aGraphicsContext);
	IMPORT_C TInt Play(const TPoint& aOffset, const TRegion* aClippingRegion, const TRect& aSourceRect, RWsSession& aWsSession, CWindowGc& aWindowGc);

	IMPORT_C TBool IsIdentical(const CCommandBuffer& aBuffer) const;
	void ExternalizeL(RWsGraphicMsgBuf& aMsgBuf, TBool aEntireBuffer);
	void Reset();	
	void Prepare(const TRect& aDrawRect);
	TInt Finish(const TRect& aDrawRect, const TRect& aBoundingRect, TBool aHasBitmapCommand);	
	
	template<typename TClass> void Write(const TClass& aValue);	
	void WriteText(const TDesC16 &aText);
	void WriteText(const TDesC8 &aText);
	
private:
	void ConstructL();
	CCommandBuffer();
	
	// Help methods
	void Tidy();
	TBool AllSectionsExternalized() const; 
	TBool CheckForDuplicate(const CDrawSection& aDrawSection) const;
	void UpdateClippingRegion(TInt aDrawSectionIndex);
	// Reading writing
	void Write(const TUint8* aSrc, TUint aLength);	
	void DoWriteTextL(const TDesC8 &aText);	
	void ReadL(TUint8* aDest, TUint aLength);
	template<typename TClass> void ReadL(TClass& aValue);	
	void ReadTextLC(TPtrC8& aText);
	void ReadTextLC(TPtrC16& aText);
	void DoReadTextLC(TPtrC8& aText,TBool a16Bit);
	
	// Drawing the commands
#ifdef SYMBIAN_GRAPHICS_GCE
	template<typename ContextType> void DoPlayL(const MWsGraphicResolver& aWsGraphicResolver, ContextType& aGraphicsContext);
	template<typename ContextType> void DrawSectionL(const CDrawSection& aDrawSection, const MWsGraphicResolver& aWsGraphicResolver, ContextType& aGraphicsContext);
#else
	void DoPlayL(const MWsGraphicResolver& aWsGraphicResolver, CBitmapContext& aBitmapContext);
	void DrawSectionL(const CDrawSection& aDrawSection, const MWsGraphicResolver& aWsGraphicResolver, CBitmapContext& aBitmapContext);
#endif
#if defined(SYMBIAN_GRAPHICS_GCE)
	template<typename ContextType> void Clear(ContextType& aGraphicsContext) const;
	template<typename ContextType> void ClearRectL(ContextType& aGraphicsContext);
	template<typename ContextType> void CopyRectL(ContextType& aGraphicsContext);
	template<typename ContextType> void BitBlt1L(ContextType& aGraphicsContext);
	void DoBitBlt1L(CWindowGc& aWindowGc, TPoint aPoint, TInt aHandle);
	void DoBitBlt1L(MWsGraphicsContext& aGraphicsContext, TPoint aPoint, TInt aHandle);
	template<typename ContextType> void BitBlt2L(ContextType& aGraphicsContext);
	void DoBitBlt2L(CWindowGc& aWindowGc, TPoint aPoint, TInt aHandle, TRect aSourceRect);
	void DoBitBlt2L(MWsGraphicsContext& aGraphicsContext, TPoint aPoint, TInt aHandle, TRect aSourceRect);
	template<typename ContextType> void BitBltMaskedL(ContextType& aGraphicsContext);
	void DoBitBltMaskedL(CWindowGc& aWindowGc, TPoint aPoint, TInt aBitmapHandle, TRect aSourceRect, TInt aMaskHandle, TBool aInvertMask);
	void DoBitBltMaskedL(MWsGraphicsContext& aGraphicsContext, TPoint aPoint, TInt aBitmapHandle, TRect aSourceRect, TInt aMaskHandle, TBool aInvertMask);
	void SetFadedL(CWindowGc& aWindowGc);
	void SetFadingParametersL(CWindowGc& aWindowGc);
	template<typename ContextType> void AlphaBlendBitmapsL(ContextType& aGraphicsContext);
	void DoAlphaBlendBitmapsL(CWindowGc& aWindowGc, TPoint aDestPoint, TInt aSrcHandle, TRect aSourceRect, TInt aAlphaHandle, TPoint aAlphaPoint);
	void DoAlphaBlendBitmapsL(MWsGraphicsContext& aGraphicsContext, TPoint aDestPoint, TInt aSrcHandle, TRect aSourceRect, TInt aAlphaHandle, TPoint aAlphaPoint);
	template<typename ContextType> void SetOriginL(ContextType& aGraphicsContext);
	template<typename ContextType> void SetDrawModeL(ContextType& aGraphicsContext);
	void DoSetDrawModeL(CWindowGc& aWindowGc, CGraphicsContext::TDrawMode aDrawMode);
	void DoSetDrawModeL(MWsGraphicsContext& aGraphicsContext, CGraphicsContext::TDrawMode aDrawMode);
	template<typename ContextType> void SetClippingRectL(ContextType& aGraphicsContext);
	template<typename ContextType> void CancelClippingRect(ContextType& aGraphicsContext);
	void Reset(CWindowGc& aWindowGc);
	void Reset(MWsGraphicsContext& aGraphicsContext);
	template<typename ContextType> void UseFontL(ContextType& aGraphicsContext);
	void DoUseFontL(CWindowGc& aWindowGc, TInt aFontHandle);
	void DoUseFontL(MWsGraphicsContext& aGraphicsContext, TInt aFontHandle);
	void DiscardFont(CWindowGc& aWindowGc) const;
	void DiscardFont(MWsGraphicsContext& aGraphicsContext) const;
	template<typename ContextType> void SetUnderlineStyleL(ContextType& aGraphicsContext);
	void DoSetUnderlineStyleL(CWindowGc& aWindowGc, TFontUnderline aUnderlineStyle);
	void DoSetUnderlineStyleL(MWsGraphicsContext& aGraphicsContext, TFontUnderline aUnderlineStyle);
	template<typename ContextType> void SetStrikethroughStyleL(ContextType& aGraphicsContext);
	void DoSetStrikethroughStyleL(CWindowGc& aWindowGc, TFontStrikethrough aStrikethroughStyle);
	void DoSetStrikethroughStyleL(MWsGraphicsContext& aGraphicsContext, TFontStrikethrough aStrikethroughStyle);
	template<typename ContextType> void SetWordJustificationL(ContextType& aGraphicsContext);
	template<typename ContextType> void SetCharJustificationL(ContextType& aGraphicsContext);
	template<typename ContextType> void SetPenColorL(ContextType& aGraphicsContext);
	template<typename ContextType> void SetPenStyleL(ContextType& aGraphicsContext);
	void DoSetPenStyleL(CWindowGc& aWindowGc, CGraphicsContext::TPenStyle aPenStyle);
	void DoSetPenStyleL(MWsGraphicsContext& aGraphicsContext, CGraphicsContext::TPenStyle aPenStyle);
	template<typename ContextType> void SetPenSizeL(ContextType& aGraphicsContext);
	void DoSetPenSizeL(CWindowGc& aWindowGc, TSize aSize);
	void DoSetPenSizeL(MWsGraphicsContext& aGraphicsContext, TSize aSize);
	template<typename ContextType> void SetBrushColorL(ContextType& aGraphicsContext);
	template<typename ContextType> void SetBrushStyleL(ContextType& aGraphicsContext);
	void DoSetBrushStyleL(CWindowGc& aWindowGc, CGraphicsContext::TBrushStyle aBrushStyle);
	void DoSetBrushStyleL(MWsGraphicsContext& aGraphicsContext, CGraphicsContext::TBrushStyle aBrushStyle);
	template<typename ContextType> void SetBrushOriginL(ContextType& aGraphicsContext);
	template<typename ContextType> void UseBrushPatternL(ContextType& aGraphicsContext);
	void DoUseBrushPatternL(CWindowGc& aWindowGc, TInt aDeviceHandle);
	void DoUseBrushPatternL(MWsGraphicsContext& aGraphicsContext, TInt aDeviceHandle);
	void DiscardBrushPattern(CWindowGc& aWindowGc) const;
	void DiscardBrushPattern(MWsGraphicsContext& aGraphicsContext) const;
	template<typename ContextType> void MoveToL(ContextType& aGraphicsContext);
	template<typename ContextType> void MoveByL(ContextType& aGraphicsContext);
	template<typename ContextType> void PlotL(ContextType& aGraphicsContext);
	template<typename ContextType> void DrawArcL(ContextType& aGraphicsContext);
	template<typename ContextType> void DrawLineL(ContextType& aGraphicsContext);
	template<typename ContextType> void DrawLineToL(ContextType& aGraphicsContext);
	template<typename ContextType> void DrawLineByL(ContextType& aGraphicsContext);
	void DrawPolyLineL(CWindowGc& aWindowGc);
	void DrawPolyLineL(MWsGraphicsContext& aGraphicsContext);
	template<typename ContextType> void DrawPieL(ContextType& aGraphicsContext);
	template<typename ContextType> void DrawEllipseL(ContextType& aGraphicsContext);
	template<typename ContextType> void DrawRectL(ContextType& aGraphicsContext);
	template<typename ContextType> void DrawRoundRectL(ContextType& aGraphicsContext);
	void DrawPolygonL(CWindowGc& aWindowGc);
	void DrawPolygonL(MWsGraphicsContext& aGraphicsContext);
	void DrawBitmap1L(CWindowGc& aWindowGc);
	void DrawBitmap1L(MWsGraphicsContext& aGraphicsContext);
	template<typename ContextType> void DrawBitmap2L(ContextType& aGraphicsContext);
	void DoDrawBitmap2L(CWindowGc& aWindowGc, TRect aDestRect, TInt aBitmapHandle);
	void DoDrawBitmap2L(MWsGraphicsContext& aGraphicsContext, TRect aDestRect, TInt aBitmapHandle);
	template<typename ContextType> void DrawBitmap3L(ContextType& aGraphicsContext);
	void DoDrawBitmap3L(CWindowGc& aWindowGc, TRect aDestRect, TInt aBitmapHandle, TRect aSourceRect);
	void DoDrawBitmap3L(MWsGraphicsContext& aGraphicsContext, TRect aDestRect, TInt aBitmapHandle, TRect aSourceRect);
	template<typename ContextType> void DrawBitmapMaskedL(ContextType& aGraphicsContext);
	void DoDrawBitmapMaskedL(CWindowGc& aWindowGc, TRect aDestRect, TInt aBitmapHandle, TRect aSourceRect, TInt aMaskHandle, TBool aInvertedMask);
	void DoDrawBitmapMaskedL(MWsGraphicsContext& aGraphicsContext, TRect aDestRect, TInt aBitmapHandle, TRect aSourceRect, TInt aMaskHandle, TBool aInvertedMask);
	template<typename ContextType> void DrawText1L(ContextType& aGraphicsContext);
	void DoDrawText1L(CWindowGc& aWindowGc, TPtrC16 aText, TPoint aPoint);
	void DoDrawText1L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, TPoint aPoint);
	template<typename ContextType> void DrawText2L(ContextType& aGraphicsContext);
	void DoDrawText2L(CWindowGc& aWindowGc, TPtrC16 aText, TRect aBox, TInt aBaselineOffset, CGraphicsContext::TTextAlign aHoriz, TInt aLeftMargin);
	void DoDrawText2L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, TRect aBox, TInt aBaselineOffset, CGraphicsContext::TTextAlign aHoriz, TInt aLeftMargin);
	template<typename ContextType> void DrawText3L(ContextType& aGraphicsContext);
	void DoDrawText3L(CWindowGc& aWindowGc, TPtrC16 aText, TPoint aPoint);
	void DoDrawText3L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, TPoint aPoint);
	template<typename ContextType> void DrawText4L(ContextType& aGraphicsContext);
	void DoDrawText4L(CWindowGc& aWindowGc, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TPoint aPoint);
	void DoDrawText4L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TPoint aPoint);
	template<typename ContextType> void DrawText5L(ContextType& aGraphicsContext);
	void DoDrawText5L(CWindowGc& aWindowGc, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TRect aBox, TInt aBaselineOffset, CGraphicsContext::TTextAlign aHoriz, TInt aLeftMargin);
	void DoDrawText5L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TRect aBox, TInt aBaselineOffset, CGraphicsContext::TTextAlign aHoriz, TInt aLeftMargin);
	template<typename ContextType> void MapColorsL(ContextType& aGraphicsContext);
	template<typename ContextType> void SetClippingRegionL(ContextType& aGraphicsContext);
	template<typename ContextType> void CancelClippingRegion(ContextType& aGraphicsContext);
	void DoCancelClippingRegion(CWindowGc& aWindowGc);
	void DoCancelClippingRegion(MWsGraphicsContext& aGraphicsContext);
	template<typename ContextType> void DrawTextVertical1L(ContextType& aGraphicsContext);
	void DoDrawTextVertical1L(CWindowGc& aWindowGc, TPtrC16 aText, TPoint aPoint, TBool aUp);
	void DoDrawTextVertical1L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, TPoint aPoint, TBool aUp);
	template<typename ContextType> void DrawTextVertical2L(ContextType& aGraphicsContext);
	void DoDrawTextVertical2L(CWindowGc& aWindowGc, TPtrC16 aText, TRect aBox, TInt aBaselineOffset, TBool aUp, CGraphicsContext::TTextAlign aVertical, TInt aMargin);
	void DoDrawTextVertical2L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, TRect aBox, TInt aBaselineOffset, TBool aUp, CGraphicsContext::TTextAlign aVertical, TInt aMargin);
	template<typename ContextType> void DrawTextVertical3L(ContextType& aGraphicsContext);
	void DoDrawTextVertical3L(CWindowGc& aWindowGc, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TPoint aPoint, TBool aUp);
	void DoDrawTextVertical3L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TPoint aPoint, TBool aUp);
	template<typename ContextType> void DrawTextVertical4L(ContextType& aGraphicsContext);
	void DoDrawTextVertical4L(CWindowGc& aWindowGc, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TRect aBox, TInt aBaselineOffset, TBool aUp, CGraphicsContext::TTextAlign aVertical, TInt aMargin);
	void DoDrawTextVertical4L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TRect aBox, TInt aBaselineOffset, TBool aUp, CGraphicsContext::TTextAlign aVertical, TInt aMargin);
	void DrawWsGraphic1L(const MWsGraphicResolver& aWsGraphicResolver);
	void DrawWsGraphic2L(const MWsGraphicResolver& aWsGraphicResolver);
	template<typename ContextType> void SetShadowColorL(ContextType& aGraphicsContext);
	void DoSetShadowColorL(CWindowGc& aWindowGc, TRgb aShadowColor);
	void DoSetShadowColorL(MWsGraphicsContext& aGraphicsContext, TRgb aShadowColor);
	template<typename ContextType> void DrawResourceToPosL(ContextType& aGraphicsContext);
	template<typename ContextType> void DrawResourceToRectL(ContextType& aGraphicsContext);
	template<typename ContextType> void DrawResourceFromRectToRectL(ContextType& aGraphicsContext);
	template<typename ContextType> void DrawResourceWithDataL(ContextType& aGraphicsContext);
	void DoDrawResourceToPos(CWindowGc& aGraphicsContext, const TAny* aDrawableSource, const TPoint& aPos, CWindowGc::TGraphicsRotation aRotation);
	void DoDrawResourceToRect(CWindowGc& aGraphicsContext, const TAny* aDrawableSource, const TRect& aRect, CWindowGc::TGraphicsRotation aRotation);
	void DoDrawResourceFromRectToRect(CWindowGc& aGraphicsContext, const TAny* aDrawableSource, const TRect& aRectDest, const TRect& aRectSrc, CWindowGc::TGraphicsRotation aRotation);
	void DoDrawResourceWithData(CWindowGc& aGraphicsContext, const TAny* aDrawableSource, const TRect& aRect, const TDesC8& aParam);
	void DoDrawResourceToPos(MWsGraphicsContext& aGraphicsContext, const TAny* aDrawableSource, const TPoint& aPos, CWindowGc::TGraphicsRotation aRotation);
	void DoDrawResourceToRect(MWsGraphicsContext& aGraphicsContext, const TAny* aDrawableSource, const TRect& aRect, CWindowGc::TGraphicsRotation aRotation);
	void DoDrawResourceFromRectToRect(MWsGraphicsContext& aGraphicsContext, const TAny* aDrawableSource, const TRect& aRectDest, const TRect& aRectSrc, CWindowGc::TGraphicsRotation aRotation);
	void DoDrawResourceWithData(MWsGraphicsContext& aGraphicsContext, const TAny* aDrawableSource, const TRect& aRect, const TDesC8& aParam);
#else //SYMBIAN_GRAPHICS_GCE
	void Clear(CBitmapContext& aBitmapContext) const;
	void ClearRectL(CBitmapContext& aBitmapContext);	
	void CopyRectL(CBitmapContext& aBitmapContext);
	void BitBlt1L(CBitmapContext& aBitmapContext);
	void BitBlt2L(CBitmapContext& aBitmapContext);
	void BitBltMaskedL(CBitmapContext& aBitmapContext);
	void SetFadedL(CBitmapContext& aBitmapContext);
	void SetFadingParametersL(CBitmapContext& aBitmapContext);
	void AlphaBlendBitmapsL(CBitmapContext& aBitmapContext);
	void SetOriginL(CBitmapContext& aBitmapContext);
	void SetDrawModeL(CBitmapContext& aBitmapContext);
	void SetClippingRectL(CBitmapContext& aBitmapContext);
	void CancelClippingRect(CBitmapContext& aBitmapContext);
	void Reset(CBitmapContext& aBitmapContext);
	void UseFontL(CBitmapContext& aBitmapContext);
	void DiscardFont(CBitmapContext& aBitmapContext) const;
	void SetUnderlineStyleL(CBitmapContext& aBitmapContext);
	void SetStrikethroughStyleL(CBitmapContext& aBitmapContext);
	void SetWordJustificationL(CBitmapContext& aBitmapContext);	
	void SetCharJustificationL(CBitmapContext& aBitmapContext);
	void SetPenColorL(CBitmapContext& aBitmapContext);
	void SetPenStyleL(CBitmapContext& aBitmapContext);
	void SetPenSizeL(CBitmapContext& aBitmapContext);
	void SetBrushColorL(CBitmapContext& aBitmapContext);
	void SetBrushStyleL(CBitmapContext& aBitmapContext);
	void SetBrushOriginL(CBitmapContext& aBitmapContext);
	void UseBrushPatternL(CBitmapContext& aBitmapContext);
	void DiscardBrushPattern(CBitmapContext& aBitmapContext) const;
	void MoveToL(CBitmapContext& aBitmapContext);
	void MoveByL(CBitmapContext& aBitmapContext);
	void PlotL(CBitmapContext& aBitmapContext);
	void DrawArcL(CBitmapContext& aBitmapContext);
	void DrawLineL(CBitmapContext& aBitmapContext);
	void DrawLineToL(CBitmapContext& aBitmapContext);
	void DrawLineByL(CBitmapContext& aBitmapContext);
	void DrawPolyLineL(CBitmapContext& aBitmapContext);
	void DrawPieL(CBitmapContext& aBitmapContext);
	void DrawEllipseL(CBitmapContext& aBitmapContext);
	void DrawRectL(CBitmapContext& aBitmapContext);
	void DrawRoundRectL(CBitmapContext& aBitmapContext);
	void DrawPolygonL(CBitmapContext& aBitmapContext);
	void DrawBitmap1L(CBitmapContext& aBitmapContext);
	void DrawBitmap2L(CBitmapContext& aBitmapContext);
	void DrawBitmap3L(CBitmapContext& aBitmapContext);
	void DrawBitmapMaskedL(CBitmapContext& aBitmapContext);
	void DrawText1L(CBitmapContext& aBitmapContext);
	void DrawText2L(CBitmapContext& aBitmapContext);
	void DrawText3L(CBitmapContext& aBitmapContext);
	void MapColorsL(CBitmapContext& aBitmapContext);
	void SetClippingRegionL(CBitmapContext& aBitmapContext);
	void CancelClippingRegion(CBitmapContext& aBitmapContext);
	void DrawTextVertical1L(CBitmapContext& aBitmapContext);
	void DrawTextVertical2L(CBitmapContext& aBitmapContext);
	void DrawWsGraphic1L(const MWsGraphicResolver& aWsGraphicResolver);
	void DrawWsGraphic2L(const MWsGraphicResolver& aWsGraphicResolver);
	void SetShadowColorL(CBitmapContext& aBitmapContext);
#endif //SYMBIAN_GRAPHICS_GCE
private:
	// Values set by the system calling play
	TPoint iMasterOrigin;
	const TRegion * iMasterClippingRegion;
	const TRegion * iActiveMasterClippingRegion;
	
	RRegionBuf<10> iDrawSectionClippingRegion;
	TRect iMasterClippingRect;
	TRect iDrawSectionRect;
	
	// Values set by the draw commands executed during play
	TPoint iOrigin;
	RRegionBuf<10> iClippingRegion;
	TRect iClippingRect;
	TBool iParsedClippingRegionIsSet;
	
#ifndef SYMBIAN_GRAPHICS_GCE 
	TPoint iOffSet; //only used by nonnga in order to preserve compatibility with nga
	RRegionBuf<10> iCurrentSectionClippingRegion; //only used by nonnga in order to preserve compatibility with nga
#else
	RRegion iIntersectedRegion;
	CDrawableCacheBase* iDrawableCache;
#endif
	
	TInt iError;		
	CBufBase* iRecordSegBuf;
	CBitmapCache* iBitmapCache;
	CFontsCache* iFontCache;	
	CBufBase* iBufRead;
	RBufReadStream iBufReadStream;
	RPointerArray<CDrawSection> iDrawSections;
	};

template<typename TClass> void CCommandBuffer::Write(const TClass& aValue)
	{
	__ASSERT_COMPILE(sizeof(TClass) <= KMaxTUint8);
	
	#ifdef _DEBUG
		const TUint8 size = sizeof(TClass);
		Write(reinterpret_cast<const TUint8*>(&size), 1);
	#endif

	Write(reinterpret_cast<const TUint8*>(&aValue), sizeof(TClass));
	}
	
template<typename TClass> void CCommandBuffer::ReadL(TClass& aValue)
	{
	__ASSERT_COMPILE(sizeof(TClass) <= KMaxTUint8);	
	
	#ifdef _DEBUG
		TUint8 size = 0;
		ReadL(reinterpret_cast<TUint8*>(&size), 1);
		ASSERT(sizeof(TClass) == size);
	#endif

	ReadL(reinterpret_cast<TUint8*>(&aValue), sizeof(TClass));	
	}
	
#endif	// __COMMANDBUFFER_H__

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

#include "Graphics/WSGRAPHICMSGBUF.H"
#include "DrawSection.h"
#include "CommandBuffer.h"
#include "BitmapCache.h"
#include "FontsCache.h"

const TInt KBufferSize = 1024;

EXPORT_C CCommandBuffer* CCommandBuffer::NewL()
	{
	CCommandBuffer* buffer = new (ELeave) CCommandBuffer;
	CleanupStack::PushL(buffer);
	buffer->ConstructL();
	CleanupStack::Pop(buffer);
	return buffer;
	}

CCommandBuffer::CCommandBuffer()
	{
	}
	
EXPORT_C CCommandBuffer::~CCommandBuffer()
	{	
	iDrawSections.ResetAndDestroy();
	iDrawSections.Close();
	iBufReadStream.Close();
	iClippingRegion.Close();
	
	iBufRead = NULL;
	delete iRecordSegBuf;
	delete iBitmapCache;
	delete iFontCache;
	}
	
void CCommandBuffer::ConstructL()
	{
	iBitmapCache = new (ELeave)	CBitmapCache;
	iRecordSegBuf = CBufSeg::NewL(KBufferSize);
	iFontCache =  new (ELeave) CFontsCache;
	}

/**
Resets the entire commandbuffer.
*/
void CCommandBuffer::Reset()
	{
	if(iRecordSegBuf)
		iRecordSegBuf->Reset();
	
	iError =  KErrNone;	
	iOrigin = TPoint(0,0);
	iClippingRegion.Clear();
	iDrawSections.ResetAndDestroy();	
	}

/**
Externalizes commandbuffer sections into a format which makes it possible to send over IPC.
If ETrue is sent as a parameter to this method, the entire commandbuffer will be externalized,
otherwise only sections which has not been externalized before will be externalized. Note that if only
not externalized sections is asked for, the flag will be reset on that section so next call
to ExternalizeLC will not externalize that section.

@param aMsgBuf A buffer used to externalize the commandbuffer to.
@param aEntireBuffer If ETrue, the entire commandbuffer will be externalized, otherwise only sections which has not been externalized before.
*/
void CCommandBuffer::ExternalizeL(RWsGraphicMsgBuf& aMsgBuf, TBool aEntireBuffer)
	{
	// Add drawsections to aMsgBuf
	const TInt sectionCount = iDrawSections.Count();
	for(TInt j = 0; j < sectionCount; j++)	
		{
		CDrawSection* section = iDrawSections[j];
		if(aEntireBuffer || !section->HasBeenExternalized())
			{
			section->ExternalizeL(aMsgBuf);
			section->SetHasBeenExternalized(ETrue);
			}
		}
	}

/**
Internalizes the entire commandbuffer from buffer containing information about all drawsections and drawcommands.

@param aBuf A buffer containing information about all drawsections and drawcommands.
*/
EXPORT_C void CCommandBuffer::InternalizeL(const TDesC8& aBuf)
	{
	// Reset the commandbuffer
	iRecordSegBuf->Reset();
	InternalizeAppendL(aBuf);
	}

/**
Internalizes and appends from a buffer containing information about some drawsections and drawcommands.

@param aBuf A buffer containing information about some drawsections and drawcommands.
*/
EXPORT_C void CCommandBuffer::InternalizeAppendL(const TDesC8& aBuf)
	{
	// Reset the commandbuffer
	TWsGraphicMsgBufParser parser(aBuf);
	
	// Load drawsections
	const TInt count = parser.Count();
	for(TInt i = 0; i < count; i++)
		{
		CDrawSection* drawSection = CDrawSection::NewL();;
		CleanupStack::PushL(drawSection);
		User::LeaveIfError(drawSection->LoadL(parser, i));
		iDrawSections.AppendL(drawSection);
		CleanupStack::Pop(drawSection);
		}
	
	// Tidy the iDrawSection array so completely blocked sections will be removed
	Tidy();
	}

/**
@return the current active clipping region of the commandbuffer.
*/	
EXPORT_C const TRegion& CCommandBuffer::ClippingRegion() const
	{
	return *iActiveMasterClippingRegion;
	}

/**
Prepares to record new drawcommands for a drawsection.
This method is called from CRemoteGc::Activate().
*/
void CCommandBuffer::Prepare(const TRect& aDrawRect)
	{
	iDrawSectionRect = aDrawRect;
	iError = KErrNone;
  	if(iRecordSegBuf)
  		iRecordSegBuf->Delete(0, iRecordSegBuf->Size()); // Reset record buffer
  	else
  		TRAP(iError, iRecordSegBuf = CBufSeg::NewL(KBufferSize));
	}
	
/**
Finishes the recording of drawcommands for a drawsection. 
This method is called from CRemoteGc::Deactivate().

@param aDrawRect The drawrect of the recorded drawcommands.
@param aBoundingRect The boundingrect of the recorded drawcommands.
*/
TInt CCommandBuffer::Finish(const TRect& aDrawRect, const TRect& aBoundingRect, TBool aHasBitmapCommand)
	{
	// If some error occured during the recording of this section, dont add the section
	if(!iError)
		{
		CDrawSection* drawSection = NULL;
		TRAP(iError, drawSection = CDrawSection::NewL(aDrawRect, aBoundingRect, aHasBitmapCommand))
		if(iError)
			return iError;
		
		// If boundingRect is empty clear the drawcommands added
		if(aBoundingRect.IsEmpty())
			iRecordSegBuf->Delete(0, iRecordSegBuf->Size());
		
		iRecordSegBuf->Compress();
		drawSection->SetBuffer(iRecordSegBuf); //Takes ownership of the memory allocated by iRecordSegBuf			
		iRecordSegBuf = NULL;		
		if(CheckForDuplicate(*drawSection))
			{
			delete drawSection;
			return KErrAlreadyExists;
			}
		
		iError = iDrawSections.Append(drawSection);
		if(iError)
			delete drawSection;
		else
			{
			Tidy();
			if(iDrawSections.Count() == 0 || AllSectionsExternalized())
				return KErrGeneral;
			}
		}

	return iError;
	}

/**
Remove drawsections that is completely blocked by another drawsection. 
*/
void CCommandBuffer::Tidy()
	{
	RRegion region;
	TInt count = 0;
	for(TInt i = 0; i < (count = iDrawSections.Count()); i++)
		{		
		TRect rect1 = iDrawSections[i]->DrawRect();
		region.Clear();
		region.AddRect(rect1);
		for(TInt j = i + 1; j < count; j++)
			{
			TRect rect2 = iDrawSections[j]->DrawRect();
			region.SubRect(rect2);
			}
			
		if(region.IsEmpty())
			{
			delete iDrawSections[i];
			iDrawSections.Remove(i--);
			}
		}
	region.Close();
	}

/** 
@return ETrue if all sections in the commandbuffer have been externalized, otherwise EFalse. 
*/ 
TBool CCommandBuffer::AllSectionsExternalized() const 
	{ 
	const TInt count = iDrawSections.Count(); 
	for(TInt i = 0; i < count; i++) 
		{ 
		if(!iDrawSections[i]->HasBeenExternalized()) 
			return EFalse; 
		} 
	return ETrue; 
	} 

/**
Checks if there exists any duplicate of aDrawSection already in the iDrawSection array.

@param aDrawSection The drawsection to look for a duplicate of.
@return ETrue if a duplicate was found, otherwise EFalse.
*/
TBool CCommandBuffer::CheckForDuplicate(const CDrawSection& aDrawSection) const
	{
	const TInt count = iDrawSections.Count();
	for(TInt i = 0; i < count; i++)
		{
		if(!iDrawSections[i]->IsIdentical(aDrawSection))
			continue;
		
		// Check if there is some drawsection that overlaps the section we found identical, 
		// if that is the case it is not a duplicate
		for(TInt j = i + 1; j < count; j++)
			{
			TRect compareRect = iDrawSections[j]->DrawRect();
			if(aDrawSection.DrawRect().Intersects(compareRect)) 
				return EFalse; //Found a drawrect that overlapped, no duplicate exists then
			}
			
		// Found duplicate
		return ETrue;
		}
		
	return EFalse;
	}
	
/**
Updates the clippingregion for a specific drawsection.

@param aDrawSectionIndex The index in iDrawSections of the drawsection to update the clippingregion for.
@param aBitmapContext The bitmapcontext to set the new clippingregion on.
*/
void CCommandBuffer::UpdateClippingRegion(TInt aDrawSectionIndex)
	{
	__ASSERT_DEBUG(aDrawSectionIndex < iDrawSections.Count(), User::Invariant());
	
	iDrawSectionClippingRegion.Clear();
	if (iMasterClippingRegion)
		{
		iDrawSectionClippingRegion.Copy(*iMasterClippingRegion);
		}
	else
		{
		TRect rect = iMasterClippingRect;
		rect.Move(iMasterOrigin);
		iDrawSectionClippingRegion.AddRect(rect);
		}

	const TInt count = iDrawSections.Count();
	for(TInt i = aDrawSectionIndex + 1; i < count; ++i)
		{
		TRect rect = iDrawSections[i]->DrawRect();
		rect.Move(iMasterOrigin);
		iDrawSectionClippingRegion.SubRect(rect);
		}
	
	if (iDrawSectionClippingRegion.CheckError())
		iActiveMasterClippingRegion = iMasterClippingRegion;
	else
		iActiveMasterClippingRegion = &iDrawSectionClippingRegion;
	}

/**
Writes data of a specific length to the recordbuffer.

@param aPtr The data to write.
@param aLength The length of the data to write.
*/
void CCommandBuffer::Write(const TUint8* aPtr, TUint aLength)
	{
	if(iError)
		return;
	
	TRAP(iError, iRecordSegBuf->InsertL(iRecordSegBuf->Size(), aPtr, aLength))
	if(iError)
		return;
	}

/**
Writes text to the recordbuffer.

@param aText The text to write to the recordbuffer.
*/
void CCommandBuffer::WriteText(const TDesC8 &aText)
	{
	if(iError)
		return;
	
	// Append the total size of the text
	Write<TInt>(aText.Size());
	if(iError)
		return;			
	
	TRAP(iError, DoWriteTextL(aText));	
	}

/**
Writes text to the recordbuffer.

@param aText The text to write to the recordbuffer.
*/
void CCommandBuffer::WriteText(const TDesC16 &aText)
	{
	if(iError)
		return;
	
	// Append the total size of the text
	Write<TInt>(aText.Size());
	if(iError)
		return;			
	
	TPtrC8 textPtr(reinterpret_cast<const TUint8*>(aText.Ptr()),aText.Size());
	TRAP(iError, DoWriteTextL(textPtr));	
	}

/**
Writes text to the recordbuffer.

@param aText The text to write to the recordbuffer.
*/	
void CCommandBuffer::DoWriteTextL(const TDesC8 &aText)
	{
	iRecordSegBuf->InsertL(iRecordSegBuf->Size(), aText, aText.Size());		
	}

/**
Reads data with a specific length from iBufReadStream.

@param aPtr The read data is written to this paramenter.
@param aLength The length of the data to be read.
*/
void CCommandBuffer::ReadL(TUint8* aPtr, TUint aLength)
	{
	iBufReadStream.ReadL(aPtr, aLength);
	}

/**
Reads text from iBufReadStream.

@param aText The read text is put into this 8-bit buffer.
*/	
void CCommandBuffer::ReadTextLC(TPtrC8& aText)
	{
	DoReadTextLC(aText,EFalse);
	}

/**
Reads text from iBufReadStream.

@param aText The read text is put into this 16-bit buffer.
*/	
void CCommandBuffer::ReadTextLC(TPtrC16& aText)
	{
	TPtrC8 text8;
	DoReadTextLC(text8,ETrue);
	aText.Set(reinterpret_cast<const TUint16*>(text8.Ptr()),text8.Size()/2);
	}
	
/**
Reads text from iBufReadStream; used by ReadTextLC

@internalComponent
*/	
void CCommandBuffer::DoReadTextLC(TPtrC8& aText,TBool a16Bit)
	{
	ASSERT(iBufRead);
	
	TInt textSize;
	ReadL<TInt>(textSize); // Read the length of the text
	if(0 > textSize)
		{
		User::Leave(KErrArgument);
		}
	
	// attempt to do it inline
	const TInt pos = iBufReadStream.Source()->TellL(MStreamBuf::ERead).Offset();
	if(!a16Bit || !(pos & 1)) // check 16bit-aligned
		{
		const TPtrC8 remaining = iBufRead->Ptr(pos);
		if(remaining.Size() >= textSize) // can do inline!
			{
			CleanupStack::PushL((TAny*)NULL); // have to push something
			iBufReadStream.Source()->SeekL(MStreamBuf::ERead,textSize);
			aText.Set(remaining.Ptr(),textSize);
			return;
			}
		}
		
	// have to copy into a continuous segment
	HBufC8* buf = HBufC8::NewLC(textSize);
	TPtr8 textPtr8(buf->Des());
	iBufReadStream.ReadL(textPtr8,textSize);
	aText.Set(*buf);
	}

/**
Compares the commands in one buffer to those in another
@param aBuffer The buffer to compare to
@return ETrue if they are an exact match, EFalse otherwise
*/
EXPORT_C TBool CCommandBuffer::IsIdentical(const CCommandBuffer & aBuffer) const
	{
	for(TInt i = 0; i < iDrawSections.Count(); ++i)
		{
		if (!iDrawSections[i]->IsIdentical(*aBuffer.iDrawSections[i]))
			return EFalse;
		}
	return ETrue;
	}

/**
Draws drawcommands that are within a specific rect to a specific position.
This function is deprecated use the other overload

@param aPosition Draws the drawcommands to this position.
@param aDrawRect Draws only drawcommands that are within this rect.
@param aBitmapContext The bitmapcontext to draw to.
@deprecated
@publishedPartner
*/
EXPORT_C TInt CCommandBuffer::Play(const TPoint& aPosition, const TRect& aDrawRect, const MWsGraphicResolver& aWsGraphicResolver, CBitmapContext& aContext)
	{
	iMasterOrigin = aPosition;
	iOrigin = TPoint(0,0); // Need to save this to be able to make Reset not affect Origin beacuse that is how CWsGc works.
	aContext.SetOrigin(iMasterOrigin);
	iMasterClippingRect = aDrawRect;
	iMasterClippingRegion=0;
		
	TRAPD(errMess, DoPlayL(aWsGraphicResolver, aContext));		
	return errMess;
	}

/**
Draws drawcommands that are within a specific rect to a specific position.

@param aMasterOrigin The origin relative to which all draw commands will be drawn
@param aMasterClippingRegion The region to which all draw commands are clipped
@param aMasterClippingRect The rectangle to which all draw commands are clipped
@param aWsGraphicResolver Any DrawWsGraphic commands will use this to draw themselves
@param aContext The bitmap context to draw to
@publishedPartner
*/
EXPORT_C TInt CCommandBuffer::Play(const TPoint& aMasterOrigin, const TRegion * aMasterClippingRegion, const TRect& aMasterClippingRect, const MWsGraphicResolver& aWsGraphicResolver, CBitmapContext& aContext)
	{
	iMasterOrigin = aMasterOrigin;
	iMasterClippingRegion = aMasterClippingRegion;
	iMasterClippingRect = aMasterClippingRect;
	
	Reset(aContext);

	TRAPD(errMess, DoPlayL(aWsGraphicResolver, aContext));		
	return errMess;
	}

/**
	@internalTechnology
*/
EXPORT_C TInt CCommandBuffer::Play(const TPoint& /*aOffset*/, const TRegion* /*aClippingRegion*/, const TRect& /*aSourceRect*/, const MWsGraphicResolver& /*aWsGraphicResolver*/, MWsGraphicsContext& /*aGraphicsContext*/) //Stub implementation to maintain compatibility with NGA Window Server
	{
	ASSERT(0);
	return KErrNotSupported;
	}

/**
	@internalTechnology
*/
EXPORT_C TInt CCommandBuffer::Play(const TPoint& /*aOffset*/, const TRegion* /*aClippingRegion*/, const TRect& /*aSourceRect*/, RWsSession& /*aWsSession*/, CWindowGc& /*aWindowGc*/) //Stub implementation to maintain compatibility with NGA Window Server
	{
	ASSERT(0);
	return KErrNotSupported;
	}

/**
Draws drawcommands that are within a specific rect.

@param aDrawRect Draws only drawcommands that are within this rect.
@param aBitmapContext The bitmapcontext to draw to.
*/
void CCommandBuffer::DoPlayL(const MWsGraphicResolver& aWsGraphicResolver, CBitmapContext& aBitmapContext)
	{
	const TInt sections = iDrawSections.Count();
	if(sections == 0)
		User::Leave(KErrEof);
	
	iBitmapCache->BeginUpdate();
	iFontCache->BeginUpdate();
	for(TInt i = 0; i < sections; i++)
		{
		UpdateClippingRegion(i);
		DrawSectionL(*iDrawSections[i], aWsGraphicResolver, aBitmapContext);		
		}
	iFontCache->EndUpdate();
	iBitmapCache->EndUpdate();	
	}

/**
Draws a specific drawsection.

@param aDrawSection The drawsection to be drawn.
@param aBitmapContext The bitmapcontext to draw to.
*/
void CCommandBuffer::DrawSectionL(const CDrawSection& aDrawSection, const MWsGraphicResolver& aWsGraphicResolver, CBitmapContext& aBitmapContext)
	{
	iDrawSectionRect = aDrawSection.DrawRect();
	Reset(aBitmapContext);

	iBufRead = aDrawSection.Buffer();
	iBufReadStream.Open(*iBufRead, 0);
	
	TDrawCode drawCode;
	while(ETrue)
		{		
	   	TRAPD(err, ReadL<TDrawCode>(drawCode));
	   	if(err == KErrEof)
	   		return;
	   	else if(err)
	   		User::Leave(err);
   	
		switch(drawCode)
			{
			case ECommandClear:
				Clear(aBitmapContext);
				break;			
			case ECommandClearRect:
				ClearRectL(aBitmapContext);
				break;
			case ECommandCopyRect:
				CopyRectL(aBitmapContext);
				break;
			case ECommandBitBlt1:
				BitBlt1L(aBitmapContext);
				break;
			case ECommandBitBlt2:
				BitBlt2L(aBitmapContext);
				break;
			case ECommandBitBltMasked:
				BitBltMaskedL(aBitmapContext);
				break;
			case ECommandSetFaded:
				SetFadedL(aBitmapContext);
				break;
			case ECommandSetFadingParameters:
				SetFadingParametersL(aBitmapContext);
				break;
			case ECommandAlphaBlendBitmaps:
				AlphaBlendBitmapsL(aBitmapContext);
				break;		
			case ECommandSetOrigin:
				SetOriginL(aBitmapContext);
				break;
			case ECommandSetDrawMode:
				SetDrawModeL(aBitmapContext);
				break;
			case ECommandSetClippingRect:
				SetClippingRectL(aBitmapContext);
				break;
			case ECommandCancelClippingRect:
				CancelClippingRect(aBitmapContext);
				break;
			case ECommandReset:
				Reset(aBitmapContext);
				break;			
			case ECommandUseFont:
				UseFontL(aBitmapContext);
				break;
			case ECommandDiscardFont:
				DiscardFont(aBitmapContext);
				break;
			case ECommandSetUnderlineStyle:
				SetUnderlineStyleL(aBitmapContext);
				break;
			case ECommandSetStrikethroughStyle:
				SetStrikethroughStyleL(aBitmapContext);
				break;
			case ECommandSetWordJustification:
				SetWordJustificationL(aBitmapContext);
				break;
			case ECommandSetCharJustification:
				SetCharJustificationL(aBitmapContext);
				break;
			case ECommandSetPenColor:
				SetPenColorL(aBitmapContext);
				break;
			case ECommandSetPenStyle:
				SetPenStyleL(aBitmapContext);
				break;
			case ECommandSetPenSize:
				SetPenSizeL(aBitmapContext);
				break;									
			case ECommandSetBrushColor:
				SetBrushColorL(aBitmapContext);
				break;						
			case ECommandSetBrushStyle:
				SetBrushStyleL(aBitmapContext);
				break;
			case ECommandSetBrushOrigin:
				SetBrushOriginL(aBitmapContext);
				break;
			case ECommandUseBrushPattern:
				UseBrushPatternL(aBitmapContext);
				break;
			case ECommandDiscardBrushPattern:
				DiscardBrushPattern(aBitmapContext);
				break;
			case ECommandMoveTo:
				MoveToL(aBitmapContext);
				break;
			case ECommandMoveBy:
				MoveByL(aBitmapContext);
				break;
			case ECommandPlot:
				PlotL(aBitmapContext);
				break;
			case ECommandDrawArc:
				DrawArcL(aBitmapContext);
				break;
			case ECommandDrawLine:
				DrawLineL(aBitmapContext);
				break;
			case ECommandDrawLineTo:
				DrawLineToL(aBitmapContext);
				break;
			case ECommandDrawLineBy:
				DrawLineByL(aBitmapContext);
				break;
			case ECommandDrawPolyLine:
				DrawPolyLineL(aBitmapContext);
				break;
			case ECommandDrawPie:
				DrawPieL(aBitmapContext);
				break;
			case ECommandDrawEllipse:
				DrawEllipseL(aBitmapContext);
				break;	
			case ECommandDrawRect:
				DrawRectL(aBitmapContext);
				break;
			case ECommandDrawPolygon:
				DrawPolygonL(aBitmapContext);
				break;	
			case ECommandDrawRoundRect:
				DrawRoundRectL(aBitmapContext);
				break;										
			case ECommandDrawBitmap1:
				DrawBitmap1L(aBitmapContext);
				break;	
			case ECommandDrawBitmap2:
				DrawBitmap2L(aBitmapContext);
				break;	
			case ECommandDrawBitmap3:
				DrawBitmap3L(aBitmapContext);
				break;	
			case ECommandDrawBitmapMasked:
				DrawBitmapMaskedL(aBitmapContext);
				break;
			case ECommandDrawText1:
				DrawText1L(aBitmapContext);
				break;
			case ECommandDrawText2:
				DrawText2L(aBitmapContext);
				break;
			case ECommandDrawText3:
				DrawText3L(aBitmapContext);
				break;	
			case ECommandMapColors:
				MapColorsL(aBitmapContext);
				break;					
			case ECommandSetClippingRegion:
				SetClippingRegionL(aBitmapContext);
				break;
			case ECommandCancelClippingRegion:
				CancelClippingRegion(aBitmapContext);
				break;
			case ECommandDrawTextVertical1:
				DrawTextVertical1L(aBitmapContext);
				break;
			case ECommandDrawTextVertical2:
				DrawTextVertical2L(aBitmapContext);
				break;						
			case ECommandDrawWsGraphic1:
				DrawWsGraphic1L(aWsGraphicResolver);
				break;						
			case ECommandDrawWsGraphic2:
				DrawWsGraphic2L(aWsGraphicResolver);
				break;						
			case ECommandSetShadowColor:
				SetShadowColorL(aBitmapContext);
				break;
			default:
				User::LeaveIfError(KErrNotFound);
				break;
			}
		}
	}

void CCommandBuffer::Clear(CBitmapContext& aBitmapContext) const
	{
	aBitmapContext.Clear();
	}

void CCommandBuffer::ClearRectL(CBitmapContext& aBitmapContext)
	{
	TRect rect;	
	ReadL<TRect>(rect);
	
	aBitmapContext.Clear(rect);
	}
	
void CCommandBuffer::CopyRectL(CBitmapContext& aBitmapContext)
	{
	TPoint point;
	TRect rect;	
	
	ReadL<TPoint>(point);
	ReadL<TRect>(rect);
	
	aBitmapContext.CopyRect(point, rect);
	}

void CCommandBuffer::BitBlt1L(CBitmapContext& aBitmapContext)
	{	
	TPoint point;
	TInt handle;
	
	ReadL<TPoint>(point);
	ReadL<TInt>(handle);
			
	if(!iBitmapCache->UseL(handle))
		aBitmapContext.BitBlt(point, iBitmapCache->Resolve(handle));
	}
	
void CCommandBuffer::BitBlt2L(CBitmapContext& aBitmapContext)
	{
	TPoint point;
	TRect sourceRect;
	TInt handle;
	
	ReadL<TPoint>(point);
	ReadL<TInt>(handle);
	ReadL<TRect>(sourceRect);
	
	if(!iBitmapCache->UseL(handle))
		aBitmapContext.BitBlt(point, iBitmapCache->Resolve(handle), sourceRect);
	}
	
void CCommandBuffer::BitBltMaskedL(CBitmapContext& aBitmapContext)
	{
	TPoint point;
	TInt bitmapHandle;
	TRect sourceRect;	
	TInt maskHandle;
	TBool invertedMask;
	
	ReadL<TPoint>(point);
	ReadL<TInt>(bitmapHandle);
	ReadL<TRect>(sourceRect);
	ReadL<TInt>(maskHandle);
	ReadL<TBool>(invertedMask);
	
	if(!iBitmapCache->UseL(bitmapHandle) && !iBitmapCache->UseL(maskHandle))
		aBitmapContext.BitBltMasked(point, iBitmapCache->Resolve(bitmapHandle), sourceRect, iBitmapCache->Resolve(maskHandle), invertedMask);	
	}
	
void CCommandBuffer::SetFadedL(CBitmapContext& aBitmapContext)
	{
	TBool faded;	
	ReadL<TBool>(faded);

	aBitmapContext.SetFaded(faded);
	}
	
void CCommandBuffer::SetFadingParametersL(CBitmapContext& aBitmapContext)
	{
	TUint8 blackMap;
	TUint8 whiteMap;
	
	ReadL<TUint8>(blackMap);
	ReadL<TUint8>(whiteMap);
	aBitmapContext.SetFadingParameters(blackMap, whiteMap);
	}
	
void CCommandBuffer::AlphaBlendBitmapsL(CBitmapContext& aBitmapContext)
	{
	TPoint destPoint;
	TInt srcHandle;
	TRect sourceRect;
	TInt alphaHandle;
	TPoint alphaPoint;
	
	ReadL<TPoint>(destPoint);
	ReadL<TInt>(srcHandle);
	ReadL<TRect>(sourceRect);
	ReadL<TInt>(alphaHandle);
	ReadL<TPoint>(alphaPoint);
	
	if(!iBitmapCache->UseL(srcHandle) && !iBitmapCache->UseL(alphaHandle))
		aBitmapContext.AlphaBlendBitmaps(destPoint, iBitmapCache->Resolve(srcHandle), sourceRect, iBitmapCache->Resolve(alphaHandle), alphaPoint);	
	}
	
void CCommandBuffer::SetOriginL(CBitmapContext& aBitmapContext)
	{
	ReadL<TPoint>(iOrigin);
	aBitmapContext.SetOrigin(iMasterOrigin + iOrigin);
	
	}
	
void CCommandBuffer::SetDrawModeL(CBitmapContext& aBitmapContext)
	{
	CGraphicsContext::TDrawMode drawMode;
	ReadL<CGraphicsContext::TDrawMode>(drawMode);
	aBitmapContext.SetDrawMode(drawMode);
	}
	
void CCommandBuffer::SetClippingRectL(CBitmapContext& aBitmapContext)
	{
	TRect rect;
	ReadL<TRect>(rect);
	rect.Intersection(iMasterClippingRect);
	rect.Intersection(iDrawSectionRect);
	aBitmapContext.SetClippingRect(rect);
	}
	
void CCommandBuffer::CancelClippingRect(CBitmapContext& aBitmapContext)
	{
	TRect rect = iMasterClippingRect;
	rect.Intersection(iDrawSectionRect);
	aBitmapContext.SetClippingRect(rect);
	}

	
void CCommandBuffer::Reset(CBitmapContext& aBitmapContext)
	{
	aBitmapContext.Reset();

	const TBool isFbsBitGc= aBitmapContext.IsFbsBitGc();
	if (isFbsBitGc)
		{
		TRgb brushColor = KRgbWhite;
		brushColor.SetAlpha(0); //make transparent
		aBitmapContext.SetBrushColor(brushColor);
		}
	
	aBitmapContext.SetOrigin(iMasterOrigin);
	if(iActiveMasterClippingRegion)
		aBitmapContext.SetClippingRegion(*iActiveMasterClippingRegion);
	CancelClippingRect(aBitmapContext);
	iOrigin = TPoint(0,0);
	iClippingRegion.Clear();
	iParsedClippingRegionIsSet = EFalse;
	}

void CCommandBuffer::UseFontL(CBitmapContext& aBitmapContext)
	{
	TInt fontHandle;
	ReadL<TInt>(fontHandle);
	if(iFontCache->UseL(fontHandle)) return;
	aBitmapContext.UseFont(iFontCache->Resolve(fontHandle));
	}
	
void CCommandBuffer::DiscardFont(CBitmapContext& aBitmapContext) const
	{	
	aBitmapContext.DiscardFont();
	}
	
void CCommandBuffer::SetUnderlineStyleL(CBitmapContext& aBitmapContext)
	{
	TFontUnderline underlineStyle;
	ReadL<TFontUnderline>(underlineStyle);
	aBitmapContext.SetUnderlineStyle(underlineStyle);
	}
	
void CCommandBuffer::SetStrikethroughStyleL(CBitmapContext& aBitmapContext)
	{
	TFontStrikethrough strikethroughStyle;
	ReadL<TFontStrikethrough>(strikethroughStyle);
	aBitmapContext.SetStrikethroughStyle(strikethroughStyle);		
	}
	
void CCommandBuffer::SetWordJustificationL(CBitmapContext& aBitmapContext)
	{
	TInt excessWidth;
	TInt numGaps;
	
	ReadL<TInt>(excessWidth);
	ReadL<TInt>(numGaps);
	aBitmapContext.SetWordJustification(excessWidth, numGaps);
	}
	
void CCommandBuffer::SetCharJustificationL(CBitmapContext& aBitmapContext)
	{
	TInt excessWidth;
	TInt numChars;
	
	ReadL<TInt>(excessWidth);
	ReadL<TInt>(numChars);
	aBitmapContext.SetCharJustification(excessWidth, numChars);
	}

void CCommandBuffer::SetPenColorL(CBitmapContext& aBitmapContext)
	{
	TRgb color;
	ReadL<TRgb>(color);
	
	aBitmapContext.SetPenColor(color);
	}

void CCommandBuffer::SetPenStyleL(CBitmapContext& aBitmapContext)
	{
	CGraphicsContext::TPenStyle penStyle;
	ReadL<CGraphicsContext::TPenStyle>(penStyle);
	
	aBitmapContext.SetPenStyle(penStyle);			
	}
	
void CCommandBuffer::SetPenSizeL(CBitmapContext& aBitmapContext)
	{
	TSize size;
	ReadL<TSize>(size);
	
	aBitmapContext.SetPenSize(size);			
	}
	
void CCommandBuffer::SetBrushColorL(CBitmapContext& aBitmapContext)
	{
	TRgb color;
	ReadL<TRgb>(color);
	
	aBitmapContext.SetBrushColor(color);
	}

void CCommandBuffer::SetBrushStyleL(CBitmapContext& aBitmapContext)
	{
	CGraphicsContext::TBrushStyle brushStyle;
	ReadL<CGraphicsContext::TBrushStyle>(brushStyle);
	
	aBitmapContext.SetBrushStyle(brushStyle);		
	}
	
void CCommandBuffer::SetBrushOriginL(CBitmapContext& aBitmapContext)
	{
	TPoint point;
	ReadL<TPoint>(point);
	
	aBitmapContext.SetBrushOrigin(point);			
	}
	
void CCommandBuffer::UseBrushPatternL(CBitmapContext& aBitmapContext)
	{
	TInt deviceHandle;
	ReadL<TInt>(deviceHandle);
	
	if(iBitmapCache->UseL(deviceHandle)) return;
	aBitmapContext.UseBrushPattern(iBitmapCache->Resolve(deviceHandle));
	}
	
void CCommandBuffer::DiscardBrushPattern(CBitmapContext& aBitmapContext) const
	{
	aBitmapContext.DiscardBrushPattern();
	}

void CCommandBuffer::MoveToL(CBitmapContext& aBitmapContext)
	{
	TPoint point;
	ReadL<TPoint>(point);
	
	aBitmapContext.MoveTo(point);
	}
	
void CCommandBuffer::MoveByL(CBitmapContext& aBitmapContext)
	{
	TPoint point;
	ReadL<TPoint>(point);
	
	aBitmapContext.MoveBy(point);
	}
	
void CCommandBuffer::PlotL(CBitmapContext& aBitmapContext)
	{
	TPoint point;
	ReadL<TPoint>(point);
	
	aBitmapContext.Plot(point);	
	}
	
void CCommandBuffer::DrawArcL(CBitmapContext& aBitmapContext)
	{
	TRect rect;
	TPoint start;
	TPoint end;
	ReadL<TRect>(rect);
	ReadL<TPoint>(start);
	ReadL<TPoint>(end);
	
	aBitmapContext.DrawArc(rect, start, end);
	}
	
void CCommandBuffer::DrawLineL(CBitmapContext& aBitmapContext)
	{
	TPoint point1;
	TPoint point2;
	ReadL<TPoint>(point1);
	ReadL<TPoint>(point2);
	
	aBitmapContext.DrawLine(point1, point2);
	}
	
void CCommandBuffer::DrawLineToL(CBitmapContext& aBitmapContext)
	{
	TPoint point;
	ReadL<TPoint>(point);
	
	aBitmapContext.DrawLineTo(point);
	}
	
void CCommandBuffer::DrawLineByL(CBitmapContext& aBitmapContext)
	{
	TPoint point;
	ReadL<TPoint>(point);
	
	aBitmapContext.DrawLineBy(point);			
	}
	
void CCommandBuffer::DrawPolyLineL(CBitmapContext& aBitmapContext)
	{	
	TInt nrOfPoints;
	ReadL<TInt>(nrOfPoints);
	
	CArrayFix<TPoint>* pointList = new (ELeave) CArrayFixFlat<TPoint>(5);
	CleanupStack::PushL(pointList);
	for(TInt i = 0; i < nrOfPoints; i++)
		{
		TPoint point;
		ReadL<TPoint>(point);
		pointList->AppendL(point);
		}
	
	aBitmapContext.DrawPolyLine(pointList);	
	CleanupStack::PopAndDestroy(pointList);
	}
	
void CCommandBuffer::DrawPieL(CBitmapContext& aBitmapContext)
	{
	TRect rect;
	TPoint start;
	TPoint end;
	ReadL<TRect>(rect);
	ReadL<TPoint>(start);
	ReadL<TPoint>(end);
	
	aBitmapContext.DrawPie(rect, start, end);
	}
	
void CCommandBuffer::DrawEllipseL(CBitmapContext& aBitmapContext)
	{
	TRect rect;
	ReadL<TRect>(rect);
	
	aBitmapContext.DrawEllipse(rect);
	}
	
void CCommandBuffer::DrawRectL(CBitmapContext& aBitmapContext)
	{
	TRect rect;
	ReadL<TRect>(rect);
	
	aBitmapContext.DrawRect(rect);
	}
	
void CCommandBuffer::DrawRoundRectL(CBitmapContext& aBitmapContext)
	{
	TRect rect;
	TSize ellipse;
	ReadL<TRect>(rect);
	ReadL<TSize>(ellipse);
	
	aBitmapContext.DrawRoundRect(rect, ellipse);
	}
	
void CCommandBuffer::DrawPolygonL(CBitmapContext& aBitmapContext)
	{
	TInt nrOfPoints;
	ReadL<TInt>(nrOfPoints);
	
	CArrayFix<TPoint>* pointList = new (ELeave) CArrayFixFlat<TPoint>(5);
	CleanupStack::PushL(pointList);
	for(TInt i = 0; i < nrOfPoints; i++)
		{
		TPoint point;
		ReadL<TPoint>(point);
		pointList->AppendL(point);
		}
		
	CGraphicsContext::TFillRule fillRule;
	ReadL<CGraphicsContext::TFillRule>(fillRule);
	aBitmapContext.DrawPolygon(pointList, fillRule);	
	CleanupStack::PopAndDestroy(pointList);
	}
	
void CCommandBuffer::DrawBitmap1L(CBitmapContext& aBitmapContext)
	{
	TPoint topLeft;
	TInt bitmapHandle;
	
	ReadL<TPoint>(topLeft);
	ReadL<TInt>(bitmapHandle);
	
	if(!iBitmapCache->UseL(bitmapHandle))
		aBitmapContext.DrawBitmap(topLeft, iBitmapCache->Resolve(bitmapHandle));
	}
	
void CCommandBuffer::DrawBitmap2L(CBitmapContext& aBitmapContext)
	{
	TRect destRect;
	TInt bitmapHandle;
	
	ReadL<TRect>(destRect);
	ReadL<TInt>(bitmapHandle);
	
	if(!iBitmapCache->UseL(bitmapHandle))
		aBitmapContext.DrawBitmap(destRect, iBitmapCache->Resolve(bitmapHandle));
	}
	
void CCommandBuffer::DrawBitmap3L(CBitmapContext& aBitmapContext)
	{
	TRect destRect;
	TRect sourceRect;
	TInt bitmapHandle;
	
	ReadL<TRect>(destRect);
	ReadL<TInt>(bitmapHandle);
	ReadL<TRect>(sourceRect);
	
	if(!iBitmapCache->UseL(bitmapHandle))
		aBitmapContext.DrawBitmap(destRect, iBitmapCache->Resolve(bitmapHandle), sourceRect);
	}
	
void CCommandBuffer::DrawBitmapMaskedL(CBitmapContext& aBitmapContext)
	{
	TRect destRect;
	TRect sourceRect;
	TInt bitmapHandle;
	TInt maskHandle;
	TBool invertedMask;
	
	ReadL<TRect>(destRect);
	ReadL<TInt>(bitmapHandle);
	ReadL<TRect>(sourceRect);
	ReadL<TInt>(maskHandle);
	ReadL<TBool>(invertedMask);
	
	if(!iBitmapCache->UseL(bitmapHandle) && !iBitmapCache->UseL(maskHandle))
		aBitmapContext.DrawBitmapMasked(destRect, iBitmapCache->Resolve(bitmapHandle), sourceRect, iBitmapCache->Resolve(maskHandle), invertedMask);
	}

void CCommandBuffer::DrawText1L(CBitmapContext& aBitmapContext)
	{
	TPoint point;

	TPtrC16 text;
	ReadTextLC(text);

	ReadL<TPoint>(point);
	
	aBitmapContext.DrawText(text, point);
	CleanupStack::PopAndDestroy(); // ReadTextLC
	}

void CCommandBuffer::DrawText2L(CBitmapContext& aBitmapContext)
	{
	TRect box;
	TInt baselineOffset;
	CGraphicsContext::TTextAlign horiz;
	TInt leftMargin;
	
	TPtrC16 text;
	ReadTextLC(text);
	
	ReadL<TRect>(box);
	ReadL<TInt>(baselineOffset);
	ReadL<CGraphicsContext::TTextAlign>(horiz);
	ReadL<TInt>(leftMargin);
	
	aBitmapContext.DrawText(text, box, baselineOffset, horiz, leftMargin);	
	CleanupStack::PopAndDestroy(); // ReadTextLC	
	}
	
void CCommandBuffer::DrawText3L(CBitmapContext& aBitmapContext)
	{	
	TPoint point;
	CGraphicsContext::TDrawTextParam param;

	TPtrC16 text;
	ReadTextLC(text);

	ReadL<TPoint>(point);
	ReadL<CGraphicsContext::TDrawTextParam>(param);
	
	aBitmapContext.DrawText(text, point, param);	
	CleanupStack::PopAndDestroy(); //ReadTextLC
	}

void CCommandBuffer::MapColorsL(CBitmapContext& aBitmapContext)
	{
	TRect rect;
	TInt nrOfPairs;
	ReadL<TRect>(rect);
	ReadL<TInt>(nrOfPairs);
	
	TRgb* colorList = new (ELeave) TRgb [nrOfPairs*2]; // Every pair has two colors
	CleanupArrayDeletePushL(colorList);
	for(TInt i = 0; i < nrOfPairs; i++)
		{
		TRgb color;
		ReadL<TRgb>(color);
		colorList[i] = color;
		
		ReadL<TRgb>(color);
		colorList[i+1] = color;		
		}
		
	TBool mapForwards;
	ReadL<TBool>(mapForwards);		
	
	aBitmapContext.MapColors(rect, colorList, nrOfPairs, mapForwards);	
	CleanupStack::PopAndDestroy(colorList);
	}

void CCommandBuffer::SetClippingRegionL(CBitmapContext& aBitmapContext)
	{
	TInt nrOfRects;
	ReadL<TInt>(nrOfRects);
	
	TRect rect;
	iClippingRegion.Clear();
	for(TInt i = 0; i < nrOfRects; i++)
		{		
		ReadL<TRect>(rect);
//		rect.Move(iMasterOrigin);
		iClippingRegion.AddRect(rect);
		}
	
	iParsedClippingRegionIsSet = ETrue;
	if(iActiveMasterClippingRegion)
		iClippingRegion.Intersect(*iActiveMasterClippingRegion);
	
	aBitmapContext.SetClippingRegion(iClippingRegion);
	}
	
void CCommandBuffer::CancelClippingRegion(CBitmapContext& aBitmapContext)
	{
	iClippingRegion.Clear();
	iParsedClippingRegionIsSet = EFalse;
	if(iActiveMasterClippingRegion)
		aBitmapContext.SetClippingRegion(*iActiveMasterClippingRegion);	
	else
		aBitmapContext.CancelClippingRegion();
	}

void CCommandBuffer::DrawTextVertical1L(CBitmapContext& aBitmapContext)
	{
	TPoint point;
	TBool up;

	TPtrC16 text;
	ReadTextLC(text);

	ReadL<TPoint>(point);
	ReadL<TBool>(up);
	
	aBitmapContext.DrawTextVertical(text, point, up);	
	CleanupStack::PopAndDestroy(); // ReadTextLC
	}
	
void CCommandBuffer::DrawTextVertical2L(CBitmapContext& aBitmapContext)
	{
	TRect box;
	TInt baselineOffset;
	TBool up;
	CGraphicsContext::TTextAlign vertical;
	TInt margin;

	TPtrC16 text;
	ReadTextLC(text);

	ReadL<TRect>(box);
	ReadL<TInt>(baselineOffset);
	ReadL<TBool>(up);
	ReadL<CGraphicsContext::TTextAlign>(vertical);
	ReadL<TInt>(margin);
	
	aBitmapContext.DrawTextVertical(text, box, baselineOffset, up, vertical, margin);	
	CleanupStack::PopAndDestroy(); //ReadTextLC
	}

void CCommandBuffer::DrawWsGraphic1L(const MWsGraphicResolver& aWsGraphicResolver)
	{
	TInt id;
	TBool isUid;
	TRect rect;
	
	ReadL<TInt>(id);
	ReadL<TBool>(isUid);
	ReadL<TRect>(rect);
	
	aWsGraphicResolver.DrawWsGraphic(id,isUid,rect,KNullDesC8());
	}
	
void CCommandBuffer::DrawWsGraphic2L(const MWsGraphicResolver& aWsGraphicResolver)
	{
	TInt id;
	TBool isUid;
	TRect rect;
	
	ReadL<TInt>(id);
	ReadL<TBool>(isUid);
	ReadL<TRect>(rect);	
	TPtrC8 text8;
	ReadTextLC(text8);
	
	aWsGraphicResolver.DrawWsGraphic(id,isUid,rect,text8);	
	CleanupStack::PopAndDestroy(); //ReadTextLC
	}

void CCommandBuffer::SetShadowColorL(CBitmapContext& aBitmapContext)
	{
	TRgb shadowColor;
	ReadL<TRgb>(shadowColor);

	aBitmapContext.SetShadowColor(shadowColor);
	}

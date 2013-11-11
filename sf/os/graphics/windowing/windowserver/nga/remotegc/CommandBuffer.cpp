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

#include "graphics/WSGRAPHICMSGBUF.H"
#include "DrawSection.h"
#include "CommandBuffer.h"
#include "BitmapCache.h"
#include "FontsCache.h"
#include "DrawableCache.h"
#include <graphics/wsgraphicscontext.h>
#include <graphics/wsdrawablesourceprovider.h>
#include <graphics/wsdrawresource.h>
#include "../SERVER/bitgditomwsgraphicscontextmappings.h"

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
	iIntersectedRegion.Close();
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
	// coverity[extend_simple_error]
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
	@internalTechnology
*/
EXPORT_C TInt CCommandBuffer::Play(const TPoint& /*aPosition*/, const TRect& /*aDrawRect*/, const MWsGraphicResolver& /*aWsGraphicResolver*/, CBitmapContext& /*aContext*/) //Stub implementation to maintain compatibility with classic wserv
	{
	ASSERT(0);
	return KErrNotSupported;
	}

/**
	@internalTechnology
*/
EXPORT_C TInt CCommandBuffer::Play(const TPoint& /*aMasterOrigin*/, const TRegion* /*aMasterClippingRegion*/, const TRect& /*aMasterClippingRect*/, const MWsGraphicResolver& /*aWsGraphicResolver*/, CBitmapContext& /*aBitmapContext*/) //Stub implementation to maintain compatibility with WSERV2
	{
	ASSERT(0);
	return KErrNotSupported;
	}

/**
Draws draw commands that are within a specific rect to a graphics context with a specific offset.

@post The draw commands have been executed on the provided graphics context

@param aOffset The offset applied on all drawing operations.
@param aClippingRegion The region to which all draw commands are clipped
@param aSourceRect Draws only draw commands that are within this rect relative to the original coordinate system, i.e. before Position is applied
@param aWsGraphicResolver The resolver to be used for DrawGraphic() calls.
@param aGraphicsContext The graphics context to draw to.
@publishedPartner
*/
EXPORT_C TInt CCommandBuffer::Play(const TPoint& aOffset, const TRegion* aClippingRegion, const TRect& aSourceRect, const MWsGraphicResolver& aWsGraphicResolver, MWsGraphicsContext& aGraphicsContext)
	{
	iMasterOrigin = aOffset;
	iMasterClippingRegion = aClippingRegion;
	iMasterClippingRect = aSourceRect;
	
	Reset(aGraphicsContext);
	TRAPD(err, {
		iDrawableCache = new(ELeave) CRenderStageDrawableCache(aGraphicsContext.ObjectInterface<MWsDrawableSourceProvider>());
		DoPlayL(aWsGraphicResolver, aGraphicsContext);
		});
	delete iDrawableCache;
	iDrawableCache = NULL;
	return err;
	}

/**
Draws draw commands that are within a specific rect to a graphics context with a specific offset.

@post The draw commands have been executed on the provided graphics context

@param aOffset The offset applied on all drawing operations.
@param aClippingRegion The region to which all draw commands are clipped
@param aSourceRect Draws only draw commands that are within this rect relative to the original coordinate system, i.e. before Position is applied
@param aWsSession The session used to create the graphics context (WindowsGc)
@param aWindowGc The graphics context to draw to.
@publishedPartner
*/
EXPORT_C TInt CCommandBuffer::Play(const TPoint& aOffset, const TRegion* aClippingRegion, const TRect& aSourceRect, RWsSession& aWsSession, CWindowGc& aWindowGc)
	{
	iMasterOrigin = aOffset;
	iMasterClippingRegion = aClippingRegion;
	iMasterClippingRect = aSourceRect;
	
	Reset(aWindowGc);
	
	//Create a null version of MWsGraphicResolver which will never be used in this instance of the call to Play
	MWsGraphicResolver* graphicResolver = NULL;
	
	TRAPD(err, {
		iDrawableCache = new(ELeave) CWindowDrawableCache(aWsSession);
		DoPlayL(*graphicResolver, aWindowGc);
		});
	delete iDrawableCache;
	iDrawableCache = NULL;
	return err;
	}

/**
Draws drawcommands that are within a specific rect.

@param aWsGraphicResolver Any DrawWsGraphic commands will use this to draw themselves
@param aGraphicsContext The graphics context to draw to.
*/
template<typename ContextType> void CCommandBuffer::DoPlayL(const MWsGraphicResolver& aWsGraphicResolver, ContextType& aGraphicsContext)
	{
	const TInt sections = iDrawSections.Count();
	if(sections == 0)
		User::Leave(KErrEof);
	
	iBitmapCache->BeginUpdate();
	iFontCache->BeginUpdate();
	for(TInt i = 0; i < sections; i++)
		{
		UpdateClippingRegion(i);
		DrawSectionL(*iDrawSections[i], aWsGraphicResolver, aGraphicsContext);		
		}
	iFontCache->EndUpdate();
	iBitmapCache->EndUpdate();	
	}

/**
Draws a specific drawsection.

@param aDrawSection The drawsection to be drawn.
@param aWsGraphicResolver Any DrawWsGraphic commands will use this to draw themselves
@param aGraphicsContext The graphics context to draw to.
*/
template<typename ContextType> void CCommandBuffer::DrawSectionL(const CDrawSection& aDrawSection, const MWsGraphicResolver& aWsGraphicResolver, ContextType& aGraphicsContext)
	{
	iDrawSectionRect = aDrawSection.DrawRect();
	Reset(aGraphicsContext);

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
			Clear(aGraphicsContext);
			break;			
		case ECommandClearRect:
			ClearRectL(aGraphicsContext);
			break;
		case ECommandCopyRect:
			CopyRectL(aGraphicsContext);
			break;
		case ECommandBitBlt1:
			BitBlt1L(aGraphicsContext);
			break;
		case ECommandBitBlt2:
			BitBlt2L(aGraphicsContext);
			break;
		case ECommandBitBltMasked:
			BitBltMaskedL(aGraphicsContext);
			break;
		case ECommandSetFaded:
			User::Leave(KErrNotSupported); //this op code is deprecated, should never be generated
			break;
		case ECommandSetFadingParameters:
			User::Leave(KErrNotSupported); //this op code is deprecated, should never be generated
			break;
		case ECommandAlphaBlendBitmaps:
			AlphaBlendBitmapsL(aGraphicsContext);
			break;		
		case ECommandSetOrigin:
			SetOriginL(aGraphicsContext);
			break;
		case ECommandSetDrawMode:
			SetDrawModeL(aGraphicsContext);
			break;
		case ECommandSetClippingRect:
			SetClippingRectL(aGraphicsContext);
			break;
		case ECommandCancelClippingRect:
			CancelClippingRect(aGraphicsContext);
			break;
		case ECommandReset:
			Reset(aGraphicsContext);
			break;			
		case ECommandUseFont:
			UseFontL(aGraphicsContext);
			break;
		case ECommandDiscardFont:
			DiscardFont(aGraphicsContext);
			break;
		case ECommandSetUnderlineStyle:
			SetUnderlineStyleL(aGraphicsContext);
			break;
		case ECommandSetStrikethroughStyle:
			SetStrikethroughStyleL(aGraphicsContext);
			break;
		case ECommandSetWordJustification:
			SetWordJustificationL(aGraphicsContext);
			break;
		case ECommandSetCharJustification:
			SetCharJustificationL(aGraphicsContext);
			break;
		case ECommandSetPenColor:
			SetPenColorL(aGraphicsContext);
			break;
		case ECommandSetPenStyle:
			SetPenStyleL(aGraphicsContext);
			break;
		case ECommandSetPenSize:
			SetPenSizeL(aGraphicsContext);
			break;									
		case ECommandSetBrushColor:
			SetBrushColorL(aGraphicsContext);
			break;						
		case ECommandSetBrushStyle:
			SetBrushStyleL(aGraphicsContext);
			break;
		case ECommandSetBrushOrigin:
			SetBrushOriginL(aGraphicsContext);
			break;
		case ECommandUseBrushPattern:
			UseBrushPatternL(aGraphicsContext);
			break;
		case ECommandDiscardBrushPattern:
			DiscardBrushPattern(aGraphicsContext);
			break;
		case ECommandMoveTo:
			MoveToL(aGraphicsContext);
			break;
		case ECommandMoveBy:
			MoveByL(aGraphicsContext);
			break;
		case ECommandPlot:
			PlotL(aGraphicsContext);
			break;
		case ECommandDrawArc:
			DrawArcL(aGraphicsContext);
			break;
		case ECommandDrawLine:
			DrawLineL(aGraphicsContext);
			break;
		case ECommandDrawLineTo:
			DrawLineToL(aGraphicsContext);
			break;
		case ECommandDrawLineBy:
			DrawLineByL(aGraphicsContext);
			break;
		case ECommandDrawPolyLine:
			DrawPolyLineL(aGraphicsContext);
			break;
		case ECommandDrawPie:
			DrawPieL(aGraphicsContext);
			break;
		case ECommandDrawEllipse:
			DrawEllipseL(aGraphicsContext);
			break;	
		case ECommandDrawRect:
			DrawRectL(aGraphicsContext);
			break;
		case ECommandDrawPolygon:
			DrawPolygonL(aGraphicsContext);
			break;	
		case ECommandDrawRoundRect:
			DrawRoundRectL(aGraphicsContext);
			break;
		case ECommandDrawBitmap1:
			DrawBitmap1L(aGraphicsContext);
			break;					
		case ECommandDrawBitmap2:
			DrawBitmap2L(aGraphicsContext);
			break;	
		case ECommandDrawBitmap3:
			DrawBitmap3L(aGraphicsContext);
			break;	
		case ECommandDrawBitmapMasked:
			DrawBitmapMaskedL(aGraphicsContext);
			break;
		case ECommandDrawText1:
			DrawText1L(aGraphicsContext);
			break;
		case ECommandDrawText2:
			DrawText2L(aGraphicsContext);
			break;
		case ECommandDrawText3:
			DrawText3L(aGraphicsContext);
			break;
		case ECommandDrawText4:
			DrawText4L(aGraphicsContext);
			break;
		case ECommandDrawText5:
			DrawText5L(aGraphicsContext);
			break;
		case ECommandMapColors:
			User::Leave(KErrNotSupported); //this op code is deprecated, should never be generated
			break;					
		case ECommandSetClippingRegion:
			SetClippingRegionL(aGraphicsContext);
			break;
		case ECommandCancelClippingRegion:
			CancelClippingRegion(aGraphicsContext);
			break;
		case ECommandDrawTextVertical1:
			DrawTextVertical1L(aGraphicsContext);
			break;
		case ECommandDrawTextVertical2:
			DrawTextVertical2L(aGraphicsContext);
			break;			
		case ECommandDrawTextVertical3:
			DrawTextVertical3L(aGraphicsContext);
			break;	
		case ECommandDrawTextVertical4:
			DrawTextVertical4L(aGraphicsContext);
			break;	
		case ECommandDrawWsGraphic1:
			DrawWsGraphic1L(aWsGraphicResolver);
			break;						
		case ECommandDrawWsGraphic2:
			DrawWsGraphic2L(aWsGraphicResolver);
			break;						
		case ECommandSetShadowColor:
			SetShadowColorL(aGraphicsContext);
			break;
		case ECommandDrawResourceToPos:
			DrawResourceToPosL(aGraphicsContext);
			break;
		case ECommandDrawResourceToRect:
			DrawResourceToRectL(aGraphicsContext);
			break;
		case ECommandDrawResourceFromRectToRect:
			DrawResourceFromRectToRectL(aGraphicsContext);
			break;
		case ECommandDrawResourceWithData:
			DrawResourceWithDataL(aGraphicsContext);
			break;
		default:
			User::LeaveIfError(KErrNotFound);
			break;
			}
		}
	}

template<typename ContextType> void CCommandBuffer::Clear(ContextType& aGraphicsContext) const
	{
	aGraphicsContext.Clear();
	}

template<typename ContextType> void CCommandBuffer::ClearRectL(ContextType& aGraphicsContext)
	{
	TRect rect;	
	ReadL<TRect>(rect);
	
	aGraphicsContext.Clear(rect);
	}
	
template<typename ContextType> void CCommandBuffer::CopyRectL(ContextType& aGraphicsContext)
	{
	TPoint point;
	TRect rect;	
	
	ReadL<TPoint>(point);
	ReadL<TRect>(rect);
	
	aGraphicsContext.CopyRect(point, rect);
	}

template<typename ContextType> void CCommandBuffer::BitBlt1L(ContextType& aGraphicsContext)
	{
	TPoint point;
	TInt handle;
	
	ReadL<TPoint>(point);
	ReadL<TInt>(handle);
	
	if(!iBitmapCache->UseL(handle))
		DoBitBlt1L(aGraphicsContext, point, handle);
	}

void CCommandBuffer::DoBitBlt1L(CWindowGc& aWindowGc, TPoint aPoint, TInt aHandle)
	{	
	aWindowGc.BitBlt(aPoint, iBitmapCache->Resolve(aHandle));
	}

void CCommandBuffer::DoBitBlt1L(MWsGraphicsContext& aGraphicsContext, TPoint aPoint, TInt aHandle)
	{	
	aGraphicsContext.BitBlt(aPoint, *iBitmapCache->Resolve(aHandle));
	}

template<typename ContextType> void CCommandBuffer::BitBlt2L(ContextType& aGraphicsContext)
	{
	TPoint point;
	TRect sourceRect;
	TInt handle;
	
	ReadL<TPoint>(point);
	ReadL<TInt>(handle);
	ReadL<TRect>(sourceRect);
	
	if(!iBitmapCache->UseL(handle))
		DoBitBlt2L(aGraphicsContext, point, handle, sourceRect);
	}

void CCommandBuffer::DoBitBlt2L(CWindowGc& aWindowGc, TPoint aPoint, TInt aHandle, TRect aSourceRect)
	{
	aWindowGc.BitBlt(aPoint, iBitmapCache->Resolve(aHandle), aSourceRect);
	}

void CCommandBuffer::DoBitBlt2L(MWsGraphicsContext& aGraphicsContext, TPoint aPoint, TInt aHandle, TRect aSourceRect)
	{
	aGraphicsContext.BitBlt(aPoint, *iBitmapCache->Resolve(aHandle), aSourceRect);
	}

template<typename ContextType> void CCommandBuffer::BitBltMaskedL(ContextType& aGraphicsContext)
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
		DoBitBltMaskedL(aGraphicsContext, point, bitmapHandle, sourceRect, maskHandle, invertedMask);
	}

void CCommandBuffer::DoBitBltMaskedL(CWindowGc& aWindowGc, TPoint aPoint, TInt aBitmapHandle, TRect aSourceRect, TInt aMaskHandle, TBool aInvertMask)
	{
	aWindowGc.BitBltMasked(aPoint, iBitmapCache->Resolve(aBitmapHandle), aSourceRect, iBitmapCache->Resolve(aMaskHandle), aInvertMask);	
	}

void CCommandBuffer::DoBitBltMaskedL(MWsGraphicsContext& aGraphicsContext, TPoint aPoint, TInt aBitmapHandle, TRect aSourceRect, TInt aMaskHandle, TBool aInvertMask)
	{
	aGraphicsContext.BitBltMasked(aPoint, *iBitmapCache->Resolve(aBitmapHandle), aSourceRect, *iBitmapCache->Resolve(aMaskHandle), aInvertMask);		
	}

template<typename ContextType> void CCommandBuffer::AlphaBlendBitmapsL(ContextType& aGraphicsContext)
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
		DoAlphaBlendBitmapsL(aGraphicsContext, destPoint, srcHandle, sourceRect, alphaHandle, alphaPoint);
	}

void CCommandBuffer::DoAlphaBlendBitmapsL(CWindowGc& aWindowGc, TPoint aDestPoint, TInt aSrcHandle, TRect aSourceRect, TInt aAlphaHandle, TPoint aAlphaPoint)
	{
	aWindowGc.AlphaBlendBitmaps(aDestPoint, iBitmapCache->Resolve(aSrcHandle), aSourceRect, iBitmapCache->Resolve(aAlphaHandle), aAlphaPoint);
	}

void CCommandBuffer::DoAlphaBlendBitmapsL(MWsGraphicsContext& aGraphicsContext, TPoint aDestPoint, TInt aSrcHandle, TRect aSourceRect, TInt aAlphaHandle, TPoint aAlphaPoint)
	{
	aGraphicsContext.BitBltMasked(aDestPoint, *iBitmapCache->Resolve(aSrcHandle), aSourceRect, *iBitmapCache->Resolve(aAlphaHandle), aAlphaPoint);	
	}
	
template<typename ContextType> void CCommandBuffer::SetOriginL(ContextType& aGraphicsContext)
	{
	ReadL<TPoint>(iOrigin);
	aGraphicsContext.SetOrigin(iMasterOrigin + iOrigin);
	}

template<typename ContextType> void CCommandBuffer::SetDrawModeL(ContextType& aGraphicsContext)
	{
	CGraphicsContext::TDrawMode drawMode;
	ReadL<CGraphicsContext::TDrawMode>(drawMode);
	
	DoSetDrawModeL(aGraphicsContext, drawMode);
	}

void CCommandBuffer::DoSetDrawModeL(CWindowGc& aWindowGc, CGraphicsContext::TDrawMode aDrawMode)
	{
	aWindowGc.SetDrawMode(aDrawMode);
	}

void CCommandBuffer::DoSetDrawModeL(MWsGraphicsContext& aGraphicsContext, CGraphicsContext::TDrawMode aDrawMode)
	{
	aGraphicsContext.SetDrawMode(BitGdiToMWsGraphicsContextMappings::LossyConvert(aDrawMode));
	}
	
template<typename ContextType> void CCommandBuffer::SetClippingRectL(ContextType& aGraphicsContext)
	{
	ReadL<TRect>(iClippingRect);
	iClippingRect.Intersection(iMasterClippingRect);
	iClippingRect.Intersection(iDrawSectionRect);
	
	//Replacement for SetClippingRect functionality which MWsGraphicsContext does not support
	iIntersectedRegion.Clear();
	if(iActiveMasterClippingRegion->Count() > 0)
		iIntersectedRegion.Copy(*iActiveMasterClippingRegion);
	iIntersectedRegion.ClipRect(iClippingRect);
	aGraphicsContext.SetClippingRegion(iIntersectedRegion);
	}

template<typename ContextType> void CCommandBuffer::CancelClippingRect(ContextType& aGraphicsContext)
	{
	iClippingRect = iMasterClippingRect;
	iClippingRect.Intersection(iDrawSectionRect);
	
	//Replacement for SetClippingRect functionality which MWsGraphicsContext does not support
	iIntersectedRegion.Clear();
	if(iActiveMasterClippingRegion && iActiveMasterClippingRegion->Count() > 0)
		iIntersectedRegion.Copy(*iActiveMasterClippingRegion);
	iIntersectedRegion.ClipRect(iClippingRect);
	aGraphicsContext.SetClippingRegion(iIntersectedRegion);
	}

void CCommandBuffer::Reset(CWindowGc& aWindowGc)
	{
	aWindowGc.Reset();

	aWindowGc.SetOrigin(iMasterOrigin);
	if(iActiveMasterClippingRegion)
		aWindowGc.SetClippingRegion(*iActiveMasterClippingRegion);
	CancelClippingRect(aWindowGc);
	iOrigin = TPoint(0,0);
	iClippingRegion.Clear();
	iParsedClippingRegionIsSet = EFalse;
	}

void CCommandBuffer::Reset(MWsGraphicsContext& aGraphicsContext)
	{
	aGraphicsContext.Reset();
	
	TRgb brushColor = KRgbWhite;
	brushColor.SetAlpha(0); //make transparent
	aGraphicsContext.SetBrushColor(brushColor);
		
	aGraphicsContext.SetOrigin(iMasterOrigin);
	if(iActiveMasterClippingRegion)
		aGraphicsContext.SetClippingRegion(*iActiveMasterClippingRegion);
	CancelClippingRect(aGraphicsContext);
	iOrigin = TPoint(0,0);
	iClippingRegion.Clear();
	iParsedClippingRegionIsSet = EFalse;
	}

template<typename ContextType> void CCommandBuffer::UseFontL(ContextType& aGraphicsContext)
	{
	TInt fontHandle;
	ReadL<TInt>(fontHandle);
	if(iFontCache->UseL(fontHandle)) return;
	
	DoUseFontL(aGraphicsContext, fontHandle);
	}

void CCommandBuffer::DoUseFontL(CWindowGc& aWindowGc, TInt aFontHandle)
	{
	aWindowGc.UseFont(iFontCache->Resolve(aFontHandle));
	}

void CCommandBuffer::DoUseFontL(MWsGraphicsContext& aGraphicsContext, TInt aFontHandle)
	{
	aGraphicsContext.SetFont(iFontCache->Resolve(aFontHandle));
	}
	
void CCommandBuffer::DiscardFont(CWindowGc& aWindowGc) const
	{	
	aWindowGc.DiscardFont();
	}

void CCommandBuffer::DiscardFont(MWsGraphicsContext& aGraphicsContext) const
	{	
	aGraphicsContext.ResetFont();
	}

template<typename ContextType> void CCommandBuffer::SetUnderlineStyleL(ContextType& aGraphicsContext)
	{
	TFontUnderline underlineStyle;
	ReadL<TFontUnderline>(underlineStyle);
	
	DoSetUnderlineStyleL(aGraphicsContext, underlineStyle);
	}

void CCommandBuffer::DoSetUnderlineStyleL(CWindowGc& aWindowGc, TFontUnderline aUnderlineStyle)
	{
	aWindowGc.SetUnderlineStyle(aUnderlineStyle);
	}

void CCommandBuffer::DoSetUnderlineStyleL(MWsGraphicsContext& aGraphicsContext, TFontUnderline aUnderlineStyle)
	{
	aGraphicsContext.SetUnderlineStyle(BitGdiToMWsGraphicsContextMappings::Convert(aUnderlineStyle));
	}

template<typename ContextType> void CCommandBuffer::SetStrikethroughStyleL(ContextType& aGraphicsContext)
	{
	TFontStrikethrough strikethroughStyle;
	ReadL<TFontStrikethrough>(strikethroughStyle);
	
	DoSetStrikethroughStyleL(aGraphicsContext, strikethroughStyle);
	}

void CCommandBuffer::DoSetStrikethroughStyleL(CWindowGc& aWindowGc, TFontStrikethrough aStrikethroughStyle)
	{
	aWindowGc.SetStrikethroughStyle(aStrikethroughStyle);		
	}

void CCommandBuffer::DoSetStrikethroughStyleL(MWsGraphicsContext& aGraphicsContext, TFontStrikethrough aStrikethroughStyle)
	{
	aGraphicsContext.SetStrikethroughStyle(BitGdiToMWsGraphicsContextMappings::Convert(aStrikethroughStyle));		
	}

template<typename ContextType> void CCommandBuffer::SetWordJustificationL(ContextType& aGraphicsContext)
	{
	TInt excessWidth;
	TInt numGaps;
	
	ReadL<TInt>(excessWidth);
	ReadL<TInt>(numGaps);
	aGraphicsContext.SetWordJustification(excessWidth, numGaps);
	}
	
template<typename ContextType> void CCommandBuffer::SetCharJustificationL(ContextType& aGraphicsContext)
	{
	TInt excessWidth;
	TInt numChars;
	
	ReadL<TInt>(excessWidth);
	ReadL<TInt>(numChars);
	aGraphicsContext.SetCharJustification(excessWidth, numChars);
	}

template<typename ContextType> void CCommandBuffer::SetPenColorL(ContextType& aGraphicsContext)
	{
	TRgb color;
	ReadL<TRgb>(color);
	
	aGraphicsContext.SetPenColor(color);
	}

template<typename ContextType> void CCommandBuffer::SetPenStyleL(ContextType& aGraphicsContext)
	{
	CGraphicsContext::TPenStyle penStyle;
	ReadL<CGraphicsContext::TPenStyle>(penStyle);
	
	DoSetPenStyleL(aGraphicsContext, penStyle);
	}

void CCommandBuffer::DoSetPenStyleL(CWindowGc& aWindowGc, CGraphicsContext::TPenStyle aPenStyle)
	{	
	aWindowGc.SetPenStyle(aPenStyle);			
	}

void CCommandBuffer::DoSetPenStyleL(MWsGraphicsContext& aGraphicsContext, CGraphicsContext::TPenStyle aPenStyle)
	{
	aGraphicsContext.SetPenStyle(BitGdiToMWsGraphicsContextMappings::Convert(aPenStyle));			
	}

template<typename ContextType> void CCommandBuffer::SetPenSizeL(ContextType& aGraphicsContext)
	{
	TSize size;
	ReadL<TSize>(size);
	
	DoSetPenSizeL(aGraphicsContext, size);
	}

void CCommandBuffer::DoSetPenSizeL(CWindowGc& aWindowGc, TSize aSize)
	{
	aWindowGc.SetPenSize(aSize);			
	}

void CCommandBuffer::DoSetPenSizeL(MWsGraphicsContext& aGraphicsContext, TSize aSize)
	{
	aGraphicsContext.SetPenSize(aSize);			
	}
	
template<typename ContextType> void CCommandBuffer::SetBrushColorL(ContextType& aGraphicsContext)
	{
	TRgb color;
	ReadL<TRgb>(color);
	
	aGraphicsContext.SetBrushColor(color);
	}

template<typename ContextType> void CCommandBuffer::SetBrushStyleL(ContextType& aGraphicsContext)
	{
	CGraphicsContext::TBrushStyle brushStyle;
	ReadL<CGraphicsContext::TBrushStyle>(brushStyle);
	
	DoSetBrushStyleL(aGraphicsContext, brushStyle);
	}

void CCommandBuffer::DoSetBrushStyleL(CWindowGc& aWindowGc, CGraphicsContext::TBrushStyle aBrushStyle)
	{
	aWindowGc.SetBrushStyle(aBrushStyle);		
	}

void CCommandBuffer::DoSetBrushStyleL(MWsGraphicsContext& aGraphicsContext, CGraphicsContext::TBrushStyle aBrushStyle)
	{
	aGraphicsContext.SetBrushStyle(BitGdiToMWsGraphicsContextMappings::Convert(aBrushStyle));		
	}
	
template<typename ContextType> void CCommandBuffer::SetBrushOriginL(ContextType& aGraphicsContext)
	{
	TPoint point;
	ReadL<TPoint>(point);
	
	aGraphicsContext.SetBrushOrigin(point);			
	}

template<typename ContextType> void CCommandBuffer::UseBrushPatternL(ContextType& aGraphicsContext)
	{
	TInt deviceHandle;
	ReadL<TInt>(deviceHandle);
	
	if(iBitmapCache->UseL(deviceHandle)) return;
	DoUseBrushPatternL(aGraphicsContext, deviceHandle);
	}

void CCommandBuffer::DoUseBrushPatternL(CWindowGc& aWindowGc, TInt aDeviceHandle)
	{
	aWindowGc.UseBrushPattern(iBitmapCache->Resolve(aDeviceHandle));
	}

void CCommandBuffer::DoUseBrushPatternL(MWsGraphicsContext& aGraphicsContext, TInt aDeviceHandle)
	{
	aGraphicsContext.SetBrushPattern(*iBitmapCache->Resolve(aDeviceHandle));
	}
	
void CCommandBuffer::DiscardBrushPattern(CWindowGc& aWindowGc) const
	{
	aWindowGc.DiscardBrushPattern();
	}

void CCommandBuffer::DiscardBrushPattern(MWsGraphicsContext& aGraphicsContext) const
	{
	aGraphicsContext.ResetBrushPattern();
	}

template<typename ContextType> void CCommandBuffer::MoveToL(ContextType& aGraphicsContext)
	{
	TPoint point;
	ReadL<TPoint>(point);
	
	aGraphicsContext.MoveTo(point);
	}
	
template<typename ContextType> void CCommandBuffer::MoveByL(ContextType& aGraphicsContext)
	{
	TPoint point;
	ReadL<TPoint>(point);
	
	aGraphicsContext.MoveBy(point);
	}
	
template<typename ContextType> void CCommandBuffer::PlotL(ContextType& aGraphicsContext)
	{
	TPoint point;
	ReadL<TPoint>(point);
	
	aGraphicsContext.Plot(point);	
	}
	
template<typename ContextType> void CCommandBuffer::DrawArcL(ContextType& aGraphicsContext)
	{
	TRect rect;
	TPoint start;
	TPoint end;
	ReadL<TRect>(rect);
	ReadL<TPoint>(start);
	ReadL<TPoint>(end);
	
	aGraphicsContext.DrawArc(rect, start, end);
	}
	
template<typename ContextType> void CCommandBuffer::DrawLineL(ContextType& aGraphicsContext)
	{
	TPoint point1;
	TPoint point2;
	ReadL<TPoint>(point1);
	ReadL<TPoint>(point2);
	
	aGraphicsContext.DrawLine(point1, point2);
	}
	
template<typename ContextType> void CCommandBuffer::DrawLineToL(ContextType& aGraphicsContext)
	{
	TPoint point;
	ReadL<TPoint>(point);
	
	aGraphicsContext.DrawLineTo(point);
	}
	
template<typename ContextType> void CCommandBuffer::DrawLineByL(ContextType& aGraphicsContext)
	{
	TPoint point;
	ReadL<TPoint>(point);
	
	aGraphicsContext.DrawLineBy(point);			
	}
	
void CCommandBuffer::DrawPolyLineL(CWindowGc& aWindowGc)
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
	
	aWindowGc.DrawPolyLine(pointList);	
	CleanupStack::PopAndDestroy(pointList);
	}

void CCommandBuffer::DrawPolyLineL(MWsGraphicsContext& aGraphicsContext)
	{	
	TInt nrOfPoints;
	ReadL<TInt>(nrOfPoints);
	
	TPoint *points=(TPoint *)(nrOfPoints+1);
	TArrayWrapper<TPoint> pointList(points, nrOfPoints);

	aGraphicsContext.DrawPolyLine(pointList);	
	}
	
template<typename ContextType> void CCommandBuffer::DrawPieL(ContextType& aGraphicsContext)
	{
	TRect rect;
	TPoint start;
	TPoint end;
	ReadL<TRect>(rect);
	ReadL<TPoint>(start);
	ReadL<TPoint>(end);
	
	aGraphicsContext.DrawPie(rect, start, end);
	}
	
template<typename ContextType> void CCommandBuffer::DrawEllipseL(ContextType& aGraphicsContext)
	{
	TRect rect;
	ReadL<TRect>(rect);
	
	aGraphicsContext.DrawEllipse(rect);
	}
	
template<typename ContextType> void CCommandBuffer::DrawRectL(ContextType& aGraphicsContext)
	{
	TRect rect;
	ReadL<TRect>(rect);
	
	aGraphicsContext.DrawRect(rect);
	}
	
template<typename ContextType> void CCommandBuffer::DrawRoundRectL(ContextType& aGraphicsContext)
	{
	TRect rect;
	TSize ellipse;
	ReadL<TRect>(rect);
	ReadL<TSize>(ellipse);
	
	aGraphicsContext.DrawRoundRect(rect, ellipse);
	}
	
void CCommandBuffer::DrawPolygonL(CWindowGc& aWindowGc)
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
	aWindowGc.DrawPolygon(pointList, fillRule);	
	CleanupStack::PopAndDestroy(pointList);
	}

void CCommandBuffer::DrawPolygonL(MWsGraphicsContext& aGraphicsContext)
	{
	TInt nrOfPoints;
	ReadL<TInt>(nrOfPoints);
	
	TPoint *points=(TPoint *)(nrOfPoints+1);
	TArrayWrapper<TPoint> pointList(points, nrOfPoints);
		
	CGraphicsContext::TFillRule fillRule;
	ReadL<CGraphicsContext::TFillRule>(fillRule);
	aGraphicsContext.DrawPolygon(pointList, BitGdiToMWsGraphicsContextMappings::Convert(fillRule));	
	}
	
void CCommandBuffer::DrawBitmap1L(CWindowGc& aWindowGc)
	{
	TPoint topLeft;
	TInt bitmapHandle;
	
	ReadL<TPoint>(topLeft);
	ReadL<TInt>(bitmapHandle);
	
	if(!iBitmapCache->UseL(bitmapHandle))
		aWindowGc.DrawBitmap(topLeft, iBitmapCache->Resolve(bitmapHandle));
	}

void CCommandBuffer::DrawBitmap1L(MWsGraphicsContext& /*aGraphicsContext*/)
	{
	User::Leave(KErrNotSupported); //this op code is deprecated, should never be generated
	}

template<typename ContextType> void CCommandBuffer::DrawBitmap2L(ContextType& aGraphicsContext)
	{
	TRect destRect;
	TInt bitmapHandle;
	
	ReadL<TRect>(destRect);
	ReadL<TInt>(bitmapHandle);
	
	if(!iBitmapCache->UseL(bitmapHandle))
		DoDrawBitmap2L(aGraphicsContext, destRect, bitmapHandle);
	}

void CCommandBuffer::DoDrawBitmap2L(CWindowGc& aWindowGc, TRect aDestRect, TInt aBitmapHandle)
	{
	aWindowGc.DrawBitmap(aDestRect, iBitmapCache->Resolve(aBitmapHandle));
	}

void CCommandBuffer::DoDrawBitmap2L(MWsGraphicsContext& aGraphicsContext, TRect aDestRect, TInt aBitmapHandle)
	{
	aGraphicsContext.DrawBitmap(aDestRect, *iBitmapCache->Resolve(aBitmapHandle));
	}

template<typename ContextType> void CCommandBuffer::DrawBitmap3L(ContextType& aGraphicsContext)
	{
	TRect destRect;
	TInt bitmapHandle;
	TRect sourceRect;
	
	ReadL<TRect>(destRect);
	ReadL<TInt>(bitmapHandle);
	ReadL<TRect>(sourceRect);
	
	if(!iBitmapCache->UseL(bitmapHandle))
		DoDrawBitmap3L(aGraphicsContext, destRect, bitmapHandle, sourceRect);
	}

void CCommandBuffer::DoDrawBitmap3L(CWindowGc& aWindowGc, TRect aDestRect, TInt aBitmapHandle, TRect aSourceRect)
	{
	aWindowGc.DrawBitmap(aDestRect, iBitmapCache->Resolve(aBitmapHandle), aSourceRect);
	}

void CCommandBuffer::DoDrawBitmap3L(MWsGraphicsContext& aGraphicsContext, TRect aDestRect, TInt aBitmapHandle, TRect aSourceRect)
	{
	aGraphicsContext.DrawBitmap(aDestRect, *iBitmapCache->Resolve(aBitmapHandle), aSourceRect);
	}

template<typename ContextType> void CCommandBuffer::DrawBitmapMaskedL(ContextType& aGraphicsContext)
	{
	TRect destRect;
	TInt bitmapHandle;
	TRect sourceRect;
	TInt maskHandle;
	TBool invertedMask;
	
	ReadL<TRect>(destRect);
	ReadL<TInt>(bitmapHandle);
	ReadL<TRect>(sourceRect);
	ReadL<TInt>(maskHandle);
	ReadL<TBool>(invertedMask);
	
	if(!iBitmapCache->UseL(bitmapHandle) && !iBitmapCache->UseL(maskHandle))
		DoDrawBitmapMaskedL(aGraphicsContext, destRect, bitmapHandle, sourceRect, maskHandle, invertedMask);
	}

void CCommandBuffer::DoDrawBitmapMaskedL(CWindowGc& aWindowGc, TRect aDestRect, TInt aBitmapHandle, TRect aSourceRect, TInt aMaskHandle, TBool aInvertedMask)
	{
	aWindowGc.DrawBitmapMasked(aDestRect, iBitmapCache->Resolve(aBitmapHandle), aSourceRect, iBitmapCache->Resolve(aMaskHandle), aInvertedMask);
	}

void CCommandBuffer::DoDrawBitmapMaskedL(MWsGraphicsContext& aGraphicsContext, TRect aDestRect, TInt aBitmapHandle, TRect aSourceRect, TInt aMaskHandle, TBool aInvertedMask)
	{
	aGraphicsContext.DrawBitmapMasked(aDestRect, *iBitmapCache->Resolve(aBitmapHandle), aSourceRect, *iBitmapCache->Resolve(aMaskHandle), aInvertedMask);
	}

template<typename ContextType> void CCommandBuffer::DrawText1L(ContextType& aGraphicsContext)
	{
	TPtrC16 text;
	TPoint point;
	
	ReadTextLC(text);
	ReadL<TPoint>(point);
	
	DoDrawText1L(aGraphicsContext, text, point);
	CleanupStack::PopAndDestroy(); // ReadTextLC
	}

void CCommandBuffer::DoDrawText1L(CWindowGc& aWindowGc, TPtrC16 aText, TPoint aPoint)
	{
	aWindowGc.DrawText(aText, aPoint);
	}

void CCommandBuffer::DoDrawText1L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, TPoint aPoint)
	{
	aGraphicsContext.DrawText(aText, /*TTextParameters*/ NULL, aPoint);
	}

template<typename ContextType> void CCommandBuffer::DrawText2L(ContextType& aGraphicsContext)
	{
	TPtrC16 text;
	TRect box;
	TInt baselineOffset;
	CGraphicsContext::TTextAlign horiz;
	TInt leftMargin;
	
	ReadTextLC(text);
	ReadL<TRect>(box);
	ReadL<TInt>(baselineOffset);
	ReadL<CGraphicsContext::TTextAlign>(horiz);
	ReadL<TInt>(leftMargin);
	
	DoDrawText2L(aGraphicsContext, text, box, baselineOffset, horiz, leftMargin);
	CleanupStack::PopAndDestroy(); // ReadTextLC
	}

void CCommandBuffer::DoDrawText2L(CWindowGc& aWindowGc, TPtrC16 aText, TRect aBox, TInt aBaselineOffset, CGraphicsContext::TTextAlign aHoriz, TInt aLeftMargin)
	{
	aWindowGc.DrawText(aText, aBox, aBaselineOffset, aHoriz, aLeftMargin);
	}

void CCommandBuffer::DoDrawText2L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, TRect aBox, TInt aBaselineOffset, CGraphicsContext::TTextAlign aHoriz, TInt aLeftMargin)
	{
	aGraphicsContext.DrawText(aText, /*TTextParameters*/ NULL, aBox, aBaselineOffset, BitGdiToMWsGraphicsContextMappings::Convert(aHoriz), aLeftMargin);	
	}

template<typename ContextType> void CCommandBuffer::DrawText3L(ContextType& aGraphicsContext)
	{
	TPtrC16 text;
	TPoint point;
	CGraphicsContext::TDrawTextParam param;

	ReadTextLC(text);
	ReadL<TPoint>(point);
	//This is now ignored in the implmentation in CGraphicsContext and hence will ignore it here.
	ReadL<CGraphicsContext::TDrawTextParam>(param); 
	
	DoDrawText3L(aGraphicsContext, text, point);
	CleanupStack::PopAndDestroy(); //ReadTextLC
	}

void CCommandBuffer::DoDrawText3L(CWindowGc& aWindowGc, TPtrC16 aText, TPoint aPoint)
	{
	aWindowGc.DrawText(aText, aPoint);
	}

void CCommandBuffer::DoDrawText3L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, TPoint aPoint)
	{
	aGraphicsContext.DrawText(aText, /*TTextParameters*/ NULL, aPoint);
	}

template<typename ContextType> void CCommandBuffer::DrawText4L(ContextType& aGraphicsContext)
	{
	TPtrC16 text;
	CGraphicsContext::TTextParameters param;
	TPoint point;

	ReadTextLC(text);
	ReadL<CGraphicsContext::TTextParameters>(param);
	ReadL<TPoint>(point);
	
	DoDrawText4L(aGraphicsContext, text, param, point);
	CleanupStack::PopAndDestroy(); // ReadTextLC
	}

void CCommandBuffer::DoDrawText4L(CWindowGc& aWindowGc, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TPoint aPoint)
	{
	aWindowGc.DrawText(aText, &aParam, aPoint);
	}

void CCommandBuffer::DoDrawText4L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TPoint aPoint)
	{
	aGraphicsContext.DrawText(aText, BitGdiToMWsGraphicsContextMappings::Convert(&aParam), aPoint);
	}

template<typename ContextType> void CCommandBuffer::DrawText5L(ContextType& aGraphicsContext)
	{
	TPtrC16 text;
	CGraphicsContext::TTextParameters param;
	TRect box;
	TInt baselineOffset;
	CGraphicsContext::TTextAlign horiz;
	TInt leftMargin;
	
	ReadTextLC(text);
	ReadL<CGraphicsContext::TTextParameters>(param);
	ReadL<TRect>(box);
	ReadL<TInt>(baselineOffset);
	ReadL<CGraphicsContext::TTextAlign>(horiz);
	ReadL<TInt>(leftMargin);
	
	DoDrawText5L(aGraphicsContext, text, param, box, baselineOffset, horiz, leftMargin);	
	CleanupStack::PopAndDestroy(); // ReadTextLC	
	}

void CCommandBuffer::DoDrawText5L(CWindowGc& aWindowGc, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TRect aBox, TInt aBaselineOffset, CGraphicsContext::TTextAlign aHoriz, TInt aLeftMargin)
	{
	aWindowGc.DrawText(aText, &aParam, aBox, aBaselineOffset, aHoriz, aLeftMargin);	
	}

void CCommandBuffer::DoDrawText5L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TRect aBox, TInt aBaselineOffset, CGraphicsContext::TTextAlign aHoriz, TInt aLeftMargin)
	{
	aGraphicsContext.DrawText(aText, BitGdiToMWsGraphicsContextMappings::Convert(&aParam), aBox, aBaselineOffset, BitGdiToMWsGraphicsContextMappings::Convert(aHoriz), aLeftMargin);	
	}

template<typename ContextType> void CCommandBuffer::SetClippingRegionL(ContextType& aGraphicsContext)
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
		
	if(!iClippingRect.IsEmpty())
		{
		iClippingRegion.ClipRect(iClippingRect);
		}
	
	aGraphicsContext.SetClippingRegion(iClippingRegion);
	}

template<typename ContextType> void CCommandBuffer::CancelClippingRegion(ContextType& aGraphicsContext)
	{
	iClippingRegion.Clear();
	iParsedClippingRegionIsSet = EFalse;
	if(iActiveMasterClippingRegion)
		aGraphicsContext.SetClippingRegion(*iActiveMasterClippingRegion);	
	else
		DoCancelClippingRegion(aGraphicsContext);
	}

void CCommandBuffer::DoCancelClippingRegion(CWindowGc& aWindowGc)
	{
	aWindowGc.CancelClippingRegion();
	}

void CCommandBuffer::DoCancelClippingRegion(MWsGraphicsContext& aGraphicsContext)
	{
	aGraphicsContext.ResetClippingRegion();
	}

template<typename ContextType> void CCommandBuffer::DrawTextVertical1L(ContextType& aGraphicsContext)
	{
	TPtrC16 text;
	TPoint point;
	TBool up;
	
	ReadTextLC(text);
	ReadL<TPoint>(point);
	ReadL<TBool>(up);
	
	DoDrawTextVertical1L(aGraphicsContext, text, point, up);
	CleanupStack::PopAndDestroy(); // ReadTextLC
	}

void CCommandBuffer::DoDrawTextVertical1L(CWindowGc& aWindowGc, TPtrC16 aText, TPoint aPoint, TBool aUp)
	{
	aWindowGc.DrawTextVertical(aText, aPoint, aUp);
	}

void CCommandBuffer::DoDrawTextVertical1L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, TPoint aPoint, TBool aUp)
	{
	aGraphicsContext.DrawTextVertical(aText, /*TTextParameters*/ NULL, aPoint, aUp);	
	}

template<typename ContextType> void CCommandBuffer::DrawTextVertical2L(ContextType& aGraphicsContext)
	{
	TPtrC16 text;
	TRect box;
	TInt baselineOffset;
	TBool up;
	CGraphicsContext::TTextAlign vertical;
	TInt margin;

	ReadTextLC(text);
	ReadL<TRect>(box);
	ReadL<TInt>(baselineOffset);
	ReadL<TBool>(up);
	ReadL<CGraphicsContext::TTextAlign>(vertical);
	ReadL<TInt>(margin);
	
	DoDrawTextVertical2L(aGraphicsContext, text, box, baselineOffset, up, vertical, margin);
	CleanupStack::PopAndDestroy(); //ReadTextLC
	}

void CCommandBuffer::DoDrawTextVertical2L(CWindowGc& aWindowGc, TPtrC16 aText, TRect aBox, TInt aBaselineOffset, TBool aUp, CGraphicsContext::TTextAlign aVertical, TInt aMargin)
	{
	aWindowGc.DrawTextVertical(aText, aBox, aBaselineOffset, aUp, aVertical, aMargin); 	
	}

void CCommandBuffer::DoDrawTextVertical2L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, TRect aBox, TInt aBaselineOffset, TBool aUp, CGraphicsContext::TTextAlign aVertical, TInt aMargin)
	{
	aGraphicsContext.DrawTextVertical(aText, /*TTextParameters*/ NULL, aBox, aBaselineOffset, aUp, BitGdiToMWsGraphicsContextMappings::Convert(aVertical), aMargin);	
	}

template<typename ContextType> void CCommandBuffer::DrawTextVertical3L(ContextType& aGraphicsContext)
	{
	TPtrC16 text;
	CGraphicsContext::TTextParameters param;
	TPoint point;
	TBool up;

	ReadTextLC(text);
	ReadL<CGraphicsContext::TTextParameters>(param);
	ReadL<TPoint>(point);
	ReadL<TBool>(up);
	
	DoDrawTextVertical3L(aGraphicsContext, text, param, point, up);	
	CleanupStack::PopAndDestroy(); // ReadTextLC
	}

void CCommandBuffer::DoDrawTextVertical3L(CWindowGc& aWindowGc, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TPoint aPoint, TBool aUp)
	{
	aWindowGc.DrawTextVertical(aText, &aParam, aPoint, aUp);	
	}

void CCommandBuffer::DoDrawTextVertical3L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TPoint aPoint, TBool aUp)
	{
	aGraphicsContext.DrawTextVertical(aText, BitGdiToMWsGraphicsContextMappings::Convert(&aParam), aPoint, aUp);	
	}

template<typename ContextType> void CCommandBuffer::DrawTextVertical4L(ContextType& aGraphicsContext)
	{
	TPtrC16 text;
	CGraphicsContext::TTextParameters param;
	TRect box;
	TInt baselineOffset;
	TBool up;
	CGraphicsContext::TTextAlign vertical;
	TInt margin;

	ReadTextLC(text);
	ReadL<CGraphicsContext::TTextParameters>(param);
	ReadL<TRect>(box);
	ReadL<TInt>(baselineOffset);
	ReadL<TBool>(up);
	ReadL<CGraphicsContext::TTextAlign>(vertical);
	ReadL<TInt>(margin);
	
	DoDrawTextVertical4L(aGraphicsContext, text, param, box, baselineOffset, up, vertical, margin);
	CleanupStack::PopAndDestroy(); //ReadTextLC
	}

void CCommandBuffer::DoDrawTextVertical4L(CWindowGc& aWindowGc, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TRect aBox, TInt aBaselineOffset, TBool aUp, CGraphicsContext::TTextAlign aVertical, TInt aMargin)
	{
	aWindowGc.DrawTextVertical(aText, &aParam, aBox, aBaselineOffset, aUp, aVertical, aMargin);	
	}

void CCommandBuffer::DoDrawTextVertical4L(MWsGraphicsContext& aGraphicsContext, TPtrC16 aText, CGraphicsContext::TTextParameters aParam, TRect aBox, TInt aBaselineOffset, TBool aUp, CGraphicsContext::TTextAlign aVertical, TInt aMargin)
	{
	aGraphicsContext.DrawTextVertical(aText, BitGdiToMWsGraphicsContextMappings::Convert(&aParam), aBox, aBaselineOffset, aUp, BitGdiToMWsGraphicsContextMappings::Convert(aVertical), aMargin);	
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

template<typename ContextType> void CCommandBuffer::SetShadowColorL(ContextType& aGraphicsContext)
	{
	TRgb shadowColor;
	ReadL<TRgb>(shadowColor);
	
	DoSetShadowColorL(aGraphicsContext, shadowColor);
	}

void CCommandBuffer::DoSetShadowColorL(CWindowGc& aWindowGc, TRgb aShadowColor)
	{
	aWindowGc.SetShadowColor(aShadowColor);
	}

void CCommandBuffer::DoSetShadowColorL(MWsGraphicsContext& aGraphicsContext, TRgb aShadowColor)
	{
	aGraphicsContext.SetTextShadowColor(aShadowColor);
	}

/**
  Helper function to draw resource at specified position. The function extracts all required parameter from the stream. 
 */
template<typename ContextType> void CCommandBuffer::DrawResourceToPosL(ContextType& aGraphicsContext)
	{
	TSgDrawableId drawableId;
	TInt screenNumber;
	TPoint pos;
	CWindowGc::TGraphicsRotation rotation;
	ReadL<TSgDrawableId>(drawableId);
	ReadL<TInt>(screenNumber);
	ReadL<TPoint>(pos);
	ReadL<CWindowGc::TGraphicsRotation>(rotation);
	if (iDrawableCache->UseL(drawableId, screenNumber) == KErrNone)
		{
		DoDrawResourceToPos(aGraphicsContext, iDrawableCache->Resolve(drawableId, screenNumber), pos, rotation);
		}
	}

void CCommandBuffer::DoDrawResourceToPos(CWindowGc& aGraphicsContext, const TAny* aDrawableSource, const TPoint& aPos, CWindowGc::TGraphicsRotation aRotation)
	{
	MWsDrawResource* drawResource = static_cast<MWsDrawResource*>(aGraphicsContext.Interface(KMWsDrawResourceInterfaceUid));
	if(drawResource)
		{
		drawResource->DrawResource(aPos, *static_cast<const RWsDrawableSource*>(aDrawableSource), aRotation);
		}
	}

void CCommandBuffer::DoDrawResourceToPos(MWsGraphicsContext& aGraphicsContext, const TAny* aDrawableSource, const TPoint& aPos, CWindowGc::TGraphicsRotation aRotation)
	{
	MWsDrawableSourceProvider* drawResource = aGraphicsContext.ObjectInterface<MWsDrawableSourceProvider>();
	if (drawResource)
		{
		drawResource->DrawResource(aDrawableSource, aPos, aRotation);
		}
	}

/**
  Helper function to draw resource into specified rectangle. The function extracts all required parameter from the stream. 
 */
template<typename ContextType> void CCommandBuffer::DrawResourceToRectL(ContextType& aGraphicsContext)
	{
	TSgDrawableId drawableId;
	TInt screenNumber;
	TRect rect;
	CWindowGc::TGraphicsRotation rotation;
	ReadL<TSgDrawableId>(drawableId);
	ReadL<TInt>(screenNumber);
	ReadL<TRect>(rect);
	ReadL<CWindowGc::TGraphicsRotation>(rotation);

	if (iDrawableCache->UseL(drawableId, screenNumber) == KErrNone)
		{
		DoDrawResourceToRect(aGraphicsContext, iDrawableCache->Resolve(drawableId, screenNumber), rect, rotation);
		}
	}

void CCommandBuffer::DoDrawResourceToRect(CWindowGc& aGraphicsContext, const TAny* aDrawableSource, const TRect& aRect, CWindowGc::TGraphicsRotation aRotation)
	{
	MWsDrawResource* drawResource = static_cast<MWsDrawResource*>(aGraphicsContext.Interface(KMWsDrawResourceInterfaceUid));
	if(drawResource)
		{
		drawResource->DrawResource(aRect, *static_cast<const RWsDrawableSource*>(aDrawableSource), aRotation);
		}
	}

void CCommandBuffer::DoDrawResourceToRect(MWsGraphicsContext& aGraphicsContext, const TAny* aDrawableSource, const TRect& aRect, CWindowGc::TGraphicsRotation aRotation)
	{
	MWsDrawableSourceProvider* drawResource = aGraphicsContext.ObjectInterface<MWsDrawableSourceProvider>();
	if (drawResource)
		{
		drawResource->DrawResource(aDrawableSource, aRect, aRotation);
		}
	}

/**
  Helper function to draw resource into specified rectangle from specified rectangle of the drawable. The function extracts all required parameter from the stream. 
 */
template<typename ContextType> void CCommandBuffer::DrawResourceFromRectToRectL(ContextType& aGraphicsContext)
	{
	TSgDrawableId drawableId;
	TInt screenNumber;
	TRect rectDest;
	TRect rectSrc;
	CWindowGc::TGraphicsRotation rotation;
	ReadL<TSgDrawableId>(drawableId);
	ReadL<TInt>(screenNumber);
	ReadL<TRect>(rectDest);
	ReadL<TRect>(rectSrc);
	ReadL<CWindowGc::TGraphicsRotation>(rotation);
	
	if (iDrawableCache->UseL(drawableId, screenNumber) == KErrNone)
		{
		DoDrawResourceFromRectToRect(aGraphicsContext, iDrawableCache->Resolve(drawableId, screenNumber), rectDest, rectSrc, rotation);
		}
	}

void CCommandBuffer::DoDrawResourceFromRectToRect(CWindowGc& aGraphicsContext, const TAny* aDrawableSource, const TRect& aRectDest, const TRect& aRectSrc, CWindowGc::TGraphicsRotation aRotation)
	{
	MWsDrawResource* drawResource = static_cast<MWsDrawResource*>(aGraphicsContext.Interface(KMWsDrawResourceInterfaceUid));
	if(drawResource)
		{
		drawResource->DrawResource(aRectDest, *static_cast<const RWsDrawableSource*>(aDrawableSource), aRectSrc, aRotation);
		}
	}

void CCommandBuffer::DoDrawResourceFromRectToRect(MWsGraphicsContext& aGraphicsContext, const TAny* aDrawableSource, const TRect& aRectDest, const TRect& aRectSrc, CWindowGc::TGraphicsRotation aRotation)
	{
	MWsDrawableSourceProvider* drawResource = aGraphicsContext.ObjectInterface<MWsDrawableSourceProvider>();
	if (drawResource)
		{
		drawResource->DrawResource(aDrawableSource, aRectDest, aRectSrc, aRotation);
		}
	}

template<typename ContextType> void CCommandBuffer::DrawResourceWithDataL(ContextType& aGraphicsContext)
	{
	TSgDrawableId drawableId;
	TInt screenNumber;
	TRect rect;
	TPtrC8 data;
	ReadL<TSgDrawableId>(drawableId);
	ReadL<TInt>(screenNumber);
	ReadL<TRect>(rect);
	ReadTextLC(data);
	
	if (iDrawableCache->UseL(drawableId, screenNumber) == KErrNone)
		{
		DoDrawResourceWithData(aGraphicsContext, iDrawableCache->Resolve(drawableId, screenNumber), rect, data);
		}

	CleanupStack::PopAndDestroy(); //ReadTextLC
	}

void CCommandBuffer::DoDrawResourceWithData(CWindowGc& aGraphicsContext, const TAny* aDrawableSource, const TRect& aRect, const TDesC8& aParam)
	{
	MWsDrawResource* drawResource = static_cast<MWsDrawResource*>(aGraphicsContext.Interface(KMWsDrawResourceInterfaceUid));
	if(drawResource)
		{
		drawResource->DrawResource(aRect, *static_cast<const RWsDrawableSource*>(aDrawableSource), aParam);
		}
	}

void CCommandBuffer::DoDrawResourceWithData(MWsGraphicsContext& aGraphicsContext, const TAny* aDrawableSource, const TRect& aRect, const TDesC8& aParam)
	{
	MWsDrawableSourceProvider* drawResource = aGraphicsContext.ObjectInterface<MWsDrawableSourceProvider>();
	if (drawResource)
		{
		drawResource->DrawResource(aDrawableSource, aRect, aParam);
		}
	}

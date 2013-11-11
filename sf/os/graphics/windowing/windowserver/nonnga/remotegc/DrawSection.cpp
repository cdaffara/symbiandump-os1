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

const TInt KBufferSize = 1000;

/**
This uid is used to identify a drawsection in the messagebuffer that is used for IPC.
*/
const TUid KDrawSection = {0x101F63B2};

/**
Creates and returns new drawsection.

@return A new instance of CDrawSection.
*/
CDrawSection* CDrawSection::NewL()
	{
	return new (ELeave) CDrawSection;
	}

/**
Creates and returns new drawsection.

@param aDrawRect The drawrect of the drawsection.
@param aBoundingRect The boundingrect of the drawcommands of this section.
@param aHasBitmapCommand ETrue if the commandbuffer of this drawsection includes a bitmap command of any sort.
@return A new instance of CDrawSection.
*/	
CDrawSection* CDrawSection::NewL(const TRect& aDrawRect, const TRect& aBoundingRect, TBool aHasBitmapCommand)
	{
	return new (ELeave) CDrawSection(aDrawRect, aBoundingRect, aHasBitmapCommand);
	}

CDrawSection::CDrawSection()
	{
	}

CDrawSection::CDrawSection(const TRect& aDrawRect, const TRect& aBoundingRect, TBool aHasBitmapCommand) : iDrawRect(aDrawRect), 
	iBoundingRect(aBoundingRect), iHasBitmapCommand(aHasBitmapCommand), iHasBeenExternalized(EFalse)
	{
	}
	
CDrawSection::~CDrawSection()
	{
	delete iSectionSegBuf;
	}

/**
Sets this drawsection's segmented commandbuffer and takes the ownership of the allocated memory.

@param aSegbuf This section's segmented commandbuffer.
*/
void CDrawSection::SetBuffer(CBufBase* aSegBuf)
	{
	__ASSERT_DEBUG(aSegBuf, User::Invariant());
	iSectionSegBuf = aSegBuf;
	}

/**
Returns this drawsection's segmented commandbuffer.

@return this drawsection's segmented commandbuffer.
*/
CBufBase* CDrawSection::Buffer() const
	{
	return iSectionSegBuf;
	}

/**
Sets if this drawsection has been externalized anytime. This is used for partial IPC updates.

@param aHasBeenExternalized ETrue if this drawsection has been externalized, otherwise EFalse.
*/
void CDrawSection::SetHasBeenExternalized(TBool aHasBeenExternalized)
	{
	iHasBeenExternalized = aHasBeenExternalized;
	}

/**
Returns ETrue if this drawsection has been externalized, otherwise EFalse.

@return ETrue if this drawsection has been externalized, otherwise EFalse.
*/	
TBool CDrawSection::HasBeenExternalized() const
	{
	return iHasBeenExternalized;
	}

/**
Compares this drawsection to aDrawSection and check if they are identical.

@param aDrawSection The drawsection to compare with.
@return ETrue if the drawsections are identical, otherwise EFalse.
*/
TBool CDrawSection::IsIdentical(const CDrawSection& aDrawSection) const
	{
	const TRect drawRect = aDrawSection.DrawRect();
	CBufBase* segBuf = aDrawSection.Buffer();
	if(!iHasBitmapCommand && iDrawRect.iTl == drawRect.iTl && iDrawRect.iBr == drawRect.iBr && segBuf->Size() == iSectionSegBuf->Size())
		{
		const TInt count = iSectionSegBuf->Size();
		for(TInt i = 0; i < count; i++)
			{
			const TPtr8 comparePtr = segBuf->Ptr(i);
			if(iSectionSegBuf->Ptr(i).CompareC(segBuf->Ptr(i))) //Not identical
				return EFalse;
			
			i += comparePtr.Size();
			}
		return ETrue;
		}
	return EFalse;
	}

/**
Returns the drawrect of this drawsection.

@return The drawrect of this drawsection.
*/
TRect CDrawSection::DrawRect() const
	{
	return iDrawRect;
	}

/**
Externalizes this drawsection to a message buffer to send over IPC.

@param aMsgBuf The message buffer to add this drawsection to.
*/
void CDrawSection::ExternalizeL(RWsGraphicMsgBuf& aMsgBuf)
	{
	const TInt length = sizeof(TRect)*2 + iSectionSegBuf->Size();
	TPtr8 msgPtr(NULL, 0);
	User::LeaveIfError(aMsgBuf.Append(KDrawSection, length, msgPtr));
	
	// Add drawRect to aMsgBuf
	const TPckgBuf<TRect> drawRect(iDrawRect);
	msgPtr.Append(drawRect);
	
	// Add boundingRect to aMsgBuf
	const TPckgBuf<TRect> boundingRect(iBoundingRect);
	msgPtr.Append(boundingRect);
	
	// Add drawcommands to aMsgBuf
	TInt count = 0;
	const TInt sectionSegBufSize = iSectionSegBuf->Size();
	while(count < sectionSegBufSize)
		{		
		TPtr8 ptr = iSectionSegBuf->Ptr(count);
		msgPtr.Append(ptr);
		count += ptr.Size();
		}	
	}

/**
Loads a drawsection from externalized data.

@param aData The parser for the externalized data.
@param aIndex The index of the drawsection in CCommandBuffer::iDrawSections to load from the externalized data.
*/
TInt CDrawSection::LoadL(const TWsGraphicMsgBufParser& aData, TInt aIndex)
	{
	if(aData.Uid(aIndex).iUid != KDrawSection.iUid)
		{
		return KErrArgument;
		}
		
	const TPtrC8 pckg = aData.Data(aIndex);	
	const TPtrC8 drawRectPtr = pckg.Left(sizeof(TRect));
	const TPtrC8 boundingRectPtr = pckg.Mid(sizeof(TRect), sizeof(TRect));
	const TPtrC8 segBufPtr = pckg.Mid(sizeof(TRect)*2);
	
	Mem::Copy(&iDrawRect, drawRectPtr.Ptr(), sizeof(TRect));
	Mem::Copy(&iBoundingRect, boundingRectPtr.Ptr(), sizeof(TRect));
	
	if(iSectionSegBuf)
		delete iSectionSegBuf;
	
	iSectionSegBuf = CBufSeg::NewL(KBufferSize);
	iSectionSegBuf->InsertL(0, segBufPtr);
	return KErrNone;	
	}
	

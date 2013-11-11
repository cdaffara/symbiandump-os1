// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <pdrstore.h>
#include "pdrtext.h"

const TInt KPageBufferSegmentSize=2048;

EXPORT_C CPageBuffer* CPageBuffer::NewL(CPrinterPort* aPrinterPort)
	{
	return NewL(aPrinterPort,KPageBufferSegmentSize);
	}

EXPORT_C CPageBuffer* CPageBuffer::NewL(CPrinterPort* aPrinterPort,TInt aGranularity)
	{
	CPageBuffer* pagebuffer=new(ELeave) CPageBuffer(aPrinterPort,aGranularity);
	CleanupStack::PushL(pagebuffer);
	pagebuffer->ConstructL();
	CleanupStack::Pop();
	return pagebuffer;
	}

EXPORT_C void CPageBuffer::StartFlush(TRequestStatus& aRequestStatus)	
	{		//  
	iRequestStatus=&aRequestStatus;
	aRequestStatus=KRequestPending;
	iWritePos=0;
	Queue();
	}

EXPORT_C void CPageBuffer::AddBytesL(const TDesC8& aDes)
	{
	iBuffer->InsertL(iBuffer->Size(),aDes);
	}

EXPORT_C RWriteStream& CPageBuffer::CreateWriteStreamL()
	{
	if (!iWriteStreamBuffer)
		{
		iWriteStreamBuffer=CBufSeg::NewL(iGranularity);
		iWriteStream.Open(*iWriteStreamBuffer,0);
		}
	else
		{
		iWriteStream.Truncate(*iWriteStreamBuffer,0);
		}
	return iWriteStream;
	}

EXPORT_C void CPageBuffer::CloseWriteStream()
	{
	iWriteStream.Close();
	CBufSeg* buf=iBuffer;
	iBuffer=iWriteStreamBuffer;
	iWriteStreamBuffer=buf;
	}

EXPORT_C CPageBuffer::~CPageBuffer()
	{
	Cancel();
	delete iBuffer;
	if (iWriteStreamBuffer)
		{
		iWriteStream.Close();
		delete iWriteStreamBuffer;
		}
	}

void CPageBuffer::DoCancel()
	{
	iPrinterPort->Cancel();	// clears my request
	if (iRequestStatus!=NULL)
		User::RequestComplete(iRequestStatus,KErrCancel); // clears my callers request
	}

void CPageBuffer::RunL()  //  Shouldn't leave in practice
	{	
	if ((iWritePos>=iBuffer->Size()) || (iStatus!=KErrNone))
		{
		iBuffer->Reset();
		if (iRequestStatus!=NULL)
			User::RequestComplete(iRequestStatus,iStatus.Int()); // clears my callers request
		}	   //  Reports back error if iStatus!=KErrNone
	else
		{
		Queue();
		}
	}

void CPageBuffer::ConstructL()
	{
	iBuffer=CBufSeg::NewL(iGranularity);
	CActiveScheduler::Add(this);
	}

CPageBuffer::CPageBuffer(CPrinterPort* aPrinterPort,TInt aGranularity):
	CActive(CActive::EPriorityLow),
	iRequestStatus(NULL),
	iWritePos(0),
	iGranularity(aGranularity),
	iBuffer(NULL),
	iPtr(NULL,0),
	iPrinterPort(aPrinterPort),
	iWriteStream(),
	iWriteStreamBuffer(NULL)
	{
	__DECLARE_NAME(_S("CPageBuffer"));
	}

void CPageBuffer::Queue()
	{
	TPtr8 ptr=iBuffer->Ptr(iWritePos);
	iPtr.Set(ptr);
	iPrinterPort->WriteRequest(iPtr,iStatus);
	iWritePos+=iPtr.Length();
	SetActive();
	}

/** 
  This function is internal only, and is not intended for use. 
  @internalTechnology
*/
EXPORT_C TTextFormat::TTextFormat():
	iUnderlineStyle(EUnderlineOff),
	iStrikethroughStyle(EStrikethroughOff),
	iColor(KRgbBlack),
	iFontString(_L8("")),
	iFontStyle()
	{
	}

EXPORT_C TTextFormat::TTextFormat(const TFontUnderline anUnderlineStyle,const TFontStrikethrough aStrikethroughStyle,const TRgb& aColor,const TDesC8& aFontString,const TFontStyle& aFontStyle):
	iUnderlineStyle(anUnderlineStyle),
	iStrikethroughStyle(aStrikethroughStyle),
	iColor(aColor),
	iFontString(aFontString),
	iFontStyle(aFontStyle)
	{
	}

EXPORT_C TBool TTextFormat::operator == (const TTextFormat& aFormat) const
	{
	return (iUnderlineStyle==aFormat.iUnderlineStyle) &&
	       (iStrikethroughStyle==aFormat.iStrikethroughStyle) &&
	       (!iFontString.Compare(aFormat.iFontString)) && 
	       (iFontStyle==aFormat.iFontStyle) && (iColor==aFormat.iColor);
	}

EXPORT_C CPageTextEntry::CPageTextEntry(const TPoint& aDrawPos,TInt aHeightInPixels,TInt aTextWidthInPixels,HBufC8* aText,TTextFormat* aTextFormat):
	iDrawPos(aDrawPos),
	iHeightInPixels(aHeightInPixels),
	iTextWidthInPixels(aTextWidthInPixels),
	iText(aText),
	iTextFormat(aTextFormat)
	{
	__DECLARE_NAME(_S("CPageTextEntry"));
	}

EXPORT_C CPageTextEntry::~CPageTextEntry()
	{
	delete iText;
	}

EXPORT_C TPoint CPageTextEntry::TopTextPos()
	{
	return iDrawPos-TPoint(0,iHeightInPixels);
	}

CPageText::CPageText()
	{
	}

void CPageText::ConstructL()
	{
	iTextFormatList = new(ELeave) CArrayPtrFlat<TTextFormat>(8);
	iPageTextEntryList = new(ELeave) CArrayPtrFlat<CPageTextEntry>(8);
	}

EXPORT_C CPageText* CPageText::NewL()
	{
	CPageText* pagetext = new(ELeave) CPageText;
	CleanupStack::PushL(pagetext);
	pagetext->ConstructL();
	CleanupStack::Pop();
	return pagetext;
	}

EXPORT_C CPageText::~CPageText()
	{
	Reset();
	delete iPageTextEntryList;
	delete iTextFormatList;
	}

EXPORT_C void CPageText::Reset()
	{
	if (iPageTextEntryList)
		iPageTextEntryList->ResetAndDestroy();
	if (iTextFormatList)
		iTextFormatList->ResetAndDestroy();
	}

EXPORT_C void CPageText::AddEntryL(const TPoint& aPoint,const TFontUnderline aUnderlineStyle,const TFontStrikethrough aStrikethroughStyle,const TRgb& aColor,const CInfoFont* aFont,const TDesC& aString)
	{
	TTextFormat textformat(aUnderlineStyle,aStrikethroughStyle,aColor,aFont->CommandString(),aFont->FontSpecInTwips().iFontStyle);
	TInt count=iTextFormatList->Count();
	TTextFormat* tf;
	TInt i;
	for (i = 0; ; i++)
		{
		if (i==count)
			{
			tf=new(ELeave) TTextFormat(textformat);
			CleanupStack::PushL(tf);
			iTextFormatList->AppendL(tf);
			CleanupStack::Pop();
			break;
			}
		tf=(*iTextFormatList)[i];
		if (textformat==*tf)
			break;
		}
	HBufC8* text=aFont->TranslateStringL(aString);
	CleanupStack::PushL(text);
	CPageTextEntry* textentry=new(ELeave) CPageTextEntry(aPoint+TPoint(0,aFont->BaselineOffsetInPixels()),aFont->HeightInPixels(),aFont->MeasureText(aString),text,tf);
	CleanupStack::Pop();

	i=0,count=iPageTextEntryList->Count();
	for (; (i<count) && (textentry->iDrawPos.iY>(*iPageTextEntryList)[i]->iDrawPos.iY); i++ )
		{
		}
	for (; (i<count) && (textentry->iDrawPos.iY==(*iPageTextEntryList)[i]->iDrawPos.iY) &&
	                    (textentry->iDrawPos.iX>(*iPageTextEntryList)[i]->iDrawPos.iX); i++)
		{
		}
	CleanupStack::PushL(textentry);
	iPageTextEntryList->InsertL(i,textentry);
	CleanupStack::Pop();
	if (textentry->iHeightInPixels>iMaxFontHeightInPixels)
		iMaxFontHeightInPixels=textentry->iHeightInPixels;
	}

EXPORT_C TInt CPageText::NumEntries()
	{
	return iPageTextEntryList->Count();
	}

EXPORT_C CPageTextEntry* CPageText::operator [] (TInt anIndex)
	{
	return (*iPageTextEntryList)[anIndex];
	}

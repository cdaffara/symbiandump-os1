// text_formatter.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32base.h>
#include "ioutils.h"
#include "pod_lexer.h"
#include "pod_formatter.h"

using namespace IoUtils;


//
// Constants.
//

_LIT(KSpace, " ");
_LIT(KNewLine, "\r\n");


//
// CTextBuffer.
//

EXPORT_C CTextBuffer* CTextBuffer::NewL(TInt aExpandSize)
	{
	CTextBuffer* self = CTextBuffer::NewLC(aExpandSize);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CTextBuffer* CTextBuffer::NewLC(TInt aExpandSize)
	{
	CTextBuffer* self = new(ELeave) CTextBuffer();
	CleanupStack::PushL(self);
	self->ConstructL(aExpandSize);
	return self;
	}

EXPORT_C CTextBuffer::~CTextBuffer()
	{
	Cancel();
	delete iBuf;
	delete iScratchBuf;
	delete iScratchBuf8;
	iAttributes.Close();
	}

EXPORT_C void CTextBuffer::Zero()
	{
	iBuf->Delete(0, iBuf->Size());
	iAttributes.Reset();
	}

EXPORT_C void CTextBuffer::Reset()
	{
	iBuf->Reset();
	iAttributes.Reset();
	}

EXPORT_C void CTextBuffer::ResetText()
	{
	iBuf->Reset();
	}

EXPORT_C void CTextBuffer::SetAttributesL(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor)
	{
	const TInt numAttributes = iAttributes.Count();
	if (numAttributes == 0)
		{
		if (Length() == 0)
			{
			TAttributes attributes(0, aAttributes, aForegroundColor, aBackgroundColor);
			iAttributes.AppendL(attributes);
			}
		else
			{
			TAttributes attributes(Length(), aAttributes, aForegroundColor, aBackgroundColor);
			if (!attributes.Matches(ConsoleAttributes::ENone, ConsoleAttributes::EUnchanged, ConsoleAttributes::EUnchanged))
				{
				TAttributes defaultAttributes(0, ConsoleAttributes::ENone, ConsoleAttributes::EUnchanged, ConsoleAttributes::EUnchanged);
				iAttributes.AppendL(defaultAttributes);
				TInt err = iAttributes.Append(attributes);
				if (err != KErrNone)
					{
					iAttributes.Reset();
					User::Leave(err);
					}
				}
			}
		}
	else if (iAttributes[numAttributes - 1].Matches(aAttributes, aForegroundColor, aBackgroundColor))
		{
		// Do nothing.
		}
	else if (iAttributes[numAttributes - 1].iPosition == Length())
		{
		TAttributes& att = iAttributes[numAttributes - 1];
		if (aAttributes & ConsoleAttributes::ENone)
			{
			att.iAttributes.iAttributes = aAttributes;
			att.iAttributes.iForegroundColor = aForegroundColor;
			att.iAttributes.iBackgroundColor = aBackgroundColor;
			}
		else
			{
			// Merge on top of existing attributes
			att.iAttributes.iAttributes |= aAttributes;
			if (aForegroundColor != ConsoleAttributes::EUnchanged) att.iAttributes.iForegroundColor = aForegroundColor;
			if (aBackgroundColor != ConsoleAttributes::EUnchanged) att.iAttributes.iBackgroundColor = aBackgroundColor;
			}
		}
	else
		{
		TAttributes attributes(Length(), aAttributes, aForegroundColor, aBackgroundColor);
		iAttributes.AppendL(attributes);
		}
	}

EXPORT_C void CTextBuffer::SetAttributesL(const ConsoleAttributes::TAttributes& aAttributes)
	{
	SetAttributesL(aAttributes.iAttributes, aAttributes.iForegroundColor, aAttributes.iBackgroundColor);
	}

EXPORT_C void CTextBuffer::GetCurrentAttributes(TUint& aAttributes, ConsoleAttributes::TColor& aForegroundColor, ConsoleAttributes::TColor& aBackgroundColor) const
	{
	if (iAttributes.Count() > 0)
		{
		const TAttributes& att = iAttributes[iAttributes.Count() - 1];
		aAttributes = att.iAttributes.iAttributes;
		aForegroundColor = att.iAttributes.iForegroundColor;
		aBackgroundColor = att.iAttributes.iBackgroundColor;
		}
	else
		{
		aAttributes = ConsoleAttributes::ENone;
		aForegroundColor = ConsoleAttributes::EUnchanged;
		aBackgroundColor = ConsoleAttributes::EUnchanged;
		}
	}

EXPORT_C void CTextBuffer::GetAttributes(TInt aPos, TUint& aAttributes, ConsoleAttributes::TColor& aForegroundColor, ConsoleAttributes::TColor& aBackgroundColor) const
	{
	ASSERT(aPos < Length());

	aAttributes = ConsoleAttributes::ENone;
	aForegroundColor = ConsoleAttributes::EUnchanged;
	aBackgroundColor = ConsoleAttributes::EUnchanged;

	const TInt numAttributes = iAttributes.Count();
	for (TInt i = 0; i < numAttributes; ++i)
		{
		const TAttributes& att = iAttributes[i];
		if (aPos < att.iPosition)
			{
			return;
			}
		else
			{
			aAttributes = att.iAttributes.iAttributes;
			aForegroundColor = att.iAttributes.iForegroundColor;
			aBackgroundColor = att.iAttributes.iBackgroundColor;
			}
		}
	}

EXPORT_C void CTextBuffer::AppendL(const TChar& aChar)
	{
	TUint16 ch = (TUint16)TUint(aChar); // We don't support surrogate pairs
	iBuf->InsertL(iBuf->Size(), &ch, 2);
	}

EXPORT_C void CTextBuffer::AppendL(const TDesC& aText)
	{
	iBuf->InsertL(iBuf->Size(), aText.Ptr(), aText.Size());
	}

EXPORT_C void CTextBuffer::AppendL(const TDesC8& aText)
	{
	if (!iScratchBuf)
		{
		iScratchBuf = HBufC::NewL(Max(aText.Length(), 256));
		}
	if (iScratchBuf->Des().MaxLength() < aText.Length())
		{
		iScratchBuf = iScratchBuf->ReAllocL(Max(iScratchBuf->Des().MaxLength()*2, aText.Length()));
		}
	iScratchBuf->Des().Copy(aText);	
	iBuf->InsertL(iBuf->Size(), iScratchBuf->Ptr(), iScratchBuf->Size());
	}

EXPORT_C void CTextBuffer::AppendL(const CTextBuffer& aText)
	{
	AppendL(aText, 0, aText.Length());
	}

EXPORT_C void CTextBuffer::AppendL(const CTextBuffer& aText, TInt aPosition)
	{
	AppendL(aText, aPosition, aText.Length() - aPosition);
	}

EXPORT_C void CTextBuffer::AppendL(const CTextBuffer& aText, TInt aPosition, TInt aLength)
	{
	const TInt numAttributes = aText.iAttributes.Count();
	if (numAttributes > 0)
		{
		TInt textPos = 0;
		TInt appendCount = 0;
		TInt blockIndex = 0;
		const TDesC* block = NULL;
		do
			{
			TUint attributes;
			ConsoleAttributes::TColor foregroundColor;
			ConsoleAttributes::TColor backgroundColor;
			aText.NextBlock(blockIndex, block, attributes, foregroundColor, backgroundColor);
			if (block != NULL)
				{
				if ((textPos + block->Length()) > aPosition)
					{
					SetAttributesL(attributes, foregroundColor, backgroundColor);
					TInt blockOffset = aPosition - textPos;
					if (blockOffset < 0)
						{
						blockOffset = 0;
						}
					TPtrC appendBlock(block->Mid(blockOffset, Min(aLength - appendCount, block->Length() - blockOffset)));
					AppendL(appendBlock);
					appendCount += appendBlock.Length();
					if (appendCount == aLength)
						{
						break;
						}
					}
				textPos += block->Length();
				}
			}
			while (block != NULL);
		}
	else
		{
		AppendL(aText.Descriptor().Mid(aPosition, aLength));
		}
	}

EXPORT_C void CTextBuffer::AppendFormatL(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	AppendFormatListL(aFmt, list);
	}

EXPORT_C void CTextBuffer::AppendFormatListL(const TDesC16& aFmt, VA_LIST& aList)
	{
	TOverflowLeave overflow;
	TInt err = KErrNone;
	do
		{
		TPtr ptr(iScratchBuf->Des());
		ptr.Zero();
		TRAP(err, ptr.AppendFormatList(aFmt, aList, &overflow));
		if (err == KErrOverflow)
			{
			iScratchBuf = iScratchBuf->ReAllocL(ptr.MaxLength() * 2);
			}
		}
		while (err == KErrOverflow);

	User::LeaveIfError(err);
	iBuf->InsertL(iBuf->Size(), iScratchBuf->Ptr(), iScratchBuf->Size());
	}
	
EXPORT_C void CTextBuffer::AppendFormatL(TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	AppendFormatListL(aFmt, list);
	}
	
EXPORT_C void CTextBuffer::AppendFormatListL(const TDesC8& aFmt, VA_LIST& aList)
	{
	TOverflowLeave8 overflow;
	TInt err = KErrNone;
	if (!iScratchBuf8)
		{
		iScratchBuf8 = HBufC8::NewL(0x100);
		}
		
	do
		{
		TPtr8 ptr(iScratchBuf8->Des());
		ptr.Zero();
		TRAP(err, ptr.AppendFormatList(aFmt, aList, &overflow));
		if (err == KErrOverflow)
			{
			iScratchBuf8 = iScratchBuf8->ReAllocL(ptr.MaxLength() * 2);
			}
		}
		while (err == KErrOverflow);

	User::LeaveIfError(err);
	
	TPtr8 ptr(iScratchBuf8->Des());
	if (iScratchBuf->Des().MaxLength() < ptr.Length())
		{
		iScratchBuf = iScratchBuf->ReAllocL(Max(iScratchBuf->Des().MaxLength()*2, ptr.Length()));
		}
	iScratchBuf->Des().Copy(*iScratchBuf8);	
	
	iBuf->InsertL(iBuf->Size(), iScratchBuf->Ptr(), iScratchBuf->Size());
	}

EXPORT_C void CTextBuffer::AppendHumanReadableSizeL(TInt aSize)
	{
	AppendHumanReadableSizeL(TInt64(aSize), EColumnAlignedRight);
	}

EXPORT_C void CTextBuffer::AppendHumanReadableSizeL(TInt64 aSize)
	{
	AppendHumanReadableSizeL(aSize, EColumnAlignedRight);
	}

EXPORT_C void CTextBuffer::AppendHumanReadableSizeL(TInt aSize, TAlignment aAlignment)
	{
	AppendHumanReadableSizeL(TInt64(aSize), aAlignment);
	}

EXPORT_C void CTextBuffer::AppendHumanReadableSizeL(TInt64 aSize, TAlignment aAlignment)
	{
	const TInt64 KB = 1024;
	const TInt64 MB = KB * KB;
	const TInt64 GB = MB * KB;
	const TInt64 TB = GB * KB;
	_LIT(KBytes, " B ");
	_LIT(KKilobytes, " KB");
	_LIT(KMegabytes, " MB");
	_LIT(KGigabytes, " GB");
	_LIT(KTerabytes, " TB");

	_LIT(KLeftWhole, "%- 7.0f");
	_LIT(KLeftFrac, "%- 7.2f");
	_LIT(KRightWhole, "%+ 7.0f");
	_LIT(KRightFrac, "%+ 7.2f");
	_LIT(KNormalWhole, "%.0f");
	_LIT(KNormalFrac, "%.2f");

	const TDesC* suff = &KBytes;
	TReal n = aSize;
	TInt64 factor = 1;

	TInt64 absSize = aSize;
	if (absSize < 0) absSize = -absSize;

	if (absSize >= TB)
		{
		suff = &KTerabytes;
		factor = TB;
		}
	else if (absSize >= GB)
		{
		suff = &KGigabytes;
		factor = GB;
		}
	else if (absSize >= MB)
		{
		suff = &KMegabytes;
		factor = MB;
		}
	else if (absSize >= KB)
		{
		suff = &KKilobytes;
		factor = KB;
		}

	n = n / (TReal)factor;
	TBool wholeNumUnits = (absSize & (factor-1)) == 0; // ie aSize % factor == 0

	const TDesC* fmt = NULL;
	if (aAlignment == EColumnAlignedLeft)
		{
		fmt = wholeNumUnits ? &KLeftWhole : &KLeftFrac;
		}
	else if (aAlignment == EColumnAlignedRight)
		{
		fmt = wholeNumUnits ? &KRightWhole : &KRightFrac;
		}
	else
		{
		fmt = wholeNumUnits ? &KNormalWhole : &KNormalFrac;
		}

	AppendFormatL(*fmt, n);
	AppendL(*suff);
	}

EXPORT_C void CTextBuffer::AppendSpacesL(TInt aCount)
	{
	ASSERT(aCount >= 0);
	while (aCount > 0)
		{
		AppendL(KSpace);
		--aCount;
		}
	}

EXPORT_C void CTextBuffer::Delete(TInt aPos, TInt aLength)
	{
	//for (TInt i = 0; i < iAttributes.Count(); i++)
	//	{
	//	RDebug::Printf("attribute %d: pos %d", i, iAttributes[i].iPosition);
	//	}

	// Find the first and last attributes that are inside the delete region.
	TInt firstAttribute = -1;
	TInt lastAttribute = -1;
	TInt numAttributes = iAttributes.Count();
	for (TInt i = (numAttributes - 1); i >= 0; --i)
		{
		const TAttributes& thisAttribute = iAttributes[i];
		if ((lastAttribute == -1) && (thisAttribute.iPosition >= aPos) && (thisAttribute.iPosition < aPos + aLength))
			{
			lastAttribute = i;
			}
		if (lastAttribute >= 0 && thisAttribute.iPosition >= aPos)
			{
			firstAttribute = i;
			}
		}
	//RDebug::Printf("firstAttribute=%d lastAttribute=%d", firstAttribute, lastAttribute);

	if ((lastAttribute > 0))
		{
		// Set the position of the last affected attribute to just after the deleted chunk.
		iAttributes[lastAttribute].iPosition = aPos + aLength;

		// Remove all but the last affected attribute. Need to keep the last affected attribute
		// because it has an impact on the formatting of text that appears after the deleted chunk.
		for (TInt i = (lastAttribute - 1); i >= firstAttribute; --i)
			{
			iAttributes.Remove(i);
			}
		}

	// Update the positions of attributes after the block being deleted.
	numAttributes = iAttributes.Count();
	for (TInt i = Max(0, firstAttribute); i < numAttributes; ++i)
		{
		TAttributes& thisAttribute = iAttributes[i];
		if (thisAttribute.iPosition >= aPos + aLength)
			{
			thisAttribute.iPosition -= aLength;
			}
		}
	iBuf->Delete(aPos * 2, aLength * 2);
	}

EXPORT_C TInt CTextBuffer::Length() const
	{
	return iBuf->Size() / 2;
	}

EXPORT_C const TDesC& CTextBuffer::Descriptor() const
	{
	TPtr8 narrowPtr(iBuf->Ptr(0));
	iPtr.Set((const TUint16 *)narrowPtr.Ptr(), narrowPtr.Length() / 2);
	return iPtr;
	}

EXPORT_C const TDesC& CTextBuffer::Descriptor(TInt aPos, TInt aLength) const
	{
	TPtr8 narrowPtr(iBuf->Ptr(aPos * 2));
	iPtr.Set((const TUint16 *)narrowPtr.Ptr(), aLength);
	return iPtr;
	}
	
EXPORT_C TPtrC8 CTextBuffer::Collapse()
	{
	TPtr8 narrowPtr(iBuf->Ptr(0));
	TPtr ptr((TUint16 *)narrowPtr.Ptr(), narrowPtr.Length() / 2, narrowPtr.Length() / 2);
	return ptr.Collapse();
	}

EXPORT_C TInt CTextBuffer::Write(RIoWriteHandle& aWriteHandle) const
	{
	return Write(aWriteHandle, 0, Length());
	}

EXPORT_C void CTextBuffer::Write(RIoWriteHandle& aWriteHandle, TRequestStatus& aStatus) const
	{
	Write(aWriteHandle, 0, Length(), aStatus);
	}

EXPORT_C TInt CTextBuffer::Write(RIoWriteHandle& aWriteHandle, TInt aPosition, TInt aLength) const
	{
	const TInt numAttributes = iAttributes.Count();
	if ((aWriteHandle.AttachedToConsole() > 0) && (numAttributes > 0))
		{
		// Write the text buffer to the console a block at a time, updating the console's attributes at the beginning of each block.
		TInt blockIndex = 0;
		TInt textPos = 0;
		TInt writeCount = 0;
		const TDesC* block = NULL;
		do
			{
			TUint attributes;
			ConsoleAttributes::TColor foregroundColor;
			ConsoleAttributes::TColor backgroundColor;
			NextBlock(blockIndex, block, attributes, foregroundColor, backgroundColor);
			if (block != NULL)
				{
				if ((textPos + block->Length()) > aPosition)
					{
					RIoConsoleWriteHandle(aWriteHandle).SetAttributes(attributes, foregroundColor, backgroundColor); // Ignore error - may not be supported.
					TInt blockOffset = aPosition - textPos;
					if (blockOffset < 0)
						{
						blockOffset = 0;
						}
					TPtrC toWrite(block->Mid(blockOffset, Min(aLength - writeCount, block->Length() - blockOffset)));
					TInt err = aWriteHandle.Write(toWrite);
					if (err)
						{
						return err;
						}
					writeCount += toWrite.Length();
					if (writeCount == aLength)
						{
						break;
						}
					}
				textPos += block->Length();
				}
			}
			while (block != NULL);
		}
	else
		{
		return aWriteHandle.Write(Descriptor().Mid(aPosition, aLength));
		}

	return KErrNone;
	}

EXPORT_C void CTextBuffer::Write(RIoWriteHandle& aWriteHandle, TInt aPosition, TInt aLength, TRequestStatus& aStatus) const
	{
	const TInt numAttributes = iAttributes.Count();
	if ((aWriteHandle.AttachedToConsole() > 0) && (numAttributes > 0))
		{
		// Write the text buffer asynchronously to the console a block at a time, updating the console's attributes at the beginning of each block.
		if (!IsAdded())
			{
			CActiveScheduler::Add(const_cast<CTextBuffer*>(this));
			}
		aStatus = KRequestPending;
		iWriteStatus = &aStatus;
		iAsyncBlockIndex = 0;
		iAsyncWritePos = 0;
		iAsyncWriteStartPos = aPosition;
		iAsyncWriteLength = aLength;
		iConsoleWriteHandle = aWriteHandle;
		AsyncWriteNextBlock();
		}
	else
		{
		aWriteHandle.Write(Descriptor().Mid(aPosition, aLength), aStatus);
		}
	}

CTextBuffer::CTextBuffer()
	: CActive(CActive::EPriorityStandard), iPtr(NULL, 0)
	{
	}

void CTextBuffer::ConstructL(TInt aExpandSize)
	{
	iBuf = CBufFlat::NewL(aExpandSize);
	iScratchBuf = HBufC::NewL(aExpandSize);
	}

void CTextBuffer::NextBlock(TInt& aBlockIndex, const TDesC*& aText, TUint& aAttributes, ConsoleAttributes::TColor& aForegroundColor, ConsoleAttributes::TColor& aBackgroundColor) const
	{
	if (iAttributes.Count() > 0)
		{
		if (aBlockIndex < iAttributes.Count())
			{
			const TAttributes& thisAttribute = iAttributes[aBlockIndex];
			aAttributes = thisAttribute.iAttributes.iAttributes;
			aForegroundColor = thisAttribute.iAttributes.iForegroundColor;
			aBackgroundColor = thisAttribute.iAttributes.iBackgroundColor;
			TInt length;
			if (aBlockIndex < (iAttributes.Count() - 1))
				{
				length = iAttributes[aBlockIndex + 1].iPosition - thisAttribute.iPosition;
				}
			else
				{
				length = Length() - thisAttribute.iPosition;
				}
			aText = &Descriptor(thisAttribute.iPosition, length);
			++aBlockIndex;
			}
		else
			{
			aText = NULL;
			}
		}
	else
		{
		// There are no attributes, so block zero is the whole buffer.
		ASSERT((aBlockIndex == 0) || (aBlockIndex == 1));
		if (aBlockIndex == 0)
			{
			aAttributes = ConsoleAttributes::ENone;
			aForegroundColor = ConsoleAttributes::EUnchanged;
			aBackgroundColor = ConsoleAttributes::EUnchanged;
			aText = &Descriptor(0, Length());
			++aBlockIndex;
			}
		else
			{
			aText = NULL;
			}
		}
	}

void CTextBuffer::AsyncWriteNextBlock() const
	{
	const TDesC* block = NULL;
	TUint attributes;
	ConsoleAttributes::TColor foregroundColor;
	ConsoleAttributes::TColor backgroundColor;
	NextBlock(iAsyncBlockIndex, block, attributes, foregroundColor, backgroundColor);
	if (block != NULL)
		{
		if ((iAsyncWritePos + block->Length()) > iAsyncWriteStartPos)
			{
			iConsoleWriteHandle.SetAttributes(attributes, foregroundColor, backgroundColor); // Ignore error - may not be supported.
			TInt blockOffset = iAsyncWriteStartPos - iAsyncWritePos;
			if (blockOffset < 0)
				{
				blockOffset = 0;
				}
			iAsyncWritePtr.Set(block->Mid(blockOffset, Min(iAsyncWriteLength, block->Length() - blockOffset)));
			iAsyncWriteLength -= iAsyncWritePtr.Length();
			iConsoleWriteHandle.Write(iAsyncWritePtr, const_cast<TRequestStatus&>(iStatus));
			const_cast<CTextBuffer*>(this)->SetActive();
			}
		iAsyncWritePos += block->Length();
		}
	else
		{
		User::RequestComplete(iWriteStatus, KErrNone);
		}
	}

void CTextBuffer::RunL()
	{
	if (iStatus.Int() || (iAsyncWriteLength <= 0))
		{
		User::RequestComplete(iWriteStatus, iStatus.Int());
		}
	else
		{
		AsyncWriteNextBlock();
		}
	}

void CTextBuffer::DoCancel()
	{
	iConsoleWriteHandle.WriteCancel();
	}

CTextBuffer::TAttributes::TAttributes(TInt aPosition, TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor)
	: iPosition(aPosition), iAttributes(aAttributes, aForegroundColor, aBackgroundColor)
	{
	}

TBool CTextBuffer::TAttributes::Matches(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor) const
	{
	return ((aAttributes == iAttributes.iAttributes) && (aForegroundColor == iAttributes.iForegroundColor) && (aBackgroundColor == iAttributes.iBackgroundColor));
	}


//
// CTextFormatter.
//

EXPORT_C CTextFormatter* CTextFormatter::NewL(TInt aAvailableWidth)
	{
	CTextFormatter* self = CTextFormatter::NewLC(aAvailableWidth);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CTextFormatter* CTextFormatter::NewLC(TInt aAvailableWidth)
	{
	CTextFormatter* self = new(ELeave) CTextFormatter(aAvailableWidth);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CTextFormatter* CTextFormatter::NewL(RIoConsoleWriteHandle& aConsoleWriteHandle)
	{
	CTextFormatter* self = CTextFormatter::NewLC(aConsoleWriteHandle);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CTextFormatter* CTextFormatter::NewLC(RIoConsoleWriteHandle& aConsoleWriteHandle)
	{
	TSize size;
	if (aConsoleWriteHandle.AttachedToConsole())
		{
		User::LeaveIfError(aConsoleWriteHandle.GetScreenSize(size));
		}
	else
		{
		size.iWidth = 80;
		}
	CTextFormatter* self = CTextFormatter::NewLC(size.iWidth);
	self->iWriteHandle = &aConsoleWriteHandle;
	self->iAttributesSupported = (aConsoleWriteHandle.SetAttributes(0) == KErrNone);
	return self;
	}

EXPORT_C CTextFormatter::~CTextFormatter()
	{
	}

CTextFormatter::CTextFormatter(TInt aAvailableWidth)
	: iAvailableWidth(aAvailableWidth - 1)
	{
	}

void CTextFormatter::ConstructL()
	{
	CTextBuffer::ConstructL(0x100);
	}

EXPORT_C void CTextFormatter::WrapL(TInt aIndent, const TDesC& aText)
	{
	AppendSpacesL(aIndent);
	WrapL(aIndent, aIndent, aText);
	}

EXPORT_C void CTextFormatter::WrapL(TInt aStartPosition, TInt aIndent, const TDesC& aText)
	{
	TUint originalAttributes = ConsoleAttributes::ENone;
	ConsoleAttributes::TColor foregroundColor = ConsoleAttributes::EUnchanged;
	ConsoleAttributes::TColor backgroundColor = ConsoleAttributes::EUnchanged;
	if (iAttributesSupported)
		{
		GetCurrentAttributes(originalAttributes, foregroundColor, backgroundColor);
		}

	CTextBuffer* buffer = CTextBuffer::NewLC(0x100);
	DecodeInteriorPodSequencesL(aText, *buffer);
	TLex lex(buffer->Descriptor());
	TInt linePos = aStartPosition;
	while (!lex.Eos())
		{
		lex.Mark();
		lex.SkipSpace();
		TPtrC whiteSpace(lex.MarkedToken());
		if (whiteSpace.Length() > 0)
			{
			TInt newLinePos;
			do
				{
				newLinePos = whiteSpace.Find(KNewLine);
				if (newLinePos >= 0)
					{
					AppendSpacesL(iAvailableWidth - linePos - 1);
					AppendL(KNewLine);
					AppendSpacesL(aIndent);
					linePos = aIndent;
					whiteSpace.Set(whiteSpace.Mid(newLinePos + KNewLine().Length()));
					}
				}
				while (newLinePos >= 0);

			if (iAvailableWidth < whiteSpace.Length())
				{
				AppendSpacesL(iAvailableWidth - linePos - 1);
				AppendL(KNewLine);
				AppendSpacesL(aIndent);
				linePos = aIndent;
				}
			else
				{
				AppendL(whiteSpace);
				linePos += whiteSpace.Length();
				}
			}

		lex.Mark();
		lex.SkipCharacters();
		TPtrC word(lex.MarkedToken());
		const TInt wordLength = word.Length();
		if (wordLength > 0)
			{
			if ((linePos + wordLength + KNewLine().Length()) > iAvailableWidth)
				{
				AppendL(KNewLine);
				AppendSpacesL(aIndent);
				linePos = aIndent;
				}

			if (iAttributesSupported)
				{
				for (TInt i = 0; i < wordLength; ++i)
					{
					TUint attributes;
					buffer->GetAttributes(lex.Offset() - wordLength + i, attributes, foregroundColor, backgroundColor);
					SetAttributesL(originalAttributes | attributes, foregroundColor, backgroundColor);
					AppendL(word[i]);
					}
				}
			else
				{
				AppendL(word);
				}
			linePos += wordLength;
			}
		}

	CleanupStack::PopAndDestroy(buffer);
	SetAttributesL(originalAttributes, foregroundColor, backgroundColor);
	}

EXPORT_C void CTextFormatter::TabulateL(TInt aIndent, TInt aGap, const TDesC& aText)
	{
	TabulateL(aIndent, aGap, aText, EWrapLastColumn);
	}

EXPORT_C void CTextFormatter::TabulateL(TInt aIndent, TInt aGap, const TDesC& aText, TTabMode aMode)
	{
	CTextBuffer* scratchBuffer = NULL;
	RArray<TInt> columnWidths;
	CleanupClosePushL(columnWidths);
	TLex lineLexer(aText);

	// Find column widths.
	TPtrC line;
	while (NextLine(lineLexer, line))
		{
		TLex columnLexer(line);
		TInt col = 0;
		TPtrC column;
		while (NextColumn(columnLexer, column))
			{
			if (col >= columnWidths.Count())
				{
				User::LeaveIfError(columnWidths.Append(aGap));
				}
			TInt thisColumnWidth = ActualLength(column);
			if (columnWidths[col] < (thisColumnWidth + aGap))
				{
				columnWidths[col] = (thisColumnWidth + aGap);
				}
			++col;
			}
		}

	const TInt numCols = columnWidths.Count();

	if (numCols == 0)
		{
		// No columns found, so just copy aText directly.
		AppendL(aText);
		CleanupStack::PopAndDestroy(&columnWidths);
		return;
		}

	if (aMode == EWrapLastColumn)
		{
		// Find the widest of all but the last column.
		TInt sum = aIndent;
		for (TInt i = 0; i < (numCols - 1); ++i)
			{
			sum += columnWidths[i];
			}
		TInt remainingSpace = iAvailableWidth - sum;
		if (remainingSpace < 0)
			{
			// If you hit this, it means that the text cannot be columnised because the columns preceding the last one take up more space than the overall.
			User::Leave(KErrTooBig); // Let's go for something marginally more descriptive than KErrGeneral
			}
		// Assign the remaining space to the last column (which will be wrapped).
		columnWidths[numCols - 1] = remainingSpace; 
		}
	else if (aMode == ETruncateLongestColumn)
		{
		// Find the total width of all the columns.
		TInt sum = aIndent;
		for (TInt i = 0; i < numCols; ++i)
			{
			sum += columnWidths[i];
			}
		TInt excess = iAvailableWidth - sum;
		if (excess < 0)
			{
			// Not enough space, so steal from the widest column until there is.
			while (excess < 0)
				{
				TInt widestColumn = -1;
				TInt widestColumnWidth = 0;
				for (TInt i = 0; i < numCols; ++i)
					{
					if (columnWidths[i] > widestColumnWidth)
						{
						widestColumnWidth = columnWidths[i];
						widestColumn = i;
						}
					}
				if (widestColumn < 0)
					{
					User::Leave(KErrGeneral);
					}
				--columnWidths[widestColumn];
				++excess;
				}
			}
		else
			{
			// Assign execess to the last column.
			columnWidths[numCols - 1] += excess; 
			}
		}
	else if (aMode == EIgnoreAvailableWidth)
		{
		// just use the column widths we've already worked out, no matter if they're too wide.
		}
	else
		{
		ASSERT(EFalse);
		}

	// Write formatted text to the buffer.
	lineLexer = TLex(aText);
	TInt linePos = 0;
	while (NextLine(lineLexer, line))
		{
		AppendSpacesL(aIndent);
		linePos += aIndent;
		TLex columnLexer(line);
		TInt col = 0;
		TPtrC column;
		while (NextColumn(columnLexer, column))
			{
			TBool isLastColumn(col == (numCols - 1));
			const TInt columnWidth = columnWidths[col];
			if ((aMode == ETruncateLongestColumn) || !isLastColumn)
				{
				if (scratchBuffer == NULL)
					{
					scratchBuffer = CTextBuffer::NewLC(0x100);
					}
				else
					{
					scratchBuffer->Zero();
					}
				DecodeInteriorPodSequencesL(column, *scratchBuffer);
				TInt availableWidth = columnWidth;
				if (!isLastColumn)
					{
					availableWidth -= aGap;
					}
				if (scratchBuffer->Descriptor().Length() > availableWidth)
					{
					scratchBuffer->Delete(availableWidth, scratchBuffer->Descriptor().Length() - availableWidth);
					}
				AppendL(*scratchBuffer);
				AppendSpacesL(columnWidth - scratchBuffer->Descriptor().Length());
				linePos += columnWidth;
				}
			else if (aMode == EIgnoreAvailableWidth)
				{
				AppendL(column);
				if (!isLastColumn)
					{
					AppendSpacesL(columnWidth - column.Length());
					}
				linePos += columnWidth;
				}
			else
				{
				WrapL(linePos, linePos, column);
				}
			++col;
			}
		AppendL(KNewLine);
		linePos = 0;
		}

	if (scratchBuffer)
		{
		CleanupStack::PopAndDestroy(scratchBuffer);
		}
	CleanupStack::PopAndDestroy(&columnWidths);
	}

EXPORT_C void CTextFormatter::ColumnizeL(TInt aIndent, TInt aGap, const TDesC& aText)
	{
	// Store the tab delimited text as an array of TPtrCs.
	RArray<TPtrC> items;
	CleanupClosePushL(items);
	TLex lex(aText);
	TPtrC column;
	while (NextColumn(lex, column))
		{
		User::LeaveIfError(items.Append(column));
		}
	ColumnizeL(aIndent, aGap, items.Array());
	CleanupStack::PopAndDestroy(1, &items);
	}

EXPORT_C void CTextFormatter::ColumnizeL(TInt aIndent, TInt aGap, const TArray<TPtrC>& aItems)
	{
	// Lays out tab delimited data as a set of columns that read like a news paper
	// (i.e. starting with the left most column, you read down to the bottom of it
	// and then start reading the next column from the top, etc.).

	// Determine the smallest number of rows that allows all the data to fit
	// the available horizontal space. The algorithm works by iteratively increasing
	// the number of rows from one until a fit is found.
	const TInt numItems = aItems.Count();
	RArray<TInt> columnWidths;
	CleanupClosePushL(columnWidths);
	TInt numRows = 0;
	TBool fits(EFalse);
	while (!fits)
		{
		++numRows;
		columnWidths.Reset();
		TInt requiredWidth = aIndent;
		TInt col = 0;
		TInt i = 0;
		while (i < numItems)
			{
			for (TInt row = 0; row < numRows; ++row)
				{
				if (columnWidths.Count() <= col)
					{
					User::LeaveIfError(columnWidths.Append(0));
					}
				TInt thisItemWidth = ActualLength(aItems[i]);
				if (thisItemWidth > columnWidths[col])
					{
					columnWidths[col] = thisItemWidth;
					}
				if (++i >= numItems)
					{
					break;
					}
				}
			requiredWidth += columnWidths[col];
			if ((requiredWidth + aGap) > iAvailableWidth)
				{
				if ((requiredWidth + KNewLine().Length()) > iAvailableWidth)
					{
					if (col > 0)
						{
						// Not enough space with this number of rows, so try another row.
						i = 0;
						break;
						}
					else
						{
						// Not enough space even with just one column. Lay out as a single column anyway and accept ugly wrapping by the console.
						CleanupStack::PopAndDestroy(1, &columnWidths);
						for (TInt i = 0; i < numItems; ++i)
							{
							AppendSpacesL(aIndent);
							AppendL(aItems[i]);
							AppendL(KNewLine);
							}
						return;
						}
					}
				else
					{
					requiredWidth += KNewLine().Length();
					}
				}
			else
				{
				requiredWidth += aGap;
				}
			++col;
			}
		if (i >= numItems)
			{
			fits = ETrue;
			}
		}

	// Layout the columns.
	const TInt numCols = columnWidths.Count();
	for (TInt row = 0; row < numRows; ++row)
		{
		AppendSpacesL(aIndent);
		for (TInt col = 0; col < numCols; ++col)
			{
			TInt index = (numRows * col) + row;
			if (index < numItems)
				{
				TPtrC item(aItems[index]);
				DoAppendPodL(item);
				TInt gap = (col == (numCols - 1)) ? 0 : aGap;
				AppendSpacesL((columnWidths[col] + gap) - ActualLength(item));
				}
			}
		AppendL(KNewLine);
		}

	CleanupStack::PopAndDestroy(1, &columnWidths);
	}

EXPORT_C void CTextFormatter::AppendPodL(const TDesC& aPod)
	{
	TPodFormatter podFormatter(*this);
	podFormatter.FormatL(aPod);
	}

EXPORT_C TInt CTextFormatter::Write()
	{
	return CTextBuffer::Write(*iWriteHandle);
	}

void CTextFormatter::DoAppendPodL(const TDesC& aPod)
	{
	DecodeInteriorPodSequencesL(aPod, *this);
	}

void CTextFormatter::DecodeInteriorPodSequencesL(const TDesC& aPod, CTextBuffer& aBuffer) const
	{
	TPodLexer lexer(aPod);
	TBool eop(EFalse);
	TBool eos(EFalse);

	TUint originalAttributes = ConsoleAttributes::ENone;
	ConsoleAttributes::TColor foregroundColor = ConsoleAttributes::EUnchanged;
	ConsoleAttributes::TColor backgroundColor = ConsoleAttributes::EUnchanged;
	if (iAttributesSupported)
		{
		aBuffer.GetCurrentAttributes(originalAttributes, foregroundColor, backgroundColor);
		}
	TUint currentAttributes = originalAttributes;

	while (!eos)
		{
		TPtrC token;
		TPodLexer::TTokenType tokenType;
		TUint attributes;
		lexer.NextTokenL(token, tokenType, attributes, eop, eos);

		switch (tokenType)
			{
			case TPodLexer::EAttributePush:
			case TPodLexer::EAttributePop:
				if (!iAttributesSupported && (attributes & TPodLexer::EBold))
					{
					aBuffer.AppendL('*');
					}
				else if (!iAttributesSupported && (attributes & TPodLexer::EItalic))
					{
					aBuffer.AppendL('\'');
					}
				else if ((!iAttributesSupported && (attributes & TPodLexer::EFileName)) || (attributes & TPodLexer::TPodLexer::ECode))
					{
					aBuffer.AppendL('"');
					}
				break;
			case TPodLexer::ELink:
				{
				TInt pos = token.Locate('|');
				if (pos >= 0)
					{
					token.Set(token.Left(pos));
					}
				}
				// Deliberate fall through.
			case TPodLexer::ETextBlock:
			case TPodLexer::ECodeBlock:
			case TPodLexer::EIndexEntry:
				if (iAttributesSupported)
					{
					if ((attributes == 0) || (attributes & (TPodLexer::ENull | TPodLexer::ECode)))
						{
						currentAttributes = originalAttributes;
						}
					if (attributes & (TPodLexer::EBold | TPodLexer::EItalic | TPodLexer::EFileName))
						{
						currentAttributes |= ConsoleAttributes::EBold;
						}
					aBuffer.SetAttributesL(currentAttributes);
					}
				aBuffer.AppendL(token);
				break;
			default:
				ASSERT(EFalse);
			}
		}

	if (iAttributesSupported && (currentAttributes != originalAttributes))
		{
		aBuffer.SetAttributesL(originalAttributes);
		}
	}

EXPORT_C void CTextFormatter::Zero()
	{
	CTextBuffer::Zero();
	}

EXPORT_C void CTextFormatter::Reset()
	{
	CTextBuffer::Reset();
	}

EXPORT_C void CTextFormatter::ResetText()
	{
	CTextBuffer::ResetText();
	}

TBool CTextFormatter::NextColumn(TLex& aLex, TPtrC& aPtr) const
	{
	aLex.Mark();
	while (!aLex.Eos())
		{
		if (aLex.Get() == '\t')
			{
			aLex.UnGet();
			aPtr.Set(aLex.MarkedToken());
			aLex.Get();
			return ETrue;
			}
		}
	if (aLex.TokenLength() > 0)
		{
		aPtr.Set(aLex.MarkedToken());
		return ETrue;
		}
	return EFalse;
	}

TBool CTextFormatter::NextLine(TLex& aLex, TPtrC& aPtr) const
	{
	aLex.Mark();
	while (!aLex.Eos())
		{
		TChar ch = aLex.Get();
		if (ch == '\r')
			{
			if (aLex.Get() == '\n')
				{
				aLex.UnGet();
				aLex.UnGet();
				aPtr.Set(aLex.MarkedToken());
				aLex.Get();
				aLex.Get();
				return ETrue;
				}
			else
				{
				aLex.UnGet();
				}
			}
		}
	if (aLex.TokenLength() > 0)
		{
		aPtr.Set(aLex.MarkedToken());
		return ETrue;
		}
	return EFalse;
	}

TInt CTextFormatter::ActualLength(const TDesC& aPod) const
	{
	CTextBuffer* buffer = CTextBuffer::NewLC(0x100);
	DecodeInteriorPodSequencesL(aPod, *buffer);
	TInt length = buffer->Descriptor().Length();
	CleanupStack::PopAndDestroy(buffer);
	return length;
	}

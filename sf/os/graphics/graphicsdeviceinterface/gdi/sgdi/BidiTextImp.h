// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BIDITEXTIMP_H_
#define BIDITEXTIMP_H_

#include "BidiText.h"

class TRunInfoCompact;

class TBidiTextImp : public TBidiText
/** This is the implementation for the class TBidiText. It is so designed so 
that it only occupies a single heap cell. This means that it is efficient in 
memory usage, and can be placed on the cleanup stack without being a C-class.
@internalComponent */
	{
public:
	static TBidiTextImp* NewL(TInt aLength, TInt aMaxLines, TInt aBdRunArraySize);
	static TBidiTextImp* Imp(TBidiText* aSelf);
	static const TBidiTextImp* Imp(const TBidiText* aSelf);
	static TInt AlignedSizeOf();
	TText* LogicalText();
	TText* VisualText();
	TInt16* LineWidthArray();
	TRunInfoCompact* BidiRunArray();
	const TText* LogicalText() const
		{ return const_cast<TBidiTextImp*>(this)->LogicalText(); }
	const TText* VisualText() const
		{ return const_cast<TBidiTextImp*>(this)->VisualText(); }
	const TInt16* LineWidthArray() const
		{ return const_cast<TBidiTextImp*>(this)->LineWidthArray(); }
	const TRunInfoCompact* BidiRunArray() const
		{ return const_cast<TBidiTextImp*>(this)->BidiRunArray(); }
	/**
	@return Current truncation character.
	@internalComponent */
	TChar TruncationChar() const
		{ return static_cast<TInt>(iTruncationCharPlane << 16)
		+ iTruncationChar16; }
	
	inline TInt TextLength() const;
	inline void SetTextLength(TInt aLength);
	inline TBool HasRightToLeftDirectionality() const;
	inline void SetRightToLeftDirectionality(TBool aRightToLeftDirectionality);
			
	static TInt RequiredBytes(TInt aLendth, TInt aMaxLines, TInt aBdRunArraySize);
	inline TInt AllocatedTextDataBytes() const;
	
private:
	inline void SetAllocatedTextDataBytes(TInt aMaxBytes);
	enum TFlags
		{
		EFRightToLeft = 0x10000000,
		EFTruncateAtBeginning = 0x20000000
		};

	enum { ELengthMask = 0x0FFFFFFF };
	TInt iTextLengthAndFlags;
	TInt iTextDataBytes;	// Number of bytes allocated off the end of the object for text data
public:	
	TInt iVisualOrderedTextLength;
	TUint16 iWrappingWidth;
	TUint16 iBidiRunArrayLength;
	TUint8 iLines;
	TUint8 iTruncationCharPlane;
	TUint16 iTruncationChar16;
	};

TInt SizeLineBreak(const TText* aText);
const TText* FindEndOfThisLine(const TText* aStart, const TText* aEnd);
TInt NumberOfLines(const TText* aStart, const TText* aEnd);

inline TBidiTextImp* TBidiTextImp::Imp(TBidiText* aSelf)
	{ return static_cast<TBidiTextImp*>(aSelf); }

inline const TBidiTextImp* TBidiTextImp::Imp(const TBidiText* aSelf)
	{ return static_cast<const TBidiTextImp*>(aSelf); }

inline TInt TBidiTextImp::AlignedSizeOf()
	{ return ((sizeof(TBidiTextImp) + 1) & 0xFFFFFFFE); }

inline TInt TBidiTextImp::TextLength() const
	{ return iTextLengthAndFlags & TBidiTextImp::ELengthMask; }
	
inline void TBidiTextImp::SetTextLength(TInt aLength)
	{ iTextLengthAndFlags &= ~ELengthMask;					// clear old size bits
	  iTextLengthAndFlags |= (aLength&ELengthMask); }		// set new size bits

inline TBool TBidiTextImp::HasRightToLeftDirectionality() const
	{ return iTextLengthAndFlags&TBidiTextImp::EFRightToLeft; }
	
inline void TBidiTextImp::SetRightToLeftDirectionality(TBool aRightToLeftDirectionality)
	{
	if (aRightToLeftDirectionality)
		iTextLengthAndFlags |= TBidiTextImp::EFRightToLeft;
	else
		iTextLengthAndFlags &= ~TBidiTextImp::EFRightToLeft;
	}
				
inline TInt TBidiTextImp::AllocatedTextDataBytes() const
	{ return iTextDataBytes; }

inline void TBidiTextImp::SetAllocatedTextDataBytes(TInt aTextDataBytes)
	{ iTextDataBytes = aTextDataBytes; }
	

/**
Bidi panic.
@internalComponent
*/
void Panic(TInt aError);

#endif

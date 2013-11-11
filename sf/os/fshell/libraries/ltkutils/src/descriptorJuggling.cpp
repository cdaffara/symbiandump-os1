// descriptorJuggling.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <fshell/ltkutils.h>
#include <fshell/descriptorutils.h>

#include <u32std.h>
const TUint16 KReplacementChar = 0xFFFD;
const TUint16 KBom = 0xFEFF;
_LIT8(KReplacementCharInUtf8, "\xEF\xBF\xBD");
using namespace LtkUtils;

// This class is used to track state between invocations of AppendUtf8L().
namespace LtkUtils
	{
	class TUtf8State
		{
	public:
		TUtf8State();

	public:
		TInt iFirstUnconvertedByte;
		TInt iBytesConsumedSoFar;
		TUint8 iUnconvertedBytesCount;
		TUint8 iUnconvertedBytes[3]; // A UTF-8 sequence is a maximum of 4 bytes long so there can only be a max of 3 bytes unconverted
		};
	} // end namespace LtkUtils

const TInt KStateCharCount = sizeof(LtkUtils::TUtf8State) / sizeof(TUint16);
__ASSERT_COMPILE((sizeof(LtkUtils::TUtf8State) & 3) == 0);
__ASSERT_COMPILE(KStateCharCount == 6); // Just checking I've done my maths right, it doesn't particularly have to be this size

EXPORT_C TUint LtkUtils::HexLexL(TLex16& aLex)
	{
	TUint result;
	User::LeaveIfError(HexLex(aLex, result));
	return result;
	}

EXPORT_C TInt LtkUtils::HexLex(TLex16& aLex, TUint& aResult)
	{
	_LIT(KHexPrefix, "0x");
	if (aLex.Remainder().Length() > 2)
		{
		TLexMark16 mark;
		aLex.Mark(mark);
		aLex.Inc(2);
		TPtrC prefix(aLex.MarkedToken(mark));
		if (prefix == KHexPrefix)
			{
			TInt err = aLex.Val(aResult, EHex);
			if (err) aLex.UnGetToMark(mark); // Make sure we leave it at the original position if error
			return err;
			}
		else
			{
			aLex.UnGetToMark(mark);
			}
		}
	return aLex.Val(aResult, EDecimal);
	}

EXPORT_C TUint LtkUtils::HexLexL(TLex8& aLex)
	{
	TUint result;
	User::LeaveIfError(HexLex(aLex, result));
	return result;
	}

EXPORT_C TInt LtkUtils::HexLex(TLex8& aLex, TUint& aResult)
	{
	_LIT8(KHexPrefix, "0x");
	if (aLex.Remainder().Length() > 2)
		{
		TLexMark8 mark;
		aLex.Mark(mark);
		aLex.Inc(2);
		TPtrC8 prefix(aLex.MarkedToken(mark));
		if (prefix == KHexPrefix)
			{
			TInt err = aLex.Val(aResult, EHex);
			if (err) aLex.UnGetToMark(mark); // Make sure we leave it at the original position if error
			return err;
			}
		else
			{
			aLex.UnGetToMark(mark);
			}
		}
	return aLex.Val(aResult, EDecimal);
	}

EXPORT_C TInt LtkUtils::ReplaceText(TDes& aDescriptor, const TDesC& aFrom, const TDesC& aTo)
	{
	TInt numReplaced = 0;
	TInt pos = 0;
	const TInt lenDelta = -aFrom.Length() + aTo.Length();
	while (ETrue)
		{
		TPtrC des(aDescriptor.Mid(pos));
		TInt found = des.Find(aFrom);
		if (found == KErrNotFound) break;
		TInt idx = pos + found;
		if (aDescriptor.Length() + lenDelta > aDescriptor.MaxLength()) return KErrOverflow;
		aDescriptor.Replace(idx, aFrom.Length(), aTo);
		pos = idx + aTo.Length();
		numReplaced++;
		}
	return numReplaced;
	}

void EnsureCapacityL(HBufC*& aBuf, TInt aExtra)
	{
	if (aBuf->Length() + aExtra > aBuf->Des().MaxLength())
		{
		aBuf = aBuf->ReAllocL(aBuf->Length() + aExtra);
		}
	}

EXPORT_C TBool LtkUtils::HasPrefix(const TDesC16& aDes, const TDesC16& aPrefix)
	{
	return aDes.Left(aPrefix.Length()) == aPrefix;
	}

EXPORT_C TBool LtkUtils::HasPrefix(const TDesC8& aDes, const TDesC8& aPrefix)
	{
	return aDes.Left(aPrefix.Length()) == aPrefix;
	}

#define KB *1024
#define MB *1024*1024
#define GB *1024*1024*1024

EXPORT_C void LtkUtils::FormatSize(TDes16& aDes, TInt64 aSize)
	{
	_LIT(KBytes, " B");
	_LIT(KKilobytes, " KB");
	_LIT(KMegabytes, " MB");
	_LIT(KGigabytes, " GB");

	const TDesC* suff = &KBytes;
	TReal n = aSize;
	TInt factor = 1;

	TInt64 absSize = aSize;
	if (absSize < 0) absSize = -absSize;

	if (absSize >= 1 GB)
		{
		suff = &KGigabytes;
		factor = 1 GB;
		}
	else if (absSize >= 1 MB)
		{
		suff = &KMegabytes;
		factor = 1 MB;
		}
	else if (absSize >= 1 KB)
		{
		suff = &KKilobytes;
		factor = 1 KB;
		}

	n = n / (TReal)factor;
	TBool wholeNumUnits = (absSize & (factor-1)) == 0; // ie aSize % factor == 0

	TRealFormat fmt(aDes.MaxLength(), wholeNumUnits ? 0 : 2);
	aDes.Num(n, fmt);
	aDes.Append(*suff);
	}


EXPORT_C void LtkUtils::FormatSize(TDes8& aDes, TInt64 aSize)
	{
	TBuf<32> buf;
	FormatSize(buf, aSize);
	aDes.Copy(buf);
	}



// RLtkBuf follows

inline TDesC16::TDesC16(TInt aType,TInt aLength)
	:iLength(aLength|(aType<<KShiftDesType16))
	{}

inline TDes16::TDes16(TInt aType,TInt aLength,TInt aMaxLength)
	: TDesC16(aType,aLength),iMaxLength(aMaxLength)
	{}

EXPORT_C LtkUtils::RLtkBuf16::RLtkBuf16()
	: TDes16(EPtr,0,0), iBuf(NULL)
	{
	}

EXPORT_C LtkUtils::RLtkBuf16::RLtkBuf16(HBufC16* aBuf)
	{
	if(aBuf)
		{
		//Create EBufCPtr type descriptor that points to aHBuf
		// I'll take base_e32's word for it that this does the right thing...
		new(this) TPtr16(aBuf->Des());
		}
	else
		{
		//Create zero-length RBuf16. It is EPtr type of descriptor that points to NULL.
		new(this) RLtkBuf16();
		}
	}

EXPORT_C HBufC16* LtkUtils::RLtkBuf16::ToHBuf()
	{
	// This transfers ownership
	HBufC16* result = iBuf;
	Assign(NULL);
	return result;
	}

EXPORT_C HBufC16* LtkUtils::RLtkBuf16::GetHBuf() const
	{
	// This doesn't transfer ownership
	return iBuf;
	}

EXPORT_C TInt LtkUtils::RLtkBuf16::ReAlloc(TInt aMaxLength)
	{
	if (!aMaxLength) // Reallocation to zero length
		{
		Close();
		return KErrNone;
		}

	if (!iBuf) // Reallocation from zero length
		return Create(aMaxLength); 

	// Need to maintain the UTF-8 state trickery
	TUtf8State* oldState = GetUtf8State();
	if (oldState)
		{
		TUtf8State state = *oldState;
		HBufC16* newbuf = iBuf->ReAlloc(aMaxLength + KStateCharCount);
		if (!newbuf) return KErrNoMemory;
		Assign(newbuf); // This will set iMaxLength
		iMaxLength = iMaxLength - KStateCharCount;
		Mem::Copy((TAny*)(Ptr() + MaxLength()), &state, sizeof(TUtf8State));
		}
	else
		{
		HBufC16* newbuf = iBuf->ReAlloc(aMaxLength);
		if (!newbuf) return KErrNoMemory;
		Assign(newbuf);
		}
	return KErrNone;
	}

EXPORT_C void LtkUtils::RLtkBuf16::ReAllocL(TInt aMaxLength)
	{
	User::LeaveIfError(ReAlloc(aMaxLength));
	}

EXPORT_C void LtkUtils::RLtkBuf16::Assign(HBufC16* aBuf)
	{
	new(this) RLtkBuf16(aBuf);
	}

EXPORT_C TInt LtkUtils::RLtkBuf16::Create(TInt aMaxLength)
	{
	if (aMaxLength)
		{
		HBufC16* buf = HBufC16::New(aMaxLength);
		if (!buf) return KErrNoMemory;
		Assign(buf);
		}
	else
		{
		Assign(NULL);
		}
	return KErrNone;
	}

EXPORT_C void LtkUtils::RLtkBuf16::CreateL(TInt aMaxLength)
	{
	User::LeaveIfError(Create(aMaxLength));
	}

EXPORT_C void LtkUtils::RLtkBuf16::CreateLC(TInt aMaxLength)
	{
	CleanupClosePushL(*this);
	User::LeaveIfError(Create(aMaxLength));
	}

EXPORT_C void LtkUtils::RLtkBuf16::Close()
	{
	delete iBuf;
	Assign(NULL);
	}

EXPORT_C void LtkUtils::RLtkBuf16::AppendL(const TDesC16& aText)
	{
	ReserveExtraL(aText.Length());
	Append(aText);
	}

EXPORT_C void LtkUtils::RLtkBuf16::AppendL(TChar aChar)
	{
	ReserveExtraL(1);
	Append(aChar);
	}

EXPORT_C void LtkUtils::RLtkBuf16::ReplaceAllL(const TDesC16& aFrom, const TDesC16& aTo)
	{
	//TInt numReplaced = 0;
	TInt pos = 0;
	const TInt lenDelta = -aFrom.Length() + aTo.Length();
	while (ETrue)
		{
		TPtrC16 des(Mid(pos));
		TInt found = des.Find(aFrom);
		if (found == KErrNotFound) break;
		TInt idx = pos + found;
		TInt neededLen = Length() + lenDelta;
		if (neededLen > MaxLength())
			{
			neededLen = Max(neededLen, MaxLength()*2);
			ReAllocL(neededLen);
			}
		Replace(idx, aFrom.Length(), aTo);
		pos = idx + aTo.Length();
		//numReplaced++;
		}
	}

EXPORT_C void LtkUtils::RLtkBuf16::ReplaceL(TInt aPos, TInt aLength, const TDesC16 &aDes)
	{
	const TInt lenDelta = -aLength + aDes.Length();
	ReserveExtraL(lenDelta);
	Replace(aPos, aLength, aDes);
	}

EXPORT_C TInt LtkUtils::RLtkBuf16::CreateMax(TInt aMaxLength)
	{
	TInt err = Create(aMaxLength);
	if (!err) SetLength(aMaxLength);
	return err;
	}

EXPORT_C void LtkUtils::RLtkBuf16::CreateMaxL(TInt aMaxLength)
	{
	User::LeaveIfError(CreateMax(aMaxLength));
	}

class TOverflowDetect : public TDes16Overflow
	{
public:
	TOverflowDetect() : iOverflowed(EFalse) {}
	virtual void Overflow(TDes16& /*aDes*/) { iOverflowed = ETrue; }

	TBool iOverflowed;
	};

EXPORT_C void LtkUtils::RLtkBuf16::AppendFormatL(TRefByValue<const TDesC16> aFmt, ...)
	{
	TInt origLen = Length();
	for (;;)
		{
		VA_LIST args;
		VA_START(args, aFmt);
		TOverflowDetect overflow;
		AppendFormatList(aFmt, args, &overflow);
		VA_END(args);
		if (overflow.iOverflowed)
			{
			SetLength(origLen); // In case we got half-way through formatting
			ReAllocL(Max(16, MaxLength() * 2));
			// And go round again
			}
		else
			{
			// We're done, no overflow
			break;
			}
		}
	}

EXPORT_C void LtkUtils::RLtkBuf16::ReserveExtraL(TInt aExtraCapacity)
	{
	User::LeaveIfError(ReserveExtra(aExtraCapacity));
	}

EXPORT_C TInt LtkUtils::RLtkBuf16::ReserveExtra(TInt aExtraCapacity)
	{
	if (Length() + aExtraCapacity > MaxLength())
		{
		TInt newlen = Max(Length() + aExtraCapacity, (MaxLength() * 3)/2);
		return ReAlloc(newlen);
		}
	return KErrNone;
	}

inline TDesC8::TDesC8(TInt aType,TInt aLength)
	:iLength(aLength|(aType<<KShiftDesType8))
	{}

inline TDes8::TDes8(TInt aType,TInt aLength,TInt aMaxLength)
	: TDesC8(aType,aLength),iMaxLength(aMaxLength)
	{}

EXPORT_C LtkUtils::RLtkBuf8::RLtkBuf8()
	: TDes8(EPtr,0,0), iBuf(NULL)
	{
	}

EXPORT_C LtkUtils::RLtkBuf8::RLtkBuf8(HBufC8* aBuf)
	{
	if(aBuf)
		{
		//Create EBufCPtr type descriptor that points to aHBuf
		// I'll take base_e32's word for it that this does the right thing...
		new(this) TPtr8(aBuf->Des());
		}
	else
		{
		//Create zero-length RBuf8. It is EPtr type of descriptor that points to NULL.
		new(this) RLtkBuf8();
		}
	}

EXPORT_C HBufC8* LtkUtils::RLtkBuf8::ToHBuf()
	{
	// This transfers ownership
	HBufC8* result = iBuf;
	Assign(NULL);
	return result;
	}

EXPORT_C HBufC8* LtkUtils::RLtkBuf8::GetHBuf() const
	{
	// This doesn't transfer ownership
	return iBuf;
	}

EXPORT_C TInt LtkUtils::RLtkBuf8::ReAlloc(TInt aMaxLength)
	{
	if (!aMaxLength) // Reallocation to zero length
		{
		Close();
		return KErrNone;
		}

	if (!iBuf) // Reallocation from zero length
		return Create(aMaxLength); 

	HBufC8* newbuf = iBuf->ReAlloc(aMaxLength);
	if (!newbuf) return KErrNoMemory;
	Assign(newbuf);
	return KErrNone;
	}

EXPORT_C void LtkUtils::RLtkBuf8::ReAllocL(TInt aMaxLength)
	{
	User::LeaveIfError(ReAlloc(aMaxLength));
	}

EXPORT_C void LtkUtils::RLtkBuf8::Assign(HBufC8* aBuf)
	{
	new(this) RLtkBuf8(aBuf);
	}

EXPORT_C TInt LtkUtils::RLtkBuf8::Create(TInt aMaxLength)
	{
	if (aMaxLength)
		{
		HBufC8* buf = HBufC8::New(aMaxLength);
		if (!buf) return KErrNoMemory;
		Assign(buf);
		}
	else
		{
		Assign(NULL);
		}
	return KErrNone;
	}

EXPORT_C void LtkUtils::RLtkBuf8::CreateL(TInt aMaxLength)
	{
	User::LeaveIfError(Create(aMaxLength));
	}

EXPORT_C void LtkUtils::RLtkBuf8::CreateLC(TInt aMaxLength)
	{
	CleanupClosePushL(*this);
	User::LeaveIfError(Create(aMaxLength));
	}

EXPORT_C void LtkUtils::RLtkBuf8::Close()
	{
	delete iBuf;
	Assign(NULL);
	}

EXPORT_C void LtkUtils::RLtkBuf8::AppendL(const TDesC8& aText)
	{
	ReserveExtraL(aText.Length());
	Append(aText);
	}

EXPORT_C void LtkUtils::RLtkBuf8::AppendL(TChar aChar)
	{
	ReserveExtraL(1);
	Append(aChar);
	}

EXPORT_C void LtkUtils::RLtkBuf8::ReplaceAllL(const TDesC8& aFrom, const TDesC8& aTo)
	{
	//TInt numReplaced = 0;
	TInt pos = 0;
	const TInt lenDelta = -aFrom.Length() + aTo.Length();
	while (ETrue)
		{
		TPtrC8 des(Mid(pos));
		TInt found = des.Find(aFrom);
		if (found == KErrNotFound) break;
		TInt idx = pos + found;
		TInt neededLen = Length() + lenDelta;
		if (neededLen > MaxLength())
			{
			neededLen = Max(neededLen, MaxLength()*2);
			ReAllocL(neededLen);
			}
		Replace(idx, aFrom.Length(), aTo);
		pos = idx + aTo.Length();
		//numReplaced++;
		}
	}

EXPORT_C void LtkUtils::RLtkBuf8::ReplaceL(TInt aPos, TInt aLength, const TDesC8& aDes)
	{
	const TInt lenDelta = -aLength + aDes.Length();
	if (Length() + lenDelta > MaxLength())
		{
		ReAllocL(Length() + lenDelta);
		}
	Replace(aPos, aLength, aDes);
	}

EXPORT_C TInt LtkUtils::RLtkBuf8::CreateMax(TInt aMaxLength)
	{
	TInt err = Create(aMaxLength);
	if (!err) SetLength(aMaxLength);
	return err;
	}

EXPORT_C void LtkUtils::RLtkBuf8::CreateMaxL(TInt aMaxLength)
	{
	User::LeaveIfError(CreateMax(aMaxLength));
	}

class TOverflowDetect8 : public TDes8Overflow
	{
public:
	TOverflowDetect8() : iOverflowed(EFalse) {}
	virtual void Overflow(TDes8& /*aDes*/) { iOverflowed = ETrue; }

	TBool iOverflowed;
	};

EXPORT_C void LtkUtils::RLtkBuf8::AppendFormatL(TRefByValue<const TDesC8> aFmt, ...)
	{
	TInt origLen = Length();
	for (;;)
		{
		VA_LIST args;
		VA_START(args, aFmt);
		TOverflowDetect8 overflow;
		AppendFormatList(aFmt, args, &overflow);
		VA_END(args);
		if (overflow.iOverflowed)
			{
			SetLength(origLen); // In case we got half-way through formatting
			ReAllocL(Max(16, MaxLength() * 2));
			// And go round again
			}
		else
			{
			// We're done, no overflow
			break;
			}
		}
	}

EXPORT_C void LtkUtils::RLtkBuf16::AppendL(const TDesC8& aText)
	{
	if (Length() + aText.Length() > MaxLength())
		{
		ReAllocL(Length() + aText.Length());
		}
	TPtr16 endBit((TUint16*)Ptr()+Length(), aText.Length(), aText.Length());
	endBit.Copy(aText);
	SetLength(Length() + aText.Length());
	}

EXPORT_C void LtkUtils::RLtkBuf8::AppendL(const TDesC16& aText)
	{
	if (Length() + aText.Length() > MaxLength())
		{
		ReAllocL(Length() + aText.Length());
		}
	TPtr8 endBit((TUint8*)Ptr()+Length(), aText.Length(), aText.Length());
	endBit.Copy(aText);
	SetLength(Length() + aText.Length());
	}

EXPORT_C void LtkUtils::RLtkBuf8::ReserveExtraL(TInt aExtraCapacity)
	{
	User::LeaveIfError(ReserveExtra(aExtraCapacity));
	}

EXPORT_C TInt LtkUtils::RLtkBuf8::ReserveExtra(TInt aExtraCapacity)
	{
	if (Length() + aExtraCapacity > MaxLength())
		{
		TInt newlen = Max(Length() + aExtraCapacity, (MaxLength() * 3)/2);
		return ReAlloc(newlen);
		}
	return KErrNone;
	}

EXPORT_C void LtkUtils::RLtkBuf8::CopyAsUtf8L(const TDesC16& aString)
	{
	// This algorithm is as per the unicode standard v5.2, section 3.9.
	// Interestingly, byte sequences C0-C1 and F5-FF are completely unused in UTF-8.
	// Means there won't be any telnet escape problems with 0xFFs...

	// This function supports the full UTF-16 spec including surrogate pairs, even though the rest of the OS doesn't.

	SetLength(0);
	const TInt len = aString.Length();
	ReserveExtraL(len); // First guess assumes it's ASCII
	for (TInt i = 0; i < len; i++)
		{
		TUint16 ch = aString[i];
		if (ch < 0x80)
			{
			// ASCII - one byte
			AppendL(ch);
			}
		else if (ch < 0x800)
			{
			// 2 byte
			TUint8 first = 0xC0 | ((ch & 0x7C0) >> 6);
			TUint8 second = 0x80 | (ch & 0x3F);
			AppendL(first);
			AppendL(second);
			}
		else if (Rng(0xD800, (TInt)ch, 0xDBFF))
			{
			// Leading surrogate - a four byter
			//const TUint32 LEAD_OFFSET = 0xD800 - (0x10000 >> 10);
			const TUint32 SURROGATE_OFFSET = 0x10000u - (0xD800u << 10) - 0xDC00u;

			if (i+1 < len && Rng(0xDC00, (TInt)aString[i+1], 0xDFFF))
				{
				TUint16 trailing = aString[i+1];
				i++;
				TUint32 fullCh = ((TUint32)ch << 10) + trailing + SURROGATE_OFFSET;
				TUint8 first  = 0xF0 | ((fullCh & 0x1C0000) >> 18);
				TUint8 second = 0x80 | ((fullCh & 0x030000) >> 16) | ((fullCh & 0xF000) >> 12);
				TUint8 third  = 0x80 | ((fullCh & 0xFC0) >> 6);
				TUint8 fourth = 0x80 | (fullCh & 0x3F);
				AppendL(first);
				AppendL(second);
				AppendL(third);
				AppendL(fourth);
				}
			else
				{
				// An orphaned leading surrogate - not allowed to just encode it blindly
				AppendL(KReplacementCharInUtf8);
				}
			}
		else if (Rng(0xDC00, (TInt)ch, 0xDFFF))
			{
			// Orphaned trailing surrogate
			AppendL(KReplacementCharInUtf8);
			}
		else
			{
			//Anything else that fits in 16 bits is a three byter.
			TUint8 first =  0xE0 | ((ch & 0xF000) >> 12);
			TUint8 second = 0x80 | ((ch & 0xFC0) >> 6);
			TUint8 third =  0x80 | (ch & 0x3F);
			AppendL(first);
			AppendL(second);
			AppendL(third);
			}
		}
	}

EXPORT_C HBufC8* LtkUtils::Utf8L(const TDesC& aString)
	{
	RLtkBuf8 result;
	CleanupClosePushL(result);
	result.CopyAsUtf8L(aString);
	CleanupStack::Pop(&result);
	HBufC8* ptr = result.ToHBuf();
	if (!ptr) ptr = HBufC8::NewL(1); // If aString was empty RLtkBuf won't bother creating an HBufC, but callers of this API would expect it
	return ptr;
	}

EXPORT_C HBufC16* LtkUtils::DecodeUtf8L(const TDesC8& aUtf8EncodedText)
	{
	RLtkBuf16 result;
	result.CreateLC(aUtf8EncodedText.Length());
	result.CopyFromUtf8L(aUtf8EncodedText);
	CleanupStack::Pop(&result);
	return result.ToHBuf();
	}

EXPORT_C void LtkUtils::RLtkBuf16::AppendUtf8L(const TDesC8& aUtf8EncodedText)
	{
	TInt dontCare;
	AppendUtf8L(aUtf8EncodedText, dontCare);
	}

EXPORT_C void LtkUtils::RLtkBuf16::CopyFromUtf8L(const TDesC8& aUtf8EncodedText)
	{
	ClearUtf8State(); // Just in case, somehow, some old state is lying around
	Zero();
	AppendUtf8L(aUtf8EncodedText);
	User::LeaveIfError(FinalizeUtf8());
	}

TUtf8State::TUtf8State()
	: iFirstUnconvertedByte(KErrNotFound), iBytesConsumedSoFar(0), iUnconvertedBytesCount(0)
	{
	}

LtkUtils::TUtf8State* LtkUtils::RLtkBuf16::GetUtf8State() const
	{
	if (!iBuf) return NULL;
	TInt hbufMaxLength = iBuf->Des().MaxLength();
	TInt ourMaxLength = iMaxLength; // Ie the max length field from TDes16
	if (hbufMaxLength == ourMaxLength + KStateCharCount)
		{
		// Then we have some state stashed Beyond The Bytes We Know
		TUtf8State* state = (TUtf8State*)(Ptr() + ourMaxLength);
		return state;
		}
	return NULL;
	}

void LtkUtils::RLtkBuf16::ClearUtf8State()
	{
	if (iBuf)
		{
		iMaxLength = iBuf->Des().MaxLength();
		}
	}

EXPORT_C TInt LtkUtils::RLtkBuf16::FinalizeUtf8()
	{
	TInt firstBadBytePosition;
	FinalizeUtf8(firstBadBytePosition);
	return firstBadBytePosition == KErrNotFound ? KErrNone : KErrCorrupt;
	}

EXPORT_C void LtkUtils::RLtkBuf16::FinalizeUtf8(TInt& aFirstBadBytePosition)
	{
	TUtf8State* state = GetUtf8State();
	if (!state)
		{
		aFirstBadBytePosition = KErrNotFound;
		return;
		}
	TInt firstUnconvertedByte = state->iFirstUnconvertedByte;
	if (state->iUnconvertedBytesCount != 0)
		{
		if (firstUnconvertedByte == KErrNotFound) firstUnconvertedByte = state->iBytesConsumedSoFar;
		ClearUtf8State();
		Append(KReplacementChar); // Can't fail or leave, we know there was enough space for the TUtf8State
		}
	else
		{
		ClearUtf8State();
		}
	aFirstBadBytePosition = firstUnconvertedByte;
	}

EXPORT_C void LtkUtils::RLtkBuf16::AppendUtf8L(const TDesC8& aText, TInt& aFirstBadBytePosition)
	{
	// This may not be the fastest algorithm, but it should hopefully be fairly easy to read and debug
	// It's basically the inverse of RLtkBuf8::CopyAsUtf8L().
	TUtf8State state;
	TUtf8State* oldState = GetUtf8State();
	if (oldState)
		{
		state = *oldState;
		}
	aFirstBadBytePosition = KErrNotFound;

	const TInt textLen = aText.Length();
	ReserveExtraL(textLen + KStateCharCount); // We will need at least this many characters, since a UTF-8 byte can never expand to less that 1 character.
	TUint16* bufPtr = (TUint16*)Ptr() + Length();
	TUint16* endPtr = (TUint16*)Ptr() + MaxLength();
	const TInt initialLeftoverSize = state.iUnconvertedBytesCount;
	TUint8 const* readPtr = initialLeftoverSize ? state.iUnconvertedBytes : aText.Ptr();
	TUint8 const*const endFragmentPtr = (readPtr + state.iUnconvertedBytesCount);

	for (TInt i = 0-state.iUnconvertedBytesCount; i < textLen; i++)
		{
		// Negative i means we're starting in the unconverted bytes left over from the last append
		TUint8 ch = *readPtr++;
		TInt validSequenceLength = 1;
		TInt sequenceLength = 0;
		// One of the benefits of UTF-8 is that you can know the sequence length just by looking at the first byte
		if ((ch & 0x80) == 0) sequenceLength = 1;
		else if ((ch & 0xE0) == 0xC0) sequenceLength = 2;
		else if ((ch & 0xF0) == 0xE0) sequenceLength = 3;
		else if ((ch & 0xF8) == 0xF0) sequenceLength = 4;
		else
			{
			validSequenceLength = 0;
			}

		TUint32 fullchar = 0;
		TInt availableSequenceLength = Min(sequenceLength, textLen - i);
		if (i >= 0 && availableSequenceLength == sequenceLength)
			{
			// The normal case, no need to be careful about readptr
			// This is the optimised code path
			switch (sequenceLength)
				{
			case 0:
				validSequenceLength = 0; // For optimisation purposes we don't bother setting this until now
				break;
			case 1:
				fullchar = ch;
				break;
			case 2:
				{
				TUint8 second = *readPtr++;
				if ((second & 0xC0) == 0x80) validSequenceLength++;
				fullchar = ((ch & 0x1F) << 6) | (second & 0x3F);
				break;
				}
			case 3:
				{
				TUint8 second = *readPtr++;
				TUint8 third = *readPtr++;
				if ((second & 0xC0) == 0x80)
					{
					validSequenceLength++;
					if ((third & 0xC0) == 0x80) validSequenceLength++;
					}
				fullchar = ((ch & 0x0F) << 12) | ((second & 0x3F) << 6) | (third & 0x3F);
				break;
				}
			case 4:
				{
				TUint8 second = *readPtr++;
				TUint8 third = *readPtr++;
				TUint8 fourth = *readPtr++;
				if ((second & 0xC0) == 0x80)
					{
					validSequenceLength++;
					if ((third & 0xC0) == 0x80)
						{
						validSequenceLength++;
						if ((fourth & 0xC0) == 0x80) validSequenceLength++;
						}
					}
				fullchar = ((ch & 0x07) << 18) | ((second & 0x3F) << 12) | ((third & 0x3F) << 6) | (fourth & 0x3F);
				break;
				}
			default:
				ASSERT(EFalse);
				}

			if (validSequenceLength != availableSequenceLength || validSequenceLength == 0)
				{
				// Failed sequence
				fullchar = KReplacementChar;
				if (state.iFirstUnconvertedByte == KErrNotFound)
					{
					// If we're still in the leftovers i will be negative and hopefully give us the right answer
					state.iFirstUnconvertedByte = state.iBytesConsumedSoFar + state.iUnconvertedBytesCount + i;
					}
				if (aFirstBadBytePosition == KErrNotFound)
					{
					// Ugh i could be less than zero if we bailed in the leftovers. We'll say zero, even though that's strictly not correct
					// Not a lot we can do because from the point of view of aFirstBadBytePosition we implicitly accepted the leftover bytes
					// at the end of the previous AppendUtf8L() call
					aFirstBadBytePosition = (i < 0) ? 0 : i;
					}
				if (validSequenceLength > 0) i += validSequenceLength-1; // No point testing the bytes we know to be valid following bytes, because they can't by definition be valid starting bytes (it would be within the spec to do so, but would mean we'd not be following best-practice regarding maximal subexpression replacement)
				readPtr -= (availableSequenceLength - validSequenceLength); // Rewind readPtr over the additional bytes it read
				}
			else
				{
				i += sequenceLength-1; // Skip over the remaining chars in the sequence
				}
			}
		else // Do it byte-by-byte and slowly
			{	
			// Validate as many bytes as we have available
			TUint8 sequence[4];
			sequence[0] = ch;
			for (TInt j = 1; j < availableSequenceLength; j++)
				{
				if (readPtr == endFragmentPtr) readPtr = aText.Ptr();
				sequence[j] = *readPtr++;
				if ((sequence[j] & 0xC0) == 0x80)
					{
					validSequenceLength++;
					}
				else
					{
					break;
					}
				}
			if (readPtr == endFragmentPtr) readPtr = aText.Ptr();

			if (validSequenceLength != availableSequenceLength)
				{
				// Failed sequence
				fullchar = KReplacementChar;
				if (state.iFirstUnconvertedByte == KErrNotFound)
					{
					// If we're still in the leftovers i will be negative and hopefully give us the right answer
					state.iFirstUnconvertedByte = state.iBytesConsumedSoFar + state.iUnconvertedBytesCount + i;
					}
				if (aFirstBadBytePosition == KErrNotFound)
					{
					// Ugh i could be less than zero if we bailed in the leftovers. We'll say zero, even though that's strictly not correct
					// Not a lot we can do because from the point of view of aFirstBadBytePosition we implicitly accepted the leftover bytes
					// at the end of the previous AppendUtf8L() call
					aFirstBadBytePosition = (i < 0) ? 0 : i;
					}
				if (validSequenceLength > 0) i += validSequenceLength-1; // No point testing the bytes we know to be valid following bytes, because they can't by definition be valid starting bytes (it would be within the spec to do so, but would mean we'd not be following best-practice regarding maximal subexpression replacement)
				}
			else if (availableSequenceLength < sequenceLength)
				{
				// Then cache what we do have in the leftovers, that wasn't already 
				TInt sequencePosNotFromLeftovers = 0;
				if (i < 0)
					{
					sequencePosNotFromLeftovers = -i;
					}
				Mem::Copy(&state.iUnconvertedBytes[state.iUnconvertedBytesCount], &sequence[sequencePosNotFromLeftovers], availableSequenceLength-sequencePosNotFromLeftovers);
				state.iUnconvertedBytesCount = availableSequenceLength;
				i += availableSequenceLength-1; // Skip over the remaining chars in the sequence
				continue;
				}
			else
				{
				// We've got everything, and it all validates
				switch (sequenceLength)
					{
				case 1:
					fullchar = ch;
					break;
				case 2:
					fullchar = ((sequence[0] & 0x1F) << 6) | (sequence[1] & 0x3F);
					break;
				case 3:
					fullchar = ((sequence[0] & 0x0F) << 12) | ((sequence[1] & 0x3F) << 6) | (sequence[2] & 0x3F);
					break;
				case 4:
					fullchar = ((sequence[0] & 0x07) << 18) | ((sequence[1] & 0x3F) << 12) | ((sequence[2] & 0x3F) << 6) | (sequence[3] & 0x3F);
					break;
					}

				state.iUnconvertedBytesCount = 0; // By definition we must have used up all the leftovers
				i += sequenceLength-1; // Skip over the remaining chars in the sequence
				}
			}

		if (fullchar == KBom && state.iBytesConsumedSoFar == 0 && (i + 1 - sequenceLength) <= 0)
			{
			// Byte order marks are ignored, so long as they're the first thing in the buffer (and this is the first buffer)
			// Otherwise they are considered to be valid zero-width non-breaking spaces (ZWNBSPs)
			continue;
			}

		// If we reach here we have a char to append
		if (bufPtr == endPtr)
			{
			// if bufPtr equals endPtr we're at the end of the buffer and need to realloc it
			SetLength(iMaxLength);
			ReserveExtraL(1);
			bufPtr = (TUint16*)Ptr() + Length();
			endPtr = (TUint16*)Ptr() + MaxLength();
			}
		*bufPtr++ = fullchar;
		}

	// Finished handling all the bytes in aText, update some state
	SetLength(bufPtr - Ptr());
	TInt bytesAddedToLeftovers = state.iUnconvertedBytesCount - initialLeftoverSize;
	state.iBytesConsumedSoFar += textLen - bytesAddedToLeftovers; // Bytes in the leftovers haven't technically been consumed yet
	// Now save the state on the end of the buffer - any reallocs must have maintained the state, but there might not have been any state to start with
	if (oldState == NULL)
		{
		ASSERT(iBuf == NULL || iMaxLength == iBuf->Des().MaxLength());
		ReserveExtraL(KStateCharCount);
		iMaxLength = iMaxLength - KStateCharCount;
		}
	Mem::Copy((TAny*)(Ptr() + iMaxLength), &state, sizeof(TUtf8State));
	}

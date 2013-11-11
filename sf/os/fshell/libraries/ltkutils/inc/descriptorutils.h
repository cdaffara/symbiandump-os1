// descriptorutils.h
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
#ifndef FSHELL_DESCRIPTORUTILS_H
#define FSHELL_DESCRIPTORUTILS_H

#include <e32std.h>

namespace LtkUtils
	{
	class TUtf8State;

	NONSHARABLE_CLASS(RLtkBuf16) : public TDes16
		{
	public:
		IMPORT_C RLtkBuf16();
		IMPORT_C RLtkBuf16(HBufC16* aBuf); // Takes ownership

		// The new, useful functions that RBuf doesn't give you
		IMPORT_C HBufC16* ToHBuf(); // Caller is responsible for deleting the result. This function Closes the RLtkBuf.
		IMPORT_C HBufC16* GetHBuf() const;
		IMPORT_C void AppendL(const TDesC16& aText);
		IMPORT_C void AppendL(const TDesC8& aText);
		IMPORT_C void AppendL(TChar aChar);
		IMPORT_C void AppendFormatL(TRefByValue<const TDesC16> aFmt, ...);
		IMPORT_C void ReplaceL(TInt aPos, TInt aLength, const TDesC16 &aDes);
		IMPORT_C void ReplaceAllL(const TDesC& aFrom, const TDesC16& aTo);
		IMPORT_C void ReserveExtraL(TInt aExtraCapacity);
		IMPORT_C TInt ReserveExtra(TInt aExtraCapacity);
		IMPORT_C void CreateLC(TInt aMaxLength);

		/**
		Leaves on OOM. Converts the argument aUtf8EncodedText from UTF-8 to the native UTF-16 and appends the result to this
		descriptor. You can handle your data in chunks by calling AppendUtf8L repeatedly. You don't have to worry about UTF-8
		sequences being split between chunks - fragmented end sequences are cached and will be reassembled if a further call
		to AppendUtf8L is made with the rest of the bytes. Such sequences are not considered malformed unless (a) the next call
		to AppendUtf8L doesn't produce a valid sequence, or (b) FinalizeUtf8 is called instead of a further append. Sequences 
		that are decided to be malformed are replaced with the unicode replacement char U+FFFD. You are free to mix calls to
		AppendUtf8L with other normal descriptor operations such as Delete, Insert or ReAlloc. You shouldn't however copy
		the buffer or call any other Append functions without finalizing it first. Once you have finished appending UTF-8
		you must call FinalizeUtf8().
		*/
		IMPORT_C void AppendUtf8L(const TDesC8& aUtf8EncodedText);

		/**
		Like the above, but returns by reference the index of the first byte of the first malformed sequence in aUtf8EncodedText (or
		KErrNotFound if it was totally valid). Useful if you want to stop appending the minute you see a malformed sequence
		and thus don't want to have to wait until you call FinalizeUtf8() to get that information.
		*/
		IMPORT_C void AppendUtf8L(const TDesC8& aUtf8EncodedText, TInt& aFirstBadBytePosition);

		/**
		Must be called at the end of a sequence of AppendUtf8L() calls. Clears all UTF-8 related state from the descriptor.
		Returns KErrCorrupt if any malformed sequences were encountered in any of the AppendUtf8L() calls. You are free to
		ignore this return code if you wish: malformed sequences get replaced with U+FFFD and the rest of the string continues
		to be parsed. Returns KErrNone if there were no problems.
		*/
		IMPORT_C TInt FinalizeUtf8();

		/**
		Like the above, but returns by reference the index of the first byte of the first malformed sequence found cumulatively
		across all invocations of AppendUtf8L(). aFirstBadBytePosition is set to KErrNotFound if the data was totally valid.
		*/
		IMPORT_C void FinalizeUtf8(TInt& aFirstBadBytePosition);


		/**
		Convenience method. Equivalent to:
		this->Zero();
		this->AppendUtf8L(aText);
		User::LeaveIfError(this->FinalizeUtf8());
		*/
		IMPORT_C void CopyFromUtf8L(const TDesC8& aUtf8EncodedText);

		// The usual RBuf-like APIs (for everything else, it just inherits from TDes16)
		IMPORT_C TInt ReAlloc(TInt aMaxLength);
		IMPORT_C void ReAllocL(TInt aMaxLength);
		IMPORT_C void Assign(HBufC16* aBuf);
		IMPORT_C TInt Create(TInt aMaxLength);
		IMPORT_C void CreateL(TInt aMaxLength);
		IMPORT_C TInt CreateMax(TInt aMaxLength);
		IMPORT_C void CreateMaxL(TInt aMaxLength);
		IMPORT_C void Close();

	private:
		TUtf8State* GetUtf8State() const;
		void ClearUtf8State();

	private:
		HBufC16* iBuf;
		};

	NONSHARABLE_CLASS(RLtkBuf8) : public TDes8
		{
	public:
		IMPORT_C RLtkBuf8();
		IMPORT_C RLtkBuf8(HBufC8* aBuf); // Takes ownership

		// The new, useful functions that RBuf doesn't give you
		IMPORT_C HBufC8* ToHBuf(); // Caller is responsible for deleting the result. This function Closes the RLtkBuf.
		IMPORT_C HBufC8* GetHBuf() const;
		IMPORT_C void AppendL(const TDesC8& aText);
		IMPORT_C void AppendL(const TDesC16& aText);
		IMPORT_C void AppendL(TChar aChar);
		IMPORT_C void AppendFormatL(TRefByValue<const TDesC8> aFmt, ...);
		IMPORT_C void ReplaceL(TInt aPos, TInt aLength, const TDesC8& aDes);
		IMPORT_C void ReplaceAllL(const TDesC8& aFrom, const TDesC8& aTo);
		IMPORT_C void ReserveExtraL(TInt aExtraCapacity);
		IMPORT_C TInt ReserveExtra(TInt aExtraCapacity);
		IMPORT_C void CreateLC(TInt aMaxLength);
		IMPORT_C void CopyAsUtf8L(const TDesC16& aString);

		// The usual RBuf-like APIs (for everything else, it just inherits from TDes8)
		IMPORT_C TInt ReAlloc(TInt aMaxLength);
		IMPORT_C void ReAllocL(TInt aMaxLength);
		IMPORT_C void Assign(HBufC8* aBuf);
		IMPORT_C TInt Create(TInt aMaxLength);
		IMPORT_C void CreateL(TInt aMaxLength);
		IMPORT_C TInt CreateMax(TInt aMaxLength);
		IMPORT_C void CreateMaxL(TInt aMaxLength);
		IMPORT_C void Close();

	private:
		HBufC8* iBuf;
		};

	typedef RLtkBuf16 RLtkBuf;

	/* Use this class for const static arrays of descriptors. Eg:
	 *
	 * const SLitC KData[] = 
	 *     {
	 *     DESC("A string"),
	 *     DESC("Another string"),
	 *     };
	 *
	 * You can treat the array items the same as TLitCs - ie most of the time they behave as descriptors but
	 * sometimes you may need to use operator().
	 */
	struct SLitC
		{
		inline const TDesC* operator&() const { return reinterpret_cast<const TDesC*>(this); }
		inline operator const TDesC&() const { return *reinterpret_cast<const TDesC*>(this); }
		inline const TDesC& operator()() const { return *reinterpret_cast<const TDesC*>(this); }

		TInt iLength;
		const TUint16* iPtr;
		};
	struct SLitC8
		{
		inline const TDesC8* operator&() const { return reinterpret_cast<const TDesC8*>(this); }
		inline operator const TDesC8&() const { return *reinterpret_cast<const TDesC8*>(this); }
		inline const TDesC8& operator()() const { return *reinterpret_cast<const TDesC8*>(this); }

		TInt iLength;
		const TUint8* iPtr;
		};

#define DESC(x) { ((sizeof(L##x) / 2)-1) | 0x10000000, (const TUint16*) L##x }
#define DESC8(x) { (sizeof(x) - 1) | 0x10000000, (const TUint8*)x }
// 0x10000000 is EPtrC<<KShiftDesType

	}

#endif

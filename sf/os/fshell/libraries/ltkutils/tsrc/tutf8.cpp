// tutf8.cpp
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

#include <fshell/ioutils.h>
#include <fshell/common.mmh>
#include <fshell/descriptorutils.h>

using namespace IoUtils;
using namespace LtkUtils;

class CCmdtutf8 : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdtutf8();
private:
	CCmdtutf8();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TFileName2 iFile;
	TInt iBlockSize;
	};

EXE_BOILER_PLATE(CCmdtutf8)

CCommandBase* CCmdtutf8::NewLC()
	{
	CCmdtutf8* self = new(ELeave) CCmdtutf8();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdtutf8::~CCmdtutf8()
	{
	}

CCmdtutf8::CCmdtutf8()
	{
	}

const TDesC& CCmdtutf8::Name() const
	{
	_LIT(KName, "tutf8");	
	return KName;
	}

const TDesC& CCmdtutf8::Description() const
	{
	_LIT(KDescription, "Test for RLtkBuf8::CopyAsUtf8L() and RLtkBuf16::AppendUtf8L().");
	return KDescription;
	}

void CCmdtutf8::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendFileNameL(iFile, _L("filename"), _L("If specified, parse this file as UTF-8 and print the results"), KValueTypeFlagOptional);
	}

void CCmdtutf8::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendIntL(iBlockSize, 'b', _L("blocksize"), _L("block size for parsing the specified file"));
	}

void CCmdtutf8::DoRunL()
	{
	if (iFile.Length() == 0)
		{
		CleanupStack::PushL((CBase*)1); // Panicker
		_LIT(KTest, "A \u03A9 \u8A9E \uFFFD \uFEFF \uD800 "); // The original UTF-16 string: A LowercaseOmega SomeGlyphOrOther ReplacementChar ZWNBSP UnmatchedLeadingSurrogate
		_LIT8(KOut, "A \xCE\xA9 \xE8\xAA\x9E \xEF\xBF\xBD \xEF\xBB\xBF \xEF\xBF\xBD "); // What it should be in UTF-8
		_LIT(KOutInUnicode, "A \u03A9 \u8A9E \uFFFD \uFEFF \uFFFD "); // Almost the same as the original, except that the UnmatchedSurrogate was transformed into ReplacementChar in UTF-8 so the last char here is U+FFFD
		RLtkBuf8 buf;
		buf.CopyAsUtf8L(KTest);
		ASSERT(buf == KOut());
		buf.Close();

		RLtkBuf16 wbuf;
		wbuf.AppendUtf8L(KOut().Left(6));
		ASSERT(wbuf.Length() == 4); // Testing that only the 4 complete characters are in there
		wbuf.AppendUtf8L(KNullDesC8());
		ASSERT(wbuf.Length() == 4); // Testing that appending a null descriptor hasn't changed the length (or crashed)
		TInt firstprob;
		wbuf.FinalizeUtf8(firstprob);
		ASSERT(firstprob == 5); // Correctly indentified the first invalid bit
		_LIT(KFirstFrag, "A \u03A9 \uFFFD");
		ASSERT(wbuf == KFirstFrag());
		wbuf.SetLength(4);
		wbuf.AppendUtf8L(KOut().Mid(5,1));
		wbuf.ReAllocL(256); // Be really evil and realloc the buffer while we have fragmented bytes cached
		ASSERT(wbuf == KOutInUnicode().Left(4));
		wbuf.AppendUtf8L(KOut().Mid(6,1));
		ASSERT(wbuf == KOutInUnicode().Left(4));
		wbuf.AppendUtf8L(KOut().Mid(7,3));
		ASSERT(wbuf == KOutInUnicode().Left(6));
		wbuf.AppendUtf8L(KOut().Mid(10));
		ASSERT(wbuf == KOutInUnicode());
		wbuf.FinalizeUtf8(firstprob);
		ASSERT(firstprob == KErrNotFound);
		wbuf.Close();

		_LIT8(KBomTest, "\xEF\xBB\xBF BB \xEF\xBB\xBF");
		_LIT(KBomOutput, " BB \uFEFF");
		wbuf.AppendUtf8L(KBomTest);
		wbuf.FinalizeUtf8(firstprob);
		ASSERT(wbuf == KBomOutput());
		ASSERT(firstprob == KErrNotFound);
		wbuf.Close();

		wbuf.AppendUtf8L(KBomTest().Left(2));
		wbuf.AppendUtf8L(KBomTest().Mid(2, 5));
		wbuf.AppendUtf8L(KBomTest().Mid(7));
		wbuf.FinalizeUtf8(firstprob);
		ASSERT(wbuf == KBomOutput());
		ASSERT(firstprob == KErrNotFound);
		wbuf.Close();

		// Maximal subexpression replacement test - Example taken from unicode standard section 3.9, table 3-8.
		_LIT8(KInvalid, "\x61\xF1\x80\x80\xE1\x80\xC2\x62\x80\x63\x80\xBF\x64");
		_LIT(KInvalidOutput, "\u0061\uFFFD\uFFFD\uFFFD\u0062\uFFFD\u0063\uFFFD\uFFFD\u0064"); // And this is how the standard recommends it is processed
		wbuf.AppendUtf8L(KInvalid);
		wbuf.FinalizeUtf8(firstprob);
		ASSERT(wbuf == KInvalidOutput());
		ASSERT(firstprob == 1);
		wbuf.Close();

		// Check that the first bad byte calculations are right
		wbuf.AppendUtf8L(_L8(" \x61\xF1"), firstprob);
		ASSERT(firstprob == KErrNotFound); // F1 is potentially valid
		wbuf.AppendUtf8L(_L8("\x80\x80\xE1"), firstprob);
		ASSERT(firstprob == 0); // Technically it's the -1th byte of what we just passed in, but we can only say zero
		wbuf.FinalizeUtf8(firstprob);
		ASSERT(firstprob == 2); // The overall first invalid byte was byte 1, the 0xF1
		wbuf.Close();
		CleanupStack::Pop(); // Panicker
		}
	else
		{
		RFile file;
		LeaveIfErr(file.Open(FsL(), iFile, EFileRead), _L("Couldn't open file %S"), &iFile);
		CleanupClosePushL(file);
		TInt fileSize;
		LeaveIfErr(file.Size(fileSize), _L("Couldn't get file size"));
		RBuf8 nbuf;
		nbuf.CreateL(iBlockSize ? iBlockSize : fileSize);
		CleanupClosePushL(nbuf);
		RLtkBuf buf;
		CleanupClosePushL(buf);
		TInt read = 0;
		while (read < fileSize)
			{
			nbuf.Zero();
			LeaveIfErr(file.Read(nbuf), _L("Couldn't read file"));
			read += nbuf.Length();
			buf.Zero();
			buf.AppendUtf8L(nbuf);
			Write(buf);
			}
		TInt unconverted;
		buf.FinalizeUtf8(unconverted);
		Printf(_L("First bad byte: %d\r\n"), unconverted);
		Write(buf);

		CleanupStack::PopAndDestroy(3, &file); // buf, nbuf, file
		}
	}

/*
This turned out to not actually make much difference, but it might be useful in future...

static const TInt8 KFirstByteSequenceLengths[] = { // 40 chars per line
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
	4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};
__ASSERT_COMPILE(sizeof(KFirstByteSequenceLengths) == 256);
ASSERT(KFirstByteSequenceLengths[0x7F] == 1);
ASSERT(KFirstByteSequenceLengths[0x80] == 0);
ASSERT(KFirstByteSequenceLengths[0xC1] == 0);
ASSERT(KFirstByteSequenceLengths[0xC2] == 2);
ASSERT(KFirstByteSequenceLengths[0xDF] == 2);
ASSERT(KFirstByteSequenceLengths[0xE0] == 3);
ASSERT(KFirstByteSequenceLengths[0xEF] == 3);
ASSERT(KFirstByteSequenceLengths[0xF0] == 4);
ASSERT(KFirstByteSequenceLengths[0xF4] == 4);
ASSERT(KFirstByteSequenceLengths[0xF5] == 0);
ASSERT(KFirstByteSequenceLengths[0xFF] == 0);
*/

// Grep.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Accenture nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#include <fshell/ioutils.h>
#include <fshell/common.mmh>
#include <fshell/descriptorutils.h>
#include <fshell/pcre/cregex.h>

using namespace IoUtils;
using namespace LtkUtils;

class CCmdGrep : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdGrep();
private:
	CCmdGrep();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	HBufC* iPattern;
	TBool iIgnoreCase;
	TBool iInvertMatch;
	TBool iCount;
	TBool iUnicode;

	RLtkBuf8 iNarrowBuf;
	CRegEx* iRegex;
	};

EXE_BOILER_PLATE(CCmdGrep)

CCommandBase* CCmdGrep::NewLC()
	{
	CCmdGrep* self = new(ELeave) CCmdGrep();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdGrep::~CCmdGrep()
	{
	delete iPattern;
	delete iRegex;
	iNarrowBuf.Close();
	}

CCmdGrep::CCmdGrep()
	{
	}

const TDesC& CCmdGrep::Name() const
	{
	_LIT(KName, "grep");	
	return KName;
	}

void CCmdGrep::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendStringL(iPattern, _L("pattern"));
	}

void CCmdGrep::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdGrepOptIgnoreCase, "ignore-case");
	_LIT(KCmdGrepOptInvertMatch, "invert-match");
	_LIT(KCmdGrepOptCount, "count");
	_LIT(KCmdGrepOptUnicode, "unicode");
	aOptions.AppendBoolL(iIgnoreCase, KCmdGrepOptIgnoreCase);
	aOptions.AppendBoolL(iInvertMatch, KCmdGrepOptInvertMatch);
	aOptions.AppendBoolL(iCount, KCmdGrepOptCount);
	aOptions.AppendBoolL(iUnicode, KCmdGrepOptUnicode);
	}

void CCmdGrep::DoRunL()
	{
	TRegExOptions options(EPcreExtended|EPcreNewlineAny);
	if (iUnicode)
		{
		iNarrowBuf.CopyAsUtf8L(*iPattern);
		options.SetUtf8(ETrue);
		}
	else
		{
		iNarrowBuf.AppendL(*iPattern);
		}
	if (iIgnoreCase) options.SetCaseless(ETrue);

	iRegex = CRegEx::NewL(iNarrowBuf, options);

	Stdin().SetReadMode(RIoReadHandle::ELine);
	TBuf<0x100> line;
	TInt count = 0;
	while (Stdin().Read(line) == KErrNone)
		{
		iNarrowBuf.Zero();
		if (iUnicode)
			{
			iNarrowBuf.CopyAsUtf8L(line);
			}
		else
			{
			iNarrowBuf.AppendL(line);
			}

		TBool matches = iRegex->PartialMatchL(iNarrowBuf);

		if (iInvertMatch)
			{
			matches = !matches;
			}
		if (matches)
			{
			if (iCount)
				{
				count++;
				}
			else
				{
				Write(line);
				}
			}
		}

	if (iCount)
		{
		Printf(_L("Count = %d"), count);
		}
	}

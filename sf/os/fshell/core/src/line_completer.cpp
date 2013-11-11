// line_completer.cpp
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

#include <f32file.h>
#include <fshell/ltkutils.h>
#include "lexer.h"
#include "command_factory.h"
#include "line_completer.h"

_LIT(KTab, "\t");
_LIT(KWild, "*");
_LIT(KDoubleQuote, "\"");
_LIT(KSpace, " ");
_LIT(KFileNameSlash, "\\");
_LIT(KPipe, "|");
_LIT(KDoublePipe, "||");
_LIT(KAmpersand, "&");
_LIT(KDoubleAmpersand, "&&");
_LIT(KAmpersandPipe, "&|");
_LIT(KSemicolon, ";");
_LIT(KDollar, "$");
_LIT(KDashDash, "--");
_LIT(KDash, "-");

CLineCompleter* CLineCompleter::NewL(RFs& aFs, CCommandFactory& aCommandFactory, IoUtils::CEnvironment& aEnv)
	{
	CLineCompleter* self = new(ELeave) CLineCompleter(aFs, aCommandFactory, aEnv);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CLineCompleter::~CLineCompleter()
	{
	delete iLexer;
	delete iLastUsedCif;
	}

void CLineCompleter::LcCompleteLineL(TConsoleLine& aLine, const TChar& aEscapeChar)
	{
	iLexer->Set(aLine.ContentsToCursor(), aEscapeChar);
	TToken token(iLexer->NextToken());
	TToken firstToken = token;
	TToken prevToken(TToken::EString, KNullDesC, aLine.ContentsToCursor().Length());
	TInt lastTokenIdx = 0;
	while (iLexer->More())
		{
		// More than one token - skip to last.
		prevToken = token;
		token = iLexer->NextToken();
		lastTokenIdx++;
		}

	switch (prevToken.Type())
		{
		case TToken::EPipe:
		case TToken::EAmpersand:
		case TToken::ESemicolon:
		case TToken::EDoublePipe:
		case TToken::EDoubleAmpersand:
		case TToken::EAmpersandPipe:
			CompleteCommandNameL(aLine, token, aEscapeChar);
			break;
		default:
			if (token.Position() + token.String().Length() < aLine.ContentsToCursor().Length())
				{
				// Looks like there's whitespace after this token, so consider it to be completing the empty token.
				token = TToken(TToken::EString, KNullDesC, aLine.ContentsToCursor().Length());
				}
			else if (lastTokenIdx == 0)
				{
				CompleteCommandNameL(aLine, token, aEscapeChar);
				break;
				}

			if (token.String().Left(1) == KDollar)
				{
				CompleteEnvVarL(aLine, token, aEscapeChar);
				}
			else if (token.String() == KDash || token.String().Left(2) == KDashDash)
				{
				// Check the CIF is loaded and it's the right one
				if (iLastUsedCif && iLastUsedCif->Name().CompareF(firstToken.String()) != 0)
					{
					delete iLastUsedCif;
					iLastUsedCif = NULL;
					}
				TInt err = KErrNone;
				if (iLastUsedCif == NULL)
					{
					// We need to supress StaticLeaveIfErrs from CCommandInfoFile::NewL causing a PrintError if there's no CIF for this command,
					// hence the use of TRAP_QUIETLY.
					TRAP_QUIETLY(err, iLastUsedCif = IoUtils::CCommandInfoFile::NewL(iFs, iEnv, firstToken.String()));
					}
				if (err == KErrNone)
					{
					CompleteOptionL(aLine, token, aEscapeChar);
					}
				}
			else
				{
				CompleteFileNameL(aLine, token, aEscapeChar);
				}
			break;
		}
	}

CLineCompleter::CLineCompleter(RFs& aFs, CCommandFactory& aCommandFactory, IoUtils::CEnvironment& aEnv)
	: iFs(aFs), iCommandFactory(aCommandFactory), iEnv(aEnv)
	{
	}

void CLineCompleter::ConstructL()
	{
	iLexer = CLexer::NewL();
	iLexer->DefineTokenTypeL(TToken::EPipe, KPipe);
	iLexer->DefineTokenTypeL(TToken::EAmpersand, KAmpersand);
	iLexer->DefineTokenTypeL(TToken::ESemicolon, KSemicolon);
	iLexer->DefineTokenTypeL(TToken::EDoublePipe, KDoublePipe);
	iLexer->DefineTokenTypeL(TToken::EDoubleAmpersand, KDoubleAmpersand);
	iLexer->DefineTokenTypeL(TToken::EAmpersandPipe, KAmpersandPipe);
	}

HBufC* SimplifyStringLC(const TDesC& aString, const TChar& aEscapeChar)
	{
	HBufC* buf = HBufC::NewLC(aString.Length());
	TPtr bufPtr(buf->Des());
	TLex lex(aString);
	while (!lex.Eos())
		{
		TChar c = lex.Get();
		if (c == aEscapeChar)
			{
			if (!lex.Eos())
				{
				bufPtr.Append(lex.Get());
				}
			}
		else if (c == '\'')
			{
			while (!lex.Eos())
				{
				c = lex.Get();
				if (c == '\'')
					{
					break;
					}
				else
					{
					bufPtr.Append(c);
					}
				}
			}
		else if (c == '"')
			{
			while (!lex.Eos())
				{
				c = lex.Get();
				if (c == aEscapeChar)
					{
					bufPtr.Append(lex.Get());
					}
				else if (c == '"')
					{
					break;
					}
				else
					{
					bufPtr.Append(c);
					}
				}
			}
		else
			{
			bufPtr.Append(c);
			}
		}
	return buf;
	}

void CLineCompleter::CompleteCommandNameL(TConsoleLine& aLine, const TToken& aToken, const TChar& aEscapeChar) const
	{
	RArray<TPtrC> commands;
	CleanupClosePushL(commands);
	iCommandFactory.ListCommandsL(commands);
	HBufC* commandBuf = SimplifyStringLC(aToken.String(), aEscapeChar);

	// Remove commands that don't match aLine.
	TInt i;
	for (i = (commands.Count() - 1); i >= 0; --i)
		{
		if (commands[i].Find(*commandBuf) != 0)
			{
			commands.Remove(i);
			}
		}

	if (commands.Count() > 0)
		{
		CompleteL(aLine, aToken, commands, NULL, NULL);
		}
	else
		{
		CompleteFileNameL(aLine, aToken, aEscapeChar);
		}
	CleanupStack::PopAndDestroy(2, &commands);
	}

void RemovePartialElement(HBufC*& aBuf)
	{
	TPtr ptr(aBuf->Des());
	while (ptr.Length() > 0)
		{
		if (ptr[ptr.Length() - 1] == KPathDelimiter)
			{
			break;
			}
		else
			{
			ptr.Delete(ptr.Length() - 1, 1);
			}
		}
	}

void CLineCompleter::CompleteFileNameL(TConsoleLine& aLine, const TToken& aToken, const TChar& aEscapeChar) const
	{
	HBufC* fileNameBuf = SimplifyStringLC(aToken.String(), aEscapeChar);
	IoUtils::TFileName2 absFileName(*fileNameBuf);
	RemovePartialElement(fileNameBuf);
	absFileName.Append(KWild);
	absFileName.MakeAbsoluteL(iEnv.Pwd());
	CDir* files;
	User::LeaveIfError(iFs.GetDir(absFileName, KEntryAttNormal | KEntryAttDir, ESortByName, files));
	CleanupStack::PushL(files);
	RArray<TPtrC> fileNames;
	CleanupClosePushL(fileNames);
	const TInt numFiles = files->Count();
	if (numFiles == 1)
		{
		absFileName = absFileName.DriveAndPath();
		absFileName.AppendComponentL((*files)[0].iName);
		TEntry entry;
		User::LeaveIfError(iFs.Entry(absFileName, entry));
		User::LeaveIfError(fileNames.Append(absFileName.NameAndExt()));
		CompleteL(aLine, aToken, fileNames, fileNameBuf, entry.IsDir() ? &KFileNameSlash : NULL);
		}
	else if (numFiles > 1)
		{
		for (TInt i = 0; i < numFiles; ++i)
			{
			User::LeaveIfError(fileNames.Append(TPtrC((*files)[i].iName)));
			}
		CompleteL(aLine, aToken, fileNames, fileNameBuf, NULL);
		}
	CleanupStack::PopAndDestroy(3, fileNameBuf);
	}

TBool NeedsQuoting(const TDesC& aDes)
	{
	return ((aDes.Locate(TChar(' ')) >= 0) || aDes.Locate(TChar('\'')) >= 0);
	}

void CLineCompleter::CompleteL(TConsoleLine& aLine, const TToken& aToken, const RArray<TPtrC> aPossibilities, const TDesC* aPrefix, const TDesC* aSuffix, TBool aPrefixIsPartOfToken) const
	{
	const TInt numPossibilities = aPossibilities.Count();

	if (numPossibilities > 1)
		{
		// Fill out possibilities buffer.
		IoUtils::CTextBuffer* possibilities = IoUtils::CTextBuffer::NewLC(0x100);
		for (TInt i = 0; i < numPossibilities; ++i)
			{
			if (aPrefixIsPartOfToken) possibilities->AppendL(*aPrefix);
			possibilities->AppendL(aPossibilities[i]);
			if (i != (numPossibilities - 1))
				{
				possibilities->AppendL(KTab);
				}
			}

		aLine.PrintCompletionPossibilitiesL(possibilities->Descriptor());
		CleanupStack::PopAndDestroy(possibilities);
		}

	if (numPossibilities > 0)
		{
		IoUtils::CTextBuffer* completion = IoUtils::CTextBuffer::NewLC(0x100);
		TPtrC commonChars(NULL, 0);
		if (numPossibilities > 1)
			{
			// Find common leading characters of the possibilities.
			TInt commonCharPos = -1;
			TBool finished(EFalse);
			do
				{
				++commonCharPos;
				TChar character(0);
				for (TInt i = 0; i < numPossibilities; ++i)
					{
					if (commonCharPos >= aPossibilities[i].Length())
						{
						finished = ETrue;
						break;
						}
					else if (i == 0)
						{
						character = aPossibilities[0][commonCharPos];
						character.Fold();
						}
					else
						{
						TChar c(aPossibilities[i][commonCharPos]);
						c.Fold();
						if (c != character)
							{
							finished = ETrue;
							break;
							}
						}
					}
				}
				while (!finished);

			commonChars.Set(aPossibilities[0].Mid(0, commonCharPos));
			}
		else
			{
			commonChars.Set(aPossibilities[0]);
			}

		TBool quote(EFalse);
		if ((aPrefix && NeedsQuoting(*aPrefix)) || NeedsQuoting(commonChars) || (aSuffix && NeedsQuoting(*aSuffix)))
			{
			quote = ETrue;
			}
		if (quote)
			{
			completion->AppendL(KDoubleQuote);
			}
		if (aPrefix)
			{
			completion->AppendL(*aPrefix);
			}
		completion->AppendL(commonChars);
		if ((numPossibilities == 1) && quote)
			{
			completion->AppendL(KDoubleQuote);
			}
		if (numPossibilities == 1)
			{
			if (aSuffix)
				{
				completion->AppendL(*aSuffix);
				}
			else
				{
				completion->AppendL(KSpace);
				}
			}
		if (completion->Descriptor().Length() > 0)
			{
			aLine.Replace(aToken.Position(), completion->Descriptor());
			}
		CleanupStack::PopAndDestroy(completion);
		}
	}

void CLineCompleter::CompleteEnvVarL(TConsoleLine& aLine, const TToken& aToken, const TChar& /*aEscapeChar*/) const
	{
	RArray<TPtrC> matchingVars;
	CleanupClosePushL(matchingVars);
	TPtrC envVar = aToken.String().Mid(1); // Lose the $

	RPointerArray<HBufC> keys;
	LtkUtils::CleanupResetAndDestroyPushL(keys);
	iEnv.GetKeysL(keys);
	
	const TInt count = keys.Count();
	for (TInt i = 0; i < count; i++)
		{
		const TDesC& key = *keys[i];
		if (key.Left(envVar.Length()) == envVar)
			{
			matchingVars.AppendL(TPtrC(key));
			}
		}
	CompleteL(aLine, aToken, matchingVars, &KDollar, NULL, ETrue);
	CleanupStack::PopAndDestroy(2, &matchingVars); // keys, matchingVars
	}

void CLineCompleter::CompleteOptionL(TConsoleLine& aLine, const TToken& aToken, const TChar& /*aEscapeChar*/) const
	{
	RArray<TPtrC> matchingOptions;
	CleanupClosePushL(matchingOptions);
	TPtrC optFrag = aToken.String();
	if (optFrag.Length() >= 2) optFrag.Set(optFrag.Mid(2)); // Lose the --
	else if (optFrag.Length() == 1) optFrag.Set(TPtrC()); // single dash gets dropped completely

	const IoUtils::RCommandOptionList& options = iLastUsedCif->Options();
	const TInt count = options.Count();
	for (TInt i = 0; i < count; i++)
		{
		const TDesC& optName = options[i].Name();;
		if (optName.Left(optFrag.Length()) == optFrag)
			{
			matchingOptions.AppendL(TPtrC(optName));
			}
		}
	CompleteL(aLine, aToken, matchingOptions, &KDashDash, NULL, ETrue);
	CleanupStack::PopAndDestroy(&matchingOptions);
	}
	


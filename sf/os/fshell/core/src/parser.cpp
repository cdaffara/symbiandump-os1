// parser.cpp
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

#include <fshell/ioutils.h>
#include <fshell/ltkutils.h>
#include "parser.h"
#include "fshell.h"
#include "lexer.h"
#include "command_factory.h"

//
// Globals.
//

_LIT(KChildError, "?");
_LIT(KPipe, "|");
_LIT(KDoublePipe, "||");
_LIT(KRedirectStdinFromFile, "<");
_LIT(KRedirectStdoutToFile1, ">");
_LIT(KRedirectStdoutToFile2, "1>");
_LIT(KRedirectStdoutToFileAppend1, ">>");
_LIT(KRedirectStdoutToFileAppend2, "1>>");
_LIT(KRedirectStdoutToStderr, "1>&2");
_LIT(KRedirectStderrToFile, "2>");
_LIT(KRedirectStderrToFileAppend, "2>>");
_LIT(KRedirectStderrToStdout, "2>&1");
_LIT(KAmpersand, "&");
_LIT(KDoubleAmpersand, "&&");
_LIT(KAmpersandPipe, "&|");
_LIT(KNewLine1, "\r");
_LIT(KNewLine2, "\n");
_LIT(KNewLine3, "\r\n");
_LIT(KNewLine4, "\n\r");
_LIT(KSemicolon, ";");
_LIT(KDollar, "$");


void MParserObserver::AboutToExecuteLine(const TDesC&, const TDesC&)
	{
	}

void MParserObserver::LineReturned(TInt)
	{
	}

CParser* CParser::NewL(TUint aMode, const TDesC& aDes, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory, MParserObserver* aObserver, TInt aStartingLineNumber)
	{
	CParser* self = new(ELeave) CParser(aMode, aDes, aIoSession, aStdin, aStdout, aStderr, aEnv, aFactory, aObserver, aStartingLineNumber);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CParser::~CParser()
	{
	delete iForegroundPipeLine;
	iBackgroundPipeLines.ResetAndDestroy();
	delete iLexer1;
	delete iLexer2;
	delete iCompletionCallBack;
	delete iNextPipeLineCallBack;
	delete iExitCallBack;
	if (iOwnsIoHandles)
		{
		iStdin.Close();
		iStdout.Close();
		iStderr.Close();
		}
	}

CParser::CParser(TUint aMode, const TDesC& aDes, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory, MParserObserver* aObserver, TInt aStartingLineNumber)
	: iMode(aMode), iData(aDes), iIoSession(aIoSession), iStdin(aStdin), iStdout(aStdout), iStderr(aStderr), iEnv(aEnv), iFactory(aFactory), iObserver(aObserver), iCompletionError(aStderr, aEnv), iNextLineNumber(aStartingLineNumber)
	{
	}

void CParser::ConstructL()
	{
	if (iObserver)
		{
		iCompletionCallBack = new(ELeave) CAsyncCallBack(TCallBack(CompletionCallBack, this), CActive::EPriorityStandard);
		}
	iNextPipeLineCallBack = new(ELeave) CAsyncCallBack(TCallBack(NextCallBack, this), CActive::EPriorityStandard);

	iLexer1 = CLexer::NewL(CLexer::EHandleSingleQuotes | CLexer::EHandleDoubleQuotes | CLexer::EHandleComments);
	iLexer1->DefineTokenTypeL(TToken::EDoublePipe, KDoublePipe);
	iLexer1->DefineTokenTypeL(TToken::EDoubleAmpersand, KDoubleAmpersand);
	iLexer1->DefineTokenTypeL(TToken::EAmpersandPipe, KAmpersandPipe);
	iLexer1->DefineTokenTypeL(TToken::ENewLine, KNewLine1);
	iLexer1->DefineTokenTypeL(TToken::ENewLine, KNewLine2);
	iLexer1->DefineTokenTypeL(TToken::ENewLine, KNewLine3);
	iLexer1->DefineTokenTypeL(TToken::ENewLine, KNewLine4);
	iLexer1->DefineTokenTypeL(TToken::ESemicolon, KSemicolon);
	iLexer1->Set(iData, iEnv.EscapeChar());

	iLexer2 = CLexer::NewL(CLexer::EHandleSingleQuotes | CLexer::EHandleDoubleQuotes);
	iLexer2->DefineTokenTypeL(TToken::EPipe, KPipe);
	iLexer2->DefineTokenTypeL(TToken::ERedirectStdinFromFile, KRedirectStdinFromFile);
	iLexer2->DefineTokenTypeL(TToken::ERedirectStdoutToFile, KRedirectStdoutToFile1);
	iLexer2->DefineTokenTypeL(TToken::ERedirectStdoutToFile, KRedirectStdoutToFile2);
	iLexer2->DefineTokenTypeL(TToken::ERedirectStdoutToFileAppend, KRedirectStdoutToFileAppend1);
	iLexer2->DefineTokenTypeL(TToken::ERedirectStdoutToFileAppend, KRedirectStdoutToFileAppend2);
	iLexer2->DefineTokenTypeL(TToken::ERedirectStdoutToStderr, KRedirectStdoutToStderr);
	iLexer2->DefineTokenTypeL(TToken::ERedirectStderrToFile, KRedirectStderrToFile);
	iLexer2->DefineTokenTypeL(TToken::ERedirectStderrToFileAppend, KRedirectStderrToFileAppend);
	iLexer2->DefineTokenTypeL(TToken::ERedirectStderrToStdout, KRedirectStderrToStdout);
	iLexer2->DefineTokenTypeL(TToken::EAmpersand, KAmpersand);

	if (iMode & EExportLineNumbers)
		{
		iEnv.SetL(KScriptLine, iNextLineNumber++);
		}
	if (iMode & EKeepGoing)
		{
		iEnv.SetL(KKeepGoing, 1);
		}
	}

void CParser::Start()
	{
	CreateNextPipeLine(NULL);
	}

void CParser::Start(TBool& aIsForeground)
	{
	CreateNextPipeLine(&aIsForeground);
	}

void CParser::Kill()
	{
	iAbort = ETrue;
	if (iForegroundPipeLine)
		{
		iForegroundPipeLine->Kill();
		}
	const TInt numBackground = iBackgroundPipeLines.Count();
	for (TInt i = 0; i < numBackground; ++i)
		{
		iBackgroundPipeLines[i]->Kill();
		}
	}

TInt CParser::Suspend()
	{
	TInt ret = KErrNone;
	if (iForegroundPipeLine)
		{
		TInt err = iForegroundPipeLine->Suspend();
		if (err)
			{
			ret = err;
			}
		}
	const TInt numBackground = iBackgroundPipeLines.Count();
	for (TInt i = 0; i < numBackground; ++i)
		{
		TInt err = iBackgroundPipeLines[i]->Suspend();
		if (err && (ret == KErrNone))
			{
			ret = err;
			}
		}
	return ret;
	}

TInt CParser::Resume()
	{
	TInt ret = KErrNone;
	if (iForegroundPipeLine)
		{
		TInt err = iForegroundPipeLine->Resume();
		if (err)
			{
			ret = err;
			}
		}
	const TInt numBackground = iBackgroundPipeLines.Count();
	for (TInt i = 0; i < numBackground; ++i)
		{
		TInt err = iBackgroundPipeLines[i]->Resume();
		if (err && (ret == KErrNone))
			{
			ret = err;
			}
		}
	return ret;
	}

TInt CParser::BringToForeground()
	{
	if (iForegroundPipeLine)
		{
		return iForegroundPipeLine->BringToForeground();
		}
	else if (iBackgroundPipeLines.Count() == 1)
		{
		return iBackgroundPipeLines[0]->BringToForeground();
		}
	return KErrGeneral;
	}

void CParser::SendToBackground()
	{
	if (iForegroundPipeLine)
		{
		iForegroundPipeLine->SendToBackground();
		}
	}

TInt CParser::Reattach(RIoEndPoint& aStdinEndPoint, RIoEndPoint& aStdoutEndPoint, RIoEndPoint& aStderrEndPoint)
	{
	RIoReadHandle stdin;
	RIoWriteHandle stdout;
	RIoWriteHandle stderr;
	TInt err = stdin.Create(iIoSession);
	if (err == KErrNone)
		{
		err = stdout.Create(iIoSession);
		if (err == KErrNone)
			{
			err = stderr.Create(iIoSession);
			if (err == KErrNone)
				{
				err = aStdinEndPoint.Attach(stdin, RIoEndPoint::EBackground);
				if (err == KErrNone)
					{
					err = aStdoutEndPoint.Attach(stdout);
					if (err == KErrNone)
						{
						err = aStderrEndPoint.Attach(stderr);
						if (err == KErrNone)
							{
							if (iForegroundPipeLine)
								{
								err = iForegroundPipeLine->Reattach(aStdinEndPoint, aStdoutEndPoint, aStderrEndPoint);
								}
							if (err == KErrNone)
								{
								const TInt numBackground = iBackgroundPipeLines.Count();
								for (TInt i = 0; i < numBackground; ++i)
									{
									err = iBackgroundPipeLines[i]->Reattach(aStdinEndPoint, aStdoutEndPoint, aStderrEndPoint);
									if (err)
										{
										break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	if (err == KErrNone)
		{
		iStdin = stdin;
		iStdout = stdout;
		iStderr = stderr;
		iOwnsIoHandles = ETrue;
		}
	else
		{
		stdin.Close();
		stdout.Close();
		stderr.Close();
		}
	return err;
	}

TBool CParser::IsDisownable() const
	{
	if (iForegroundPipeLine && !iForegroundPipeLine->IsDisownable())
		{
		return EFalse;
		}
	const TInt numBackground = iBackgroundPipeLines.Count();
	for (TInt i = 0; i < numBackground; ++i)
		{
		if (!(iBackgroundPipeLines[i]->IsDisownable()))
			{
			return EFalse;
			}
		}
	return ETrue;
	}

void CParser::Disown()
	{
	if (iForegroundPipeLine)
		{
		iForegroundPipeLine->Disown();
		}
	const TInt numBackground = iBackgroundPipeLines.Count();
	for (TInt i = 0; i < numBackground; ++i)
		{
		iBackgroundPipeLines[i]->Disown();
		}
	}

TBool HandleRedirectionL(TToken::TType aTokenType, const TDesC& aCwd, CLexer& aLexer, RPipeSection& aPipeSection)
	{
	RPipeSection::TRedirection* redirection;
	switch (aTokenType)
		{
		case TToken::ERedirectStdinFromFile:
			{
			redirection = &aPipeSection.iStdinRedirection;
			break;
			}
		case TToken::ERedirectStdoutToFile:
		case TToken::ERedirectStdoutToFileAppend:
		case TToken::ERedirectStdoutToStderr:
			{
			redirection = &aPipeSection.iStdoutRedirection;
			break;
			}
		case TToken::ERedirectStderrToFile:
		case TToken::ERedirectStderrToFileAppend:
		case TToken::ERedirectStderrToStdout:
			{
			redirection = &aPipeSection.iStderrRedirection;
			break;
			}
		default:
			{
			redirection = NULL;
			ASSERT(EFalse);
			}
		}

	if (aTokenType == TToken::ERedirectStderrToStdout)
		{
		redirection->iType = RPipeSection::TRedirection::EHandle;
		redirection->iHandle = RPipeSection::TRedirection::EStdout;
		}
	else if (aTokenType == TToken::ERedirectStdoutToStderr)
		{
		redirection->iType = RPipeSection::TRedirection::EHandle;
		redirection->iHandle = RPipeSection::TRedirection::EStderr;
		}
	else
		{
		if (aLexer.More())
			{
			redirection->iType = ((aTokenType == TToken::ERedirectStdoutToFileAppend) || (aTokenType == TToken::ERedirectStderrToFileAppend)) ? RPipeSection::TRedirection::EFileAppend : RPipeSection::TRedirection::EFile;
			TToken fileName(aLexer.NextToken());
			redirection->SetFileNameL(aCwd, fileName.String());
			}
		else
			{
			return EFalse;
			}
		}

	return ETrue;
	}

void CleanupPipeSectionsArray(TAny* aArray)
	{
	RArray<RPipeSection>* array = static_cast<RArray<RPipeSection>*>(aArray);
	const TInt numSections = array->Count();
	for (TInt i = 0; i < numSections; ++i)
		{
		(*array)[i].Close();
		}
	array->Close();
	}

void CParser::CreateNextPipeLine(TBool* aIsForeground)
	{
	TRAPD(err, CreateNextPipeLineL(aIsForeground));
	if (err && iObserver)
		{
		iCompletionError.Set(err, TError::EFailedToCreatePipeLine);
		iCompletionCallBack->CallBack();
		}
	}

void CParser::CreateNextPipeLineL(TBool* aIsForeground)
	{
	// Parsing is now carried out in three main steps:
	//
	// 1) Use iLexer1 to find the next "pipe-line's worth" of data (carried out by FindNextPipeLine).
	// 2) Expand environment variables in this data into a new HBufC ('expandedPipeLine' returned by ExpandVariablesLC).
	// 3) Use iLexer2 to parse the data in this expanded descriptor.

	if (aIsForeground)
		{
		*aIsForeground = ETrue;
		}

	TPtrC pipeLineData;
	TBool reachedLineEnd(EFalse);
	FindNextPipeLineL(pipeLineData, iCondition, reachedLineEnd);
	HBufC* expandedPipeLine = ExpandVariablesLC(pipeLineData);
	iLexer2->Set(*expandedPipeLine, iEnv.EscapeChar());

	const TDesC& cwd = iEnv.Pwd();
	RArray<RPipeSection> pipeSections;
	CleanupStack::PushL(TCleanupItem(CleanupPipeSectionsArray, &pipeSections));
	RPipeSection pipeSection;
	CleanupClosePushL(pipeSection);
	TInt offset = iLexer2->CurrentOffset();
	TBool background(EFalse);
	while (iLexer2->More())
		{
		TToken token(iLexer2->NextToken());
		switch (token.Type())
			{
			case TToken::EPipe:
				{
				pipeSection.iFullName.Set(iData.Ptr() + offset, iLexer2->CurrentOffset() - offset - token.String().Length());
				offset = iLexer2->CurrentOffset();
				User::LeaveIfError(pipeSections.Append(pipeSection));
				new(&pipeSection) RPipeSection;
				break;
				}
			case TToken::EAmpersand:
				{
				background = ETrue;
				break;
				}
			case TToken::ERedirectStdinFromFile:
			case TToken::ERedirectStdoutToFile:
			case TToken::ERedirectStdoutToFileAppend:
			case TToken::ERedirectStdoutToStderr:
			case TToken::ERedirectStderrToFile:
			case TToken::ERedirectStderrToFileAppend:
			case TToken::ERedirectStderrToStdout:
				{
				if (HandleRedirectionL(token.Type(), cwd, *iLexer2, pipeSection))
					{
					break;
					}
				// Deliberate fall through - if it wasn't possible to handle the redirection, treat token as a string.
				}
			case TToken::EString:
			default:
				{
				if (token.String().Length() > 0)
					{
					if (pipeSection.iCommandName.Length() == 0)
						{
						pipeSection.iCommandName.Set(token.String());
						}
					else
						{
						User::LeaveIfError(pipeSection.iCommandArguments.Append(TPtrC(token.String())));
						}
					}
				break;
				}
			}
		}

	if (pipeSection.iCommandName.Length() > 0)
		{
		_LIT(KExit, "exit");
		if ((pipeSections.Count() == 0) && (pipeSection.iCommandName.CompareF(KExit) == 0) && (pipeSection.iCommandArguments.Count() == 0))
			{
			// Special case the handling of 'exit'. This allows the concept of 'local commands' (i.e. commands that run in either fshell's main
			// thread or in the thread belonging to a 'source' or 'debug' command) to be dropped. That's a good thing, because local commands
			// can't synchronously interact with iosrv without risk of deadlock when two or more thread commands are run in a pipe-line. 
			CleanupStack::PopAndDestroy(2, &pipeSections);
			if (CActiveScheduler::Current()->StackDepth() > 0)
				{
				CActiveScheduler::Stop();
				}
			else
				{
				// The active scheduler hasn't been started yet. Probably because someone is doing something crazy like 'fshell -e exit'.
				iExitCallBack = new(ELeave) CAsyncCallBack(TCallBack(ExitCallBack, this), CActive::EPriorityStandard);
				iExitCallBack->Call();
				}
			}
		else
			{
			pipeSection.iFullName.Set(iData.Ptr() + offset, iLexer2->CurrentOffset() - offset);
			User::LeaveIfError(pipeSections.Append(pipeSection));
			CleanupStack::Pop(&pipeSection);
			if ((iMode & EDebug) && iObserver)
				{
				iObserver->AboutToExecuteLine(pipeLineData, *expandedPipeLine);
				}
			if (background)
				{
				CPipeLine* pipeLine = CPipeLine::NewLC(iIoSession, iStdin, iStdout, iStderr, iEnv, iFactory, pipeSections, background, this, iCompletionError);
				User::LeaveIfError(iBackgroundPipeLines.Append(pipeLine));
				CleanupStack::Pop(pipeLine);
				}
			else
				{
				ASSERT(iForegroundPipeLine == NULL);
				iForegroundPipeLine = CPipeLine::NewL(iIoSession, iStdin, iStdout, iStderr, iEnv, iFactory, pipeSections, background, this, iCompletionError);
				}
			CleanupStack::PopAndDestroy(&pipeSections);
			if (aIsForeground && !iLexer1->More())
				{
				*aIsForeground = !background;
				}
			if (background && iLexer1->More())
				{
				iNextPipeLineCallBack->Call();
				}
			}
		}
	else
		{
		if (iObserver && (iForegroundPipeLine == NULL) && (iBackgroundPipeLines.Count() == 0))
			{
			iCompletionCallBack->CallBack();
			}
		CleanupStack::PopAndDestroy(2, &pipeSections);
		}

	CleanupStack::PopAndDestroy(expandedPipeLine);

	if (reachedLineEnd && (iMode & EExportLineNumbers))
		{
		iEnv.SetL(KScriptLine, iNextLineNumber++);
		}
	}

void CParser::FindNextPipeLineL(TPtrC& aData, TCondition& aCondition, TBool& aReachedLineEnd)
	{
	aReachedLineEnd = EFalse;
	aCondition = ENone;
	TInt startOffset = iLexer1->CurrentOffset();
	TInt endOffset = -1;

	TBool foundSomething(EFalse);
	while (iLexer1->More())
		{
		TBool finished(EFalse);
		TToken token(iLexer1->NextToken());

		switch (token.Type())
			{
			case TToken::EString:
				{
				foundSomething = ETrue;
				endOffset = iLexer1->CurrentOffset();
				break;
				}
			case TToken::EDoublePipe:
				{
				finished = ETrue;
				aCondition = EOr;
				break;
				}
			case TToken::EDoubleAmpersand:
				{
				finished = ETrue;
				aCondition = EAnd;
				break;
				}
			case TToken::EAmpersandPipe:
				{
				finished = ETrue;
				aCondition = EAndOr;
				break;
				}
			case TToken::ENull:
			case TToken::ENewLine:
				{
				if (foundSomething)
					{
					// Leave it to CreateNextPipeLineL to increment SCRIPT_LINE when it's finished building the pipe-line.
					aReachedLineEnd = ETrue;
					}
				else
					{
					// Reached the end of a comment line - increment SCRIPT_LINE.
					if (iMode & EExportLineNumbers)
						{
						iEnv.SetL(KScriptLine, iNextLineNumber++);
						}
					}
				// Deliberate fall through.
				}
			case TToken::ESemicolon:
				{
				if (foundSomething)
					{
					finished = ETrue;
					}
				else
					{
					// Nothing on this line - adjust 'startOffset' to pretend it doesn't exist.
					startOffset = iLexer1->CurrentOffset();
					}
				break;
				}
			default:
				{
				ASSERT(EFalse);
				break;
				}
			}

		if (finished)
			{
			break;
			}
		}

	if (foundSomething)
		{
		aData.Set(iData.Ptr() + startOffset, endOffset - startOffset);
		}
	else
		{
		aData.Set(KNullDesC);
		}
	}

HBufC* ExpandVariablesLC(const TDesC& aData, CLexer& aLexer, IoUtils::CEnvironment& aEnv, TBool aEscape)
	{
	TChar escapeChar = aEnv.EscapeChar();
	RArray<TInt> charsToEscape;
	CleanupClosePushL(charsToEscape);
	HBufC* buf = aData.AllocLC();

	// Repeatedly check the data for environment variable tokens. This is done in a loop
	// because there could be variables within variables and we want to expand them all.
	aLexer.Set(*buf, escapeChar);
	FOREVER
		{
		TToken token(aLexer.NextToken());
		if (token.Type() == TToken::ENull)
			{
			break;
			}
		else if (token.Type() == TToken::EVariable)
			{
			const TDesC& val = aEnv.GetAsDesL(token.String().Mid(1));
			TPtr bufPtr(buf->Des());
			const TInt freeSpace = bufPtr.MaxLength() - bufPtr.Length();
			TInt requiredSpace = val.Length() - token.String().Length();
			if (aEscape)
				{
				charsToEscape.Reset();
				TLex lex(val);
				while (!lex.Eos())
					{
					TChar c = lex.Get();
					if ((c == aEnv.EscapeChar()) || (c == '\"'))
						{
						charsToEscape.AppendL(token.Position() + lex.Offset() - 1);
						++requiredSpace;
						}
					}
				}
			if (requiredSpace > freeSpace)
				{
				HBufC* oldBuf = buf;
				buf = buf->ReAllocL(bufPtr.MaxLength() + requiredSpace - freeSpace);
				CleanupStack::Pop(oldBuf);
				CleanupStack::PushL(buf);
				bufPtr.Set(buf->Des());
				}

			bufPtr.Replace(token.Position(), token.String().Length(), val);

			if (aEscape)
				{
				TPtrC escape((TUint16*)&escapeChar, 1);
				while (charsToEscape.Count() > 0)
					{
					bufPtr.Insert(charsToEscape[charsToEscape.Count() - 1], escape);
					charsToEscape.Remove(charsToEscape.Count() - 1);
					}
				}

			aLexer.Set(*buf, escapeChar);
			}
		}

	CleanupStack::Pop(buf);
	CleanupStack::PopAndDestroy(&charsToEscape);
	CleanupStack::PushL(buf);

	return buf;
	}

HBufC* CParser::ExpandVariablesLC(const TDesC& aData)
	{
	CLexer* lexer1 = CLexer::NewLC(CLexer::EHandleSingleQuotes | CLexer::EHandleDoubleQuotes  | CLexer::EHandleComments);
	CLexer* lexer2 = CLexer::NewLC(0);

	// Populate 'lexer2' with a token definition for each environment variable (preceded with '$').
	RPointerArray<HBufC> keys;
	LtkUtils::CleanupResetAndDestroyPushL(keys);
	iEnv.GetKeysL(keys);

	const TInt numVars = keys.Count();
	for (TInt i = 0; i < numVars; ++i)
		{
		keys[i] = keys[i]->ReAllocL(keys[i]->Length() + KDollar().Length());
		keys[i]->Des().Insert(0, KDollar);
		lexer2->DefineTokenTypeL(TToken::EVariable, *keys[i]);
		}

	HBufC* buf = aData.AllocLC();
	lexer1->Set(*buf, iEnv.EscapeChar());
	FOREVER
		{
		TToken token(lexer1->NextToken());
		if (token.Type() == TToken::ENull)
			{
			break;
			}
		else if (token.Type() == TToken::EString)
			{
			if (token.String()[0] != '\'')
				{
				HBufC* expandedString = ::ExpandVariablesLC(token.String(), *lexer2, iEnv, token.String()[0] == '\"');
				if (*expandedString != token.String())
					{
					TPtr bufPtr(buf->Des());
					const TInt freeSpace = bufPtr.MaxLength() - bufPtr.Length();
					const TInt requiredSpace = expandedString->Length() - token.String().Length();

					if (requiredSpace > freeSpace)
						{
						HBufC* oldBuf = buf;
						buf = buf->ReAllocL(bufPtr.MaxLength() + requiredSpace - freeSpace);
						CleanupStack::Pop(expandedString);
						CleanupStack::Pop(oldBuf);
						CleanupStack::PushL(buf);
						CleanupStack::PushL(expandedString);
						bufPtr.Set(buf->Des());
						}

					bufPtr.Replace(token.Position(), token.String().Length(), *expandedString);
					lexer1->Set(*buf, iEnv.EscapeChar());
					}
				CleanupStack::PopAndDestroy(expandedString);
				}
			}
		}

	CleanupStack::Pop(buf);
	CleanupStack::PopAndDestroy(3, lexer1);
	CleanupStack::PushL(buf);
	return buf;
	}

TInt CParser::SkipLineRemainder()
	{
	while (iLexer1->More())
		{
		TToken token(iLexer1->NextToken());
		if (token.Type() == TToken::ENewLine)
			{
			if (iMode & EExportLineNumbers)
				{
				// can we do something better with errors here?
				TRAPD(err, iEnv.SetL(KScriptLine, iNextLineNumber++));
				if (err!=KErrNone)
					{
					iCompletionError.Set(err, TError::EFailedToSetScriptLineVar);
					return err;
					}
				}
			break;
			}
		}
	return KErrNone;
	}

void CParser::SkipToEnd()
	{
	while (iLexer1->More())
		{
		iLexer1->NextToken();
		}
	}

TInt CParser::CompletionCallBack(TAny* aSelf)
	{
	CParser* self = static_cast<CParser*>(aSelf);
	self->iObserver->HandleParserComplete(*self, self->iCompletionError);
	return KErrNone;
	}

TInt CParser::NextCallBack(TAny* aSelf)
	{
	CParser* self = static_cast<CParser*>(aSelf);
	self->CreateNextPipeLine(NULL);
	return KErrNone;
	}

TInt CParser::ExitCallBack(TAny*)
	{
	CActiveScheduler::Stop();
	return KErrNone;
	}

void CParser::HandlePipeLineComplete(CPipeLine& aPipeLine, const TError& aError)
	{
	TRAPD(err, iEnv.SetL(KChildError, aError.Error()));
	if (err)
		{
		iCompletionError.Set(err, TError::EFailedToSetChildErrorVar);
		iCompletionCallBack->CallBack();
		return;
		}

	if ((iMode & EDebug) && iObserver)
		{
		iObserver->LineReturned(aError.Error());
		}

	if (iForegroundPipeLine == &aPipeLine)
		{
		switch (iCondition)
			{
			case ENone:
				{
				if ((aError.Error() != KErrNone) && !(iMode & EKeepGoing))
					{
					// Bail out of script if an error is found that isn't "handled" by an "&&" or and "||".
					SkipToEnd();
					iCompletionError.Set(aError);
					}
				break;
				}
			case EAnd:
				{
				if (iAbort)
					{
					SkipToEnd();
					}
				else if (aError.Error() != KErrNone)
					{
					TInt err = SkipLineRemainder();
					if (err!=KErrNone) SkipToEnd();
					}
				break;
				}
			case EOr:
				{
				if (iAbort)
					{
					SkipToEnd();
					}
				else if (aError.Error() == KErrNone)
					{
					TInt err = SkipLineRemainder();
					if (err!=KErrNone) SkipToEnd();
					}
				break;
				}
			case EAndOr:
				{
				if (iAbort)
					{
					SkipToEnd();
					}
				break;
				}
			default:
				{
				ASSERT(EFalse);
				}
			}

		delete iForegroundPipeLine;
		iForegroundPipeLine = NULL;

		if (iLexer1->More())
			{
			iNextPipeLineCallBack->Call();
			}
		}
	else
		{
		TInt pos = iBackgroundPipeLines.Find(&aPipeLine);
		ASSERT(pos >= 0);
		iBackgroundPipeLines.Remove(pos);
		if (aError.Error() != KErrNone)
			{
			iCompletionError.Set(aError);
			}
		delete &aPipeLine;
		}

	if (iObserver && !iLexer1->More() && (iForegroundPipeLine == NULL) && (iBackgroundPipeLines.Count() == 0))
		{
		iCompletionCallBack->CallBack();
		}
	}


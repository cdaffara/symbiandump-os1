// pod_formatter.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "pod_formatter.h"

const TInt KGap = 2;

_LIT(KPartialCommandStart, "\r\n=");
_LIT(KCommandStart, "\r\n\r\n=");
_LIT(KBlankLine, "\r\n\r\n");
_LIT(KNewLine, "\r\n");

using namespace IoUtils;


TPodFormatter::TPodFormatter(CTextFormatter& aFormatter)
	: iFormatter(aFormatter)
	{
	}

void TPodFormatter::FormatL(const TDesC& aPod)
	{
	iPodLex = aPod;
	while (!iPodLex.Eos())
		{
		TPtrC command(NextCommand());
		if (command == _L("pod"))
			{
			SkipToNextBlankLine();
			}
		else if ((command == _L("head1")) || (command == _L("head2")))
			{
			iPodLex.SkipSpaceAndMark();
			SkipToLineEnd();
			iFormatter.SetAttributesL(ConsoleAttributes::EBold | ConsoleAttributes::EUnderscore);
			iFormatter.WrapL(0, iPodLex.MarkedToken());
			iFormatter.SetAttributesL(ConsoleAttributes::ENone);
			iFormatter.AppendL(KBlankLine);
			}
		else if (command == _L("cut"))
			{
			SkipToNextBlankLine();
			break;
			}
		else if (command == _L("begin"))
			{
			TPtrC nextCommand;
			do
				{
				nextCommand.Set(NextCommand());
				}
				while ((nextCommand.Length() > 0) && (nextCommand != _L("end")));

			SkipToNextBlankLine();
			}
		else if (command == _L("for"))
			{
			SkipToNextBlankLine();
			}
		else if (command == _L("over"))
			{
			iPodLex.SkipSpace();
			TUint indent;
			User::LeaveIfError(iPodLex.Val(indent));
			OverL(indent);
			}

		FormatToNextCommandL();
		}
	}

void TPodFormatter::OverL(TInt aIndent)
	{
	CTextBuffer* buffer = CTextBuffer::NewLC(0x100);

	while (!iPodLex.Eos())
		{
		FormatToNextCommandL(aIndent);

		TPtrC command(NextCommand());
		if (command == _L("item"))
			{
			iPodLex.SkipSpaceAndMark();
			SkipToLineEnd();
			buffer->AppendL(iPodLex.MarkedToken());
			iPodLex.SkipSpace();
			//buffer->AppendL('\t');
			//buffer->AppendL(TextToNextCommand());
			// Need to split the text up putting a tab in front of every paragraph of text, in case the item description is multi-paragraph.
			// If we don't, the next paragraph will end up in column one which will break the column formatter.
			// If there's a neater way of doing this, someone feel free! -TomS
			TPtrC text(TextToNextCommand());
			while(text.Length())
				{
				TInt nextNewLine = text.Find(KNewLine);
				if (nextNewLine == KErrNotFound) nextNewLine = text.Length();
				TPtrC line(text.Left(nextNewLine+KNewLine().Length()));
				buffer->AppendL('\t');
				buffer->AppendL(line);
				text.Set(text.Mid(line.Length()));
				}
			buffer->AppendL(KNewLine);
			}
		else if (command == _L("back"))
			{
			break;
			}
		else if (command == _L("over"))
			{
			if (buffer->Descriptor().Length() > 0)
				{
				iFormatter.TabulateL(aIndent, KGap, buffer->Descriptor(), EWrapLastColumn);
				iFormatter.AppendL(KNewLine);
				buffer->Zero();
				}
			iPodLex.SkipSpace();
			TUint indent;
			User::LeaveIfError(iPodLex.Val(indent));
			OverL(indent + aIndent);
			}
		}

	if (buffer->Descriptor().Length() > 0)
		{
		iFormatter.TabulateL(aIndent, KGap, buffer->Descriptor(), EWrapLastColumn);
		iFormatter.AppendL(KNewLine);
		}

	CleanupStack::PopAndDestroy(buffer);
	}

void TPodFormatter::FormatToNextCommandL(TInt aIndent)
	{
	FOREVER
		{
		TPtrC paragraph(NextParagraph());
		if (paragraph.Length() > 0)
			{
			if (TChar(paragraph[0]).IsSpace())
				{
				iFormatter.AppendL(paragraph);
				}
			else
				{
				iFormatter.WrapL(aIndent, paragraph);
				}
			iFormatter.AppendL(KNewLine);
			}
		else
			{
			break;
			}
		}
	}

TPtrC TPodFormatter::NextCommand()
	{
	TPtrC command;
	if (SkipToNextCommand() == KErrNone)
		{
		iPodLex.Mark();
		iPodLex.SkipCharacters();
		command.Set(iPodLex.MarkedToken());
		}
	return command;
	}

TPtrC TPodFormatter::NextParagraph()
	{
	// Consume leading new line characters.
	while (iPodLex.Remainder().Find(KNewLine) == 0)
		{
		if (AtCommandStart())
			{
			return TPtrC();
			}
		iPodLex.Inc(KNewLine().Length());
		}
	
	iPodLex.Mark();

	if (iPodLex.Peek().IsSpace())
		{
		// This line has leading white space - treat everything up to the next blank line as a single paragraph.
		if (SkipToNextBlankLine() != KErrNone)
			{
			SkipToLineEnd();
			}
		}
	else if (SkipToNextLine() != KErrNone)
		{
		SkipToLineEnd();
		}

	return iPodLex.MarkedToken();
	}

TPtrC TPodFormatter::TextToNextCommand()
	{
	TInt initialOffset = iPodLex.Offset();
	iPodLex.Mark();

	if (SkipToNextCommand() == KErrNone)
		{
		if (iPodLex.Offset() >= KCommandStart().Length())
			{
			iPodLex.Inc(-(KCommandStart().Length()));
			}
		else
			{
			iPodLex.Inc(-(iPodLex.Remainder().Length()));
			}
		}
	else
		{
		iPodLex.Inc(iPodLex.Remainder().Length());
		}

	if (initialOffset < iPodLex.Offset())
		{
		return iPodLex.MarkedToken();
		}
	else
		{
		return TPtrC(NULL, 0);
		}
	}

void TPodFormatter::SkipToLineEnd()
	{
	if (SkipTo(KNewLine) != KErrNone)
		{
		iPodLex.Inc(iPodLex.Remainder().Length());
		}
	}

TInt TPodFormatter::SkipToNextLine()
	{
	TInt ret = SkipTo(KNewLine);
	if (ret == KErrNone)
		{
		iPodLex.Inc(KNewLine().Length());
		}
	return ret;
	}

TInt TPodFormatter::SkipToNextCommand()
	{
	if (iPodLex.Remainder().Find(KPartialCommandStart) == 0)
		{
		if (iPodLex.Offset() == 0)
			{
			iPodLex.Inc(KPartialCommandStart().Length());
			return KErrNone;
			}
		else
			{
			iPodLex.Inc(-2);
			if (iPodLex.Remainder().Find(KCommandStart) == 0)
				{
				iPodLex.Inc(KCommandStart().Length());
				return KErrNone;
				}
			else
				{
				iPodLex.Inc(2);
				}
			}
		}

	TInt ret = SkipTo(KCommandStart);
	if (ret == KErrNone)
		{
		iPodLex.Inc(KCommandStart().Length());
		}
	return ret;
	}

TInt TPodFormatter::SkipToNextBlankLine()
	{
	TInt ret = SkipTo(KBlankLine);
	if (ret == KErrNone)
		{
		iPodLex.Inc(2);
		}
	return ret;
	}

TInt TPodFormatter::SkipTo(const TDesC& aDes)
	{
	TInt ret = iPodLex.Remainder().Find(aDes);
	if (ret >= 0)
		{
		iPodLex.Inc(ret);
		return KErrNone;
		}
	return KErrNotFound;
	}

TBool TPodFormatter::AtCommandStart()
	{
	return ((iPodLex.Remainder().Find(KCommandStart) == 0) || (iPodLex.Remainder().Find(KPartialCommandStart) == 0));
	}


// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// NetDial Script Commands
// 
//

/**
 @file Scommand.cpp 
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SCOMMANDTraces.h"
#endif

#include "SSCRREAD.H"
#include "SSCREXEC.H"
#include "SIO.H"

// Command names

_LIT(KSetCommand,"SET");
_LIT(KSendCommand,"SEND");
_LIT(KWaitCommand,"WAIT");
_LIT(KLoopCommand,"LOOP");
_LIT(KGotoCommand,"GOTO");
_LIT(KExitCommand,"EXIT");
_LIT(KDTRCommand,"DTR");
_LIT(KDropDTRCommand,"RAISE");
_LIT(KRaiseDTRCommand,"DROP");
_LIT(KReadCommand,"READ");
_LIT(KCharmapCommand,"CHARMAP");
_LIT(KTempVarName,"$$TMPVAR$$%d");	//< Temporary variable name

// Characters used in scripts

const TText KCommentChar='!';	
const TText KPlusChar='+';
const TText KQuoteChar='\"';
const TText KOpenChevronChar='<';
const TText KCloseChevronChar='>';
const TText KOpenExprChar='{';
const TText KCloseExprChar='}';
const TText KEqualsChar='=';
const TText KTabChar='\t';
const TText KSpaceChar=' ';
const TText KColonChar=':';
const TText KOpenSquareBracketChar='[';
const TText KCloseSquareBracketChar=']';
const TInt KLabelArrayGranularity=5;
const TInt32 KMinScriptInteger=0;
const TInt32 KMaxScriptInteger=500000;
const TReal KMinScriptReal=0.0;
const TReal KMaxScriptReal=500000.0;
const TInt KMinLoopCounter=1;
const TInt KMaxLoopCounter=10000;

//
// Glossary
// ========
// String				- a text string enclosed in ".."
// Character			- a single character expressed numnerically enclosed in <..>
// Variable				- named variable (preset or set in script) ending in $
// Expression			- any one of the three above
// Compound expression	- a number of expressions concatenated with +
// Token				- an expression, compound expression, label or command
//

//
// CScriptCommandBase definitions
//

CScriptCommandBase::CScriptCommandBase(TScriptStatus& aStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv)
	: iScriptReader(aScriptReader), iVarMan(aVarMan), iCharConv(aCharConv), iTempVarNum(0), iStatus(aStatus)
/**
Constructor for CScriptCharacterConverter.
*/
	{}

CScriptCommandBase::~CScriptCommandBase()
/**
Destructor.
*/
	{}

void CScriptCommandBase::ConstructL()
/**
Instantiates member variables.
*/
	{}

void CScriptCommandBase::Cleanup()
/**
Cleanup.
*/
	{}

TPtrC CScriptCommandBase::ParseCompoundExpressionL(TInt& aOffset)
/**
Parses a compound string expression starting at aOffset into a single string
constant and returns it.
*/
	{
	TPtrC wholeExpr;
	wholeExpr.Set(ParseExpressionL(aOffset));
	TBuf<KMaxVarNameLength> varName;
	varName.Format(KTempVarName,iTempVarNum++);
	iVarMan->AddVariableL(varName,wholeExpr);
	HBufC* val=NULL;
	HBufC* oldVal=NULL;
	
	FOREVER
		{
		EatSpaces(aOffset);
		if (aOffset>=iStatus.iLine.Length())
			break;
		if (iStatus.iLine[aOffset]!=KPlusChar)
			break;
		TPtrC nextExpr;
		nextExpr.Set(ParseExpressionL(++aOffset));
		val=HBufC::NewLC(wholeExpr.Length()+nextExpr.Length());
		TPtr currentExpr(val->Des());
		currentExpr.Append(wholeExpr);
		currentExpr.Append(nextExpr);
		varName.Format(KTempVarName,iTempVarNum++);
		iVarMan->AddVariableL(varName,val->Des());
		CleanupStack::Pop();
		wholeExpr.Set(val->Des());
		delete oldVal;
		oldVal=val;
		}

	delete val;
	User::LeaveIfError(iVarMan->FindVariable(varName,wholeExpr));
	return wholeExpr;
	}

TPtrC CScriptCommandBase::ParseExpressionL(TInt& aOffset)
/**
Parses string expression starting at aOffset according to starting character
and returns value.
*/
	{
	EatSpaces(aOffset);
	if(iStatus.iLine.Length()<=aOffset)
		User::Leave(KErrNoExpression);

	switch(iStatus.iLine[aOffset])		//switch(iLine[aOffset+1])
		{
	case KQuoteChar:
		aOffset++;							// skip quote char
		return ParseStringL(aOffset);
	case KOpenChevronChar:
		aOffset++;							// skip chevron char
		return ParseCharacterL(aOffset);
	default:
		return ParseVariableL(aOffset);
		}
	}

TPtrC CScriptCommandBase::ParseStringL(TInt& aOffset)
/**
Parses a string constant enclosed in "" and returns its value.
*/
	{
	return ParseEnclosedStringL(aOffset,KQuoteChar,KErrMissingQuote);
	}

TPtrC CScriptCommandBase::ParseCharacterL(TInt& aOffset)
/**
Parses a character enclosed in <> and returns its value.
*/
	{
	TPtrC charString=ParseEnclosedStringL(aOffset,KCloseChevronChar,KErrMissingChevron);
	TRadix radix=EDecimal;
	TInt len=charString.Length();
	if (len>2)
		{
		if(charString[0]=='0')
			{
			switch(charString[1])
				{
			case 'x':
			case 'X':
				len-=2;
				radix=EHex;
				break;
			case 'd':
			case 'D':
				len-=2;
				radix=EDecimal;
				break;
			case 'o':
			case 'O':
				len-=2;
				radix=EOctal;
				break;
			case 'b':
			case 'B':
				len-=2;
				radix=EBinary;
				break;
			default:
				break;
				}
			}
		}

	TLex lex(charString.Right(len));
	TUint val=0;
	if (lex.Val(val,radix)!=KErrNone)
		User::Leave(KErrInvalidNumber);
	TBuf<1> character=(TText*)&val;
	TBuf<KMaxVarNameLength> varName;
	varName.Format(KTempVarName,iTempVarNum++);
	iVarMan->AddVariableL(varName,character);
	TPtrC temp;
	iVarMan->FindVariable(varName,temp);
	return temp;
	}

TPtrC CScriptCommandBase::ParseVariableL(TInt& aOffset)
/**
Parses a variable and returns its value.
*/
	{
	TInt end=FindTokenEnd(aOffset);
	TPtrC varName=iStatus.iLine.Mid(aOffset,end-aOffset);
	aOffset=end;
	TPtrC varValue;
	if (iVarMan->FindVariable(varName,varValue)==KErrNotFound)
		{
		if(!iStatus.iSkip)
			User::Leave(KErrVariableNotFound);
		return TPtrC();
		}
	return varValue;
	}

TInt32 CScriptCommandBase::ParseIntegerL(TInt& aOffset)
/**
Parses an integer and returns its value.
*/
	{
	EatSpaces(aOffset);
	TLex lex(iStatus.iLine);
	lex.Inc(aOffset);
	TInt32 val=0;
	
	if (lex.Val(val)!=KErrNone)
		{
		User::Leave(KErrInvalidNumber);
		return KMinScriptInteger;
		}

	if ((val<KMinScriptInteger) || (val>KMaxScriptInteger))
		{
		User::Leave(KErrNumberOutOfRange);
		return KMinScriptInteger;
		}

	aOffset=lex.Offset();
	return val;
	}

TReal CScriptCommandBase::ParseRealL(TInt& aOffset)
/**
Parses a real number and returns its value.
*/
	{
	EatSpaces(aOffset);
	TLex lex(iStatus.iLine);
	lex.Inc(aOffset);
	TReal val=0.0;

	if (lex.Val(val)!=KErrNone)
		{	
		User::Leave(KErrInvalidNumber);
		return KMinScriptReal;
		}
	if ((val<KMinScriptReal) || (val>KMaxScriptReal))
		{
		User::Leave(KErrNumberOutOfRange);
		return KMinScriptReal;
		}

	aOffset=lex.Offset();
	return val;
	}

TPtrC CScriptCommandBase::ParseCharacterTypeL(TInt& aOffset)
/**
Parses character type.
*/
	{
	EatSpaces(aOffset);

	if(iStatus.iLine.Length()<=aOffset)		// nothing specified so assume default
		return TPtrC();

	TInt startChar=aOffset;
	if (iStatus.iLine[startChar++]!=KOpenSquareBracketChar)
		return TPtrC();				// No open bracket so assume default

	aOffset=startChar;
	return ParseEnclosedStringL(aOffset,KCloseSquareBracketChar,KErrMissingBracket);
	}

HBufC8* CScriptCommandBase::ConvertLC(const TDesC& aString,TInt& aOffset)
/**
Converts to correct character set.
*/
	{
	TBuf<KMaxCharacterTypeLength> type=ParseCharacterTypeL(aOffset);
	return iCharConv->ConvertLC(aString,type);
	}

TInt CScriptCommandBase::FindTokenEnd(TInt aOffset)
/**
Finds end of string beginning at aOffset and returns length.
*/
	{
	__ASSERT_DEBUG(aOffset<=iStatus.iLine.Length(), NetDialPanic(EOffsetExceedsLineLength));

	if(aOffset==iStatus.iLine.Length())
		return aOffset;

	TInt end;
	for (end=aOffset; IsValidChar(iStatus.iLine[end]); end++)
		{
		if (end>=(iStatus.iLine.Length()-1))
			{
			end++;
			break;
			}
		}
	return end;
	}

TBool CScriptCommandBase::IsValidChar(const TText& aChar)
/**
Checks if aChar is a valid character in a script command.
*/
	{
	return TChar(aChar).IsAlphaDigit() || (aChar==KDollarChar) || (aChar==KUnderscoreChar) || (aChar==KCommentChar);
	}

void CScriptCommandBase::EatSpaces(TInt& aOffset)
/**
Ignores spaces, tabs, line feeds and carriage returns and sets aOffset to 
next non-blank character, or end of line
*/
	{
	if(aOffset>=iStatus.iLine.Length())
		return;

	while((iStatus.iLine[aOffset]==KSpaceChar) || (iStatus.iLine[aOffset]==KTabChar) || (iStatus.iLine[aOffset]==KLineFeed) || (iStatus.iLine[aOffset]==KCarriageReturn))
		{
		aOffset++;
		if(aOffset>=iStatus.iLine.Length())
			break;
		}
	}

void CScriptCommandBase::EatSpacesAndLinesL()
/**
Ignores spaces and empty lines and sets aOffset to next non-blank character.
*/
	{
	EatSpaces(iStatus.iOffset);
	while(iStatus.iOffset>=iStatus.iLine.Length())
		{
		User::LeaveIfError(iScriptReader->GetNextLine());		// This also resets iStatus.iOffset
		EatSpaces(iStatus.iOffset);
		}	
	}

void CScriptCommandBase::EatSpacesAndLinesAndCommentsL()
/**
Ignores spaces, empty lines and comments and sets aOffset to next non-blank or 
non-comment character
*/
	{
	EatSpacesAndLinesL();
	while (iStatus.iLine[iStatus.iOffset]==KCommentChar)
		{
		iStatus.iOffset=iStatus.iLine.Length();
		EatSpacesAndLinesL();		// this will reset iStatus.iOffset if necessary
		}
	}	

TPtrC CScriptCommandBase::ParseEnclosedStringL(TInt& aOffset,TText aChar,TInt aError)
/**
Parses enclosed string
*/
	{
	if(aOffset>=iStatus.iLine.Length())
		User::Leave(aError);			// no end character before EOL
	
	TInt end=aOffset;
	while (end<iStatus.iLine.Length())
		{
		if (iStatus.iLine[end]==aChar)
			break;
		end++;
		}

	if (end>=iStatus.iLine.Length())
		User::Leave(aError);			// no end character before EOL

	TInt start=aOffset;
	aOffset=end+1;
	return iStatus.iLine.Mid(start,end-start);
	}

//
// CSetCommand definitions
//

CSetCommand* CSetCommand::NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv)
/**
2 phased constructor for CSetCommand, first phase.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CSetCommand object.
*/
	{
	CSetCommand* c=new(ELeave) CSetCommand(aScriptStatus,aScriptReader,aVarMan,aCharConv);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}

CSetCommand::CSetCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv)
	: CScriptCommandBase(aScriptStatus,aScriptReader,aVarMan,aCharConv)
/**
Constructor for CSetCommand, used in the first phase of construction.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
*/
	{}

CSetCommand::~CSetCommand()
/**
Destructor.
*/
	{}

TBool CSetCommand::ParseL()
/**
Parses a SET command. Determines variable name and value and adds to variable list.
*/
	{
	EatSpacesAndLinesAndCommentsL();
	TInt end=FindTokenEnd(iStatus.iOffset);
	if (iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset).CompareF(KSetCommand)==KErrNone)
		{
		if (!iStatus.iSkip)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSETCOMMAND_PARSEL_1, "Script:\tExecuting Set");
			}
		iStatus.iOffset=end;
		EatSpaces(iStatus.iOffset);
		end=FindTokenEnd(iStatus.iOffset);
		TPtrC varName=iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset);
		EatSpaces(end);
		if (iStatus.iLine[end++]!=KEqualsChar)
			User::Leave(KErrNoEquals);
		EatSpaces(end);
		TPtrC value=ParseCompoundExpressionL(end);
		if (!iStatus.iSkip)
			{
			iVarMan->AddVariableL(varName,value);
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSETCOMMAND_PARSEL_2, "Script:\tSet Var: %S  To %S",varName,value);
			}
		iStatus.iOffset=end;
		return ETrue;		// Consumed
		}
	return EFalse;			// Not Consumed
	}

//
// CSendCommand definitions
//

CSendCommand* CSendCommand::NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptIO* aScriptIO)
/**
2 phased constructor for CSendCommand, first phase.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@param aScriptIO a pointer to serial comms I/O handler.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CSendCommand object.
*/
	{
	CSendCommand* c=new(ELeave) CSendCommand(aScriptStatus,aScriptReader,aVarMan,aCharConv,aScriptIO);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}

CSendCommand::CSendCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptIO* aScriptIO)
	: CScriptCommandBase(aScriptStatus,aScriptReader,aVarMan,aCharConv), iScriptIO(aScriptIO)
/**
Constructor for CSendCommand, used in the first phase of construction.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@param aScriptIO a pointer to serial comms I/O handler.
*/
	{}

CSendCommand::~CSendCommand()
/**
Destructor.
*/
	{}

TBool CSendCommand::ParseL()
/**
Parses SEND command.  Parses expression to send and sends it.
*/
	{
	EatSpacesAndLinesAndCommentsL();
	TInt end=FindTokenEnd(iStatus.iOffset);
	if(iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset).CompareF(KSendCommand)==0)
		{
		if(!iStatus.iSkip)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSENDCOMMAND_PARSEL_1,"Script:\tExecuting Send");
			}
		iStatus.iOffset=end;
		EatSpaces(iStatus.iOffset);
		TPtrC temp;
		temp.Set(ParseCompoundExpressionL(iStatus.iOffset));
		HBufC8* buf=ConvertLC(temp,iStatus.iOffset);
		iSendString.Set(buf->Des());
		if(!iStatus.iSkip)
			{
			iScriptIO->Write(iSendString);
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSENDCOMMAND_PARSEL_2, "Script:\tSending %s",iSendString);
			}
		CleanupStack::PopAndDestroy();
		return ETrue;		// Consumed
		}
	return EFalse;			// Not Consumed
	}

//
// CLabelSearch definitions
//

CLabelSearch* CLabelSearch::NewLC(const TDesC& aLabelName)
/**
2 phased constructor for CLabelSearch, first phase.

@param aLabelName is label name.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CLabelSearch object.
*/
	{
	CLabelSearch* label=new(ELeave) CLabelSearch();
	CleanupStack::PushL(label);
	label->ConstructL(aLabelName);
	return label;
	}

CLabelSearch::CLabelSearch()
	: iStatus(ENotFound), iPosition()
/**
Constructor for CLabelSearch, used in the first phase of construction.
*/
	{}

void CLabelSearch::ConstructL(const TDesC& aLabelName)
/**
Instantiates member variables.
*/
	{
	iChatString=NULL;
	iLabelName=HBufC::NewL(aLabelName.Length());
	(*iLabelName)=aLabelName;
	}

CLabelSearch::~CLabelSearch()
/**
Destructor.
Don't delete iChatString - that is dealt with by CScriptIO.
*/
	{
	delete iLabelName;
	}

void CLabelSearch::CreateCommChatStringL(const TDesC8& aDes,TBool aIsFolded)
/**
Creates CCommChatString object.
*/
	{
	iChatString=CCommChatString::NewL(aDes,aIsFolded);
	}

//
// CWaitCommand definitions
//

CWaitCommand* CWaitCommand::NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptIO* aScriptIO,CScriptLabelMan* aLabelMan, CScriptExecutor* aScriptExec)
/**
2 phased constructor for CWaitCommand, first phase.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@param aScriptIO a pointer to serial comms I/O handler.
@param aLabelMan a pointer to label manager.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CWaitCommand object.
*/
	{
	CWaitCommand* c=new(ELeave) CWaitCommand(aScriptStatus,aScriptReader,aVarMan,aCharConv,aScriptIO,aLabelMan,aScriptExec);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}

CWaitCommand::CWaitCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptIO* aScriptIO,CScriptLabelMan* aLabelMan, CScriptExecutor* aScriptExec)
	: CScriptCommandBase(aScriptStatus,aScriptReader,aVarMan,aCharConv), iScriptIO(aScriptIO), iLabelMan(aLabelMan), iScriptExec(aScriptExec)
/**
Constructor for CWaitCommand, used in the first phase of construction.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@param aScriptIO a pointer to serial comms I/O handler.
@param aLabelMan a pointer to label manager.
*/
	{}

CWaitCommand::~CWaitCommand()
/**
Destructor.
Clears label array.
*/
	{
	DeleteLabelArray();
	}

TBool CWaitCommand::ParseL()
/**
Parses WAIT command. Parse according to whether in skip mode or not.
*/
	{
	EatSpacesAndLinesAndCommentsL();
	TInt end=FindTokenEnd(iStatus.iOffset);
	if (iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset).CompareF(KWaitCommand)==KErrNone)
		{
		iStatus.iOffset=end;
		if (iStatus.iSkip)
			ParseSkipL();
		else
			ParseActionL();
		return ETrue;				// Consumed
		}
	return EFalse;					// Not Consumed
	}

void CWaitCommand::Cleanup()
/**
Cancels the read and clears out the labels
*/
	{
	if (!iStatus.iSkip)
		{
		iScriptIO->Cancel();
		DeleteLabelArray();
		iLabelArray=NULL;
		}
	}

void CWaitCommand::ParseActionL()
/**
Parses WAIT command when not in skip mode.  Parses wait period, strings and labels and 
queue a read.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAITCOMMAND_PARSEACTIONL_1, "Script:\tExecuting Wait");
	EatSpaces(iStatus.iOffset);
	TReal waitPeriod=ParseRealL(iStatus.iOffset);
	EatSpacesAndLinesAndCommentsL();
	if (iStatus.iLine[iStatus.iOffset++]!=KOpenExprChar)
		User::Leave(KErrNoOpenExpression);

	iLabelArray=new(ELeave) CLabelSearchArray(KLabelArrayGranularity);
	TBool completed=EFalse;
	while(!completed)
		{
		EatSpacesAndLinesAndCommentsL();
		if (iStatus.iLine[iStatus.iOffset]==KCloseExprChar)
			{
			completed=ETrue;
			iStatus.iOffset++;
			}
		else
			{
			TPtrC temp=ParseCompoundExpressionL(iStatus.iOffset);
			HBufC8* buf=ConvertLC(temp,iStatus.iOffset);
			CLabelSearch* label=ParseLabelLC();
			TPtrC8 waitString(buf->Des());	
			label->CreateCommChatStringL(waitString,EFalse);
			iLabelArray->AppendL(label);
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAITCOMMAND_PARSEACTIONL_2, "Script:\tIf %s is found, Goto %S",waitString,label->LabelName());
			CleanupStack::Pop();			// label - will be deleted from array
			CleanupStack::PopAndDestroy();	// buf - will have been copied into label array
			}			
		}
	// Tell the script executor how long we are going to wait since it
	// may need to force the server to resend the login prompt and will
	// need to know how long to wait the second time around
	iScriptExec->SetRetransmittedLoginTimeout(waitPeriod);
	iScriptIO->Read(iLabelArray,waitPeriod);
	}

void CWaitCommand::ParseSkipL()
/**
Parses a WAIT command when in skip mode. Parses expressions and labels but do not queue read.
*/
	{
	EatSpaces(iStatus.iOffset);
	ParseRealL(iStatus.iOffset);					// ignore the actual value returned
	EatSpacesAndLinesAndCommentsL();
	if (iStatus.iLine[iStatus.iOffset++]!=KOpenExprChar)
		User::Leave(KErrNoOpenExpression);

	TBool completed=EFalse;
	while (!completed)
		{
		EatSpacesAndLinesAndCommentsL();
		if (iStatus.iLine[iStatus.iOffset]==KCloseExprChar)
			{
			completed=ETrue;
			iStatus.iOffset++;
			}
		else
			{
			ParseCompoundExpressionL(iStatus.iOffset);			// ignore the return value in skip mode
			ParseLabelLC();
			CleanupStack::PopAndDestroy();				// ignore the label returned
			}
		}
	}

CLabelSearch* CWaitCommand::ParseLabelLC()
/**
Parses label.  Parses label name and looks for it in the label list.
Returns results of search.
*/
	{
	EatSpaces(iStatus.iOffset);
	TInt start=iStatus.iOffset;
	iStatus.iOffset=FindTokenEnd(start);
	TPtrC var=iStatus.iLine.Mid(start,iStatus.iOffset-start);
	CLabelSearch* labelSearch=CLabelSearch::NewLC(var);
	TLinePosition pos;
	if(iLabelMan->FindLabel(var,pos)==KErrNotFound)
		{
		pos.Reset();
		labelSearch->Set(CLabelSearch::ENotFound,pos);
		}
	else
		labelSearch->Set(CLabelSearch::EResolved,pos);
	return labelSearch;
	}

TPtrC CWaitCommand::LabelFromIndexL(TInt aIndex)
/**
Gets label from array by index.
*/
	{
	if ((aIndex<0) && (aIndex>iLabelArray->Count()))
		User::Leave(KErrIllegalWaitLabelIndex);
	return (*iLabelArray)[aIndex]->LabelName();
	}

void CWaitCommand::DeleteLabelArray()
/**
Deletes label array.
*/
	{
	if (iLabelArray!=NULL)
		{
		TInt count=iLabelArray->Count();
		for(TInt i=0; i<count; i++)
			{
			delete (*iLabelArray)[i];
			}
		iLabelArray->Delete(0,count);
		delete iLabelArray;
		}
	}
	
//
// CLoopCommand definitions
//

CLoopCommand* CLoopCommand::NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv)
/**
2 phased constructor for CLoopCommand, first phase.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CLoopCommand object.
*/
	{
	CLoopCommand* c=new(ELeave) CLoopCommand(aScriptStatus,aScriptReader,aVarMan,aCharConv);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}

CLoopCommand::CLoopCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv)
	: CScriptCommandBase(aScriptStatus,aScriptReader,aVarMan,aCharConv), iLoop(EFalse)
/**
Constructor for CLoopCommand, used in the first phase of construction.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
*/
	{}

CLoopCommand::~CLoopCommand()
/**
Destructor.
*/
	{}

TBool CLoopCommand::ParseL()
/**
Parses LOOP command. Checks how many times to loop and records position of beginning of loop.
*/
	{
	EatSpacesAndLinesAndCommentsL();
	TInt end=FindTokenEnd(iStatus.iOffset);
	if (iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset).CompareF(KLoopCommand)==KErrNone)
		{
		if (iLoop)
			User::Leave(KErrNestedLoop);
		if (!iStatus.iSkip)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CLOOPCOMMAND_PARSEL_1,"Script:\tExecuting Loop");
			}
		iStatus.iOffset=end;
		EatSpaces(iStatus.iOffset);
		iLoopCounter=ParseIntegerL(iStatus.iOffset);
		if ((iLoopCounter<KMinLoopCounter) || (iLoopCounter>KMaxLoopCounter))
			User::Leave(KErrLoopCounterOutOfRange);
		EatSpacesAndLinesAndCommentsL();
		if (iStatus.iLine[iStatus.iOffset++]!=KOpenExprChar)
			User::Leave(KErrNoOpenExpression);
		iLoop=ETrue;
		iScriptReader->CurrentPos(iLoopPosition,iStatus.iOffset);
		if (!iStatus.iSkip)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CLOOPCOMMAND_PARSEL_2, "Script:\tLoop Counter %d",iLoopCounter);
			}
		return ETrue;		// Consumed
		}
	return EFalse;			// Not Consumed
	}

TBool CLoopCommand::CheckLoopL()
/**
Checks how many times loop has been executed and returns to beginning if necessary.
*/
	{
	if (iLoop)
		{
		EatSpacesAndLinesAndCommentsL();
		if(iStatus.iLine[iStatus.iOffset]==KCloseExprChar)
			{
			if((--iLoopCounter==0) || (iStatus.iSkip))
				{
				iLoop=EFalse;
				iStatus.iOffset++;
				}
			else
				{
				iScriptReader->SetCurrentPos(iLoopPosition);
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CLOOPCOMMAND_CHECKLOOPL_1,"Script:\tRepeat Loop");
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CLOOPCOMMAND_CHECKLOOPL_2,"Script:\tLoop Counter %d",iLoopCounter);
				}
			return ETrue;				// Consumed Something
			}
		}
	return EFalse;						// Nothing doing...
	}

//
// CGotoCommand definitions
//

CGotoCommand* CGotoCommand::NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptLabelMan* aLabelMan)
/**
2 phased constructor for CGotoCommand, first phase.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@param aLabelMan a pointer to label manager.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CGotoCommand object.
*/	
	{
	CGotoCommand* c=new(ELeave) CGotoCommand(aScriptStatus,aScriptReader,aVarMan,aCharConv,aLabelMan);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}

CGotoCommand::CGotoCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptLabelMan* aLabelMan)
	: CScriptCommandBase(aScriptStatus,aScriptReader,aVarMan,aCharConv), iLabelMan(aLabelMan)
/**
Constructor for CGotoCommand, used in the first phase of construction.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@param aLabelMan a pointer to label manager.
*/
	{}

CGotoCommand::~CGotoCommand()
/**
Destructor.
*/
	{}

TBool CGotoCommand::ParseL()
/**
Parses GOTO command. Parses label and goto label.
*/
	{
	EatSpacesAndLinesAndCommentsL();
	TInt end=FindTokenEnd(iStatus.iOffset);
	if (iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset).CompareF(KGotoCommand)==KErrNone)
		{
		if(!iStatus.iSkip)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGOTOCOMMAND_PARSEL_1,"Script:\tExecuting Goto");
			}
		iStatus.iOffset=end;
		EatSpaces(iStatus.iOffset);
		end=FindTokenEnd(iStatus.iOffset);
		TPtrC var=iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset);
	//	HBufC* labelName=HBufC::NewLC(var.Length());
	//	(*labelName)=var;
	//	GotoL(labelName->Des());
	//	CleanupStack::PopAndDestroy();
		Goto(var);
		iStatus.iOffset=end;
		return ETrue;		// Consumed
		}
	return EFalse;			// Not Consumed
	}

void CGotoCommand::Goto(const TDesC& aLabelName)
/**
Finds label in those passed already, or skip lines until it is found.
*/
	{
	TLinePosition pos;
	if(!iStatus.iSkip)
		{
		if (iLabelMan->FindLabel(aLabelName,pos)==KErrNotFound)
			{
			iSearchName.Copy(aLabelName);
			iStatus.iSkipModeToggleReq=ETrue;
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGOTOCOMMAND_GOTO_1,"Script:\tSearching for Label %S",aLabelName);
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGOTOCOMMAND_GOTO_2, "Script:\tEntering Skip Mode");
			}
		else
			iScriptReader->SetCurrentPos(pos);
		}
	}

TBool CGotoCommand::ParseLabelL()
/**
Parses label and adds it to label list. Compares label against one to be found.
*/
	{
	EatSpacesAndLinesAndCommentsL();
	TInt start=iStatus.iOffset;
	TInt end=FindTokenEnd(start);
	if ((iStatus.iLine.Length()>=(end+1)) && (iStatus.iLine[end]==KColonChar))
		{
		TLinePosition pos;
		iScriptReader->CurrentPos(pos,end+1);
		iLabelMan->AddLabelL(iStatus.iLine.Mid(start,end-start),pos);
		iStatus.iOffset=++end;
		TLinePosition dummyPos;
		if (iStatus.iSkip && (iLabelMan->FindLabel(iSearchName,dummyPos)==KErrNone))
			{
			iStatus.iSkipModeToggleReq=ETrue;
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGOTOCOMMAND_PARSELABELL_1,"Script:\tExiting Skip Mode");
			}
		return ETrue;
		}
	return EFalse;
	}

void CGotoCommand::ServiceSkipReqs()
/**
Toggles skip mode.
*/
	{
	if(iStatus.iSkipModeToggleReq)
		{
		iStatus.iSkipModeToggleReq=EFalse;
		iStatus.iSkip=!iStatus.iSkip;
		}
	}

//
// CDTRCommand definitions
//

CDTRCommand* CDTRCommand::NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptIO* aScriptIO)
/**
2 phased constructor for CDTRCommand, first phase.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@param aScriptIO a pointer to serial comms I/O handler.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDTRCommand object.
*/
	{
	CDTRCommand* c=new(ELeave) CDTRCommand(aScriptStatus,aScriptReader,aVarMan,aCharConv,aScriptIO);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}

CDTRCommand::CDTRCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptIO* aScriptIO)
	: CScriptCommandBase(aScriptStatus,aScriptReader,aVarMan,aCharConv), iScriptIO(aScriptIO)
/**
Constructor for CDTRCommand, used in the first phase of construction.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@param aScriptIO a pointer to serial comms I/O handler.
*/
	{}

CDTRCommand::~CDTRCommand()
/**
Destructor.
*/
	{}

TBool CDTRCommand::ParseL()
/**
Parses DTR command. Drops or raises appropriately.
*/
	{
	EatSpacesAndLinesAndCommentsL();
	TInt end=FindTokenEnd(iStatus.iOffset);
	if (iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset).CompareF(KDTRCommand)==KErrNone)
		{
		if(!iStatus.iSkip)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDTRCOMMAND_PARSEL_1,"Script:\tExecuting DTR");
			}
		iStatus.iOffset=end;
		EatSpaces(iStatus.iOffset);
		end=FindTokenEnd(iStatus.iOffset);
		if (iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset).CompareF(KRaiseDTRCommand)==KErrNone)
			{
			if(!iStatus.iSkip)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDTRCOMMAND_PARSEL_2,"Script:\tRaising DTR");
				iScriptIO->RaiseDTR(NULL);
				}	
			}
		else if (iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset).CompareF(KDropDTRCommand)==KErrNone)
			{
			if(!iStatus.iSkip)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDTRCOMMAND_PARSEL_3,"Script:\tDropping DTR");
				iScriptIO->DropDTR(NULL);
				}	
			}
		else
			User::Leave(KErrNoDropOrRaise);
		iStatus.iOffset=end;
		return ETrue;		// Consumed
		}
	return EFalse;			// Not Consumed
	}

//
// CReadPCTCommand definitions
//

CReadPCTCommand* CReadPCTCommand::NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptExecutor* aScriptExec)
/**
2 phased constructor for CReadPCTCommand, first phase.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@param aScriptExec a pointer to script executioner.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CReadPCTCommand object.
*/
	{
	CReadPCTCommand* c=new(ELeave) CReadPCTCommand(aScriptStatus,aScriptReader,aVarMan,aCharConv,aScriptExec);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}

CReadPCTCommand::CReadPCTCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv,CScriptExecutor* aScriptExec)
	: CScriptCommandBase(aScriptStatus,aScriptReader,aVarMan,aCharConv), iScriptExec(aScriptExec)
/**
Constructor for CDTRCommand, used in the first phase of construction.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@param aScriptExec a pointer to script executioner.
*/
	{}

CReadPCTCommand::~CReadPCTCommand()
/**
Destructor.
*/
	{}

TBool CReadPCTCommand::ParseL()
/**
Parses Read command.
*/
	{
	EatSpacesAndLinesAndCommentsL();
	TInt end=FindTokenEnd(iStatus.iOffset);
	if (iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset).CompareF(KReadCommand)==KErrNone)
		{
		if (!iStatus.iSkip)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREADPCTCOMMAND_PARSEL_1, "Script:\tExecuting Read");
			iCharSet=ParseCharacterTypeL(iStatus.iOffset);
			iScriptExec->ReadPct();
			}
		iStatus.iOffset=end;
		return ETrue;		// Consumed
		}
	return EFalse;			// Not Consumed
	}

TBool CReadPCTCommand::CheckReadL()
/**
Checks Read.
*/
	{
	EatSpacesAndLinesAndCommentsL();
	TInt end=FindTokenEnd(iStatus.iOffset);
	if (iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset).CompareF(KReadCommand)==KErrNone)
		return ETrue;
	else
		return EFalse;
	}

TPtrC CReadPCTCommand::CharSet()
/**
Returns used character set.
*/
	{
	return iCharSet;
	}

//
// CCharMapCommand definitions
//

CCharMapCommand* CCharMapCommand::NewL(TScriptStatus& aScriptStatus, CScriptReader* aScriptReader, CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv)
/**
2 phased constructor for CCharMapCommand, first phase.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CCharMapCommand object.
*/
	{
	CCharMapCommand* c=new(ELeave) CCharMapCommand(aScriptStatus,aScriptReader,aVarMan,aCharConv);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}

CCharMapCommand::CCharMapCommand(TScriptStatus& aScriptStatus, CScriptReader* aScriptReader, CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv)
	: CScriptCommandBase(aScriptStatus,aScriptReader,aVarMan,aCharConv)
/**
Constructor for CCharMapCommand, used in the first phase of construction.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
*/
	{}

CCharMapCommand::~CCharMapCommand()
/**
Destructor.
*/
	{}	

TBool CCharMapCommand::ParseL()
/**
Parse.
*/
	{
	EatSpacesAndLinesAndCommentsL();
	TInt end=FindTokenEnd(iStatus.iOffset);
	if (iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset).CompareF(KCharmapCommand)==KErrNone)
		{
		iStatus.iOffset=end;
		if (!iStatus.iSkip)
			{
			iCharConv->SetDefaultCharSet(ParseCharacterTypeL(iStatus.iOffset));
			}
		return ETrue;		// Consumed
		}
	return EFalse;			// Not Consumed
	}

//
// CExitCommand definiton
//

CExitCommand* CExitCommand::NewL(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv)
/**
2 phased constructor for CExitCommand, first phase.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CExitCommand object.
*/
	{
	CExitCommand* c=new(ELeave) CExitCommand(aScriptStatus,aScriptReader,aVarMan,aCharConv);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}

CExitCommand::CExitCommand(TScriptStatus& aScriptStatus,CScriptReader* aScriptReader,CScriptVarMan* aVarMan,CScriptCharacterConverter* aCharConv)
	: CScriptCommandBase(aScriptStatus,aScriptReader,aVarMan,aCharConv)
/**
Constructor for CExitCommand, used in the first phase of construction.

@param aScriptStatus is script status.
@param aScriptReader a pointer to script reader.
@param aVarMan a pointer to variable manager.
@param aCharConv a pointer to script character converter.
*/
	{}

CExitCommand::~CExitCommand()
/**
Destructor.
*/
	{}

TBool CExitCommand::ParseL()
/**
Parses EXIT command. Parses number or variable and leaves with the appropriate value.
*/
	{
	EatSpacesAndLinesAndCommentsL();
	TInt end=FindTokenEnd(iStatus.iOffset);
	if (iStatus.iLine.Mid(iStatus.iOffset,end-iStatus.iOffset).CompareF(KExitCommand)==KErrNone)
		{
		if (!iStatus.iSkip)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CEXITCOMMAND_PARSEL_1,"Script:\tExecuting Exit");
			}
		iStatus.iOffset=end;
		EatSpaces(iStatus.iOffset);
		TPtrC exitStatus;
		TRAPD(ret,(exitStatus.Set(ParseCompoundExpressionL(iStatus.iOffset))));
		if (!iStatus.iSkip)
			{
			if (ret==KErrNone)
				{
				TInt32 val;
				TLex lex(exitStatus);
				if(lex.Val(val)!=KErrNone)
					User::Leave(KErrInvalidNumber);
				if(val>0)
					User::Leave(KErrNumberOutOfRange);
				if(val==0)
					val=(TInt32)KErrScriptCompleted;
				User::Leave(val);
				}
			else if (ret==KErrNoExpression)
				User::Leave(KErrScriptCompleted);
			else
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CEXITCOMMAND_PARSEL_2,"Script:\tExit With Error %d",ret);
				User::Leave(ret);
				}
			}
		return ETrue;		// Consumed
		}
	return EFalse;			// Not Consumed
	}

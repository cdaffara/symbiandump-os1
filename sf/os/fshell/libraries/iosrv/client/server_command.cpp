// server_command.cpp
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

#include <fshell/ioutils.h>
#include <fshell/line_editor.h>
#include "command_base.h"

using namespace IoUtils;


//
// Constants.
//

_LIT(KTab, "\t");
_LIT(KSpace, " ");
_LIT(KNewLine, "\r\n");
_LIT(KHelpCommandName, "help");


// used for sorting RArray<TPtrC>'s
TInt PtrOrder(const TPtrC& aDennis, const TPtrC& aGnasher)
	{
	return aDennis.Compare(aGnasher);
	}


//
// TServerCommandId.
//

EXPORT_C TServerCommandId::TServerCommandId()
	: iId(0)
	{
	}

EXPORT_C TServerCommandId::TServerCommandId(TUint aValue)
	: iId(aValue)
	{
	}

EXPORT_C TUint TServerCommandId::Value() const
	{
	return iId;
	}

EXPORT_C void TServerCommandId::Set(const TServerCommandId& aId)
	{
	iId = aId.iId;
	}

EXPORT_C TBool TServerCommandId::operator==(const TServerCommandId& aId) const
	{
	return (iId == aId.iId);
	}


//
// CServerCommandBase.
//

EXPORT_C CServerCommandBase::~CServerCommandBase()
	{
	}

EXPORT_C const TServerCommandId& CServerCommandBase::Id() const
	{
	return iId;
	}

EXPORT_C void CServerCommandBase::SetId(const TServerCommandId& aId)
	{
	iId.Set(aId);
	}

EXPORT_C void CServerCommandBase::SetFactory(CServerCommandFactory& aFactory)
	{
	iFactory = &aFactory;
	}

EXPORT_C CServerCommandBase::CServerCommandBase()
	{
	}

EXPORT_C CServerCommandBase::CServerCommandBase(TUint aFlags)
	: CCommandBase(aFlags)
	{
	}

EXPORT_C CServerCommandFactory& CServerCommandBase::Factory()
	{
	__ASSERT_ALWAYS(iFactory, Panic(ENoFactory));
	return *iFactory;
	}

EXPORT_C void CServerCommandBase::PrintList(TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	if (iReporter)
		{
		iReporter->ReportList(iId, aFmt, aList);
		}
	else
		{
		CCommandBase::PrintList(aFmt, aList);
		}
	}

EXPORT_C void CServerCommandBase::PrintList(TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
	{
	if (iReporter)
		{
		iReporter->ReportList(iId, aFmt, aList);
		}
	else
		{
		CCommandBase::PrintList(aFmt, aList);
		}
	}

EXPORT_C void CServerCommandBase::PrintErrorList(TInt aError, TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	if (iReporter)
		{
		iReporter->ReportErrorList(iId, aError, aFmt, aList);
		}
	else
		{
		CCommandBase::PrintErrorList(aError, aFmt, aList);
		}
	}

EXPORT_C void CServerCommandBase::PrintWarningList(TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	if (iReporter)
		{
		iReporter->ReportWarningList(iId, aFmt, aList);
		}
	else
		{
		CCommandBase::PrintWarningList(aFmt, aList);
		}
	}

void CServerCommandBase::SetReporter(MServerCommandReporter* aReporter)
	{
	iReporter = aReporter;
	}


//
// CHelpCommand.
//

NONSHARABLE_CLASS(CHelpCommand) : public CServerCommandBase
	{
public:
	static CServerCommandBase* NewLC();
	~CHelpCommand();
	static const TDesC& NameS();
private:
	CHelpCommand();
	void ConstructL();
	void GenerateTextL(const CServerCommandFactory& aFactory, TInt aIndent = 0);
	HBufC* FullCommandNameLC() const;
	void Write(TInt aIndent, const TDesC& aCommandName);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
private:
	CTextBuffer* iBuffer;
	CTextFormatter* iFormatter;
	RArray<TPtrC> iCommandNameStack;
	RFile iPodIndex;
	};

CServerCommandBase* CHelpCommand::NewLC()
	{
	CHelpCommand* self = new(ELeave) CHelpCommand();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CHelpCommand::~CHelpCommand()
	{
	delete iBuffer;
	delete iFormatter;
	iPodIndex.Close();
	iCommandNameStack.Close();
	}

const TDesC& CHelpCommand::NameS()
	{
	return KHelpCommandName;
	}

CHelpCommand::CHelpCommand()
	{
	}

void CHelpCommand::ConstructL()
	{
	BaseConstructL();
	}

const TDesC& CHelpCommand::Name() const
	{
	return KHelpCommandName;
	}

const TDesC& CHelpCommand::Description() const
	{
	_LIT(KDescription, "List the commands the amsrv supports.");
	return KDescription;
	}
	
void CHelpCommand::GenerateTextL(const CServerCommandFactory& aFactory, TInt aIndent)
	{
	RArray<TPtrC> commands;
	CleanupClosePushL(commands);
	aFactory.ListCommandsL(commands);
	commands.Sort(TLinearOrder<TPtrC>(PtrOrder));
	
	const TInt numCommands = commands.Count();
	for (TInt i = 0; i < numCommands; ++i)
		{
		const TDesC& commandName = commands[i];
		Write(aIndent, commandName);

		const CServerCommandConstructor& cmnd = aFactory.GetSubCommandL(commandName);
		if (cmnd.IsFactory())
			{
			GenerateTextL((const CServerCommandFactory&)cmnd, aIndent+1);
			}
		}
	CleanupStack::PopAndDestroy();
	}

HBufC* CHelpCommand::FullCommandNameLC() const
	{
	const TInt numCommands = iCommandNameStack.Count();
	TInt length = 0;
	for (TInt i = 0; i < numCommands; ++i)
		{
		length += iCommandNameStack[i].Length();
		if (i > 0)
			{
			++length;
			}
		}
	HBufC* name = HBufC::NewLC(length);
	TPtr namePtr(name->Des());
	for (TInt i = 0; i < numCommands; ++i)
		{
		if (i > 0)
			{
			namePtr.Append('_');
			}
		namePtr.Append(iCommandNameStack[i]);
		}
	return name;
	}

void CHelpCommand::Write(TInt aIndent, const TDesC& aCommandName)
	{
	_LIT(KIndent, "  ");

	if (iBuffer)
		{
		for (TInt j=0; j<aIndent; ++j)
			{
			iBuffer->AppendL(KIndent);
			}
		iBuffer->AppendL(aCommandName);
		iBuffer->AppendL(KTab);
		}
	else
		{
		for (TInt j=0; j<aIndent; ++j)
			{
			CCommandBase::Write(KIndent);
			}
		CCommandBase::Write(aCommandName);
		CCommandBase::Write(KNewLine);
		}
	}

void CHelpCommand::DoRunL()
	{
	if (Stdout().AttachedToConsole())
		{
		iBuffer = CTextBuffer::NewL(0x100);
		iFormatter = CTextFormatter::NewL(Stdout());
		}
	GenerateTextL(Factory());
	if (iBuffer)
		{
		iFormatter->ColumnizeL(0, 2, iBuffer->Descriptor());
		CCommandBase::Write(iFormatter->Descriptor());
		}
	CCommandBase::Write(_L("\r\n"));
	}


//
// CRootCommand.
//

NONSHARABLE_CLASS(CRootCommand) : public CBranchCommandBase
	{
public:
	static CRootCommand* NewL(CServerCommandFactory& aFactory);
	~CRootCommand();
private:
	CRootCommand();
	void ConstructL(CServerCommandFactory& aFactory);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
private: // From CAmCmndParent.
	virtual const TDesC& TypeDescription();
	};

CRootCommand* CRootCommand::NewL(CServerCommandFactory& aFactory)
	{
	CRootCommand* root = new(ELeave)CRootCommand();
	CleanupStack::PushL(root);
	root->ConstructL(aFactory);
	CleanupStack::Pop(root);
	return root;
	}

CRootCommand::~CRootCommand()
	{
	}

CRootCommand::CRootCommand()
	{
	}

void CRootCommand::ConstructL(CServerCommandFactory& aFactory)
	{
	SetFactory(aFactory);
	}

const TDesC& CRootCommand::Name() const
	{
	_LIT(KCommandName, "root");
	return KCommandName;
	}

const TDesC& CRootCommand::Description() const
	{
	_LIT(KDescription, "A pseudo-command that is the ancestor of all other commands available in this server. Not intended to be run directly.");
	return KDescription;
	}

const TDesC& CRootCommand::TypeDescription()
	{
	return KNullDesC;
	}


//
// CServerBase.
//

EXPORT_C CServerBase::~CServerBase()
	{
	delete iLineEditor;
	delete iFactory;
	}

EXPORT_C CServerBase::CServerBase(TUint aFlags, const TDesC& aPrompt, const TDesC& aCommandHistoryFileName)
	: CCommandBase(aFlags), iPrompt(aPrompt), iCommandHistoryFileName(aCommandHistoryFileName), iWriterAdaptor(Stdout())
	{
	}

EXPORT_C CServerCommandFactory& CServerBase::Factory()
	{
	return *iFactory;
	}

EXPORT_C void CServerBase::Exit(TInt aError)
	{
	iExitReason = aError;
	iExit = ETrue;
	}

EXPORT_C void CServerBase::BaseConstructL()
	{
	CCommandBase::BaseConstructL();
	}

EXPORT_C void CServerBase::DoRunL()
	{
	iFactory = CServerCommandFactory::NewL(this, Cif());
	iLineEditor = CLineEditor::NewL(FsL(), iWriterAdaptor, *this, *iFactory, iCommandHistoryFileName);
	InitializeL();
	RProcess::Rendezvous(KErrNone);

	do
		{
		CheckNewConsoleLine();
		Write(iPrompt);
		ParseLineL(NextLineL());
		}
		while (!iExit);

	Complete(iExitReason);
	}

EXPORT_C void CServerBase::PrintList(TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	TOverflowTruncate overflow;
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, aList, &overflow);
	Report(TServerCommandId(), buf);
	}

EXPORT_C void CServerBase::PrintList(TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
	{
	TOverflowTruncate8 overflow;
	TBuf8<0x100> buf;
	buf.AppendFormatList(aFmt, aList, &overflow);
	Report(TServerCommandId(), buf);
	}

EXPORT_C void CServerBase::PrintErrorList(TInt aError, TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	if (!iErrorReported)
		{
		TOverflowTruncate overflow;
		TBuf<0x100> buf;
		buf.AppendFormatList(aFmt, aList, &overflow);
		ReportError(TServerCommandId(), aError, buf);
		iErrorReported = ETrue;
		}
	}

EXPORT_C void CServerBase::PrintWarningList(TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	TOverflowTruncate overflow;
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, aList, &overflow);
	ReportWarning(TServerCommandId(), buf);
	}

EXPORT_C void CServerBase::Report(const TServerCommandId& aId, TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	ReportList(aId, aFmt, list);
	VA_END(list);
	}

EXPORT_C void CServerBase::ReportList(const TServerCommandId& aId, TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	TOverflowTruncate overflow;
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, aList, &overflow);
	Report(aId, buf);
	}

EXPORT_C void CServerBase::Report(const TServerCommandId& aId, TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	ReportList(aId, aFmt, list);
	VA_END(list);
	}

EXPORT_C void CServerBase::ReportList(const TServerCommandId& aId, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
	{
	TOverflowTruncate8 overflow;
	TBuf8<0x100> buf;
	buf.AppendFormatList(aFmt, aList, &overflow);
	Report(aId, buf);
	}

EXPORT_C void CServerBase::ReportWarning(const TServerCommandId& aId, TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	ReportWarningList(aId, aFmt, list);
	VA_END(list);
	}

EXPORT_C void CServerBase::ReportWarningList(const TServerCommandId& aId, TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	TOverflowTruncate overflow;
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, aList, &overflow);
	ReportWarning(aId, buf);
	}

EXPORT_C void CServerBase::ReportError(const TServerCommandId& aId, TInt aError, TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	ReportErrorList(aId, aError, aFmt, list);
	VA_END(list);
	}

EXPORT_C void CServerBase::ReportErrorList(const TServerCommandId& aId, TInt aError, TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	if (!iErrorReported)
		{
		TOverflowTruncate overflow;
		TBuf<0x100> buf;
		buf.AppendFormatList(aFmt, aList, &overflow);
		ReportError(aId, aError, buf);
		iErrorReported = ETrue;
		}
	}

EXPORT_C void CServerBase::LeoHandleLine(const TDesC& aLine)
	{
	iLine = aLine;
	iLineRead = ETrue;
	}

void CServerBase::ParseLineL(const TDesC& aLine)
	{
	if (aLine.Length() > 0)
		{
		iErrorReported = EFalse;
		CRootCommand* root = CRootCommand::NewL(Factory());
		root->SetReporter(this);
		root->RunCommand(IoSession(), Stdin(), Stdout(), Stderr(), &aLine, &Env(), NULL);
		__ASSERT_ALWAYS(root->IsComplete(), Panic(EUnexpectedAsyncCommand));
		delete root;
		}
	}

const TDesC& CServerBase::NextLineL()
	{
retry:
	iLine.Zero();

	if (Stdin().AttachedToConsole())
		{
		iLineEditor->Start(KNullDesC);
		iLineEditor->ReinstatePromptAndUserInput();
		iLineRead = EFalse;
		do
			{
			TRequestStatus status;
			Stdin().WaitForKey(status);
			User::WaitForRequest(status);
			if (status.Int() == KErrNotSupported)
				{
				// Likely that we're no longer attached to a console and are instead attached to a pipe.
				// Note, using "pcons" directly gives us a console, but using "am.exe" gives us a pipe.
				iLineEditor->Abort();
				goto retry;
				}
			User::LeaveIfError(status.Int());
			iLineEditor->HandleKey(Stdin().KeyCode(), Stdin().KeyModifiers());
			}
			while (!iLineRead);
		}
	else
		{
		LeaveIfErr(Stdin().SetReadMode(RIoReadHandle::ELine), _L("Couldn't set stdin to line mode"));
		Stdin().SetLineSeparatorL(KNewLine());
		User::LeaveIfError(Stdin().Read(iLine));
		}

	iLine.TrimRight();
	return iLine;
	}

EXPORT_C void CServerBase::CheckNewConsoleLine()
	{
	if (Stdout().AttachedToConsole())
		{
		TPoint cursorPos(0, 0);
		Stdout().GetCursorPos(cursorPos);
		if (cursorPos.iX != 0)
			{
			Stdout().Write(KNewLine);
			}
		}
	}


//
// CServerCommandConstructor.
//

EXPORT_C CServerCommandConstructor* CServerCommandConstructor::NewLC(const TDesC& aName, TServerCommandConstructor aConstructor, CServerCommandFactory& aFactory, MServerCommandReporter* aReporter, const CCommandInfoFile* aCif)
	{
	CServerCommandConstructor* self = new(ELeave)CServerCommandConstructor(aName, aConstructor, aFactory, aReporter, aCif);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CServerCommandConstructor::~CServerCommandConstructor()
	{
	}

EXPORT_C const TPtrC& CServerCommandConstructor::Name() const
	{
	return iName;
	}

EXPORT_C CServerCommandBase* CServerCommandConstructor::CreateImplementationLC() const
	{
	CServerCommandBase* commandBase = (*iConstructorLC)();
	commandBase->SetReporter(iReporter);
	commandBase->SetFactory(iFactory);
	if (iCif)
		{
		commandBase->SetCif(*iCif);
		}
	return commandBase;	
	}

EXPORT_C TBool CServerCommandConstructor::IsFactory() const
	{
	return EFalse;
	}

CServerCommandConstructor::CServerCommandConstructor(const TDesC& aName, TServerCommandConstructor aConstructor, CServerCommandFactory& aFactory, MServerCommandReporter* aReporter, const CCommandInfoFile* aCif)
	: iName(aName), iConstructorLC(aConstructor), iFactory(aFactory), iReporter(aReporter), iCif(aCif)
	{
	}

TUint32 DefaultTPtrCHash(const TPtrC& aPtr)
	{
	return DefaultHash::Des16(aPtr);
	}
	
TBool DefaultIdTPtrCRelation(const TPtrC& aPtr1, const TPtrC& aPtr2)
	{
	return DefaultIdentity::Des16(aPtr1, aPtr2);
	}

	
//
// CServerCommandFactory.
//

EXPORT_C CServerCommandFactory* CServerCommandFactory::NewL(MServerCommandReporter* aReporter, const CCommandInfoFile* aCif)
	{
	CServerCommandFactory* self = new(ELeave)CServerCommandFactory(aReporter, aCif);
	CleanupStack::PushL(self);
	self->ConstructL(ERoot);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CServerCommandFactory* CServerCommandFactory::NewLC(const TDesC& aName, TServerCommandConstructor aConstructor, MServerCommandReporter* aReporter, const CCommandInfoFile* aCif)
	{
	CServerCommandFactory* self = new(ELeave)CServerCommandFactory(aName, aConstructor, aReporter, aCif);
	CleanupStack::PushL(self);
	self->ConstructL(EBranch);
	return self;
	}
	
EXPORT_C CServerCommandFactory::~CServerCommandFactory()
	{
	THashMapIter<const TPtrC, const CServerCommandConstructor*> iter(iSubCommands);
	while (iter.NextKey())
		{
		delete *iter.CurrentValue();
		}
	iSubCommands.Close();
	}

EXPORT_C const CServerCommandConstructor& CServerCommandFactory::GetSubCommandL(const TDesC& aName) const
	{
	const CServerCommandConstructor* cmnd = GetSubCommand(aName);
	if (!cmnd)
		{
		if (iReporter)
			{
			iReporter->ReportError(TServerCommandId(), KErrNotFound, _L("Couldn't create command \"%S\""), &aName);
			}
		User::Leave(KErrNotFound);
		}
	return *cmnd;
	}

const CServerCommandConstructor* CServerCommandFactory::GetSubCommand(const TDesC& aName) const
	{
	TPtrC name(aName);
	const CServerCommandConstructor* const * cmnd = iSubCommands.Find(name);
	if (!cmnd)
		{
		return NULL;
		}
	return *cmnd;
	}

EXPORT_C TBool CServerCommandFactory::IsFactory() const
	{
	return ETrue;
	}

EXPORT_C void CServerCommandFactory::AddSubCommandL(const CServerCommandConstructor* aCommand)
	{
	iSubCommands.InsertL(aCommand->Name(), aCommand);
	}

CServerCommandFactory::CServerCommandFactory(MServerCommandReporter* aReporter, const CCommandInfoFile* aCif)
	: CServerCommandConstructor(KNullDesC, NULL, *this, aReporter, aCif), iSubCommands(THashFunction32<const TPtrC>(&DefaultTPtrCHash), TIdentityRelation<const TPtrC>(&DefaultIdTPtrCRelation))
	{
	}

CServerCommandFactory::CServerCommandFactory(const TDesC& aName, TServerCommandConstructor aConstructor, MServerCommandReporter* aReporter, const CCommandInfoFile* aCif)
	: CServerCommandConstructor(aName, aConstructor, *this, aReporter, aCif), iSubCommands(THashFunction32<const TPtrC>(&DefaultTPtrCHash), TIdentityRelation<const TPtrC>(&DefaultIdTPtrCRelation))
	{
	}

void CServerCommandFactory::ConstructL(TFactoryType aType)
	{
	if (aType == ERoot)
		{
		AddLeafCommandL<CHelpCommand>();
		}
	}

void CServerCommandFactory::LcCompleteLineL(TConsoleLine& aLine, const TChar&)
	{
	TLex lex(aLine.ContentsToCursor());
	TInt pos = 0;
	
	const CServerCommandFactory* factory = this;
	
	do
		{
		lex.SkipSpaceAndMark();
		pos = lex.Offset();
		lex.SkipCharacters();
		
		if (!lex.Eos())
			{
			const CServerCommandConstructor* command = GetSubCommand(lex.MarkedToken());
			if (!command || !command->IsFactory())
				{
				User::Leave(KErrNotFound);
				}
			factory = static_cast<const CServerCommandFactory*>(command);
			}
		
		} while (!lex.Eos());
	
	
	
	RArray<TPtrC> commands;
	CleanupClosePushL(commands);
	TPtrC partialCommandName(lex.MarkedToken());

	factory->ListCommandsL(commands);

	// Remove commands that don't match.
	TInt i;
	for (i = (commands.Count() - 1); i >= 0; --i)
		{
		if (commands[i].Find(partialCommandName) != 0)
			{
			commands.Remove(i);
			}
		}

	if (commands.Count() > 0)
		{
		CompleteLineL(aLine, pos, commands);
		}

	CleanupStack::PopAndDestroy(&commands);
	}

void CServerCommandFactory::CompleteLineL(TConsoleLine& aLine, TInt aPos, const RArray<TPtrC> aPossibilities) const
	{
	const TInt numPossibilities = aPossibilities.Count();

	if (numPossibilities > 1)
		{
		// Fill out possibilities buffer.
		IoUtils::CTextBuffer* possibilities = IoUtils::CTextBuffer::NewLC(0x100);
		for (TInt i = 0; i < numPossibilities; ++i)
			{
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

		completion->AppendL(commonChars);
		if (numPossibilities == 1)
			{
			completion->AppendL(KSpace);
			}
		if (completion->Descriptor().Length() > 0)
			{
			aLine.Replace(aPos, completion->Descriptor());
			}
		CleanupStack::PopAndDestroy(completion);
		}
	}

EXPORT_C void CServerCommandFactory::ListCommandsL(RArray<TPtrC>& aList) const
	{
	THashMapIter<const TPtrC, const CServerCommandConstructor*> iter(iSubCommands);
	while (iter.NextKey())
		{
		aList.AppendL(*iter.CurrentKey());
		}
	aList.Sort(TLinearOrder<TPtrC>(PtrOrder));
	}



//
// CBranchCommandBase.
//

EXPORT_C CBranchCommandBase::~CBranchCommandBase()
	{
	delete iSubCommandEnum;
	delete iSubCommandArgs;
	}

EXPORT_C CBranchCommandBase::CBranchCommandBase()
	{
	}

EXPORT_C void CBranchCommandBase::DoRunL()
	{
	TPtrC subCommandName(iArguments.AsString(&iType));

	CServerCommandBase* subCommand = Factory().GetSubCommandL(subCommandName).CreateImplementationLC();
	ConfigureSubCommand(*subCommand);
	subCommand->RunCommand(IoSession(), Stdin(), Stdout(), Stderr(), iSubCommandArgs ? iSubCommandArgs : &KNullDesC, &Env(), NULL);
	if (subCommand->IsComplete())
		{
		HandleSubCommandComplete(*subCommand);
		CleanupStack::PopAndDestroy(subCommand);
		}
	else
		{
		HandleBackgroundSubCommand(*subCommand);
		CleanupStack::Pop(subCommand);
		}
	}

void CBranchCommandBase::BuildEnumL()
	{
	_LIT(KComma, ",");
	RArray<TPtrC> subCommands;
	CleanupClosePushL(subCommands);
	Factory().ListCommandsL(subCommands);
	subCommands.Sort(TLinearOrder<TPtrC>(PtrOrder));
	iSubCommandEnum = IoUtils::CTextBuffer::NewL(0x10);
	for (TInt i=0; i<subCommands.Count(); ++i)
		{
		iSubCommandEnum->AppendL(subCommands[i]);
		iSubCommandEnum->AppendL(KComma);
		}
	CleanupStack::PopAndDestroy(&subCommands);
	}
	
EXPORT_C void CBranchCommandBase::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgType, "type");
	_LIT(KArgArgs, "arguments");

	if (!iSubCommandEnum)
		{
		BuildEnumL();
		}

	if (UsingCif())
		{
		aArguments.AppendEnumL((TInt&)iType, KArgType, iSubCommandEnum->Descriptor());
		aArguments.AppendStringL(iSubCommandArgs, KArgArgs);
		}
	else
		{
		aArguments.AppendEnumL((TInt&)iType, KArgType, TypeDescription(), iSubCommandEnum->Descriptor());

		_LIT(KArgArgsDescription, "A set of type-specific arguments and options.");
		aArguments.AppendStringL(iSubCommandArgs, KArgArgs, KArgArgsDescription, KValueTypeFlagOptional | KValueTypeFlagLast);
		}
	}

EXPORT_C const TDesC& CBranchCommandBase::TypeDescription()
	{
	return KNullDesC;
	}

EXPORT_C void CBranchCommandBase::ConfigureSubCommand(CServerCommandBase&)
	{
	}

EXPORT_C void CBranchCommandBase::HandleSubCommandComplete(CServerCommandBase& aCommand)
	{
	if (aCommand.CompletionReason() < 0)
		{
		PrintError(aCommand.CompletionReason(), _L("Couldn't run sub-command \"%S\" of command \"%S\""), &aCommand.Name(), &Name());
		}
	}

EXPORT_C void CBranchCommandBase::HandleBackgroundSubCommand(CServerCommandBase&)
	{
	}

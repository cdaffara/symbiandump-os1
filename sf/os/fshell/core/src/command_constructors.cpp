// command_constructors.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <fshell/descriptorutils.h>
#include "command_constructors.h"
#include "command_wrappers.h"


//
// CCommandConstructorBase.
//

const TDesC& CCommandConstructorBase::CommandName() const
	{
	return *iCommandName;
	}

TUint CCommandConstructorBase::Attributes() const
	{
	return iAttributes;
	}

CCommandConstructorBase::TType CCommandConstructorBase::Type() const
	{
	return iType;
	}

void CCommandConstructorBase::SetAttributes(TUint aAttributes)
	{
	iAttributes = aAttributes;
	}

void CCommandConstructorBase::BaseConstructL(const TDesC& aCommandName)
	{
	iCommandName = aCommandName.AllocL();
	iCommandName->Des().Fold();
	}

CCommandConstructorBase::CCommandConstructorBase()
	: iType(ETypeUnknown)
	{
	}

CCommandConstructorBase::CCommandConstructorBase(TType aType)
	: iType(aType)
	{
	}

CCommandConstructorBase::~CCommandConstructorBase()
	{
	delete iCommandName;
	}


//
// CThreadCommandConstructor.
//

CThreadCommandConstructor* CThreadCommandConstructor::NewLC(TCommandConstructor aConstructor, TUint aFlags, MTaskRunner* aTaskRunner)
	{
	CCommandBase* command = (*aConstructor)();
	CThreadCommandConstructor* self = CThreadCommandConstructor::NewLC(command->Name(), aConstructor, aFlags, aTaskRunner);
	CleanupStack::Pop(self);
	CleanupStack::PopAndDestroy(command);
	CleanupStack::PushL(self);
	return self;
	}

CThreadCommandConstructor* CThreadCommandConstructor::NewLC(const TDesC& aCommandName, TCommandConstructor aConstructor, TUint aFlags, MTaskRunner* aTaskRunner)
	{
	CThreadCommandConstructor* self = new(ELeave) CThreadCommandConstructor(aFlags, aConstructor, aTaskRunner);
	CleanupStack::PushL(self);
	self->BaseConstructL(aCommandName);
	return self;
	}

CThreadCommandConstructor::CThreadCommandConstructor(TUint aFlags, TCommandConstructor aConstructor, MTaskRunner* aTaskRunner)
	: CCommandConstructorBase(ETypeThread), iFlags(aFlags), iConstructor(aConstructor), iTaskRunner(aTaskRunner)
	{
	}

MCommand* CThreadCommandConstructor::ConstructCommandL()
	{
	return CThreadCommand::NewL(CommandName(), iConstructor, iFlags, iTaskRunner);
	}

void CThreadCommandConstructor::AppendDescriptionL(RLtkBuf16& aBuf) const
	{
	aBuf.AppendFormatL(_L("built-in command '%S'"), &CommandName());
	if (Attributes() & EAttAlias) aBuf.AppendFormatL(_L(" (alias)")); // In this case we don't record what it's aliased to, unfortunately
	}

//
// CExeCommandConstructor.
//

CExeCommandConstructor* CExeCommandConstructor::NewLC(const TDesC& aCommandName, const TDesC& aExeName)
	{
	CExeCommandConstructor* self = new(ELeave) CExeCommandConstructor();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandName, aExeName);
	return self;
	}

CExeCommandConstructor::CExeCommandConstructor()
	: CCommandConstructorBase(ETypeExe)
	{
	}

CExeCommandConstructor::~CExeCommandConstructor()
	{
	delete iExeName;
	}

void CExeCommandConstructor::ConstructL(const TDesC& aCommandName, const TDesC& aExeName)
	{
	iExeName = aExeName.AllocL();
	BaseConstructL(aCommandName);
	}

MCommand* CExeCommandConstructor::ConstructCommandL()
	{
	return CProcessCommand::NewL(*iExeName);
	}

const TDesC& CExeCommandConstructor::ExeName() const
	{
	return *iExeName;
	}

void CExeCommandConstructor::AppendDescriptionL(RLtkBuf16& aBuf) const
	{
	aBuf.AppendFormatL(_L("exe command '%S'"), &ExeName());
	}

//
// CPipsCommandConstructor.
//

CPipsCommandConstructor* CPipsCommandConstructor::NewLC(const TDesC& aCommandName)
	{
	CPipsCommandConstructor* self = new(ELeave) CPipsCommandConstructor();
	CleanupStack::PushL(self);
	self->BaseConstructL(aCommandName);
	return self;
	}

CPipsCommandConstructor::CPipsCommandConstructor()
	: CCommandConstructorBase(ETypePips)
	{
	}

MCommand* CPipsCommandConstructor::ConstructCommandL()
	{
	return CPipsCommand::NewL(CommandName());
	}


void CPipsCommandConstructor::AppendDescriptionL(RLtkBuf16& aBuf) const
	{
	aBuf.AppendFormatL(_L("PIPS exe '%S.exe'"), &CommandName());
	}

//
// CAliasCommandConstructor.
//

CAliasCommandConstructor* CAliasCommandConstructor::NewLC(const TDesC& aCommandName, CCommandConstructorBase* aAliasedConstructor, const TDesC* aAdditionalArguments, const TDesC* aReplacementArguments)
	{
	CAliasCommandConstructor* self = new(ELeave) CAliasCommandConstructor(aAliasedConstructor);
	CleanupStack::PushL(self);
	self->ConstructL(aCommandName, aAdditionalArguments, aReplacementArguments);
	return self;
	}

CAliasCommandConstructor::CAliasCommandConstructor(CCommandConstructorBase* aAliasedConstructor)
	: CCommandConstructorBase(ETypeAlias), iAliasedConstructor(aAliasedConstructor)
	{
	}

CAliasCommandConstructor::~CAliasCommandConstructor()
	{
	delete iAliasedConstructor;
	delete iAdditionalArguments;
	delete iReplacementArguments;
	}

void CAliasCommandConstructor::ConstructL(const TDesC& aCommandName, const TDesC* aAdditionalArguments, const TDesC* aReplacementArguments)
	{
	if (aAdditionalArguments)
		{
		iAdditionalArguments = aAdditionalArguments->AllocL();
		}
	if (aReplacementArguments)
		{
		iReplacementArguments = aReplacementArguments->AllocL();
		}
	BaseConstructL(aCommandName);
	}

MCommand* CAliasCommandConstructor::ConstructCommandL()
	{
	MCommand* aliasCommand = iAliasedConstructor->ConstructCommandL();
	CleanupStack::PushL(aliasCommand);
	MCommand* command = CAliasCommand::NewL(*aliasCommand, iAdditionalArguments, iReplacementArguments);
	CleanupStack::Pop(aliasCommand); // Now owned by "command".
	return command;
	}

void CAliasCommandConstructor::AppendDescriptionL(RLtkBuf16& aBuf) const
	{
	aBuf.AppendFormatL(_L("aliased to '%S"), &iAliasedConstructor->CommandName());
	// Hope I've understood what CAliasCommand::CmndRun does correctly...
	if (iReplacementArguments)
		{
		aBuf.AppendFormatL(_L(" %S"), iReplacementArguments);
		}
	else if (iAdditionalArguments)
		{
		aBuf.AppendFormatL(_L(" %S <args>"), iAdditionalArguments);
		}
	aBuf.AppendL(_L("'"));
	}

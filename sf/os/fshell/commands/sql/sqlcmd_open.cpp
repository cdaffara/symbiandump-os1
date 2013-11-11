// sqlcmd_open.cpp
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

#include "sqlcmd_open.h"
#include "sqlsrv.h"

/////////////////////////////////////////////////////////////////////////////////////////
_LIT(KCommandName, "open");

CServerCommandBase* CSqlCmdOpen::NewLC()
	{
	CSqlCmdOpen* self = new(ELeave) CSqlCmdOpen();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSqlCmdOpen::~CSqlCmdOpen()
	{
	delete iFileName;	
	}

const TDesC& CSqlCmdOpen::NameS()
	{
	return KCommandName;
	}

void CSqlCmdOpen::ConstructL()
	{
	BaseConstructL();
	}

const TDesC& CSqlCmdOpen::Name() const
	{
	return KCommandName;
	}

const TDesC& CSqlCmdOpen::Description() const
	{
	_LIT(KDescription, "open an existing SQL database file");
	return KDescription;
	}

//virtual 
void CSqlCmdOpen::ArgumentsL(RCommandArgumentList& aArguments)
{
	_LIT(KName, "filename");
	_LIT(KDescription, "filename of the SQL database file. Should contain path.");
	aArguments.AppendStringL(iFileName, KName, KDescription, KValueTypeFlagOptional);
}

void CSqlCmdOpen::DoRunL()
	{
	if (iFileName==NULL || iFileName->Length()==0 )
		{
		LeaveIfErr(KErrArgument, _L("Database filename unspecified."));
		}
	
	CCmdSqlSrv* pSrv = CCmdSqlSrv::GetServer(); 
	pSrv->SqlOpenL(*iFileName, this);
	}

/////////////////////////////////////////////////////////////////////////////////////
_LIT(KCommandExit, "exit");

CServerCommandBase* CSqlCmdExit::NewLC()
	{
	CSqlCmdExit* self = new(ELeave) CSqlCmdExit();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSqlCmdExit::~CSqlCmdExit()
	{
	}

const TDesC& CSqlCmdExit::NameS()
	{
	return KCommandExit;
	}

void CSqlCmdExit::ConstructL()
	{
	BaseConstructL();
	}

const TDesC& CSqlCmdExit::Name() const
	{
	return KCommandExit;
	}

const TDesC& CSqlCmdExit::Description() const
	{
	_LIT(KDescription, "Shut down amsrv.");
	return KDescription;
	}

void CSqlCmdExit::DoRunL()
	{
	CCmdSqlSrv* pSrv = CCmdSqlSrv::GetServer(); 
	pSrv->Exit(KErrNone);

	}

/////////////////////////////////////////////////////////////////////////////////////
_LIT(KCommandClose, "close");

CServerCommandBase* CSqlCmdClose::NewLC()
	{
	CSqlCmdClose* self = new(ELeave) CSqlCmdClose();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSqlCmdClose::~CSqlCmdClose()
	{
	}

const TDesC& CSqlCmdClose::NameS()
	{
	return KCommandClose;
	}

void CSqlCmdClose::ConstructL()
	{
	BaseConstructL();
	}

const TDesC& CSqlCmdClose::Name() const
	{
	return KCommandClose;
	}

const TDesC& CSqlCmdClose::Description() const
	{
	_LIT(KDescription, "close a SQL database file");
	return KDescription;
	}

void CSqlCmdClose::DoRunL()
	{
	CCmdSqlSrv* pSrv = CCmdSqlSrv::GetServer(); 
	pSrv->SqlClose(this);

	}

/////////////////////////////////////////////////////////////////////////////////////
_LIT(KCommandCreate, "create");

CServerCommandBase* CSqlCmdCreate::NewLC()
	{
	CSqlCmdCreate* self = new(ELeave) CSqlCmdCreate();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSqlCmdCreate::~CSqlCmdCreate()
	{
	delete iFileName;
	}

const TDesC& CSqlCmdCreate::NameS()
	{
	return KCommandCreate;
	}

void CSqlCmdCreate::ConstructL()
	{
	BaseConstructL();
	}

const TDesC& CSqlCmdCreate::Name() const
	{
	return KCommandCreate;
	}

const TDesC& CSqlCmdCreate::Description() const
	{
	_LIT(KDescription, "Create a SQL database");
	return KDescription;
	}

//virtual 
void CSqlCmdCreate::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KName, "filename");
	_LIT(KDescription, "filename of the SQL database file. Should contain path.");
	aArguments.AppendStringL(iFileName, KName, KDescription, KValueTypeFlagOptional);
	}

void CSqlCmdCreate::DoRunL()
	{
	if (iFileName==NULL || iFileName->Length()==0 )
		{
		LeaveIfErr(KErrArgument, _L("Database filename unspecified."));
		}
	
	CCmdSqlSrv* pSrv = CCmdSqlSrv::GetServer(); 
	pSrv->SqlCreateL(*iFileName, this);
	}

///////////////////////////////////////////////////////////////////////////////

_LIT(KCommandExec, "exec");

CServerCommandBase* CSqlCmdExec::NewLC()
	{
	CSqlCmdExec* self = new(ELeave) CSqlCmdExec();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSqlCmdExec::~CSqlCmdExec()
	{
	delete iExec;
	}

const TDesC& CSqlCmdExec::NameS()
	{
	return KCommandExec;
	}

void CSqlCmdExec::ConstructL()
	{
	BaseConstructL();
	}

const TDesC& CSqlCmdExec::Name() const
	{
	return KCommandExec;
	}

const TDesC& CSqlCmdExec::Description() const
	{
	_LIT(KDescription, "Execute a SQL statement without parameter and response.");
	return KDescription;
	}

void CSqlCmdExec::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KName, "statement");
	_LIT(KDescription, "The SQL statement to execute");
	aArguments.AppendStringL(iExec, KName, KDescription, KValueTypeFlagOptional|KValueTypeFlagLast);
	}

void CSqlCmdExec::DoRunL()
	{
	if (iExec==NULL || iExec->Length()==0 )
		{
		LeaveIfErr(KErrArgument, _L("SQL statement unspecified."));
		}
	CCmdSqlSrv::StripWrapDoubleQuote(*iExec);
	
	CCmdSqlSrv* pSrv = CCmdSqlSrv::GetServer(); 
	pSrv->SqlExecL(*iExec, this);
	}
/////////////////////////////////////////////////////////////////////////////////////////////

_LIT(KCommandState, "state");

CServerCommandBase* CSqlCmdState::NewLC()
	{
	CSqlCmdState* self = new(ELeave) CSqlCmdState();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSqlCmdState::~CSqlCmdState()
	{
	delete iExec;
	
	delete iOptFileName;
	
	//delete optional multiple parameter file
		{
		TInt optParamCnt = iOptParamFile.Count();
		if (optParamCnt)
			{
			for (TInt i=0; i<optParamCnt; i++)
				{
				HBufC* pEle = iOptParamFile[i];
				delete pEle;			
				}		
			}
		iOptParamFile.Close();
		}
		
/*	
	//release resource for command-specific arguments 
	TInt cmdArguCnt = iCmdArgu.Count();
	if (cmdArguCnt)
		{
		for (TInt i=0; i<cmdArguCnt; i++)
			{
			HBufC* pEle = iCmdArgu[i];
			delete pEle;			
			}		
		}
	iCmdArgu.Close();
*/			
	}

const TDesC& CSqlCmdState::NameS()
	{
	return KCommandState;
	}

void CSqlCmdState::ConstructL()
	{
	BaseConstructL();
	}

const TDesC& CSqlCmdState::Name() const
	{
	return KCommandState;
	}

const TDesC& CSqlCmdState::Description() const
	{
	_LIT(KDescription, "Execute a SQL statement that may come with parameter and response.");
	return KDescription;
	}

void CSqlCmdState::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KName, "statement");
	_LIT(KDescription, "The SQL statement to execute");
	aArguments.AppendStringL(iExec, KName, KDescription, KValueTypeFlagOptional|KValueTypeFlagLast);	
	}

void CSqlCmdState::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptFile, "temp");
	_LIT(KOptFileDescription, "Specify TEMP file template(used to dump large binary content to file)");
	aOptions.AppendStringL(iOptFileName, TChar('t'), KOptFile, KOptFileDescription);
	
	_LIT(KOptParam, "param");
	_LIT(KOptParamDescription, "Specify file names, of which the content will be used to fill parameters in SQL statement");
	aOptions.AppendStringL(iOptParamFile, TChar('p'), KOptParam, KOptParamDescription);
	}


void CSqlCmdState::DoRunL()
	{
	//this command requires at least one argument
	//first argument is a SQL statement, 
	//and remaining arguments are optional parameters
	
	TInt paramCnt = iOptParamFile.Count();
	if (iExec==0 || iExec->Length()==0 )
		{
		LeaveIfErr(KErrArgument, _L("SQL statement unspecified."));
		}
	
	CCmdSqlSrv::StripWrapDoubleQuote(*iExec);
	
	CCmdSqlSrv* pSrv = CCmdSqlSrv::GetServer(); 
	pSrv->SqlStateL(*iExec, iOptParamFile, this, iOptFileName);
	}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
_LIT(KCommandAttach, "attach");

CServerCommandBase* CSqlCmdAttach::NewLC()
	{
	CSqlCmdAttach* self = new(ELeave) CSqlCmdAttach();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSqlCmdAttach::~CSqlCmdAttach()
	{
	delete iOptFileName;
	delete iOptDBName;
	}

const TDesC& CSqlCmdAttach::NameS()
	{
	return KCommandAttach;
	}

void CSqlCmdAttach::ConstructL()
	{
	BaseConstructL();
	}

const TDesC& CSqlCmdAttach::Name() const
	{
	return KCommandAttach;
	}

const TDesC& CSqlCmdAttach::Description() const
	{
	_LIT(KDescription, "Attach additional database onto main database file");
	return KDescription;
	}

//virtual 
void CSqlCmdAttach::ArgumentsL(RCommandArgumentList& /*aArguments*/)
	{
	}

void CSqlCmdAttach::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptFile, "file");
	_LIT(KOptFileDescription, "Specify additional database file.");
	aOptions.AppendStringL(iOptFileName, TChar('f'), KOptFile, KOptFileDescription);

	_LIT(KOptDB, "name");
	_LIT(KOptDBDescription, "Specify additional database name.");
	aOptions.AppendStringL(iOptDBName, TChar('n'), KOptDB, KOptDBDescription);
	}

void CSqlCmdAttach::DoRunL()
	{
	if (iOptFileName==NULL || iOptFileName->Length()==0 )
		{
		LeaveIfErr(KErrArgument, _L("Database filename unspecified."));
		}
	if (iOptDBName==NULL || iOptDBName->Length()==0 )
		{
		LeaveIfErr(KErrArgument, _L("Database name unspecified."));
		}
	
	CCmdSqlSrv* pSrv = CCmdSqlSrv::GetServer(); 
	pSrv->SqlAttachL(*iOptFileName, *iOptDBName, this);
	}

//////////////////////////////////////////////////////////////////////////////////////

_LIT(KCommandDetach, "detach");

CServerCommandBase* CSqlCmdDetach::NewLC()
	{
	CSqlCmdDetach* self = new(ELeave) CSqlCmdDetach();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSqlCmdDetach::~CSqlCmdDetach()
	{
	delete iOptDBName;
	}

const TDesC& CSqlCmdDetach::NameS()
	{
	return KCommandDetach;
	}

void CSqlCmdDetach::ConstructL()
	{
	BaseConstructL();
	}

const TDesC& CSqlCmdDetach::Name() const
	{
	return KCommandDetach;
	}

const TDesC& CSqlCmdDetach::Description() const
	{
	_LIT(KDescription, "Detach additional database which is previously attached onto main database file.");
	return KDescription;
	}

void CSqlCmdDetach::ArgumentsL(RCommandArgumentList& /*aArguments*/)
	{
	}

void CSqlCmdDetach::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptDB, "name");
	_LIT(KOptDBDescription, "Specify additional database name.");
	aOptions.AppendStringL(iOptDBName, TChar('n'), KOptDB, KOptDBDescription);
	}

void CSqlCmdDetach::DoRunL()
	{
	if (iOptDBName==NULL || iOptDBName->Length()==0 )
		{
		LeaveIfErr(KErrArgument, _L("Database name unspecified."));
		}
	
	CCmdSqlSrv* pSrv = CCmdSqlSrv::GetServer(); 
	pSrv->SqlDetachL(*iOptDBName, this);
	}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
#ifdef SQL_COMPACT

_LIT(KCommandCompact, "compact");

CServerCommandBase* CSqlCmdCompact::NewLC()
	{
	CSqlCmdCompact* self = new(ELeave) CSqlCmdCompact();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSqlCmdCompact::~CSqlCmdCompact()
	{
	delete iOptDBName;
	}

const TDesC& CSqlCmdCompact::NameS()
	{
	return KCommandCompact;
	}

void CSqlCmdCompact::ConstructL()
	{
	BaseConstructL();
	}

const TDesC& CSqlCmdCompact::Name() const
	{
	return KCommandCompact;
	}

const TDesC& CSqlCmdCompact::Description() const
	{
	_LIT(KDescription, "Compacts the database.(RSqlDatabase::Compact)");
	return KDescription;
	}

//virtual 
void CSqlCmdCompact::ArgumentsL(RCommandArgumentList& aArguments)
{
}

void CSqlCmdCompact::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptDB, "name");
	_LIT(KOptDBDescription, "Specify database name, if unspecified, then main database will be compacted");
	aOptions.AppendStringL(iOptDBName, TChar('n'), KOptDB, KOptDBDescription);
	}

void CSqlCmdCompact::DoRunL()
	{
	CCmdSqlSrv* pSrv = CCmdSqlSrv::GetServer(); 
	pSrv->SqlCompactL(iOptDBName, this);
	}
#endif

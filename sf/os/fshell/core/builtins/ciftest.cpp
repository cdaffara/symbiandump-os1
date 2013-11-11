// ciftest.cpp
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

#include "ciftest.h"
#include "fshell.h"
#include "command_factory.h"

CCommandBase* CCmdCifTest::NewLC()
	{
	CCmdCifTest* self = new(ELeave) CCmdCifTest();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdCifTest::~CCmdCifTest()
	{
	delete iCmd;
	delete iParser;
	delete iEnvForScript;
	delete iCurrentCif;
	iCifFiles.ResetAndDestroy();
	}

CCmdCifTest::CCmdCifTest()
	: CCommandBase(EManualComplete | EReportAllErrors)
	{
	}

const TDesC& CCmdCifTest::Name() const
	{
	_LIT(KName, "ciftest");	
	return KName;
	}

void CCmdCifTest::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendStringL(iCmd, _L("command"));
	}

void CCmdCifTest::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, _L("verbose"));
	aOptions.AppendBoolL(iKeepGoing, _L("keep-going"));
	}

void CCmdCifTest::DoRunL()
	{
	if (iCmd)
		{
		CCommandInfoFile* cif = CCommandInfoFile::NewL(FsL(), Env(), *iCmd);
		TestCifL(cif); // Takes ownership
		}
	else
		{
		_LIT(KCifDir, "y:\\resource\\cif\\fshell\\");
		TFindFile find(FsL());
		CDir* dir = NULL;
		TInt found = find.FindWildByDir(_L("*.cif"), KCifDir, dir);
		while (found == KErrNone)
			{
			for (TInt i = 0; i < dir->Count(); i++)
				{
				iFileName.Copy(TParsePtrC(find.File()).DriveAndPath()); // The docs for TFindFile state you shouldn't need the extra TParsePtrC::DriveAndPath(). Sigh.
				iFileName.Append((*dir)[i].iName);
				iCifFiles.AppendL(iFileName.AllocLC());
				CleanupStack::Pop();
				}
			delete dir;
			dir = NULL;
			found = find.FindWild(dir);
			}
		NextCif();
		}
	}

void CCmdCifTest::NextCif()
	{
	if (iNextCif == iCifFiles.Count())
		{
		if (iVerbose)
			{
			Printf(_L("%d tests run, %d passes %d failures."), iPasses + iFailures, iPasses, iFailures);
			if (iCifFiles.Count()) Printf(_L(" %d commands have no tests defined."), iCifFiles.Count() - iPasses - iFailures);
			Printf(_L("\r\n"));
			}
		Complete(KErrNone);
		}
	else
		{
		CCommandInfoFile* cif = NULL;
		TRAPD(err, cif = CCommandInfoFile::NewL(FsL(), *iCifFiles[iNextCif]));
		if (!err)
			{
			TRAP(err, TestCifL(cif));
			if (err) PrintError(err, _L("Error setting up test for CIF %S"), iCifFiles[iNextCif]);
			}
		iNextCif++;
		
		if (err)
			{
			iFailures++;
			TestCompleted(err);
			}
		}
	}

void CCmdCifTest::TestCifL(CCommandInfoFile* aCif)
	{
	iCurrentCif = aCif;
	if (iVerbose) Printf(_L("Checking %S\r\n"), &aCif->CifFileName());

	const TDesC& scriptData = aCif->SmokeTest();
	if (scriptData.Length() == 0)
		{
		if (iVerbose) Printf(_L("Cif has no smoketest section\r\n"));
		TestCompleted(KErrNone);
		return;
		}

	iEnvForScript = CEnvironment::NewL(Env());
	iEnvForScript->SetL(_L("Error"), _L("fshell -e 'echo \"Test failed, env is:\" && env && error'"));
	iEnvForScript->SetL(_L("Quiet"), _L(">/dev/null"));
	iEnvForScript->SetL(_L("Silent"), _L("2>&1 >/dev/null"));
	iEnvForScript->Remove(_L("Verbose")); // In case it's ended up in our parent env
	if (iVerbose) iEnvForScript->SetL(_L("Verbose"), 1);
	iFileName.Copy(aCif->CifFileName());
	iFileName.Append(_L(":smoke-test"));
	TParsePtrC parse(iFileName);
	iEnvForScript->SetL(KScriptName, parse.NameAndExt());
	iEnvForScript->SetL(KScriptPath, parse.DriveAndPath());
	iEnvForScript->SetL(_L("0"), iFileName);

	iParser = CParser::NewL(CParser::EExportLineNumbers, scriptData, IoSession(), Stdin(), Stdout(), Stderr(), *iEnvForScript, gShell->CommandFactory(), this, aCif->GetSmokeTestStartingLineNumber());
	iParser->Start();
	}

void CCmdCifTest::HandleParserComplete(CParser& /*aParser*/, const TError& aError)
	{
	TInt err = aError.Error();
	if (err)
		{
		iFailures++;
		PrintError(err, _L("%S failed at line %d"), &aError.ScriptFileName(), aError.ScriptLineNumber());
		}
	else
		{
		if (iVerbose)
			{
			Printf(_L("Smoketest for %S completed ok.\r\n"), &iCurrentCif->Name());
			}
		iPasses++;
		}
	TestCompleted(err);
	}

void CCmdCifTest::TestCompleted(TInt aError)
	{
	// Delete interim data
	delete iEnvForScript;
	iEnvForScript = NULL;
	delete iParser;
	iParser = NULL;
	delete iCurrentCif;
	iCurrentCif = NULL;

	if (aError == KErrNone || iKeepGoing)
		{
		// Async call NextCif()
		TRequestStatus* stat = &iStatus;
		User::RequestComplete(stat, KErrNone);
		SetActive();
		}
	else
		{
		Complete(aError);
		}
	}

void CCmdCifTest::RunL()
	{
	NextCif();
	}

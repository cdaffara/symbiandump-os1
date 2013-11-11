// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code   
*/

#include "sysmontesthelper.h"
#include <f32file.h>
#include <s32file.h>
#include <e32property.h>

_LIT(KEIgnoreOnFailure, "EIgnoreOnFailure");
_LIT(KERestartOS, "ERestartOS");
_LIT(KERestartOSWithMode, "ERestartOSWithMode");
_LIT(KECriticalNoRetriesMode, "ECriticalNoRetries");

EXPORT_C TRecoveryMethod CSysMonTestHelper::StringToRecoveryMethodL(const TDesC& aRecoveryMethodString)
	{
	if (aRecoveryMethodString == KEIgnoreOnFailure)
		{
		return EIgnoreOnFailure;
		}
	else if (aRecoveryMethodString == KERestartOS)
		{
		return ERestartOS;
		}
	else if (aRecoveryMethodString == KERestartOSWithMode)
		{
		return ERestartOSWithMode;
		}
	else if (aRecoveryMethodString == KECriticalNoRetriesMode)
		{
		return ECriticalNoRetries;
		}

	User::Leave(KErrArgument);
	return EIgnoreOnFailure; //lint !e527 Unreachable
	}
		
EXPORT_C CSysMonTestHelper* CSysMonTestHelper::NewLC()
	{
	CSysMonTestHelper* self = new(ELeave) CSysMonTestHelper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CSysMonTestHelper* CSysMonTestHelper::NewL()
	{
	CSysMonTestHelper* self = CSysMonTestHelper::NewLC();
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C void CSysMonTestHelper::GetRecoveryMethodArgsL(TRecoveryMethod& aRecoveryMethod, TInt& aRecoveryMode)
	{
	aRecoveryMethod = CSysMonTestHelper::StringToRecoveryMethodL(iArgs->Arg(2));
	TLex lex(iArgs->Arg(3));
	User::LeaveIfError(lex.Val(aRecoveryMode));
	}
	
EXPORT_C void CSysMonTestHelper::GetTimeoutArgL(TInt& aTimeout)
	{
	TLex lex(iArgs->Arg(2));
	User::LeaveIfError(lex.Val(aTimeout));
	}
	
EXPORT_C void CSysMonTestHelper::WriteResultL(const TInt& aResult)
	{
	RDebug::Print(_L("Result for %S: %d"), &iTestId, aResult);
	TBuf<KMaxFileName> fileName;
	fileName.Append(KResultsRoot);
	fileName.Append(iTestId);
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TInt err = fs.MkDirAll(KResultsRoot);
	if (err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	
	RFileWriteStream file;
	file.PushL();
	User::LeaveIfError(file.Replace(fs, fileName, EFileWrite));
	file.WriteInt32L(aResult);
	file.Close();
	
	CleanupStack::PopAndDestroy(2, &fs);
	}
	
EXPORT_C void CSysMonTestHelper::WriteDeregResultL(const TInt& aResult)
	{
	RDebug::Print(_L("Dereg Result for %S: %d"), &iTestId, aResult);
	TBuf<KMaxFileName> fileName;
	fileName.Append(KResultsRootDereg);
	fileName.Append(iTestId);
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TInt err = fs.MkDirAll(KResultsRoot);
	if (err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	
	RFileWriteStream file;
	file.PushL();
	User::LeaveIfError(file.Replace(fs, fileName, EFileWrite));
	file.WriteInt32L(aResult);
	file.Close();
	
	CleanupStack::PopAndDestroy(2, &fs);
	}
	
EXPORT_C void CSysMonTestHelper::GetResultIntL(const TDesC& aTestId, TInt& aResult)
	{
	TBuf<KMaxFileName> fileName;
	fileName.Append(KResultsRoot);
	fileName.Append(aTestId);
	
	RDebug::Print(_L("Checking in file: %S"), &fileName);
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	RFileReadStream file;
	file.PushL();
	User::LeaveIfError(file.Open(fs, fileName, EFileRead));
	aResult = file.ReadInt32L();
	file.Close();
	
	CleanupStack::PopAndDestroy(2, &fs);
	}
	
EXPORT_C void CSysMonTestHelper::GetDeregResultIntL(const TDesC& aTestId, TInt& aResult)
	{
	TBuf<KMaxFileName> fileName;
	fileName.Append(KResultsRootDereg);
	fileName.Append(aTestId);
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	RFileReadStream file;
	file.PushL();
	User::LeaveIfError(file.Open(fs, fileName, EFileRead));
	aResult = file.ReadInt32L();
	file.Close();
	
	CleanupStack::PopAndDestroy(2, &fs);
	}
	
EXPORT_C TInt CSysMonTestHelper::ReadRunCountL(const TDesC& aProcessFilename)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TParse parse;
	fs.Parse(aProcessFilename, parse);
	
	TBuf<KMaxFileName> fileName;
	fileName.Append(KRunCountsRoot);
	fileName.Append(parse.NameAndExt());
	fileName.Append(KTxtExtension);
	RDebug::Print(_L("Checking in file: %S"), &fileName);

	RFileReadStream file;
	TInt err = 0;
	err = file.Open(fs, fileName, EFileRead);
	TInt result = 0;
	if (err == KErrNotFound || err == KErrPathNotFound)
		{
		CleanupStack::PopAndDestroy(&fs);
		}
	else
		{
		file.PushL();
		User::LeaveIfError(err);
		result = file.ReadInt32L();
		file.Close();
		CleanupStack::PopAndDestroy(2, &fs);
		}
	RDebug::Print(_L("Number of previous runs for %S : %d"), &aProcessFilename, result);
	return result;
	}
	
EXPORT_C void CSysMonTestHelper::IncrementRunCountL(const TDesC& aProcessFilename)
	{
	TInt runcount = ReadRunCountL(aProcessFilename);
	runcount++;

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TParse parse;
	fs.Parse(aProcessFilename, parse);
	
	TBuf<KMaxFileName> fileName;
	fileName.Append(KRunCountsRoot);
	fileName.Append(parse.NameAndExt());
	fileName.Append(KTxtExtension);

	TInt err = fs.MkDirAll(KRunCountsRoot);
	if (err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}

	RFileWriteStream file;
	User::LeaveIfError(file.Replace(fs, fileName, EFileWrite));
	file.PushL();
	file.WriteInt32L(runcount);
	file.Close();

	RDebug::Print(_L("New runcount for %S : %d"), &aProcessFilename, runcount);
	
	CleanupStack::PopAndDestroy(2, &fs);
	}
	
EXPORT_C TInt CSysMonTestHelper::GetRestartCountL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	RFile file;
	TInt err = 0;
	err = file.Open(fs, KRestartCountFile, EFileRead);
	TInt result = 0;
	if (err == KErrNotFound || err == KErrPathNotFound)
		{
		CleanupStack::PopAndDestroy(&fs);
		}
	else
		{
		CleanupClosePushL(file);
		User::LeaveIfError(err);
		TBuf8<8> content;
		file.Read(content);
		content.Delete(0,6);
		TLex8 myLex(content);
		User::LeaveIfError(myLex.Val(result));
		file.Close();
		CleanupStack::PopAndDestroy(2, &fs);
		}
	return result;
	}

EXPORT_C TInt CSysMonTestHelper::GetRegisterCount(TInt aKey)
	{
	TInt result = 0;
	TInt error = RProperty::Get(KTestSysMon, aKey, result);
	RDebug::Printf("RProperty::Get() with key %d returned with %d", aKey, error);
	return result;
	}
	
EXPORT_C void CSysMonTestHelper::GetTestId(TDes& aTestId)
	{
	aTestId = iTestId;
	}
	
EXPORT_C void CSysMonTestHelper::TestErrorcodeL(TInt& aErrorcode)
	{
	if (aErrorcode != KErrNone)
		{
		this->WriteResultL(aErrorcode);
		this->WriteDeregResultL(aErrorcode);
		User::Leave(aErrorcode);
		}
	}
	
void CSysMonTestHelper::ConstructL()
	{
	iArgs = CCommandLineArguments::NewL();
	iTestId = TBufC<KMaxTestIdLength>(iArgs->Arg(1));
	}
	
CSysMonTestHelper::CSysMonTestHelper()
	{}
	
EXPORT_C CSysMonTestHelper::~CSysMonTestHelper()
	{
	delete iArgs;
	}

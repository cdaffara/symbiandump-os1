// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// trpdispatchresolver.cpp
// Implementation of class CHwrmUiPluginResolver. It encapsulates the logic to select which TRP ECOM plugin to load.
// 
//

/**
 @file
*/

#include "hwrmuipluginresolver.h"
#include <ecom/ecom.h>
#include <utf.h>
#include "inifile.h"
#include "HWRMtrace.h"

_LIT(KTestFile,"testuiplugins.ini");

/**
Factory function. Leaves instantiated object on cleanup stack.
*/
EXPORT_C CHwrmUiPluginResolver* CHwrmUiPluginResolver::NewLC()
	{
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::NewLC"));
	CHwrmUiPluginResolver* self = new (ELeave) CHwrmUiPluginResolver();
	CleanupStack::PushL(self);
	self->ConstructL();
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::NewLC - return"));
	return self;
	}

/**
Factory function.
*/
EXPORT_C CHwrmUiPluginResolver* CHwrmUiPluginResolver::NewL()
	{
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::NewL"));
	CHwrmUiPluginResolver* self = NewLC();
	CleanupStack::Pop(self);
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::NewL - return"));
	return self;
	}

/**
First-phase constructor.
*/
CHwrmUiPluginResolver::CHwrmUiPluginResolver()
	{
	}

/**
Second-phase constructor.
*/		
void CHwrmUiPluginResolver::ConstructL()
	{
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::ConstructL"));
	User::LeaveIfError(iFs.Connect());
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::ConstructL - return"));
	}

/**
Destructor.
*/	
EXPORT_C CHwrmUiPluginResolver::~CHwrmUiPluginResolver()
	{
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::~CHwrmUiPluginResolver"));
	
	iFs.Close();
	
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::~CHwrmUiPluginResolver - return"));
	}

EXPORT_C TInt CHwrmUiPluginResolver::GetDesiredPluginL(TUid& aImplUid)
	{
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::GetDesiredPlugin"));
	TFileName fullFilePath;
	GetTestFilePath(fullFilePath);
	
	if (IsTestFilePresent(fullFilePath) == EFalse)
		{
		return KErrNotFound;
		}

	CIniFile* iniFile = NULL;
	TRAPD(err, iniFile = CIniFile::NewL(KTestFile,fullFilePath));
	if (err != KErrNone)
		{
		return KErrNotFound;
		}

	TFileName exeFileName = RProcess().FileName();
	TParsePtrC parser(exeFileName);
	exeFileName = parser.Name();
		
	COMPONENT_TRACE4(_L("Searching for %S in config file in %S%S"),&exeFileName,&fullFilePath,&KTestFile);
		
	TPtrC16 dataPtr;
	_LIT(KUiPluginSection,"Plugin");
	TBool found = iniFile->FindVar(KUiPluginSection,exeFileName,dataPtr);
	if (found)
		{
		TLex lex(dataPtr);
		TInt uidInt;
		lex.Val(uidInt);
		aImplUid = TUid::Uid(uidInt);
		}

	delete iniFile;
	
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::GetDesiredPluginDataL - return"));
	return found ? KErrNone : KErrNotFound;
	}

void CHwrmUiPluginResolver::GetTestFilePath(TDes& aFilePath)
	{
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::GetTestFilePath"));
	_LIT(KTestDataPath,"z:\\testdata\\");
	aFilePath.Copy(KTestDataPath);
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::GetTestFilePath - present"));
	}

/**
Checks for the presence of the file that indicates that this is a test run.

@return ETrue if test file present. EFalse otherwise.
*/	
TBool CHwrmUiPluginResolver::IsTestFilePresent(const TDesC& aFilePath)
	{
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::IsTestFilePresent"));
	COMPONENT_TRACE2(_L("Searching for test file in %S"),&aFilePath);

	TFindFile finder(iFs);	
	TInt err = finder.FindByPath(KTestFile,&aFilePath);
	
	COMPONENT_TRACE1(_L("CHwrmUiPluginResolver::IsTestFilePresent - return"));
	return (err == KErrNone);	
	}

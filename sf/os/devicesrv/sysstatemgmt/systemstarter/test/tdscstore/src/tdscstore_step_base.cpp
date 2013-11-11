// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tdscstore_step_base.h"
#include "tdscstore_constants.h"

#include <tefexportconst.h>

/**
   Constructor
 */
CAppfwkDscStoreTestStepBase::CAppfwkDscStoreTestStepBase()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KAppfwkDscStoreTestStepBase);
	}
	
/**
   Destructor
 */
CAppfwkDscStoreTestStepBase::~CAppfwkDscStoreTestStepBase()
	{	
	delete iEnumData;
	delete iTEFParams;
	}

void CAppfwkDscStoreTestStepBase::SetEnumFileL(const TDesC& aFileName)
	{
	//Set the EnumsFile	
	if (!iEnumData)
		{
		iEnumData = CIniData::NewL(aFileName);
		}
	}

/**  
  @param - aKeyName  TestExecute.ini's key to be overridden
  @param - aValue  	 Value of key which is to be written  
  */
void CAppfwkDscStoreTestStepBase::SetTEFParameterL(const TDesC& aKeyName, const TDesC& aValue)
	{	
	if (!iTEFParams)
		{
		iTEFParams = CIniData::NewL(KTEFDefaultPath);
		
		if (! iTEFParams)
			{
			iTEFParams = CIniData::NewL(KTEFAlternatePath);
			}
		}
	
	if (iTEFParams)
		{		
		//Override TEF Parameters (present in testexecute.ini)
		TInt errVal = iTEFParams->SetValue(KTestExecuteName, aKeyName, aValue);
		User::LeaveIfError(errVal);
		
		//Write to TestExecute.ini file
		iTEFParams->WriteToFileL();		
		}
	else
		{
		_LIT(KErrCreationTEFIni, "Could not create IniData file for Testexecute.ini");
		ERR_PRINTF1(KErrCreationTEFIni);
		}
	}
	
/** 
  @return - ETrue if TEF parameter was found else EFalse
  @param - aKeyName  TestExecute.ini's key to be read
  @param - aValue  	 Value of key which is read  
  */
TBool CAppfwkDscStoreTestStepBase::GetTEFParameterL(const TDesC& aKeyName, TPtrC& aValue)
	{
	if (!iTEFParams)
		{
		iTEFParams = CIniData::NewL(KTEFDefaultPath);
		
		if (! iTEFParams)
			{
			iTEFParams = CIniData::NewL(KTEFAlternatePath);
			}
		}
	
	TBool readResult = EFalse;
	
	if (iTEFParams)
		{
		readResult = iTEFParams->FindVar(KTestExecuteName, aKeyName, aValue);		
		}
	else
		{
		_LIT(KErrCreationTEFIni, "Could not create IniData file for Testexecute.ini");
		ERR_PRINTF1(KErrCreationTEFIni);
		}
	
	return readResult;	
	}

	
/**
  @return - ETrue if enum was resolved to integral value, else EFalse
  @param - aSectName Enum file section to be read
  @param - aKeyName  Key/Enum to be read
  @param - aResult   Updated enum's integral value
  */
TBool CAppfwkDscStoreTestStepBase::GetEnumFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult)
	{
	_LIT(KEnumFileNotSet, "EnumFile");
	__ASSERT_ALWAYS(iEnumData, User::Panic(KEnumFileNotSet, KErrNotFound));
	
	TBool readResult = EFalse;
	TPtrC enumString;
	
	readResult = GetStringFromConfig(aSectName, aKeyName, enumString);
	
	if (readResult)
		{
		readResult = iEnumData->FindVar(KEnums, enumString, aResult);
		}
	else
		{
		_LIT(KEnumKeyNotfound, "EnumKey %S not found");
		WARN_PRINTF2(KEnumKeyNotfound, &aKeyName);
		}
	
	return readResult;
	}

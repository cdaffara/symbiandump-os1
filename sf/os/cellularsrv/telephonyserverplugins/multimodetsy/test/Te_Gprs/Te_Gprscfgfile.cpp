// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// \file TCfgFile.cpp
// This cpp file is taken from the t_inet project.
// It uses a ini file to store settings that can
// be changed like IP addresses and webpage addresses.
// The loading of the drivers has been removed from this file.
// 
//

#include "Te_GprsTestStepBase.h"
#include "Te_Gprscfgfile.h"
#include <f32file.h>




//PhilippeG 15/05/2000
//New object to load and parse the test parameters file
CConfigParams* CConfigParams::NewL(const TDesC &aCfgFileName)
	{
	CConfigParams *f;
	f=new (ELeave) CConfigParams();
	CleanupStack::PushL(f);
	//A leave during the ConstrucL is not fatal, just means there's no param file
	TRAP_IGNORE(f->ConstructL(aCfgFileName));
	CleanupStack::Pop();
	return f;
	}
void CConfigParams::ConstructL(const TDesC &aCfgFileName)
	{
	//Find the config file
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();
	//Location for the test parameter config file
	_LIT(KTinetConfigFilePaths,"\\;\\system\\data\\");
	TFindFile filePath(fs.iObj);
	User::LeaveIfError(filePath.FindByPath(aCfgFileName,&KTinetConfigFilePaths));
	//read the content of the file
	TAutoClose<RFile> fl;
	fl.PushL();
	User::LeaveIfError(fl.iObj.Open(fs.iObj,filePath.File(),EFileShareExclusive));
	User::LeaveIfError(fl.iObj.Read(iConfigParamBuf8));
	iConfigParamBuf.Copy(iConfigParamBuf8);
	CleanupStack::Pop(2);
	iFileExist = ETrue;
	}

const TPtrC CConfigParams::FindAlphaVar(const TDesC &aVarName, const TDesC &aDefault)
	{
	if(!iFileExist)
		return TPtrC(aDefault);
	TInt pos=iConfigParamBuf.Find(aVarName);
	if (pos==KErrNotFound)
		return TPtrC(aDefault);
	TLex lex(iConfigParamBuf.Mid(pos));
	lex.SkipCharacters();
	lex.SkipSpaceAndMark();		// Should be at the start of the data
	lex.SkipCharacters();
	return TPtrC(lex.MarkedToken().Ptr(),lex.MarkedToken().Length());
	}
TInt CConfigParams::FindNumVar(const TDesC &aVarName, const TInt aDefault)
	{
	TInt result;
	TPtrC ptr = FindAlphaVar(aVarName,KNullDesC);
	if(ptr.Length())
		{
		TLex lex(ptr);
		if (lex.Val(result)==KErrNone)
			return result;
		}
	return aDefault;
	}

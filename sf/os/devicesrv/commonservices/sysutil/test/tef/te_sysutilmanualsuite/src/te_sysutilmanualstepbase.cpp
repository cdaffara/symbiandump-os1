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
 @internalTechnology
*/

#include "te_sysutilsmanualtestepbase.h"

CTeSysUtilsSuiteStepBase::~CTeSysUtilsSuiteStepBase()
	{
	iFs.Close();
	}

CTeSysUtilsSuiteStepBase::CTeSysUtilsSuiteStepBase()
	{
	}

TInt CTeSysUtilsSuiteStepBase::CheckIsFileExist( const TDesC& aFilename )
	{
    	RFs fs;
    	TInt err;
    	err = fs.Connect();
    	if (err != KErrNone)
        	return err;

    	RFile file;
    	err = file.Open(fs, aFilename, EFileRead | EFileStreamText | EFileShareReadersOnly);

    	file.Close();
    	fs.Close();

    	if ( err == KErrNotFound || err == KErrPathNotFound )
        	{
        	err = KErrNotSupported;
        	}

	return err;
	}

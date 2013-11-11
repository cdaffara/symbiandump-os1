// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// file sysutilfunctions.cpp
// 
//

#include <e32property.h>
#include <f32file.h>
#include <bafl/sysutil.h>
#include "sysutilconstants.h"
#include "sysutilfunctions.h"
#include "sysutilsetup.h"

/**
Checks if the given file exists.

@param	aFilename			The path of the file to be checked.
@return KErrNone			The file exists.
@return KErrNotSupported	The file does not exist.
@return ...					One of the other system wide error codes.
*/
TInt CheckIsFileExist(const TDesC& aFilename)
	{
	RFs fs;
	TInt err;
	err = fs.Connect();
	if (err != KErrNone)
		{
		return err;
		}
	
	RFile file;
	err = file.Open(fs, aFilename, EFileRead | EFileStreamText | EFileShareReadersOnly);
	
	file.Close();
	fs.Close();
	
	if (err == KErrNotFound || err == KErrPathNotFound)
		{
		err = KErrNotSupported;
		}
	
	return err;
	}

/**
Deletes all the Publish and Subscribe properties.

@return KErrNone	If Publish and Subscribe properties where deleted successfully.
@return ...			One of the other system wide error codes.
*/
TInt DeletePSProperties()
	{
	_LIT( KExeName, "Z:\\sys\\bin\\sysutilsetup.exe" );
	const TUidType KExeUid( KNullUid, KNullUid, KSysUtilSetupUid );
	
	RProcess setupProcess;
	TInt err = setupProcess.Create( KExeName, KDeletePropertiesFlag, KExeUid );
	if( KErrNone != err )
		{
		return err;
		}
	
	TRequestStatus status;
	setupProcess.Rendezvous( status );
	if( KRequestPending != status.Int() )
		{
		setupProcess.Kill( 0 );		// Abort startup
		}
	else
		{
		setupProcess.Resume();		// Logon OK - start the server
		}
	
	User::WaitForRequest( status );	// Wait for start or death
	
	// We can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	err = setupProcess.ExitType() == EExitPanic ? KErrGeneral : status.Int();
	setupProcess.Close();
	return err;
	}

/**
Switches the test paths on and off. The paths as as follows:

Default Paths:
Z:\\resource\\versions\\deviceattributes.ini
Z:\\sys\\bin\\sysutilplugin.dll

Test Paths:
<system drive>:\\versions\\deviceattributes.ini
<system drive>:\\sys\\bin\\sysutilplugin.dll

@param	aTestPathOn	If ETrue, test paths are turned on. If EFalse, test paths are turned off.
@return	KErrNone	If the test paths were turned on/off successfully.
@return	...			One of the other system wide error codes.
*/
TInt SetTestPath(const TBool aTestPathOn)
	{
	_LIT_SECURITY_POLICY_C1( KSecurityPolicyNone, ECapability_None );
	
	TInt err = RProperty::Define( KUidSystemCategory, KTestFilePathsUid.iUid, RProperty::EInt, KSecurityPolicyNone, KSecurityPolicyNone );
	if(KErrNone != err && KErrAlreadyExists != err)
		{
		return err;
		}
	err = RProperty::Set(KUidSystemCategory, KTestFilePathsUid.iUid, aTestPathOn);
	
	return err;
	}

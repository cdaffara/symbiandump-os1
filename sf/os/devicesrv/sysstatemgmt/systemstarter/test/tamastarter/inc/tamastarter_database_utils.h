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


#include <startupproperties.h>
#include <startup.hrh>
#include <dscstoredefs.h>
#include <dscstore.h>
#include <dscitem.h>
#include <sysmonclisess.h>


_LIT(KDbmsDscDdbbOriginal,"c:\\private\\100012a5\\DBS_2000836D_DBS_DSC.db");
_LIT(KDbmsDscDdbbBackup,"c:\\private\\100012a5\\DBS_2000836D_DBS_DSC.db.backup");
_LIT(KSqlDscDdbbOriginal,"c:\\private\\10281e17\\[2000836D]DBS_DSC.db");
_LIT(KSqlDscDdbbBackup,"c:\\private\\10281e17\\[2000836D]DBS_DSC.db.backup");

_LIT(KAMCFileNameApp1,"tamastarter_app1.exe");
_LIT(KAMCFileNameApp2,"tamastarter_app2.exe");
_LIT(KAMCFileNameApp3,"tamastarter_app3.exe");
_LIT(KAMCFileNameApp4,"tamastarter_app4.exe");
_LIT(KAMCFileNameNonExistantApp,"tamastarter_nonexistantapp.exe");
_LIT(KAMCFileNameAppWillNeverRun,"tamastarter_appwillneverrun.exe");
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
_LIT(KSysStartTescProcSlowLog,"testprocslowlog.exe");
#endif // SYMBIAN_SYSTEM_STATE_MANAGEMENT


void DscDatabaseBackupL();
void DscDatabaseRestoreL();
void CreateDscItemL(RDscStore& aStore, TUid aDscUid, const TDesC& aAmaFilename, const TDesC& aAmaArguments, TStartupType aStartupType, TStartMethod aStartMethod, TNoOfRetries aNoOfRetries, TInt aTimeout, TBool aIsMonitored, TBool aIsStartInBackground, TBool aIsViewless, TDscPosition aDscPosition);
TBool CompareDscItemsL(TUid aDscUid, const TDesC& aAmaFilename, const TDesC& aAmaArguments, TStartupType aStartupType, TStartMethod aStartMethod, TNoOfRetries aNoOfRetries, TInt aTimeout, TBool aIsMonitored, TBool aIsStartInBackground, TBool aIsViewless);
void CreateTestDatabaseL();



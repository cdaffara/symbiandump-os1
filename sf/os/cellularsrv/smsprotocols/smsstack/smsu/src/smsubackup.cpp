// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements CBackupAndRestore
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smsubackupTraces.h"
#endif

#include <e32std.h>
#include "SmsuBackup.h"
#include <babackup.h>


const TInt KBURPartMask = 0x000000FF;

#include <connect/sbdefs.h>

using namespace conn;


EXPORT_C CBackupAndRestore* CBackupAndRestore::NewL(MNBBackupAndRestoreObserver& aBackupAndRestoreObserver)
 	{
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CBACKUPANDRESTORE_NEWL_1, "CBackupAndRestore::NewL()");

 	CBackupAndRestore* me = new(ELeave) CBackupAndRestore(aBackupAndRestoreObserver);
 	CleanupStack::PushL(me);
 	me->ConstructL();
 	CleanupStack::Pop(me);
 	return me;
 	} // CBackupAndRestore::NewL


CBackupAndRestore::CBackupAndRestore(MNBBackupAndRestoreObserver& aBackupAndRestoreObserver)
 	:CActive(EPriorityStandard)
 	{
 	iBackupAndRestoreObserver=&aBackupAndRestoreObserver;
 	}


void CBackupAndRestore::ConstructL()
 	{
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBACKUPANDRESTORE_CONSTRUCTL_1, "CBackupAndRestore::ConstructL()");

 	iBackupOrRestoreInProgress = EFalse;
	User::LeaveIfError(iBackupProperty.Attach(KUidSystemCategory,KUidBackupRestoreKey));
	CActiveScheduler::Add(this);
 	iBackupProperty.Subscribe(iStatus);
 	SetActive();
 	}


EXPORT_C CBackupAndRestore::~CBackupAndRestore()
 	{
 	Cancel();
 	iBackupProperty.Close();
 	} // CBackupAndRestore::~CBackupAndRestore


void CBackupAndRestore::DoCancel()
 	{
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBACKUPANDRESTORE_DOCANCEL_1, "CBackupAndRestore::DoCancel()");

 	iBackupProperty.Cancel();
 	} // CBackupAndRestore::DoCancel


EXPORT_C TBool CBackupAndRestore::IsBackupOrRestoreInProgress()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CBACKUPANDRESTORE_ISBACKUPORRESTOREINPROGRESS_1, "CBackupAndRestore::IsBackupOrRestoreInProgress()");

	return iBackupOrRestoreInProgress;
	} // CBackupAndRestore::IsBackupOrRestoreInProgress


/**
 *  @leave KErrNotSupported if unknown backup flag is found
 */
void CBackupAndRestore::RunL()
 	{
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBACKUPANDRESTORE_RUNL_1, "CBackupAndRestore::RunL()");

 	iBackupProperty.Subscribe(iStatus);
 	SetActive();


 	TInt backupRestoreFlag;
 	iBackupProperty.Get(backupRestoreFlag);
 	switch(backupRestoreFlag & KBURPartMask)
 		{
 		case EBURUnset:
 		case EBURNormal:
 			if(iBackupOrRestoreInProgress) //only report complete if already reported starting
 				{
 				iBackupAndRestoreObserver->HandleBackupOrRestoreCompleteL();
 				iBackupOrRestoreInProgress=EFalse;
 				}
 			break;
 		case EBURBackupFull:
 		case EBURBackupPartial:
		case EBURRestoreFull:
		case EBURRestorePartial:
			iBackupOrRestoreInProgress=ETrue;
			iBackupAndRestoreObserver->HandleBackupOrRestoreStartingL();
			break;
		default:
			User::Leave(KErrNotSupported); //unexpected value
		};
 	} // CBackupAndRestore::RunL


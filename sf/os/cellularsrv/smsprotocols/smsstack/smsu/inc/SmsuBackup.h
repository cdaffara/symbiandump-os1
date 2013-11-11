// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declares CBackupAndRestore
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SMSU_BACKUP_H__
#define SMSU_BACKUP_H__

#include <e32base.h>
#include "smsuact.h"
#include <e32property.h>
#include "nbbackuprestoreint.h"


class CBackupAndRestore : public CActive
	{
public:
	IMPORT_C static CBackupAndRestore* NewL(MNBBackupAndRestoreObserver& aBackupAndRestoreObserver);
	IMPORT_C ~CBackupAndRestore();
	IMPORT_C TBool IsBackupOrRestoreInProgress();
private:
	CBackupAndRestore(MNBBackupAndRestoreObserver& aBackupAndRestoreObserver);
	void ConstructL();
	void RunL();
	void DoCancel();
private:
	MNBBackupAndRestoreObserver * iBackupAndRestoreObserver;
	RProperty iBackupProperty;
	TBool iBackupOrRestoreInProgress;
	};

class MBackupOperationObserver;
class CBaBackupSessionWrapper;

/**
 *  @internalComponent
 */
const TInt KSmsuBackupObserverWait = 1000000 * 30; //<30 seconds
const TInt KSmsuBackupMaxRetries = 5; //< Number of times to try to connect to the backup server

#endif // SMSU_backup_H__

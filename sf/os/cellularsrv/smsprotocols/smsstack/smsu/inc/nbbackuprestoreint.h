// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__NBBACKUPANDRESTOREOBSERVER_H__)
#define __NBBACKUPANDRESTOREOBSERVER_H__

#include <e32std.h>
#include <e32base.h>


/**
 *  Nbprotocols Publish and Subscribe Backup and Restore Interface.
 *  
 *  classes should implement this interface to receive notifications
 *  when a backup or restore operation starts.
 *  
 *  @see CBackupAndRestore::RegisterBackupAndRestoreObserverL()
 *  @internalComponent
 */
class MNBBackupAndRestoreObserver
	{
public:
	// Called when a backup or restore operation either starts or ends.
	virtual void HandleBackupOrRestoreStartingL()=0;
	virtual void HandleBackupOrRestoreCompleteL()=0;
	};

#endif

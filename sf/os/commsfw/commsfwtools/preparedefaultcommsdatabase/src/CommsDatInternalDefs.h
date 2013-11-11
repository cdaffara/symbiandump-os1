// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * @file CommsDatInternalDefs.h
 * 
 * Definitions for internal use within CommsDat component
 *
 * @internalTechnology
 * @released
 * 
 */

#if (! defined COMMSDATINTERNALDEFS_H)
#define        COMMSDATINTERNALDEFS_H


#include <e32base.h>
#include <e32std.h>


// From netmeta, iterator used internally
#include <comms-infras/metaiterator.h>

// Storage server
#include <centralrepository.h>

// CommsDat interface headers
#include "CommsDat.h"
#include "MetaDatabase.h"

// Internal headers
#include "Commsdat_Log.h"
#include "MetaDatabaseSessionImpl.h"
#include "CommsDatUtilImpl.h"
#include "CommsDatNotifier.h"
#include "CommsDatMapper.h"
#include "MetaDatabaseVisitor.h" 

namespace CommsDatInternal
/*    
Contains definitions in use within the CommsDat component

@internalTechnology
*/
{


// Repository for comms data
const TUid KCDCommsRepositoryId = { 0xCCCCCC00 };

// Repository for user defined data
const TUid KCDAppsRepositoryId  = { 0xAAAAAA00 };


/*
Empty Mask for use with CentralRepository calls
Central Repository will match the id as written.

@internalComponent
*/
#define KCDCenRepNoMask	     0xffffffff               

 
/*
Flag to indicate a field is not null 
Not for use in database.  Just with fields
Same value as KCDUtilityFlag as these should not clash
@internalComponent
*/
#define KCDNotNullFlag       0x80000000

/*
Flag to indicate utility data
Same value as KCDIsNullFlag deliberately
@internalComponent
*/
#define KCDUtilityFlag      0x80000000


/*
Flag to indicate flag used to define if RecordId or Tag is used for 
Link Record data loading
@internalComponent
*/
#define KLinkableTag	KCDUtilityFlag
/*
Location of home-made write-lock semaphore in db

@internalComponent
*/
#define KCDWriteLock        0x00000000

/*
@internalComponent
*/
#define KCDGlobalSettingsRecordId        0x00000100




NONSHARABLE_CLASS(TCDNotifiableEvent)
    {
    public:

    enum TEvent 
    /*
    These events designed to match RDBNotifier events in dbms
    */
	    {
	    /** The database has been closed. */
	    EClose,
	    /** All read locks have been removed. */
	    EUnlock,
	    /** A transaction has been committed. */
	    ECommit,
	    /** A transaction has been rolled back */
	    ERollback,
	    /** The database has been recovered */
	    ERecover
        };
    };

/*
*/
//const TUid KUidCommDbNotificationEvent = {0x10008FFF};
/** A record in the `DEFAULT_GPRS` table has been modified or deleted. */


} //end namespace CommsDatInternal



namespace CommsDat
{

// Maximum size of the MMetaDBVisitor based object in terms of sizeof(TAny*).
// KMMetaTypeMaxSize * sizeof(TAny*) is the size of the memory reserved
// on the stack for the in-place instantiation of every MMetaDBVisitor class
const TInt KMMetaDBVisitorMaxSize = 10;

const TInt KCommsDatSvrRealMaxFieldLength=50;


// For Pub/sub notifications
const TUint KSlashChar='\\';
const TUid KCommsDbPolicyUid = {0x10004e1d};


const TCapability KCommsDatNotificationSetCapability = ECapabilityWriteDeviceData;

} // end namespace CommsDat

      
#endif  // COMMSDATINTERNALDEFS_H
        

















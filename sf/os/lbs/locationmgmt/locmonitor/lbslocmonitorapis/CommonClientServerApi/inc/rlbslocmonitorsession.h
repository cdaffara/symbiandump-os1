/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


/**
@InternalTechnology
@prototype
*/

#ifndef RLBSLOCMONITORSESSION_H
#define RLBSLOCMONITORSESSION_H

#include "rsecuresessionbase.h"

/**
Client-side session with the Location Monitor server.
 */
class RLbsLocMonitorSession : public RSecureSessionBase
	{
	
public:

	IMPORT_C RLbsLocMonitorSession();
	
	IMPORT_C TInt Connect();

	IMPORT_C void Close();

	/* Close the database and wipe out the DB file.
	*/
	IMPORT_C void WipeOutDatabase(TRequestStatus& aStatus) const;
   	
   	/* Cancel previous request to delete the Database
   	of positions.
   	*/
   	IMPORT_C TInt CancelDatabaseWipeOut() const;
   	
   	IMPORT_C TVersion Version();
   	
protected:
	
	// From RSecureSessionBase
	TInt StartServer(TServerStartParams& aParams);
	};

#endif // RLBSLOCMONITORSESSION_H

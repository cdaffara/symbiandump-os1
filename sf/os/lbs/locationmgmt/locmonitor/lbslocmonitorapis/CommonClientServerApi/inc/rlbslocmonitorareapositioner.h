/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
@file
@InternalTechnology
*/

#ifndef RLBSLOCMONITORAREAPOSITIONER_H
#define RLBSLOCMONITORAREAPOSITIONER_H

#include <lbspositioninfo.h>
#include <lbsareainfo.h>
#include "rlbslocmonitorsession.h"
#include "lbslocmonitorserverdata.h"
#include "clbslocmonitorptrholder.h"

/**
This class is used by clients of the Location Monitor server to
create a sub-session with the server.

Before using the class, a primary connection must have already been
established with the server with RLbsLocMonitorSession::Connect().

@see RLbsLocMonitorSession
*/
class RLbsLocMonitorAreaPositioner : public RSecureSubSessionBase
	{

public:

	IMPORT_C RLbsLocMonitorAreaPositioner();
	IMPORT_C void OpenL(RLbsLocMonitorSession& aLocMonSession);
	IMPORT_C void Close();

   	/* Called by the Location Server to report the last
   	 Position obtained from any of its PSYs that are not otherwise
   	 available to the Location Monitor. The position
   	 will be stored in a database.
   	 */
   	IMPORT_C TInt SetLastKnownPosition(const TPositionInfoBase& aPosInfo) const;
   	
   	/* Retrieve the position last stored in the database */
    IMPORT_C void GetLastKnownPosition(TPositionInfoBase& aPosInfo,
                                	 TRequestStatus& aStatus) const;
    
    /* Cancel a previous call to GetLastKnownPosition*/
    IMPORT_C TInt CancelGetLastKnownPosition() const;                 

  	/* Returns in aPosInfo a position which had been previously stored 
  	in association with "context information" that is bound to a 
  	geographical area (e.g. Global Cell Id).
  	
  	Current context information will be sensed by the server and used
  	to search for a position in the database. If found, the position 
  	will be an approximation of the current position of the handset.
  	If no position is found in the database for the current context
  	information, the position last stored in the database is returned.
  	
  	The method returns in aAreaInfo a rough indication of how accurate the
  	returned position is based on an estimation of the size of the
  	geographical area covered by the sensed context information.
   	*/
   	IMPORT_C void GetLastKnownPositionArea(TPositionInfoBase& aPosInfo,
				      				 TPositionAreaInfoBase& aAreaInfo,
				      				 TPosAreaReqParams& aParameters,
                                	 TRequestStatus& aStatus) const;

    /* Cancel a call to GetLastKnownPositionArea */
    IMPORT_C TInt CancelGetLastKnownPositionArea() const;

                                	 	 
protected:

	IMPORT_C virtual void ConstructL();
	IMPORT_C virtual void Destruct();

private:

	/** A pointer to a container that holds pointer descriptors, needed to point
	to the clients request data during asynchronous requests */
	CLbsLocMonitorPtrHolder* iPtrHolder;                                	 
	};
	
#endif // RLBSLOCMONITORAREAPOSITIONER_H

/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//
//  EPos_MPosParameterObserver.h
//
// Copyright (c) Symbian Software Ltd 2004.  All rights reserved.
//

#ifndef __EPOS_MPOSPARAMETEROBSERVER_H__
#define __EPOS_MPOSPARAMETEROBSERVER_H__

#include <e32def.h>
#include <lbscommon.h>

// CLASS DECLARATION

/**
nterface implemented by the EPos server to supply positioning related
parameter values.

@internalComponent
@released
*/
class MPosParameterObserver
    {
public:  // New functions
	/**
	Get the quality of service requested by the client.

	@param aPositionQuality The requested quality of service.
	@return KErrNotFound if there is no requested quality of service,
	KErrNone if there is.
	*/
	virtual TInt GetRequiredPositionQuality(TPositionQuality& aPositionQuality) const = 0;

	/**
	Get the current query id.

	Query id is a reference to the latest request verification and can be
	used by a PSY, which encapsulate a positioning service that requires
	its own verification.

	The query id is only valid during an outstanding
	@see NotifyPositionUpdate() call.

	@return The query id of the current position request.
	*/
	virtual TInt QueryId() const = 0;

	/**
	Get the earliest allowed time of an old position fix, based on the
	current max age set by the client.

	The PSY may save the result from the latest position request and
	return the same result in the next position request if the client
	says it's ok. Max age tells the PSY how old the stored position is
	allowed to be. If the stored position is too old or the PSY does not
	support max age, a normal positioning should be performed.

	@param aMaxAge On return, will contain the earliest allowed time of 
	an old position fix. If no max age is defined aMaxAge will contain 
	a time set to zero.
	*/
	virtual void GetMaxAge(TTime& aMaxAge) const = 0;

	/**
	Checks if the client allows a partial position update.

	A partial update result does not need to contain all parameters. The
	only mandatory parameters are latitude, longitude and time of fix.
	Everything else is optional.

	If a partial update is returned to the client in a
	@see NotifyPositionUpdate call, the completion code must be set to
	KPositionPartialUpdate.

	@return ETrue if partial position updates are allowed, otherwise
	EFalse.
	*/
	virtual TBool IsPartialUpdateAllowed() const = 0;
    };  

#endif  // MPOSPARAMETEROBSERVER_H

// End of File

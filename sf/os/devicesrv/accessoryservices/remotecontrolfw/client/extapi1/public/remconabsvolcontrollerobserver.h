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
 @file
 @publishedAll
 @released
*/

#ifndef REMCONABSVOLCONTROLLEROBSERVER_H
#define REMCONABSVOLCONTROLLEROBSERVER_H

#include <e32base.h>

/**
Client-implemented mixin- notifies the client of incoming AbsVol commands.
*/
class MRemConAbsVolControllerObserver
	{
public:
	/** 
	A response to a 'get absolute volume' command has been received.
	@param aAbsVol The relative volume.
	@param aMaxVol The maximum against which aAbsVol is relative.
	@param aError The response error.
	*/
	virtual void MrcavcoGetAbsoluteVolumeResponse(TUint aAbsVol, TUint aMaxVol, TInt aError);

	/** 
	A response to a 'set absolute volume' command has been received.
	@param aError The response error.
	*/
	virtual void MrcavcoSetAbsoluteVolumeResponse(TInt aError);
	};

#endif // REMCONABSVOLCONTROLLEROBSERVER_H

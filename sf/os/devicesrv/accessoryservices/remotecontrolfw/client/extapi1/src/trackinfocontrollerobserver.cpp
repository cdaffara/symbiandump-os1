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
 @internalComponent
*/

#include <remcontrackinfocontrollerobserver.h>

EXPORT_C void MRemConTrackInfoControllerObserver::MrcticoSetTrackNameResponse(TInt /*aError*/)
	{
	}

//Only used in the internal pan-shared back to back test builds. Should never be in a released product.
#ifdef SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
/**
A response to a 'get track name' command has been received.
	
@param aError The response error.
@param aTrackName The track name.
*/
EXPORT_C void MRemConTrackInfoControllerObserver::MrcticoGetTrackNameResponse(TInt /*aError*/, const TDesC& /*aTrackName*/)
	{
	}

/**
A response to a 'get artist' command has been received.
	
@param aError The response error.
@param aArtist The artist.
*/	
EXPORT_C void MRemConTrackInfoControllerObserver::MrcticoGetArtistResponse(TInt /*aError*/, const TDesC& /*aArtist*/)
	{
	}

/**
A response to a 'get track duration' command has been received.
	
@param aError The response error.
@param aDuration The track duration.
*/	
EXPORT_C void MRemConTrackInfoControllerObserver::MrcticoGetTrackDurationResponse(TInt /*aError*/, const TTime& /*aDuration*/)
	{
	}
#endif //SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT

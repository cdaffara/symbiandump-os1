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

#include <remcontrackinfotargetobserver.h>

EXPORT_C void MRemConTrackInfoTargetObserver::MrctitoGetTrackName()
	{
	}

EXPORT_C void MRemConTrackInfoTargetObserver::MrctitoGetArtist()
	{
	}

EXPORT_C void MRemConTrackInfoTargetObserver::MrctitoGetTrackDuration()
	{
	}

//Only used in the internal pan-shared back to back test builds. Should never be in a released product.
#ifdef SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
/**
A 'set track name' command has been received.
	
@param aTrackName The new track name from the controller.
*/
EXPORT_C void MRemConTrackInfoTargetObserver::MrctitoSetTrackName(const TDesC& /*aTrackName*/)
	{
	}
#endif //SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT

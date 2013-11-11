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

#ifndef REMCONTRACKINFOCONTROLLEROBSERVER_H
#define REMCONTRACKINFOCONTROLLEROBSERVER_H

#include <e32base.h>

/**
Client-implemented mixin- notifies the client of incoming TrackInfo responses.
*/
class MRemConTrackInfoControllerObserver
	{
public:
	/**
	A response to a 'set track name' command has been received.
	@param aError The response error.
	*/
	virtual void MrcticoSetTrackNameResponse(TInt aError);
	
/**
The following section of code is included to allow internal back to back 
testing of this component within Symbian Software Ltd. It will only be 
compiled in specially configured test builds. It will never be available 
in any release and so should never be used outside of Symbian owned test 
code.
*/
#ifdef SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
	/**
	This method is only available in specially configured test builds and will 
	not be found in any released code.
	*/
	virtual void MrcticoGetTrackNameResponse(TInt aError, const TDesC& aTrackName);
	
	/**
	This method is only available in specially configured test builds and will 
	not be found in any released code.  
	*/
	virtual void MrcticoGetArtistResponse(TInt aError, const TDesC& aArtist);
	
	/**
	This method is only available in specially configured test builds and will 
	not be found in any released code.  
	*/
	virtual void MrcticoGetTrackDurationResponse(TInt aError, const TTime& aDuration);
#endif //SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
	};

#endif // REMCONTRACKINFOCONTROLLEROBSERVER_H

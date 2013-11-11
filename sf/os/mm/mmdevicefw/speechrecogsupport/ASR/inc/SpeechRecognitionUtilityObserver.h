// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SPEECHRECOGNITIONUTILITYOBSERVER_H__
#define __SPEECHRECOGNITIONUTILITYOBSERVER_H__

//  INCLUDES

#include <mmf/common/speechrecognitiondataclient.h>

// CLASS DECLARATION

/**
@publishedAll
@released

This is the main class of Speech Recognition Utility Observer

@since  8.0
*/
class MSpeechRecognitionUtilityObserver
{

public:
	/**
	Pure virtual function that receives speech recognition utility events.

	@param  aEvent
	        The event code indicating the event type.
	@param  aError
			The result code.

	@since  8.0
	*/
	virtual void MsruoEvent( TUid aEvent, TInt aError )=0;

};

#endif	// __SPEECHRECOGNITIONUTILITYOBSERVER_H__

// End of File

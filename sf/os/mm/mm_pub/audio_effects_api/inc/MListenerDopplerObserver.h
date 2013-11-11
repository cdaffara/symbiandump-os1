/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the definition of the Listener Doppler Effect observer 
*                class.
*
*
*/


#ifndef MLISTENERDOPPLEROBSERVER_H
#define MLISTENERDOPPLEROBSERVER_H

// INCLUDES

#include <e32base.h>
#include <ListenerDopplerBase.h>
#include <MDopplerObserver.h>


// CLASS DECLARATION

/**
*  This is the Listener doppler effect observer class. Object interested in receiving notification
*  when the CDoppler object changes state should derive from this class and implement its
*  interface.
*
*  @since 3.0
*/

class MListenerDopplerObserver : public MDopplerObserver
	{

	};

#endif	// of MLISTENERDOPPLEROBSERVER_H

// End of File
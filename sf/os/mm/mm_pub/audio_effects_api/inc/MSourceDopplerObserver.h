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
* Description:  This is the definition of the Source Doppler Effect observer class.
*
*
*/


#ifndef MSOURCEDOPPLEROBSERVER_H
#define MSOURCEDOPPLEROBSERVER_H

// INCLUDES

#include <e32base.h>
#include <SourceDopplerBase.h>
#include <MDopplerObserver.h>


// CLASS DECLARATION

/**
*  This is the doppler effect observer class. Object interested in receiving notification
*  when the CDoppler object changes state should derive from this class and implement its
*  interface.
*
*  @since 3.0
*/

class MSourceDopplerObserver : public MDopplerObserver
	{
	};

#endif	// of MSOURCEDOPPLEROBSERVER_H

// End of File
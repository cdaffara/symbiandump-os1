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
* Description:  This is the definition of the Source location effect observer class.
*
*
*/


#ifndef MSOURCELOCATIONOBSERVER_H
#define MSOURCELOCATIONOBSERVER_H

// INCLUDES

#include <e32base.h>
#include <SourceLocationBase.h>
#include <MLocationObserver.h>


// CLASS DECLARATION

/**
*  This is the Source Location effect observer class. Object interested in receiving notification
*  when the CSourceLocation object changes state should derive from this class and implement its
*  interface.
*
*  @since 3.0
*/

class MSourceLocationObserver : public MLocationObserver
	{
		// No specific event ID other than the ones from the base observer class.
	};

#endif	// of MSOURCELOCATIONOBSERVER_H

// End of File
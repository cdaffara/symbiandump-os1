// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent
*/

#include <eikstart.h>

#include "t_fpsappdoc.h"
#include "t_fpsapp.h"
#include "t_wservconsts.h" //KUidPseudoapp


/**
 *	Gets the application specific UID.
 *	@return - UID of the simpleapp application
 *
 */
TUid CFpsApplication::AppDllUid() const
	{
	return KUidFpsApp;
	}

/**
 *	Creates a document object for the application
 *	@return pointer to the newly created document
 *
 */
CApaDocument* CFpsApplication::CreateDocumentL()
	{
	return new (ELeave) CFpsDocument(*this);
	}

/**
 *	Creates and returns the object of the application
 *	@return pointer to the newly created application
 *
 */
LOCAL_C CApaApplication* NewApplication()
	{
	return new CFpsApplication;
	}

/**
 *	Entry point for the application
 *
 */
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}



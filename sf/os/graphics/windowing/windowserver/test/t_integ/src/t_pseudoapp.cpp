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

#include "t_pseudoappdoc.h"
#include "t_pseudoapp.h"
#include "t_wservconsts.h" //KUidPseudoapp / KUidPseudoApp1


/**
 *	Gets the application specific UID.
 *	@return - UID of the simpleapp application
 *
 */
TUid CPseudoApplication::AppDllUid() const
	{
#ifndef T_PSEUDOAPP1
	return KUidPseudoApp;
#else
	return KUidPseudoApp1;
#endif //T_PSEUDOAPP1
	}

/**
 *	Creates a document object for the application
 *	@return pointer to the newly created document
 *
 */
CApaDocument* CPseudoApplication::CreateDocumentL()
	{
	return new (ELeave) CPseudoDocument(*this);
	}

/**
 *	Creates and returns the object of the application
 *	@return pointer to the newly created application
 *
 */
LOCAL_C CApaApplication* NewApplication()
	{
	return new CPseudoApplication;
	}

/**
 *	Entry point for the application
 *
 */
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}



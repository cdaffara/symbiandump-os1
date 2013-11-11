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
#include "t_dsaappdoc.h"
#include "t_dsaapp.h"
#include "t_wservconsts.h"

/**
 *	Gets the application specific UID.
 *	@return - UID of the simpleapp application
 *	
 */  
TUid CDsaApplication::AppDllUid() const
	{
	return KUidDsaApp;
	}
	
/**
 *	Creates a document object for the application
 *	@return pointer to the newly created document
 *	
 */  
CApaDocument* CDsaApplication::CreateDocumentL()
	{
	return new (ELeave) CDsaDocument(*this);
	}

/**
 *	Creates and returns the object of the application
 *	@return pointer to the newly created application
 *	
 */  
LOCAL_C CApaApplication* NewApplication()
	{
	return new CDsaApplication;
	}

/**
 *	Entry point for the application
 *	
 */  
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
	


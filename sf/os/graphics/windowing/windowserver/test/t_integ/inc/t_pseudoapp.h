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

#ifndef __T_PSEUDO_APP_H__
#define __T_PSEUDO_APP_H__

#include <eikapp.h>

// Entry point
GLDEF_C TInt E32Main(void);

/**
 *	CPseudoApplication creates a new instance of the document
 *   associated with this application
 *
 */
class CPseudoApplication : public CEikApplication
	{
private:
	// Inherited from class CApaApplication to create a new instance of the document
	CApaDocument* CreateDocumentL();
	//gets the Application's UID
	TUid AppDllUid() const;
	};

#endif //__T_PSEUDO_APP_H__

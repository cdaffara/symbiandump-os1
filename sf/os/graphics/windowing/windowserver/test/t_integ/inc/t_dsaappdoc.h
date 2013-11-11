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

#ifndef __T_DSA_APP_DOC_H__
#define __T_DSA_APP_DOC_H__

#include <eikdoc.h>

/**
 *	CDsaDocument for creating the application UI
 *	
 */  
class CDsaDocument : public CEikDocument
	{
public:
	// creates a CDsaDocument object
	static CDsaDocument* NewL(CEikApplication& aApp);
	CDsaDocument(CEikApplication& aApp);
	void ConstructL();
private: 
	// Inherited from CEikDocument for creating the AppUI
	CEikAppUi* CreateAppUiL();
	};
	
#endif // __T_DSA_APP_DOC_H__

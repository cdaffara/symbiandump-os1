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
// tcustom_cmd.cpp
// Provides a reference example of a Custom Command dll within the ssma architecture.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#include "tcustom_cmdimpl.h"

/**
Provides initialisation of the dll
@param aRfs 	An initialised File System object
@return	One of the system-wide error codes.
*/	
EXPORT_C MSsmCustomCommand* Entrypoint()
	{
	return new RefCustomCommand;
	}


 // Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <eikstart.h>
#include "testlauncherapplication.h"

/**
 * Factory function to create the Hello World Basic application class
 */
LOCAL_C CApaApplication* NewApplication()
	{
	return new CHelloWorldBasicApplication;
	}

/**
 * A normal Symbian OS executable provides an E32Main() function which is
 * called by the operating system to start the program.
 */
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}


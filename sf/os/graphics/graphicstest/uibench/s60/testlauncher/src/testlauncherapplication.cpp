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


#include "testlauncherdocument.h"
#include "testlauncherapplication.h"


// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidHelloWorldBasicApp = { 0xA0000180 };

// Creates CApaDocument object
CApaDocument* CHelloWorldBasicApplication::CreateDocumentL()
    {
    // Create an HelloWorldBasic document, and return a pointer to it
    return (static_cast<CApaDocument*>(CHelloWorldBasicDocument::NewL(*this)));
    }

// CHelloWorldBasicApplication::AppDllUid()
TUid CHelloWorldBasicApplication::AppDllUid() const
    {
    // Return the UID for the HelloWorldBasic application
    return KUidHelloWorldBasicApp;
    }

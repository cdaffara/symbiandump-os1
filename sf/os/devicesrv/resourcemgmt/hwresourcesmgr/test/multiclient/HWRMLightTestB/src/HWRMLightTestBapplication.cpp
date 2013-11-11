// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "HWRMLightTestBdocument.h"
#include "HWRMLightTestBapplication.h"

// UID for the application; this should correspond to the uid defined in the mmp file
const TUid KUidHWRMLightTestBApp = {0x10005B94};

CApaDocument* CHWRMLightTestBApplication::CreateDocumentL()
    {  
    // Create an HWRMLightTestB document, and return a pointer to it
	return (static_cast<CApaDocument*>(CHWRMLightTestBDocument::NewL(*this))); 
    }

TUid CHWRMLightTestBApplication::AppDllUid() const
    {
    // Return the UID for the HWRMLightTestB application
    return KUidHWRMLightTestBApp;
    }
    


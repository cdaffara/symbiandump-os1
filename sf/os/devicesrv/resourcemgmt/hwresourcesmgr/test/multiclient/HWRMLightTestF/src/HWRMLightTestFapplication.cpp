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




#include "HWRMLightTestFdocument.h"
#include "HWRMLightTestFapplication.h"

// UID for the application; this should correspond to the uid defined in the mmp file
const TUid KUidHWRMLightTestFApp = {0x10005B92};

CApaDocument* CHWRMLightTestFApplication::CreateDocumentL()
    {  
    // Create an HWRMLightTestF document, and return a pointer to it
	return (static_cast<CApaDocument*>(CHWRMLightTestFDocument::NewL(*this))); 
    }

TUid CHWRMLightTestFApplication::AppDllUid() const
    {
    // Return the UID for the HWRMLightTestF application
    return KUidHWRMLightTestFApp;
    }
    


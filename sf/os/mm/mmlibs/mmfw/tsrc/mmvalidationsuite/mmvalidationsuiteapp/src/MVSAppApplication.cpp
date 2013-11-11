// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Part of the MVS Application for TechView
//

#include "MVSApp.h"
#include "MVSAppApplication.h"


//
//The UID of the application
//
const TUid KUidMVSApp = TUid::Uid(0x102737E8);




//
//AppDllUid()
//
// Returns the UID of the application.
//
TUid CMVSAppApplication::AppDllUid() const 
    {
    return KUidMVSApp;
    }



//
//CreateDocumentL()           *** This method can LEAVE ***
//
// Create a CAudioDocument object
//
CApaDocument* CMVSAppApplication::CreateDocumentL() 
    {
    return new(ELeave) CMVSAppDocument(*this);
    }


//
//

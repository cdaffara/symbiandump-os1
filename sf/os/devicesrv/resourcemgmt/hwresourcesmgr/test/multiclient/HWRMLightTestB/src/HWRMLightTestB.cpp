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




#include "HWRMLightTestBapplication.h"

#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return (static_cast<CApaApplication*>(new CHWRMLightTestBApplication)); //CHWRMLightTestBApplication;
    //return new CMyApplication;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }




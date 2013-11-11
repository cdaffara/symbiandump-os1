/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Main application class
*
*/


// INCLUDE FILES
#include <eikstart.h>
#include "audioclientstubApplication.h"

LOCAL_C CApaApplication* NewApplication()
    {
    RDebug::Print( _L("AudioClientStub::NewApplication") ) ;
    
    return new CaudioclientstubApplication;
    }

GLDEF_C TInt E32Main()
    {
    RDebug::Print( _L("AudioClientStub::E32Main") ) ;  
    
    return EikStart::RunApplication( NewApplication );
    }


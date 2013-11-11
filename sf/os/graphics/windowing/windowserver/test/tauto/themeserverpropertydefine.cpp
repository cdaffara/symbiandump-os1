// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This Process called with the argument KThemeServerPropertyDefineCmdDefine, defines
// the theme server RProperty catagory to be the same as the theme servers process ID. 
// Called with the argument KThemeServerPropertyDefineCmdDelete, it will delete the 
// theme server RProperty. This is because an RProperty can only be defined and deleted 
// from within a process with the same UID3 as the RProperty catogory you are trying to 
// define/delete.
//

/**
 @file
 @test
 @internalComponent - Internal Nokia test code
 */

#include <BACLINE.H> 
#include <e32base.h>
#include <e32property.h>
#include "..\..\nga\server\renderorientationtracker.h" //for KThemeOrientationCatagory and KThemeOrientationKey
#include "themeserverpropertydefine.h"

void DefineThemeServerPropertyL()
    {
    RProperty themeServerOrientationProperty;
    const TSecurityPolicy KThemeOrientationReadSecurityPolicy(
            TSecurityPolicy::EAlwaysPass);
    const TSecurityPolicy KThemeOrientationWriteSecurityPolicy(
            TSecurityPolicy::EAlwaysPass);
    User::LeaveIfError(RProperty::Define(   KThemeOrientationCategory,
                                            KThemeOrientationKey, 
                                            RProperty::EInt,
                                            KThemeOrientationReadSecurityPolicy,
                                            KThemeOrientationWriteSecurityPolicy));
    themeServerOrientationProperty.Close();
    }

void DeleteThemeServerPropertyL()
    {
    TInt err = KErrNone;
    RProperty themeServerOrientationProperty;
    User::LeaveIfError(themeServerOrientationProperty.Delete(
            KThemeOrientationCategory, KThemeOrientationKey));
    themeServerOrientationProperty.Close();
    }

void MainL()
    {
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    TPtrC argumentPrt(args->Arg(1));
    if (argumentPrt == KThemeServerPropertyDefineCmdDefine)
        {
        DefineThemeServerPropertyL();
        }
    else if (argumentPrt == KThemeServerPropertyDefineCmdDelete)
        {
        DeleteThemeServerPropertyL();
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    CleanupStack::PopAndDestroy(args);
    }

GLDEF_C TInt E32Main()
    {
    CTrapCleanup* cleanUpStack = CTrapCleanup::New();
    if (cleanUpStack == NULL)
        {
        return KErrNoMemory;
        }
    TRAPD(err, MainL());
    delete cleanUpStack;
    return (err);
    }


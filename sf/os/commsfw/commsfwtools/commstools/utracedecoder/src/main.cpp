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

#include "utracedecoderapp.h"

int main(int aArgc, char** aArgv)
    {
    CUTraceDecoderApp app;
    
    if (app.Initialise(aArgc, aArgv))
        {
        if (!app.TestMode())
            {
            app.ProcessLog();
            }
        }
    else
        {
        app.ShowUsage();
        return 1;
        }

    return 0;
    }


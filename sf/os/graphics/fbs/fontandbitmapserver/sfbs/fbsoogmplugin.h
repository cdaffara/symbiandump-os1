/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Graphics-Out-Of-Memory plugin for Font and Bitmap server.
*
*/

#ifndef FBSOOGMPLUGIN_H
#define FBSOOGMPLUGIN_H

#include <goommonitorplugin.h>

class CFbsOogmMonitorPlugin : public CGOomMonitorPlugin
    {
public:
    CFbsOogmMonitorPlugin();
    ~CFbsOogmMonitorPlugin();
    void ConstructL();

public:
    void FreeRam( TInt aBytesToFree, TInt aFlags );
    void MemoryGood( TInt aFlags );
    };

#endif /* FBSOOGMPLUGIN_H */

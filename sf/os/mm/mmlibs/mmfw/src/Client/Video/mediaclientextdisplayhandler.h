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
//

#ifndef MEDIACLIENTVIDEODISPLAYHANDLER_H
#define MEDIACLIENTVIDEODISPLAYHANDLER_H

#include <w32std.h>

/**
@internalTechnology
@released

*/
NONSHARABLE_CLASS(CMediaClientExtDisplayHandler) : public CBase
    {
public:

    static CMediaClientExtDisplayHandler* NewL(TInt aDisplayId);
    
    RWindowBase* Window();
    TSize DisplaySizeInPixels();
    void UpdateWindow();
    
    ~CMediaClientExtDisplayHandler();

private:
    
    CMediaClientExtDisplayHandler(TInt aDisplayId);
    void ConstructL();
    
private:
    
    TInt iDisplayId;
    CWsScreenDevice* iScreen;
    RWindow iExternalDisplayWindow;
    RWsSession iWs;
    RWindowGroup iRootWindow;
    };

#endif // MEDIACLIENTVIDEODISPLAYHANDLER_H


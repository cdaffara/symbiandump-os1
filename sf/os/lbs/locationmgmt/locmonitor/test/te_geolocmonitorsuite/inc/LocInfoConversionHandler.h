/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: STIF testclass declaration
*
*/

#ifndef LOCINFOCONVERSIONHANDLER_H_
#define LOCINFOCONVERSIONHANDLER_H_

#include <e32base.h>

class CLocInfoConversionHandler : public CActive
    {
public:
    static CLocInfoConversionHandler* NewL();
    
    ~CLocInfoConversionHandler();
    
    void Start();
protected: // from CActive    
    void RunL();
    void DoCancel();
    
private:
    CLocInfoConversionHandler();
    
    void ConstructL();
    };

#endif /*LOCINFOCONVERSIONHANDLER_H_*/

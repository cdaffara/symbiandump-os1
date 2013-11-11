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
* Description:  ?Description
*
*/


#ifndef CSTUBTIMER_H
#define CSTUBTIMER_H


#include <e32base.h>




/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CStubTimer : public CActive
    {

public:


    static CStubTimer* NewL();


    virtual ~CStubTimer();

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    void Start( TTimeIntervalMicroSeconds32 aInterval, TCallBack aCallBack );

// from base class CActive

    void DoCancel();
    void RunL();


private:

    CStubTimer();

    void ConstructL();

private: // data

    TTimeIntervalMicroSeconds32 iInterval;
    TCallBack iCallBack;
    
    RTimer iTimer;

    };




#endif // ? C_CLASSNAME_H

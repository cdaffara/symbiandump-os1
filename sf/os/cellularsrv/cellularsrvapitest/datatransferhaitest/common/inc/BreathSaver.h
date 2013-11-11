/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/


#ifndef BREATHSAVER_H
#define BREATHSAVER_H

#include <f32file.h>


// Forward declarations
class CDataDealer;

/**
 * BreathSaver provides ways to save data, for example to filesystem (c:\)
 */
class CBreathSaver : public CBase
    {
public:
    static CBreathSaver* NewL();
    ~CBreathSaver();

	TInt StartDealer(const TUint aKey);
    TInt StartFile(const TDesC &aFileName);
    TInt Append(TDes8 &aBuffer);
    TInt End();

protected:
    CBreathSaver();
    void ConstructL();

private:
    RFs iRFsession;
    RFile iFile;

    TBool iFileStarted; // while file is started but not ended new start is not allowed
    TBool iDealerStarted; // while file is started but not ended new start is not allowed
	CDataDealer* iDataDealer;
    };

#endif	// BREATHSAVER_H

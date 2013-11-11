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
* Description:  Header file for autoaudioasy main service
*
*/

#ifndef CAUTOAUDIOASYMAINSERVICEBASE_H_
#define CAUTOAUDIOASYMAINSERVICEBASE_H_

#include <asymainservicebase.h>

//Forward declarations
class CAutoAudioAsyService;


/**
 *  Creates ASY main service
 */
NONSHARABLE_CLASS(CAutoAudioAsyMainServiceBase): public CASYMainServiceBase
    {
public:  // Constructors and destructor
        
    static CAutoAudioAsyMainServiceBase* NewL ();
    virtual ~CAutoAudioAsyMainServiceBase ();

public: // From CASYMainServiceBase
                
    TInt StartL();

private: // Functions

    CAutoAudioAsyMainServiceBase (); 
    void ConstructL ();
	
	void InitPSAutoAudioL ();

private: //Data

    /**
     * Handle to service class.
     * Own.
     */
    CAutoAudioAsyService* iService;
    };

#endif // CAUTOAUDIOASYMAINSERVICEBASE_H_

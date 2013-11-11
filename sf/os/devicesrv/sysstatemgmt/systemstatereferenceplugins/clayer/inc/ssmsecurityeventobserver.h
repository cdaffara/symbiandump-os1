/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Declaration of CStrtSecurityEventObserver class
*
*/



#ifndef __SSMSECURITYEVENTOBSERVER_H__
#define __SSMSECURITYEVENTOBSERVER_H__

#include <e32base.h>
#include <etelmm.h>
#include <ssm/ssmutility.h>
#include "strtsecuritynotetype.h"

/* 
* @publishedPartner
* @released
*/

class CStrtSecurityNoteController;
class CStrtSecPhaseObserver;

NONSHARABLE_CLASS( CStrtSecurityEventObserver ) : public CActive, public MSsmUtility
    {
public:
	//From MSsmUtility
	void InitializeL();
	void StartL();
	void Release();
    
public:
    /**
     * Two-phased constructor.
     */
    IMPORT_C static MSsmUtility* NewL();

protected:
// from base class CActive

    /**
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();

    /**
     * Handles an active object’s request completion event.
     * This RunL will never leave, so RunError not needed.
     */
    void RunL();

private:
    /**
    * Destructor.
    */
    ~CStrtSecurityEventObserver();
    
    /**
     * C++ default constructor.
     */
    CStrtSecurityEventObserver();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Send notify security event to the ETel and set active.
     * Should not be called if already active.
     */
    void Activate();

    /**
     * Processes commands from the ETel.   
     */
    void ProcessSecurityEvent();

    /**
     * Sends SIM code request to the controller.
     *
     * @param aNoteType Identifies the security note to show.
     */
    void SimCodeRequest( const TStrtSecurityNoteType aNoteType );

    /**
     * Sends security code request to the controller.
     *
     * @param aNoteType Identifies the security note to show.
     */
    void SecCodeRequest( const TStrtSecurityNoteType aNoteType );

    /**
     * Sends other type (i.e. application PIN) 
     * code request to the controller.
     *
     * @param aNoteType Identifies the security note to show.
     */
    void OtherCodeRequest( const TStrtSecurityNoteType aNoteType );

    /**
     * Sends code verified command to the controller.
     *
     * @param aNoteType Identifies the security note which was shown.
     */
    void CodeVerifyIndication( const TStrtSecurityNoteType aNoteType );

private: // data
    /**
     * Observes the startup security phase.
     */
    CStrtSecPhaseObserver* iSecPhaseObserver;

    /**
     * Controls showing security notifications.
     */    
    CStrtSecurityNoteController* iController;

    /**
     * TelServer client
     */
    RTelServer iTelServer;
    
    /**
     * Mobile phone network client
     */
    RMobilePhone iPhone;
        
    /**
     * Notify Security Event received from TSY.
     */
    RMobilePhone::TMobilePhoneSecurityEvent iEvent;
    HBufC* iTsyModuleName;

#ifdef TEST_CLAYER_MACRO
	friend class CLayerTestSsmEventObserver;
#endif //TEST_CLAYER_MACRO
    };
#endif // __SSMSECURITYEVENTOBSERVER_H__

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
* Declaration of CStrtSecPhaseObserver class
*
*/



#ifndef C_STRTSECPHASEOBSERVER_H
#define C_STRTSECPHASEOBSERVER_H

#include <e32base.h>
#include <e32property.h>

/*
* @publishedPartner
* @released
*/

/**
 *  Monitors changes in KStarterSecurityPhase P&S property.
 *
 *  lib : strtsecobs.lib
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CStrtSecPhaseObserver ) : public CActive
    {

public:

    /**
     * Two-phased constructor.
     */
    static CStrtSecPhaseObserver* NewL();

    /**
    * Destructor.
    */
    virtual ~CStrtSecPhaseObserver();
    
    /**
     * Used to ask is startup SIM phase over.
     *    
     * @return TRUE if startup SIM phase is over.
     */
    TBool IsSecurityPhaseSimOk() const;
    
    /**
     * Used to ask is startup SEC phase over.
     *     
     * @return TRUE if startup SEC phase is over.
     */
    TBool IsSecurityPhaseSecOk() const;

private:
// from base class CActive

    /**
     * From CActive.
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();

    /**
     * From CActive.
     * Handles an active object’s request completion event.
     * This RunL will never leave, so RunError not needed.
     */
    void RunL();

private:

    /**
     * C++ default constructor.
     */
    CStrtSecPhaseObserver();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Subscribe to the property and set active. Should not be called if already
     * active.
     */
    void IssueRequest();

private: // data

    /**
     * Property to observe.
     */
    RProperty iProperty;

    /**
     * Current value of the property.
     */
    TInt iValue;

#ifdef TEST_CLAYER_MACRO
	friend class CLayerTestSsmEventObserver;
#endif //TEST_CLAYER_MACRO
    };

#endif // C_STRTSECPHASEOBSERVER_H

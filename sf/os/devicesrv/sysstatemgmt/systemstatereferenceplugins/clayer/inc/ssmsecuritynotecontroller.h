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
* Declaration of CStrtSecurityNoteController class
*
*/



#ifndef __SSMSECURITYNOTECONTROLLER_H__
#define __SSMSECURITYNOTECONTROLLER_H__

#include <e32base.h>

#include "strtsecuritynotetype.h"

/*
 *  @publishedPartner
 *  @released
*/


class CSsmSecurityCheckNotifier;
class CStrtSecNoteRequestQueue;

/**
 *  This class controls security notes.
 *
 *  lib : strtsecobs.lib
 *  @since S60 3.2
 *  
 */
NONSHARABLE_CLASS( CStrtSecurityNoteController ) : public CActive
    {

public:

    /**
     * Two-phased constructor.
     */
    static CStrtSecurityNoteController* NewL();

    /**
    * Destructor.
    */
    virtual ~CStrtSecurityNoteController();

    /**
     * Initiate a security code query or information note, if there is no note
     * currently active.
     * If there is an active note, put this note to queue.
     *
     * @param aNoteType Identifies the security note to show.
     * @return One of the system-wide Symbian error codes.
     */
    TInt SecurityNoteRequested( const TStrtSecurityNoteType aNoteType );

    /**
     * A security code has been verified. Remove it from the queue (if it is
     * there).
     *
     * @param aNoteType Identifies the security code that has been verified.
     */
    void SecurityCodeVerified( const TStrtSecurityNoteType aNoteType );

protected:
// from base class CActive

    /**
     * From CActive.
     * Implements cancellation of an outstanding request.
     */
    virtual void DoCancel();

    /**
     * From CActive.
     * Handles an active object's request completion event.
     */
    virtual void RunL();

	/**
     * From CActive.
     * Handles any leaves originating from the active object's RunL().
     */
        
    virtual TInt RunError(TInt aError);

private:

    /**
     * C++ default constructor.
     */
    CStrtSecurityNoteController();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Jump to RunL.
     */
    void CompleteSelf();

private: // data

    /**
     * Queue of requested security notes. May not be NULL.
     * Own.
     */    
    CStrtSecNoteRequestQueue* iQueue;

    /**
     * Object for showing different security notes to user. May not be NULL.
     * Own.
     */    
    CSsmSecurityCheckNotifier* iSecurityNote;

#ifdef TEST_CLAYER_MACRO
	friend class CLayerTestSsmEventObserver;
#endif //TEST_CLAYER_MACRO

    };
#endif // __SSMSECURITYNOTECONTROLLER_H__

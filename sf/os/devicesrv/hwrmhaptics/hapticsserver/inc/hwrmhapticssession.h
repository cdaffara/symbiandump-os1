/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics server side session interface.
*
*/


#ifndef C_HWRMHAPTICSSESSION_H
#define C_HWRMHAPTICSSESSION_H

#include <e32base.h>

class CHWRMHapticsServer;
class CHWRMHapticsService;

/**
*  Represents a session for a client thread on the server-side.
*  A session acts as a channel of communication between the client 
*  and the server.
*/
class CHWRMHapticsSession : public CSession2
    {
public:

    /**
     * Two-phased constructor.
     *
     * @return Instance of this class.
     */
    static CHWRMHapticsSession* NewL();

    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsSession();
    
    /**
     * From CSession2.
     * 
     * @param aMessage The message containing the client request
     */
    virtual void ServiceL( const RMessage2& aMessage );

    /**
     * From CSession2
     */
    virtual void CreateL();

    /**
     * From CSession2
     */
    void Disconnect(const RMessage2& aMessage);

private:

    /**
     * C++ default constructor.
     */
    CHWRMHapticsSession();

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Handles commands from client.
     * 
     * @param aMessage The message containing the client request
     */
    void DispatchMessageL( const RMessage2& aMessage );

    /**
     * Method for getting the Server object to which this session
     * belongs to.
     */
     CHWRMHapticsServer& MyServer();

private:

    /**
     * Pointer to the service object.
     */
    CHWRMHapticsService* iService;

    /**
     * A flag to indicate if message completion is needed in ServiceL.
     * Async and plugin calls set this to false.
     */ 
    TBool iCompletionNeeded;
    };

#endif      // C_HWRMHAPTICSSESSION_H 
            
// End of File

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
* Description:  Definition of haptics client side session.
*
*/


#ifndef R_HWRMHAPTICSSESSION_H
#define R_HWRMHAPTICSSESSION_H

class RSessionBase;

/**
 * The client-side handle to a session with the Haptics Server.
 */
class RHWRMHapticsSession : public RSessionBase
    {
public:

    /**
     * Connects to server
     *
     * @return KErrNone if connect succesfully, otherwise system wide error
     */
    TInt Connect();

    /**
     * Connects to server asynchronously.
     *
     * @param aStatus Status for async call.
     */
    void Connect( TRequestStatus& aStatus );

    /**
     * Send server a command. This method is just a wrapper for 
     * SendReceive.
     *
     * @param aCommand Command identifier.
     * @param aArgs Command arguments.
     * @return KErrNone if sent succesfully, otherwise system wide error
     */
    TInt ExecuteOperation( TInt aCommand, const TIpcArgs& aArgs ) const;

    /**
     * Send server a command. This method is just a wrapper for 
     * SendReceive.
     *
     * @param aCommand Command identifier.
     * @return KErrNone if sent succesfully, otherwise system wide error
     */
    TInt ExecuteOperation( TInt aCommand ) const;

    /**
     * Send server a command. This method is just a public wrapper for 
     * asynchronous version of SendReceive.
     *
     * @param aCommand Command identifier.
     * @param aArgs Command arguments.
     * @param aStatus Status object for async call
     * @return KErrNone if sent succesfully, otherwise system wide error
     */
    void ExecuteAsyncOperation( TInt aCommand, const TIpcArgs& aArgs,
                                TRequestStatus& aStatus ) const;

private:

    /**
     * Returns server location,
     *
     * @return The location of the server exe.
     */
    TFullName ServerLocation() const;

    /**
     * Returns the version of the server.
     * 
     * @return The version of the server.
     */
    TVersion ServerVersion() const;
         
    /**
     * Starts server.
     * 
     * @return KErrNone if started succesfully, otherwise system wide error
     */
    TInt StartServer() const;

    /**
     * Makes connection to server.
     * 
     * @return KErrNone if connected succesfully, otherwise system wide error
     */
    TInt MakeConnection();
    };

#endif // R_HWRMHAPTICSSESSION_H   
            
// End of File

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




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMCLIENT_H
#define HWRMCLIENT_H


/**
* Resource Type enumeration. 
*/
enum THWRMResourceType
    {
    HWRMResourceTypeVibra = 0,
    HWRMResourceTypeLight,
    HWRMResourceTypePower,
    HWRMResourceTypeFmTx,
    HWRMResourceTypeFmTxWatcher
    };

class RSessionBase;


/**
*  Base class of the client-side handle to a session with the HWRM Server.
*/
class RHWRMClient : public RSessionBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        * @param
        * @return 
        */
         //RHWRMClient();

    public: // New functions

        /**
        * Connects to server
        *
         * @param aType Resource type
        * @return KErrNone if connect succesfully, otherwise system wide error
        */
        IMPORT_C TInt Connect(THWRMResourceType aType);

        /**
         * Send server a command. This method is just a public wrapper for 
         * SendReceive.
         *
         * @param aCommand Command identifier.
         * @param aArgs Command arguments.
         * @return KErrNone if connect succesfully, otherwise system wide error
         */
        IMPORT_C TInt ExecuteOperation(TInt aCommand, const TIpcArgs& aArgs) const;

         /**
          * Send server a command. This method is just a public wrapper for 
          * asynchronous version of SendReceive.

          * @param aCommand Command identifier.
          * @param aArgs Command arguments.
          * @param aStatus Status object for async call
          * @return KErrNone if connect succesfully, otherwise system wide error
          */
        IMPORT_C void ExecuteAsyncOperation(TInt aCommand, const TIpcArgs& aArgs, TRequestStatus& aStatus) const;

    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes

    private:

        /**
        * Return server location
        * @param
        * @return location of server
        */
         TFullName ServerLocation() const;

        /**
        * Return version of server
        * @param
        * @return version of server
        */
         TVersion ServerVersion() const;
         
        /**
        * Starts server
        * @return KErrNone if started succesfully, otherwise system wide error
        */
        TInt StartServer() const;

    public:     // Data
    
    protected:  // Data

    private:    // Data        

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // HWRMCLIENT_H   
            
// End of File

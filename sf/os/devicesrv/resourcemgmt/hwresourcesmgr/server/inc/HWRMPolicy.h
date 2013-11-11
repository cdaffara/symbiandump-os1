/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HWRMPOLICY_H
#define HWRMPOLICY_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>    // For RFs

// CONSTANTS
const TInt KHighestPriority( 1 );
const TInt KLowestPriority( 1000 );
// If no 'ALL' client exists in file this is the default priority
const TInt KDefaultNormalPriority( 500 );
// HWRM Lights policy
_LIT( KLightsPolicyFilename, "HWRMLightsPolicy.ini" );
// HWRM Vibra policy
_LIT( KVibraPolicyFilename, "HWRMVibraPolicy.ini" );
// HWRM FM Tx policy
_LIT( KFmTxPolicyFilename, "HWRMFmTxPolicy.ini" );
// Customer policy filename prefix
_LIT( KCustomerPolicyPrefix, "Customer" );
// Product policy filename prefix
_LIT( KProductPolicyPrefix, "Product" );

// Drive used to store policy files
_LIT( KDrivename, "Z:" );


// CLASS DECLARATION

/**
*  This class is intended to parse the associated priority text file and 
*  provide an API to return the clients priority.
*
*/
class CHWRMPolicy : public CBase
    {
    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aFilename  Filename of policy file, filename should not
        *                   include the path as the file is stored in 
        *                   the private directory.
        */
        static CHWRMPolicy* NewL( const TDesC& aFilename );

        /**
        * Two-phased constructor.
        * Leaves instance on the cleanup stack.
        * @param aFilename  Filename of policy file, filename should not
        *                   include the path as the file is stored in 
        *                   the private directory.
        */
        static CHWRMPolicy* NewLC( const TDesC& aFilename );
        
        /**
        * Destructor.
        */
        virtual ~CHWRMPolicy();

    public:     // New functions

        /**
        * Method to retrieve the priority of a client with the given ID.
        * This ID can be the UID or SID defined in the policy file.
        * @param aMsg     Message from client, used to obtain SID
        * @param aTrusted Returns ETrue if client is trusted, i.e. 
        *                   explicitly specified in policy file.
        * @return         Priority of client.
        *                 If client does not exist default priority defined 
        *                   by client ALL.
        * Method to return indication of feedback client
        * @param aSid       Clients SID
        * @return   ETrue if feedback client, otherwise EFalse
        */
        TInt GetPriority( const TSecureId aSid, TBool& aTrusted ) const;

        /**
        * Method to return indication of feedback client
        * @param aSid       Clients SID
        * @return   ETrue if feedback client, otherwise EFalse
        */
        TBool FeedbackClient(TSecureId aSid) const;

    private:    // New functions

        /**
        * C++ constructor.
        */
        CHWRMPolicy();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aFilename  Filename of policy file, filename should not
        *                   include the path as the file is stored in 
        *                   the private directory.
        */
        void ConstructL( const TDesC& aFilename );
    
        /**
        * Method constructs the array of policy clients from the 
        * given stream (usually an RFileReadStream)
        * @param aFs          File server session
        * @param aFilename    Filename to read
        */
        void ParsePriorityFileL( RFs& aFs, const TDesC& aFilename );

        /**
        * Helper method to convert and validate SID from a descriptor.
        * @param aSidDes    The Sid to be converted
        * @param aSid       Reference that will contain the SID
        * @return           KErrNone if successful, KErrCorrupt if invalid
        */
        TInt ConvertId( const TDesC8& aSidDes, TSecureId& aSid ) const;
        
        /**
        * Adds a policy client to the array, first checks that no other
        * client has been registered with the identical id. If one already
        * exists KErrAlreadyExists is returned.
        * @param aPriority  Clients priorty
        * @param aSid       Clients SID or KAllPolicySid
        * @param aFeedbackClient Indication whether feedback client
        * @param aAppName   Clients application name
        * @return           KErrNone if successful
        *                   KErrAlreadyExists if client id already exists
        */
        TInt AddPolicyClientL( const TInt aPriority, const TSecureId& aSid, 
                               const TBool aFeedbackClient, const TDesC8& aAppName );

        /**
        * Searches for client with the given id
        * @param aId        Clients SID or KAllPolicySid
        * @return           Client position in iClientArray
        *                   KErrNotFound if client does not exist
        */
        TInt FindClient( const TSecureId& aSid ) const;
        
    private:    // Data

    /**
    *  Embedded class to contain policy client data intended 
    *  to be read from the policy file.
    *
    */
    class CPolicyClient : public CBase
        {
        public:     // Constructors and destructor

            /**
            * Two-phased constructor.
            * @param aPriority  Clients priorty
            * @param aSid       Clients SID or KAllPolicySid
            * @param aFeedbackClient Indication whether feedback client
            * @param aAppName   Clients application name
            */
            static CPolicyClient* NewL( const TInt aPriority, 
                                        const TSecureId& aSid,
                                        const TBool aFeedbackClient,
                                        const TDesC8& aAppName );

            /**
            * Two-phased constructor.
            * Leaves instance on the cleanup stack.
            * @param aPriority  Clients priorty
            * @param aFeedbackClient Indication whether feedback client
            * @param aAppName   Clients application name
            */
            static CPolicyClient* NewLC( const TInt aPriority,
                                         const TSecureId& aSid,
                                         const TBool aFeedbackClient,
                                         const TDesC8& aAppName );
            
            /**
            * Destructor.
            */
            virtual ~CPolicyClient();

        public:     // New functions
        
            /**
            * Method to return the clients priority
            * @return   Priority of client
            */
            TInt Priority() const;

            /**
            * Method to returns the clients SID
            * @return   SID of client
            */
            TSecureId Sid() const;

            /**
            * Method to return indication whether feedback client
            * @return   ETrue if feedback client, otherwise EFalse
            */
            TBool FeedbackClient() const;

            /**
            * Method to returns the client application name
            * @return   Application name of the client
            */
            const TDesC& AppName() const;
        

        
        private:    // New functions
        
            /**
            * C++ constructor.
            * @param aPriority  Clients priorty
            * @param aFeedbackClient Indication whether feedback client
            */
            CPolicyClient( const TInt aPriority, const TSecureId& aSid, 
                const TBool aFeedbackClient );
            
            
            /**
            * By default Symbian 2nd phase constructor is private.
            * @param aAppName   Clients application name
            */
            void ConstructL( const TDesC8& aAppName );
            
        private:    // Data
            
            // priority of client
            TInt        iPriority;
            // the sid of client
            TSecureId   iSid;
            // boolean indicating whether tactile feedback client
            TBool       iFeedbackClient;
            // clients app name
            HBufC*      iAppName;
        
        };

        // Array of objects representing each client from the policy file.
        RPointerArray<CPolicyClient> iClientArray;

        // Default policy for 'ALL' client, not stored in array for efficiency
        CPolicyClient* iAllPolicyClient;
    };


#endif      // HWRMPOLICY_H

// End of File

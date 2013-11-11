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
* Description:  Haptics policy header file.
*                This class is designed to parse the associated text file
*                and determine the HWRMHaptics clients priorities for 
*                reservation of resources.
*
*/


#ifndef C_HWRMHAPTICSPOLICY_H
#define C_HWRMHAPTICSPOLICY_H

#include <e32base.h>

class RFs;

/**
 *  This class is intended to parse the associated priority text file and 
 *  provide an API to return the clients priority.
 *
 *  @since S60 5.1
 */
class CHWRMHapticsPolicy : public CBase
    {
public:
    
    /**
     * Two-phased constructor.
     *
     * @param aFilename  Filename of policy file, which should not
     *                   include the path as the file is stored in 
     *                   the private directory.
     */
    static CHWRMHapticsPolicy* NewL( const TDesC& aFilename );

    /**
     * Two-phased constructor.
     * Leaves instance on the cleanup stack.
     * 
     * @param aFilename  Filename of policy file, which should not
     *                   include the path as the file is stored in 
     *                   the private directory.
     */
    static CHWRMHapticsPolicy* NewLC( const TDesC& aFilename );
        
    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsPolicy();

public:

    /**
     * Method to retrieve the priority of a client with the given ID.
     * This ID can be the UID or SID defined in the policy file.
     *       
     * @param aSid     Secure id of the client.
     * @param aTrusted Returns ETrue if client is trusted, i.e. 
     *                 it is specified in policy file.
     * @param          Priority of client. If client does not exist,
     *                 default priority is returned.
     */
    void GetPriority( const TSecureId aSid, TBool& aTrusted, TInt& aPriority ) const;

private:

    /**
     * C++ constructor.
     */
    CHWRMHapticsPolicy();

    /**
     * By default Symbian 2nd phase constructor is private.
     *
     * @param aFilename  Filename of policy file, which should not
     *                   include the path as the file is stored in 
     *                   the private directory.
     */
    void ConstructL( const TDesC& aFilename );
    
    /**
     * Method constructs the array of policy clients from the 
     * given stream (usually an RFileReadStream)
     * 
     * @param aFs File server session
     * @param aFilename Filename to read
     */
    void ParsePriorityFileL( RFs& aFs, const TDesC& aFilename );

    /**
     * Helper method to convert and validate SID from a descriptor.
     * 
     * @param aSidDes The Sid to be converted
     * @param aSid Reference that will contain the SID
     * @return KErrNone if successful, KErrCorrupt if invalid
     */
    TInt ConvertId( const TDesC8& aSidDes, TSecureId& aSid ) const;
        
    /**
     * Adds a policy client to the array, first checks that no other
     * client has been registered with the identical id. If one already
     * exists KErrAlreadyExists is returned.
     * 
     * @param aPriority  Clients priority
     * @param aSid       Clients SID or KAllPolicySid
     * @param aFeedbackClient Indication whether feedback client
     * @param aAppName   Clients application name
     * @return           KErrNone if successful
     *                   KErrAlreadyExists if client id already exists
     */
    TInt AddPolicyClientL( const TInt aPriority, const TSecureId& aSid, 
                           const TBool aFeedbackClient, 
                           const TDesC8& aAppName );

    /**
     * Searches for client with the given id.
     * 
     * @param aSid Clients SID or KAllPolicySid.
     * @return Client position in iClientArray,
     *         KErrNotFound if client does not exist.
     */
    TInt FindClient( const TSecureId& aSid ) const;
        
private:

    /**
     *  Embedded class to contain policy client data intended 
     *  to be read from the policy file.
     *
     *  @since S60 5.1
     */
    class CPolicyClient : public CBase
        {
    public:

        /**
         * Two-phased constructor.
         * 
         * @param aPriority  Clients priority
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
         * 
         * @param aPriority  Clients priorty
         * @param aSid       Clients SID or KAllPolicySid
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

    public:
        
        /**
         * Method to return the clients priority.
         *
         * @return Priority of client
         */
        TInt Priority() const;

        /**
         * Returns the client's SID.
         * 
         * @return SID of client
         */
        TSecureId Sid() const;

        /**
         * Returns indication whether feedback client.
         * 
         * @return ETrue if feedback client, otherwise EFalse.
         */
        TBool FeedbackClient() const;

        /**
         * Returns the client application name.
         * 
         * @return Application name of the client.
         */
        const TDesC& AppName() const;
        
    private:
        
        /**
         * C++ constructor.
         * 
         * @param aPriority  Clients priority
         * @param aSid       Clients SID or KAllPolicySid
         * @param aFeedbackClient Indication whether feedback client.
         */
        CPolicyClient( const TInt aPriority, 
                       const TSecureId& aSid, 
                       const TBool aFeedbackClient );
            
        /**
         * By default Symbian 2nd phase constructor is private.
         * 
         * @param aAppName   Clients application name
         */
        void ConstructL( const TDesC8& aAppName );
            
    private: // Data
            
        /**
         * Priority of client
         */
        TInt iPriority;
        
        /** 
         * SID of client
         */
        TSecureId iSid;
        
        /**
         * Whether or not the client is tactile feedback clien.
         */
        TBool iFeedbackClient;
        
        /**
         * Client's application name. Owned.
         */
        HBufC* iAppName;
        };

private:

    /**
     * Array of objects representing each client from the policy file.
     */
    RPointerArray<CPolicyClient> iClientArray;
    };

#endif // C_HWRMHAPTICSPOLICY_H

// End of File

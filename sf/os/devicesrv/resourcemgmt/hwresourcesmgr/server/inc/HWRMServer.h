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

#ifndef HWRMSERVER_H
#define HWRMSERVER_H

//  INCLUDES
#include <e32svr.h>
#include "HWRMClientServer.h"
#include "HWRMClient.h"  // THWRMResourceType

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CHWRMPluginHandler;
class CHWRMReservationHandler;
class CHWRMVibraCommonService;
class CHWRMVibraCommonData;
class CHWRMLightCommonData;
class CHWRMFmTxCommonData;
class CHWRMFmTxStateUtility;
class CHWRMPowerStatePluginHandler;
class CHWRMPowerCommonData;

// CLASS DECLARATION

/**
*  Hardware Resource Manager server implementation 
*
*/
class CHWRMServer : public CPolicyServer
    {
    public:  // Constructors and destructor
        
        /**
        * Creates a new server.
        * @param aPriority Priority of the server.
        * @return A pointer to the created object.
        */
        static CHWRMServer* NewL( const TInt aPriority );
        
        /**
        * Creates a new server. Newly created instance is left in the cleanup stack.
        * @param aPriority Priority of the server.
        * @return A pointer to the created object.
        */
        static CHWRMServer* NewLC( const TInt aPriority );

        /**
        * Destructor.
        */
        virtual ~CHWRMServer();

    	/**
    	* Panics client thread.
    	*
    	* @param aMessage Message that is being handled
    	* @param aPanic Panic type
    	*/
    	static void PanicClient(const RMessage2& aMessage, THWRMPanic aPanic);
    	
    	/**
    	* Get pointer to plugin handler of specified type.
    	* Returns NULL if specified handler is not initialized yet.
    	*
    	* @param aType Resource type
    	* @return pointer to correct plugin handler
    	*/
    	CHWRMPluginHandler* GetPluginHandler(THWRMResourceType aType);

    	/**
    	* Get pointer to reservation handler of specified type.
    	* Returns NULL if specified handler is not initialized yet.
    	*
    	* @param aType Resource type
    	* @return pointer to correct reservation handler
    	*/
    	CHWRMReservationHandler* GetReservationHandler(THWRMResourceType aType);
    	
    	/**
    	* Get pointer to vibra common service.
    	*
    	* @return pointer to vibra common service
    	*         Returns NULL if resource not supported.
    	*/
    	CHWRMVibraCommonService* GetVibraCommonService();



    	/**
    	* Get pointer to vibra common data.
    	*
    	* @return pointer to vibra common data
    	*         Returns NULL if resource not supported.
    	*/
    	CHWRMVibraCommonData* GetVibraCommonData();

    	/**
    	* Get pointer to light common data.
    	*
    	* @return pointer to light common data.
    	*         Returns NULL if resource not supported.
    	*/
    	CHWRMLightCommonData* GetLightCommonData();

    	/**
     	* Get pointer to Power common data.
     	*
     	* @return pointer to Power common data.
     	*         Returns NULL if resource not supported.
     	*/
     	CHWRMPowerCommonData* GetPowerCommonData();
     	
    	/**
     	* Get pointer to FmTx common data.
     	*
     	* @return pointer to FmTx common data.
     	*         Returns NULL if resource not supported.
     	*/
     	CHWRMFmTxCommonData* GetFmTxCommonData();
     	
        /**
        * This method is called by CActive if RunL leaves.
        *
        * @param aError Symbian error code.
        * @return Returns KErrNone if the error was handled or aError otherwise.
        */
        //TInt RunError(TInt aError);


    public: // New functions      

    public: // Functions from base classes
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ constructor.
        */
        CHWRMServer( const TInt aPriority, 
                     const TServerType aType = EUnsharableSessions );
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Creates a new session when client connects.
        * @param aVersion Version
        * @param aMessage 'connect' message from the client
        * @return Pointer to created session or leaves with codes
        *         KErrNotSupported if versions does not match
        *         KErrNoMemory if creation of new session fails.
        */
        CSession2* NewSessionL( const TVersion& aVersion,
                                const RMessage2& aMessage ) const;
        /**
        * Panics the server thread
        * @param aCategory Panicer's id 
        * @param aPanic    Reason of panic 
        * @return void
        */
        void Panic( const TDesC& aCategory, const TInt aReason );
        
        /**
        * Method to initialize vibra. 
        * Vibra is not initialized in ConstructL in order to
        * speed up the startup.
        */
        void InitVibra();

        /**
        * Method to initialize FM Tx. 
        * FM Tx is not initialized in ConstructL in order to
        * speed up the startup.
        */
		void InitFmTxL();

		/**
		* Method to initialize Power. 
        * Power is not initialized in ConstructL in order to
		* speed up the startup.
		*/
        void InitPower();


    public:     // Data
    
    protected:  // Data

    private:    // Data

        // Server has one plugin handler for each hardware resource
        CHWRMPluginHandler*            iVibraPluginHandler;
        CHWRMPluginHandler*            iLightPluginHandler;
        CHWRMPowerStatePluginHandler*  iPowerStatePluginHandler;
        CHWRMPluginHandler*            iFmTxPluginHandler;
        CHWRMPluginHandler*            iFmTxWatcherPluginHandler; // Own.
		CHWRMPluginHandler*            iPowerPluginHandler;

        // Server has one reservation handler for each hardware resource
        CHWRMReservationHandler*   iVibraReservationHandler;
        CHWRMReservationHandler*   iLightReservationHandler;
		CHWRMReservationHandler*   iFmTxReservationHandler;

        // Server has one common vibra service instance for all vibra sessions
        CHWRMVibraCommonService* iVibraCommonService;

        // Server has one common data instance for each hardware resource
        CHWRMVibraCommonData* iVibraCommonData;
        CHWRMLightCommonData* iLightCommonData;
        CHWRMFmTxCommonData*  iFmTxCommonData;
        CHWRMFmTxStateUtility* iFmTxStateUtility;
        CHWRMPowerCommonData* iPowerCommonData;
        
        TBool iVibraInitAttempted;  // If ETrue, vibra initialization has been tried before.
		TBool iFmTxInitAttempted;  // If ETrue, FM Tx initialization has been tried before.
        TBool iPowerInitAttempted;  // If ETrue, power initialization has been tried before.        
        TInt iPluginTimeout;        // Stores plugin timeout for vibra initialization
        

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
        //friend class CHWRMSession;
        //friend class CHWRMPluginHandler;
    };

#endif      // HWRMSERVER_H 
            
// End of File

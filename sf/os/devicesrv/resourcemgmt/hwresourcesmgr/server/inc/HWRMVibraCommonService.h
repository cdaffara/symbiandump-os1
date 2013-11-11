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

#ifndef HWRMVIBRACOMMONSERVICE_H
#define HWRMVIBRACOMMONSERVICE_H

//  INCLUDES
#include <e32base.h>
#include "HWRMVibra.h"
#include "HWRMService.h"

// CONSTANTS
const TInt KUseDefaultIntensity = KMaxTInt;

// FORWARD DECLARATIONS
class CHWRMPluginHandler;
class CHWRMReservationHandler;

// CLASS DECLARATIONS
class TVibraClientData
    {
    public:
        /**
        * C++ constructor.
        */
        TVibraClientData( CHWRMService* aClient, TSecureId aSid, TInt aIntensity )
            : iClient( aClient ),
              iSid( aSid ),
              iIntensity( aIntensity )
            {
            };
        
        CHWRMService* iClient; // Session from which request comes. Not owned.
        TSecureId iSid;
        TInt iIntensity;
    };


/**
*  Hardware Resource Manager server side service object for all Vibra sessions.
*  Purpose of this function is to keep track of actively vibrating clients
*  and their data. When pulse API is used, it is specified that any ongoing vibration
*  will be continued after pulse, if there is still time remaining for it. Each client
*  has own service and this service keeps track of overlapping vibrations within service.
*  However, one client may interrupt another client's vibration and that is why there
*  must be common place to keep track of overlapping vibrations.
*/
class CHWRMVibraCommonService : public CHWRMService
    {
    public: // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        *
        * @param aPluginHandler The plugin handler to use with this service.
        * @param aReservationHandler The reservation handler to use with this service.
        */
        static CHWRMVibraCommonService* NewL(CHWRMPluginHandler* aPluginHandler,
                                       CHWRMReservationHandler* aReservationHandler);
        
        /**
        * Destructor.
        */
        virtual ~CHWRMVibraCommonService();
        
    public: // Base class methods
    
        // From CHWRMService
    	virtual TBool ExecuteMessageL(const RMessage2& aMessage);
    	virtual void ProcessResponseL( TInt aCommandId,  TUint8 aTransId, TDesC8& aData, TBool aTimeout );
    	virtual void SuspendSubResource(TInt aSubResource);
    	virtual void ResumeSubResource(TInt aSubResource);
        virtual void ActivateSubResource(TInt aSubResource, TBool aActivate);    	    	
        
    public: // New methods
        
        /**
        * Sets active vibrating client data, which can be used
        * for policing and overlapping vibrations.
        *
        * @param aData Vibra client data.
        */
        void SetClientData( TVibraClientData aData );
        
        /**
        * Checks whether client has vibra data.
        *
        * @param aClient Session from which reset request comes.
        * @return Indication whether this session has still requests.
        */
        TBool HasData( CHWRMService* aClient );

        /**
        * Resets active vibrating client data.
        *
        * @param aClient Session from which reset request comes.
        */
        void ResetClientData( CHWRMService* aClient );

        /**
        * Returns ETrue if active vibrating client's priority is
        * higher than asking client's priority.
        *
        * @param aSid Secure ID of client asking for priority.
        * @return Indication whether vibra execution is requested
        * by a higher priority client than this client.
        */
        TBool ExecutingPriorityHigher(TSecureId aSid);

        /**
        * Returns ETrue if specified client is allowed to 
        * request vibra pulse. Otherwise EFalse.
        *
        * @param aSid Secure ID of client.
        * @return Indication whether vibra pulse request
        * is allowed for this client.
        */
        TBool VibraPulseAllowed(TSecureId aSid);

    private: 
    
        /**
        * C++ default constructor.
        */
        CHWRMVibraCommonService();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CHWRMPluginHandler* aPluginHandler,
                        CHWRMReservationHandler* aReservationHandler);
        
        /**
        * Restarts vibration with last suspended intensity.
        */
        void RestartVibraL();

    private:  // data
    
        RArray<TVibraClientData> iVibraClients;  // Vibra client objects. 
    	                                 // Last client requesting vibration is always added as first in list. 
    };

#endif  // HWRMVIBRACOMMONSERVICE_H
            
// End of File

/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor Control API
*
*/




#ifndef SENSRVCHANNELFINDERIMPL_H
#define SENSRVCHANNELFINDERIMPL_H

//  INCLUDES
#include <e32base.h>
#include "sensrvchannelfinder.h"
#include "sensrvtypes.h"
#include "sensrvclient.h"
#include "sensrvchannellistener.h"
#include "sensrvchannelchangehandlerobserver.h"

class CSensrvChannelChangeHandler;


/**
 *  Sensor control for controlling and searching sensors
 *
 *  @lib SensrvClient.lib
 *  @since S60 5.0
 */

NONSHARABLE_CLASS(CSensrvChannelFinderImpl) : public CSensrvChannelFinder, 
                                              public MSensrvChannelChangeHandlerObserver
    {

    friend class CSensrvChannelFinder;
    
    public:
    
        /**
        * Destructor.
        */
        virtual ~CSensrvChannelFinderImpl();
        
    public:
      
        /**
        * Find channels which have properties given in the parameters
        *
        * @since S60 5.0
        * @param aChannelList List of channel info objects
        * @param aSearchConditions Search conditions for channels to be fetched
        * @return void Description
        */    
         void FindChannelsL( RSensrvChannelInfoList& aChannelList, 
                                      const TSensrvChannelInfo& aSearchConditions );
                                      
         /**
        * Set channel listener. The channel listener may listen new channels installed to
        * the system. If a new SSY is installed notification for each channel it provides is
        * reveived.
        *
        * @since S60 5.0
        * @param[in] aChannelListener Pointer to channel listener instance.
        * @param[in] aSearchParameters Search parameters.
        */
        void SetChannelListenerL( MSensrvChannelListener* aChannelListener,
                                  const TSensrvChannelInfo& aSearchParameters );

    public:  // From MSensrvChannelChangeHandlerObserver
    
        /**
        * Indicates channel availability change caused by installation or removal
        * of new software. 
        *
        * @since S60 5.0
        * @param[in] aDetectedChannel Channel information of the channel.
        * @param[in] aChangeType If ESensrvChannelAdded, channel is a newly available
        *                        channel. 
        *                        If ESensrvChannelRemoved, channel was removed and is  
        *                        no longer available.
        */
        virtual void ChannelChangeDetected( const TSensrvChannelInfo& aDetectedChannel,
                                            TSensrvChannelChangeType aChangeType  );

        /**
        * Channel change listening failed.
        * If error was fatal, sensor server session has also been terminated, and
        * related finder is no longer usable.
        * If error was minor, some notifications have potentially been lost.
        *
        * @since S60 5.0
        * @param[in] aError Error code.
        */
        virtual void ChannelDetectionError( TSensrvErrorSeverity aError );    
                 
    private:
     
        /**
        * C++ constructor
        *
        * @since S60 5.0
        */
         CSensrvChannelFinderImpl();
     
        /**
        * 2nd phase of construction
        *
        * @since S60 5.0
        */
        void ConstructL();
         
    private:
    
        /**
        * Sensor server client
        */
        RSensrvClient iSensrvClient;

        /**
        * Property handler
        * Own         
        */
        CSensrvChannelChangeHandler* iChannelChangeHandler;

        /**
        * Property listener
        * Not own         
        */
        MSensrvChannelListener* iChannelChangeListener;
        
    };


#endif //SENSRVCHANNELFINDERIMPL_H
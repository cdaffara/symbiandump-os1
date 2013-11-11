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
* Description:  Plugin stub
*
*/


#ifndef SENSORDATACOMPENSATORPLUGINSTUB_H
#define SENSORDATACOMPENSATORPLUGINSTUB_H

// INCLUDES
#include <e32std.h>
#include <e32property.h>
#include <e32cmn.h>
#include <sensrvtypes.h>
#include <sensordatacompensatorplugin.h>
#include <sensordatacompensatorplugintypes.h>

// FORWARD DECLARATIONS
class MSensorDataCompensatorObserver;

// CLASS DECLARATION

/**
 * Class implements sensor data compensator plugin stub.
 *  
 * @lib sensordatacompensatorplugin.lib
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CSensorDataCompensatorPluginStub ) : public CSensorDataCompensatorPlugin
    {
    public:
        // Constructors and destructor
    
        /**
        * Two-phase constructor
        */  
        static CSensorDataCompensatorPluginStub* NewL( 
            TSensrvChannelDataTypeId aDataType,
            TSensorCompensationType aType, 
            MSensorDataCompensatorObserver& aObserver );
    
        /**
        * Destructor. 
        */
        virtual ~CSensorDataCompensatorPluginStub();

    public:
        // From CSensorDataCompensatorPlugin
    
        virtual TInt GetCompensationItemCount();
        virtual TPtrC8 GetCompensationItem( TInt aIndex );

    private:
        /**
         * Default constructor
         */
        CSensorDataCompensatorPluginStub( 
            TSensrvChannelDataTypeId aDataType,
            TSensorCompensationType aType,
            MSensorDataCompensatorObserver& aObserver );
    
        /**
         * 2nd phase of construction
         */
        void ConstructL();

        NONSHARABLE_CLASS(CPsWatcher) : public CActive
            {
        public:
            static CPsWatcher* NewL(CSensorDataCompensatorPluginStub& aObserver,const TUid& aUid,TInt aKey);
            ~CPsWatcher();
            inline TInt Key() const {return iKey;}
            inline void Get(TInt& aValue) { iProperty.Get(aValue); }
            inline void Get(TDes8& aValue) { iProperty.Get(aValue); }
        private:
            CPsWatcher(CSensorDataCompensatorPluginStub& aObserver,const TUid& aUid,TInt aKey);
            void ConstructL();
            void DoCancel();
            void RunL();
        private:
            CSensorDataCompensatorPluginStub& iObserver;
            RProperty iProperty;
            TUid iUid;
            TInt iKey;
            };

        void PropertyChangedL( CPsWatcher& aWatcher );

    private:        
        // Requested data type
        TSensrvChannelDataTypeId iDataType;
        
        // Requested compensation type
        TSensorCompensationType iType;

        // Observer
        MSensorDataCompensatorObserver& iObserver;

        RPointerArray<HBufC8> iDataArray;

        CPsWatcher* iPsWatcherDataAppend;

        CPsWatcher* iPsWatcherDataReset;

        CPsWatcher* iPsWatcherReturnErrorOnDataUpdate;

        TInt iErrorOnUpdate;
    };

#endif // SENSORDATACOMPENSATORPLUGINSTUB_H

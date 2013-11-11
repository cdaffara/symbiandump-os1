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
* Description:  Mediates control flow from server main thread to plugin thread
*
*/


#ifndef SENSRVSSYMEDIATOR_H
#define SENSRVSSYMEDIATOR_H

#include <e32base.h>
#include "sensrvmediatorbase.h"
#include "ssycallback.h"
#include "sensrvresourcechannelinfo.h"
#include "sensrvgeneralproperties.h"

class CSensrvTransaction;
class CSsyControl;
class CSensrvChannel;
class CSensrvChannelListener;

/**
 * Mediates control flow from server main thread to plugin thread
 *
 * @since S60 5.0
 */
class CSensrvSsyMediator : public CSensrvMediatorBase,
                           public MSsyCallback
    {
    public:

        /**
        * Two phase constructor
        *
        * @since S60 5.0
        * @param aProxy The proxy controlling related plugin.
        * @return New CSensrvSsyMediator instance
        * @exception KErrNoMemory Out of memory
        */
        static CSensrvSsyMediator* NewL(CSensrvPluginProxy* aProxy);

        /**
        * Destructor.
        */
        virtual ~CSensrvSsyMediator();

        /**
         * Used to destruct object without calling destructor.
         */
        void Destruct();

        /**
        * Notifies this SSY mediator that cleanup is needed.
        * Since deletion must of SSY mediator must be done in SSY thread
        * to avoid possible complications with various SSY implementations,
        * proxy will not delete the SSY mediator directly, but use this
        * method instead.
        *
        * @since S60 5.0
        */
        void Cleanup();

        /**
        * Get raw channel infos pointer.
        *
        * @since S60 5.0
        * @return Pointer to data block containing count and raw data of channel infos.
        */
        inline TAny** RawInfos() {return &iRawInfos;}

        /**
        * Get ETransTypeMediatorChannelsChanged transaction pointer
        *
        * @since S60 5.0
        * @return Pointer to ETransTypeMediatorChannelsChanged transaction or NULL.
        */
        inline CSensrvTransaction* ChannelChangeTransaction() { return iChannelChangeTransaction; };

        /**
        * Get all the affected channels
        *
        * @since S60 5.0
        */
        void AffectedChannels( TSensrvChannelId aChannelId, MSsyPropertyProvider* aSensorPropertyProvider, RSensrvChannelList* aAffectedChannels );

        /**
        * Get all the availability property affected channels
        *
        * @since S60 5.0
        */
        void AvailabilityAffectedChannels( TSensrvChannelId aChannelId, MSsyPropertyProvider* aSensorPropertyProvider, RSensrvChannelList* aAvailabilityAffectedChannels, TSensrvAvailability aValue );

    public: // Inherited from base classes

        // MSsyCallback methods.
        // All called from plugin thread only.
        void ChannelOpened( TSensrvChannelId aChannelId,
                                    TInt aErrorCode,
                                    MSsyChannelDataProvider* aChannelDataProvider,
                                    MSsyPropertyProvider* aSensorPropertyProvider);
        void ChannelClosed( TSensrvChannelId aChannelId);
        void BufferFilled( TSensrvChannelId aChannelId, TInt aWriteCount, TUint8*& aNewBuffer, TInt &aNewCount);
        void PropertyChanged( const TSensrvChannelId aChannelId,
                                  const RSensrvChannelList& aAffectedChannels,
                                  const TSensrvProperty& aProperty );
        void RegisterChannelsL( RSensrvChannelInfoList& aChannelInfoList);
    	void GetCallbackInterfaceL( TUid aInterfaceUid, TAny*& aInterface );

    // CSensrvMediatorBase overrides

        /**
        * Initializes the mediator.
        * Must be called from thread this mediator mediates.
        * If child class overloads this, base implementation must
        * be called also.
        *
        * @since S60 5.0
        */
        TInt Initialize();

    protected: // From base classes

        /**
        * Handles transaction. Handling depends on mediator type,
        * so this must be implemented in inheriting class.
        *
        * @since S60 5.0
        * @param aTransaction The transaction to be handled.
        */
        void HandleTransaction(CSensrvTransaction* aTransaction);

        /**
        * Handles notify failure
        *
        * @since S60 5.0
        */
        void HandleNotifyFailure();

    private:

        /**
        * C++ constructor
        *
        * @since S60 5.0
        */
        CSensrvSsyMediator();

        /**
        * 2nd phase of construction
        *
        * @since S60 5.0
        */
        void ConstructL();

        /**
        * Checks that the client has enough priority to change
        * the value of the property.
        *
        * If there are no affected channels for this property,
        * then check is always passed. If there are affected channels,
        * then check is passed if the priority of the client is highest
        * among the all clients of affected channels.
        *
        * @since S60 5.0
        * @param[in] aChannel Channel where the listener belongs to.
        * @param[in] aListener Listener of the process
        *            which is setting the property value.
        * @param[in] aAffectedChannels Return list of the channel which will be
        *            affected if the property value will be set.
        * @return ETrue if policy check is passed, otherwise EFalse.
        */
        TBool PolicyCheck( CSensrvChannel& aChannel,
                           CSensrvChannelListener& aListener,
                           const RSensrvChannelList& aAffectedChannels );

        /**
        * Gets all affected channels.
        *
        * @since S60 5.0
        * @param aChannelId The channel identifier
        * @param[in] aSensorPropertyProvider The property provider.
        */
        void GetAffectedChannels( const TSensrvChannelId& aChannelId, MSsyPropertyProvider* aSensorPropertyProvider, RSensrvChannelList* aAffectedChannels );

        /**
        * Gets all availability affected channels.
        *
        * @since S60 5.0
        * @param aChannelId The channel identifier
        * @param[in] aSensorPropertyProvider The property provider.
        */
		void GetAvailabilityAffectedChannels( const TSensrvChannelId& aChannelId, MSsyPropertyProvider* aSensorPropertyProvider, RSensrvChannelList* aAvailabilityAffectedChannels, const TSensrvAvailability& aValue );

    private: // Member variables

        /**
        * SSY plugin control interface.
        * Own. Note that this is allocated in SSY thread heap, instead
        * of server heap like SSY mediator.
        */
        CSsyControl* iSsyControl;

        /**
        * Indicates that unloading is requested.
        */
        TBool iUnloading;

        /**
        * Raw channel infos count and data.
        * Allocated always on server main heap.
        * Own.
        */
        TAny* iRawInfos;

        /**
        * Transaction for notifying channel registration changes.
        * It is stored and reused to  ensure it always
        * exists, as the transaction is initiated by SSY.
        * Own.
        */
        CSensrvTransaction* iChannelChangeTransaction;

    };



#endif // SENSRVSSYMEDIATOR_H

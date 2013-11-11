// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Declares the DVB-H tuner hardware adaptation information classes.
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef DVBHRECEIVERINFO_H
#define DVBHRECEIVERINFO_H

#include <e32base.h>
#include <mobiletv/hai/dvbh/dvbhtypes.h>

class TIp6Addr;

/**
* @publishedPartner
* @prototype
* @see CDvbhReceiverInfo::SetStateObserver()
*
* Defines an interface for receiving notifications of RDvbhReceiver state changes.
* Clients wishing to monitor state changes should provide a CDvbhReceiverInfo
* instance with an implementation of this interface via CDvbhReceiverInfo::SetStateObserver().
*/
class MDvbhStateObserver
    {
public:
    /**
    * @param aNewState Identifies the state to which the RDvbhReceiver has just transitioned. 
    *
    * When a client has registered an implementation with a CDvbhReceiverInfo instance, 
    * this method will be called whenever RDvbhReceiver changes state, in order to 
    * inform the client of the new state.
    */
    virtual void DvbhStateChange( TDvbhState aNewState ) = 0;
    };

/**
* @publishedPartner
* @prototype
* @see CDvbhReceiverInfo::SetSignalQualityObserver()
*
* Defines an interface for receiving notifications of changes to signal quality.
* Clients wishing to monitor signal quality should provide a CDvbhReceiverInfo
* instance with an implementation of this interface via CDvbhReceiverInfo::SetSignalQualityObserver().
*/
class MDvbhSignalQualityObserver
    {
public:
    /**
    * @param aNewSignalQuality Identifies the new signal quality value. 
    *
    * When a client has registered an implementation with a CDvbhReceiverInfo instance, 
    * this method will be called whenever the signal quality changes, in order to 
    * inform the client of the new signal quality.
    */    
    virtual void DvbhSignalQualityChange( const TDvbhSignalQuality& aNewSignalQuality ) = 0;    
    };

/**
* @publishedPartner
* @prototype
* @see CDvbhReceiverInfo::SetPlatformObserver()
*
* Defines an interface for receiving notifications of changes of the current IP platform.
* Clients wishing to monitor IP platform changes should provide a CDvbhReceiverInfo
* instance with an implementation of this interface via CDvbhReceiverInfo::SetPlatformObserver().
*/
class MDvbhPlatformObserver
    {
public:
    /**
    * @param aNewPlatform Identifies the IP platform to which the receiver has just switched. 
	* @param aESGRoot The IP address of the bootstrap ESG sevice on the new platform.
    *
    * When a client has registered an implementation with a CDvbhReceiverInfo instance, 
    * this method will be called whenever the current IP platform changes, in order to 
    * inform the client of the new IP platform and bootstrap ESG IP address.
    */    
    virtual void DvbhPlatformChange( const TDvbhPlatform& aNewPlatform, const TIp6Addr& aESGRoot ) = 0;
    };

/**
* @publishedPartner
* @prototype
* @see CDvbhReceiverInfo::SetNetworkTimeObserver()
*
* Defines an interface for notifying of updates to the network time.
* Clients wishing to monitor network time changes should provide a CDvbhReceiverInfo
* instance with an implementation of this interface via CDvbhReceiverInfo::SetNetworkTimeObserver().
* Network time update is triggered via call to RDvbhReceiver::UpdateNetworkTime()
*/
class MDvbhNetworkTimeObserver
    {
public:
    /**
    * When a client has registered an implementation with a CDvbhReceiverInfo instance, 
    * this method will be called whenever the current network time has been updated.
    */  
    virtual void DvbhNetworkTimeUpdate() = 0;
    };

/**
* @publishedPartner
* @prototype
* @see CDvbhReceiverInfo::SetFrequencyObserver()
*
* Defines an interface for receiving notifications of changes to the frequency to which the receiver is tuned.
* Clients wishing to monitor frequency changes should provide a CDvbhReceiverInfo
* instance with an implementation of this interface via CDvbhReceiverInfo::SetFrequencyObserver().
*/
class MDvbhFrequencyObserver
    {
public:
    /**
    * @param aNewFrequency Identifies the frequency to which the tuner has just tuned.
    *
    * When a client has registered an implementation with a CDvbhReceiverInfo instance, 
    * this method will be called whenever the tuned frequency changes, in order to 
    * inform the client of the new frequency.
    */ 
    virtual void DvbhFrequencyChange( const TDvbhFrequency& aNewFrequency ) = 0;    
    };

/**
* @publishedPartner
* @prototype
* @see CDvbhReceiverInfo::SetCellIdObserver()
*
* Defines an interface for receiving notifications of changes to current cellId.
* Clients wishing to monitor cellId changes should provide a CDvbhReceiverInfo
* instance with an implementation of this interface via CDvbhReceiverInfo::SetCellIdObserver().
*/
class MDvbhCellIdObserver
    {
public:
    /**
    * @param aNewCellId The new cellId.
    *
    * When a client has registered an implementation with a CDvbhReceiverInfo instance, 
    * this method will be called whenever the current cellId changes, in order to 
    * inform the client of the new cellId.
    */   
    virtual void DvbhCellIdChange( const TDvbhCellId& aNewCellId ) = 0;    
    };

/**
* @publishedPartner
* @prototype
* @see CDvbhReceiverInfo::SetNetworkIdObserver()
*
* Defines an interface for receiving notifications of changes to current networkId.
* Clients wishing to monitor networkId changes should provide a CDvbhReceiverInfo
* instance with an implementation of this interface via CDvbhReceiverInfo::SetNetworkIdObserver().
*/
class MDvbhNetworkIdObserver
    {
public:
    /**
    * @param aNewNetworkId The ID of the network to which the receiver is now tuned.
    *
    * When a client has registered an implementation with a CDvbhReceiverInfo instance, 
    * this method will be called whenever the ID of the network to which it is tuned changes,
    * in order to inform the client of the new networkId.
    */   
    virtual void DvbhNetworkIdChange( const TDvbhNetworkId& aNewNetworkId ) = 0;    
    };


/**
* @publishedPartner
* @prototype
* @see CDvbhReceiverInfo::SetBatteryStateObserver()
*
* Defines an interface for receiving notifications of changes in external DVB-H receiver battery state.
* Clients wishing to monitor battery changes should provide a CDvbhReceiverInfo
* instance with an implementation of this interface via CDvbhReceiverInfo::SetBatteryStateObserver().
*/
class MDvbhExtBatteryStateObserver
    {
    public:
        /**
        * @param aNewState The new battery state of external receiver.
        *
        * When a client has registered an implementation with a CDvbhReceiverInfo instance, 
        * this method will be called whenever the battery state of external receiver is changed,
        * in order to inform the client of the new aNewState.
        */   
        virtual void DvbhExtBatteryStateChange( TDvbhExtBatteryState aNewState ) = 0;
    };

/**
* @publishedPartner
* @prototype
* @see CDvbhReceiverInfo::SetExtConnectionStateObserver()
*
* Defines an interface for receiving notifications of changes in external DVB-H receiver connection
* Clients wishing to monitor connection changes should provide a CDvbhReceiverInfo
* instance with an implementation of this interface via CDvbhReceiverInfo::SetExtConnectionStateObserver().
*/
class MDvbhExtConnectionObserver
    {
    public:

        /**
        * @param aNewState The new external receiver connection state.
        * @param aReceiverType Type of the receiver
        *
        * When a client has registered an implementation with a CDvbhReceiverInfo instance, 
        * this method will be called whenever the connection state of external receiver is changed,
        * in order to inform the client of the new aNewState.
        */   
        virtual void DvbhExtConnectionStateChange( 
            const TDvbhExtConnectionState& aNewState, const TDvbhReceiverType& aReceiverType ) = 0;
    };

    
/**
* @publishedPartner
* @prototype
* @see CDvbhReceiverInfo::SetExtAntennaConnectionStateObserver()
*
* Defines an interface for receiving notifications of changes in external DVB-H receiver antenna connection.
* Clients wishing to monitor connection changes should provide a CDvbhReceiverInfo
* instance with an implementation of this interface via CDvbhReceiverInfo::SetExtAntennaConnectionStateObserver().
*/
class MDvbhExtAntennaConnectionObserver
    {
    public:

        /**
        * @param aNewState The new external antenna connection state.
        * @param aReceiverType Type of the receiver
        *
        * When a client has registered an implementation with a CDvbhReceiverInfo instance, 
        * this method will be called whenever the connection state of external receiver antenna is changed,
        * in order to inform the client of the new aNewState.
        */   
        virtual void DvbhExtAntennaConnectionStateChange( 
            const TDvbhExtAntennaConnectionState& aNewState, const TDvbhReceiverType& aReceiverType ) = 0;
    };


/**
* @publishedPartner
* @prototype
*
* Provides operations for reading and tracking information about the receiver, such
* as state, signal quality, frequency, etc.  Refer to the individual class methods
* for the full list.
* 
* Use of this class is safe in the sense that it will not interfere with the
* receiver operation in any way. There can be multiple simultaneous instances
* of CDvbhReceiverInfo at one time.
*/
class CDvbhReceiverInfo : public CBase
    {
public:
    /**
	* @return A new instance of CDvbhReceiverInfo.
	*
    * Factory function for creating a CDvbhReceiverInfo object. 
    */        
    IMPORT_C static CDvbhReceiverInfo* NewL();
    
    /**
	* @return A new instance of CDvbhReceiverInfo.
	*
    * Factory function for creating a CDvbhReceiverInfo object, with
    * the new object left on the CleanupStack. 
    */   
    IMPORT_C static CDvbhReceiverInfo* NewLC();
    
    /**
    * C++ destructor.
    */        
    IMPORT_C ~CDvbhReceiverInfo();

    /**
	* @param aState Updated with the current state of the receiver.
	* @return KErrNone on success, otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the current state of the receiver.  
    */        
    IMPORT_C static TInt GetState( TDvbhState& aState );
    
    /**
	* @param aObserver An instance of an MDvbhStateObserver implementation.
	* @return KErrNone on success, otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see MDvbhStateObserver
	*
    * Used to register an MDvbhStateObserver implementation with the object.  Once a client
    * has registered such an observer, it will be notified of any changes in receiver
    * state via that observer until either a new observer is registered or this object is
    * destroyed.
    *
    * If called more than once on an instance, the most recent observer registered 
    * overrides any previously registered observers.
    */          
    IMPORT_C TInt SetStateObserver( MDvbhStateObserver& aObserver );
            
    /**
	* @param aSignalQuality Updated with the current signal quality value.
	* @return KErrNone on success, KErrNotReady if receiver is not in the Receiving state, or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the current signal quality value.  
    */  
    IMPORT_C static TInt GetSignalQuality( TDvbhSignalQuality& aSignalQuality );
    
    /**
	* @param aObserver An instance of an MDvbhSignalQualityObserver implementation.
	* @return KErrNone on success, otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see MDvbhSignalQualityObserver
	*
    * Used to register an MDvbhSignalQualityObserver implementation with the object.  Once a client
    * has registered such an observer, it will be notified of any changes to signal
    * quality via that observer until either a new observer is registered or this object is
    * destroyed.
    *
    * If called more than once on an instance, the most recent observer registered 
    * overrides any previously registered observers.
    */           
    IMPORT_C TInt SetSignalQualityObserver( MDvbhSignalQualityObserver& aObserver );
    
    /**
	* @param aPlatform Updated with the currently active IP platform.
	* @param aESGRoot Updated with the IP address of the bootstrap ESG service on the platform.
	* @return KErrNone on success, KErrNotReady if platform has not been set on the receiver, or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the currently active IP platform and IP address of its bootstrap ESG service.  
    */          
    IMPORT_C static TInt GetPlatform( TDvbhPlatform& aPlatform, TIp6Addr& aESGRoot );
  
    /**
	* @param aObserver An instance of an MDvbhPlatformObserver implementation.
	* @return KErrNone on success, otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see MDvbhPlatformObserver
	*
    * Used to register an MDvbhPlatformObserver implementation with the object.  Once a client
    * has registered such an observer, it will be notified of any changes to IP platform
    * via that observer until either a new observer is registered or this object is
    * destroyed.
    *
    * If called more than once on an instance, the most recent observer registered 
    * overrides any previously registered observers.
    */                                    
    IMPORT_C TInt SetPlatformObserver( MDvbhPlatformObserver& aObserver );

    /**
	* @param aNetworkTime Upated with the current network time.
	* @param aValid Updated with ETrue if the receiver is in Ready or Receiving state; or EFalse if the time offset of a previous platform is available and has been used as a fallback to calculate the given time.
	* @return KErrNone on success, KErrNotReady if no platform has ever been set on the receiver, or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see RDvbhReceiver::UpdateNetworkTime()
	*
    * Retrieves the current network time by calculating it from the offset obtained 
    * after the last call to RDvbhReceiver::UpdateNetworkTime().
    */         
    IMPORT_C static TInt GetNetworkTime( TTime& aNetworkTime, TBool& aValid );
    
    /**
	* @param aObserver An instance of an MDvbhNetworkTimeObserver implementation.
	* @return KErrNone on success, otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see MDvbhNetworkTimeObserver
	*
    * Used to register an MDvbhNetworkTimeObserver implementation with the object.  Once a client
    * has registered such an observer, it will be notified whenever the network time has been
    * updated via that observer until either a new observer is registered or this object is
    * destroyed.
    *
    * If called more than once on an instance, the most recent observer registered 
    * overrides any previously registered observers.
    */    
    IMPORT_C TInt SetNetworkTimeObserver( MDvbhNetworkTimeObserver& aObserver );
            
    /**
	* @param aPerformanceData Updated with the current usage and performance data of the receiver.
	* @return KErrNone on success, KErrNotReady if the receiver is not in Receiving state, or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the current performance and usage data from the receiver. 
    */ 
    IMPORT_C static TInt GetPerformanceData( TDvbhPerformanceData& aPerformanceData );
            
    /**
	* @param aFrequency Updated with the frequency to which the receiver is currently tuned.
	* @return KErrNone on success, KErrNotReady if platform has not been set on the receiver, or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the frequency to which the receiver is currently tuned. 
    */
    IMPORT_C static TInt GetFrequency( TDvbhFrequency& aFrequency );

    /**
	* @param aObserver An instance of an MDvbhFrequencyObserver implementation.
	* @return KErrNone on success, otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see MDvbhFrequencyObserver
	*
    * Used to register an MDvbhFrequencyObserver implementation with the object.  Once a client
    * has registered such an observer, it will be notified whenever the frequency to which
    * the receiver is tuned changes via that observer until either a new observer is 
    * registered or this object is destroyed.
    *
    * If called more than once on an instance, the most recent observer registered 
    * overrides any previously registered observers.
    */         
    IMPORT_C TInt SetFrequencyObserver( MDvbhFrequencyObserver& aObserver );

    /**
	* @param aCellId Updated with the current cellId.
	* @return KErrNone on success, KErrNotReady if platform has not been set on the receiver, or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the current cellId. 
    */
    IMPORT_C static TInt GetCellId( TDvbhCellId& aCellId );

    /**
	* @param aObserver An instance of an MDvbhCellIdObserver implementation.
	* @return KErrNone on success, otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see MDvbhCellIdObserver
	*
    * Used to register an MDvbhCellIdObserver implementation with the object.  Once a client
    * has registered such an observer, it will be notified whenever the current cellId changes 
    * via that observer until either a new observer is registered or the object is destroyed.
    *
    * If called more than once on an instance, the most recent observer registered 
    * overrides any previously registered observers.
    */           
    IMPORT_C TInt SetCellIdObserver( MDvbhCellIdObserver& aObserver );

    /**
	* @param aNetworkId Updated with the ID of the network to which the receiver is currently tuned.
	* @return KErrNone on success, KErrNotReady if platform has not been set on the receiver, or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the networkId of the network to which the receiver is currently tuned. 
    */
    IMPORT_C static TInt GetNetworkId( TDvbhNetworkId& aNetworkId );
    
    /**
	* @param aObserver An instance of an MDvbhNetworkIdObserver implementation.
	* @return KErrNone on success, otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see MDvbhNetworkIdObserver
	*
    * Used to register an MDvbhNetworkIdObserver implementation with the object.  Once a client
    * has registered such an observer, it will be notified whenever the ID of the network to which
    * the receiver is tuned changes via that observer until either a new observer is 
    * registered or this object is destroyed.
    *
    * If called more than once on an instance, the most recent observer registered 
    * overrides any previously registered observers.
    */        
    IMPORT_C TInt SetNetworkIdObserver( MDvbhNetworkIdObserver& aObserver );

    /**
	* @param aState Updated with the battery state of external receiver
	* @return KErrNone on success, or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the battery state of external receiver
    */
    IMPORT_C static TInt GetBatteryState( TDvbhExtBatteryState& aState );
        
    /**
	* @param aObserver An instance of an MDvbhExtBatteryStateObserver implementation.
	* @return KErrNone on success, otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see MDvbhExtBatteryStateObserver
	*
    * Used to register an MDvbhExtBatteryStateObserver implementation with the object.  Once a client
    * has registered such an observer, it will be notified whenever the battery state of external receiver
    * has changed via that observer until either a new observer is 
    * registered or this object is destroyed.
    *
    * If called more than once on an instance, the most recent observer registered 
    * overrides any previously registered observers.
    */        
    IMPORT_C TInt SetBatteryStateObserver( MDvbhExtBatteryStateObserver& aObserver );
                
    /**
	* @param aConnectionState Updated with the connection state of external receiver
	* @param aReceiver Input for which receiver type the connection state is queried
	* @return KErrNone on success, or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the connection state of external receiver
    */
    IMPORT_C static TInt GetExtConnectionState( TDvbhExtConnectionState& aConnectionState, const TDvbhReceiverType& aReceiver );
        
    /**
	* @param aObserver An instance of an MDvbhExtConnectionObserver implementation.
	* @return KErrNone on success, otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see MDvbhExtConnectionObserver
	*
    * Used to register an MDvbhExtConnectionObserver implementation with the object.  Once a client
    * has registered such an observer, it will be notified whenever the connection state of external
    * receiver has changed via that observer until either a new observer is 
    * registered or this object is destroyed.
    *
    * If called more than once on an instance, the most recent observer registered 
    * overrides any previously registered observers.
    */        
    IMPORT_C TInt SetExtConnectionStateObserver( MDvbhExtConnectionObserver& aObserver );

    /**
	* @param aConnectionState Updated with the antenna connection state of external receiver
	* @param aReceiver Input for which receiver type the connection state is queried
	* @return KErrNone on success, or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the antenna connection state of external receiver.
    */
    IMPORT_C static TInt GetExtAntennaConnectionState( TDvbhExtAntennaConnectionState& aConnectionState, const TDvbhReceiverType& aReceiver );
        
    /**
	* @param aObserver An instance of an MDvbhExtAntennaConnectionObserver implementation.
	* @return KErrNone on success, otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see MDvbhExtAntennaConnectionObserver
	*
    * Used to register an MDvbhExtAntennaConnectionObserver implementation with the object.  Once a client
    * has registered such an observer, it will be notified whenever the antenna connection state of external
    * receiver has changed via that observer until either a new observer is 
    * registered or this object is destroyed.
    *
    * If called more than once on an instance, the most recent observer registered 
    * overrides any previously registered observers.
    */        
    IMPORT_C TInt SetExtAntennaConnectionStateObserver( MDvbhExtAntennaConnectionObserver& aObserver );

    /**
	* @param aDeviceInfo Updated with the device information of external receiver
	* @param aReceiver Input for which receiver type the device information is queried
	* @return KErrNone on success, KErrNotSupported if receiver type is not supported, or another system-wide error code.  
	* Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the device information of external receiver. Please note that the external receiver must be connected to get valid 
    * device information. 
    */
    IMPORT_C static TInt GetExtDeviceInfo( TDvbhAccessoryInfo& aDeviceInfo, const TDvbhReceiverType& aReceiver );

private:
    /**
    * C++ constructor.
    */
    CDvbhReceiverInfo();
    
    /**
    * Symbian 2nd phase constructor.
    */
    void ConstructL();

private:    
    /*
    * Implementers of this interface should declare their own CPrivateData
    * class containing whatever private data is needed for their implementation.
    */
	class CPrivateData;
	CPrivateData* iData;
    };

#endif // DVBHRECEIVERINFO_H

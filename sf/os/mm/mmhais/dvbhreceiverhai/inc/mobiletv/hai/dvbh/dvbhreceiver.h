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
// Declares the DVB-H tuner hardware adaptation interface classes.
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef DVBHRECEIVER_H
#define DVBHRECEIVER_H

#include <mobiletv/hai/dvbh/dvbhtypes.h>

class TIp6Addr;

/**
* @publishedPartner
* @prototype
* @see RDvbhReceiver::Scan()
*
* Defines an interface for receiving events produced by a receiver's scan operation.
* Clients must provide an RDvbhReceiver instance with an implementation 
* of this interface in order to perform a signal scan.
*/
class MDvbhScanObserver
    {
public:
    /**
	* @param aResult An intermediate scan result for a frequency.
	*
    * After a client has initiated an RDvbhReceiver signal scan, this method will be 
    * called to report intermediate results, whenever the scan has finished processing
    * a frequency.  Results are reported until the frequency range has been
    * scanned. 
    *
    * More than one result may be reported back for a single frequency.  For example
    * if two platforms were found on one frequency, then two results will 
    * be reported for this frequency, one for each platform.       
    */
    virtual void DvbhScanProgress( const TDvbhScanResult& aResult ) = 0;
    };

/**
* @publishedPartner
* @prototype
* @see RDvbhReceiver::ReceiveIPData()
*
* Defines an interface for receiving IP data packets captured from the DVB-H broadcast.
* Clients may provide an RDvbhReceiver instance with an implementation 
* of this interface in order to receive DVB-H IP data.
*/
class MDvbhDataObserver
    {
public:
    /**
	* @param aPacket A descriptor containing the raw IP packet data, as received by the receiver hardware.
	*
    * If a client has registered an MDvbhDataObserver implementation with an RDvbhReceiver instance, 
    * then this method will be called to report IP packets back to the client, whenever a packet requested
    * by the client has been received.
    * 
    * Note: the packets may sometimes come in non-sequential order and there 
    * is no guarantee that all broadcasted IP packets are ever received.
    *
    * Note: aPacket should not be assumed valid beyond the life of this method.
    */                        
    virtual void DvbhPacketReceived( const TDesC8& aPacket ) = 0;
    };

/**
* @publishedPartner
* @prototype
*
* A generic interface to DVB-H receiver drivers/hardware.
* It provides a hardware control and data reception interface for user-space clients that
* need to operate the receiver hardware. There can be multiple simultaneous sessions 
* open at one time.
*/
class RDvbhReceiver
    {
public:
    /**
    * C++ constructor.
    */
    IMPORT_C RDvbhReceiver();
        
    /**
	* @param aVersion Updated with the version number of the driver.
	* @return KErrNone on success, otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the version number of the driver.
    */    
    IMPORT_C TInt GetDriverVersion( TVersion& aVersion );
        
    /**
	* @param aReceiverTypes Array of supported receiver types by this adaptation
    * @return KErrNone on success, otherwise a system-wide error code.
	*
    * Gets supported receiver types.
    */    
    IMPORT_C TInt GetSupportedReceiverTypes( RArray<TDvbhReceiverType>& aReceiverTypes );

    /**
	* @param aReceiverType Receiver type to be used.
    * @return KErrNone on success, KErrInUse if other receiver type was selected.
    * otherwise a system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Opens a session to the receiver hardware/drivers. This must be done before 
    * calling all other API methods except GetDriverVersion() and GetSupportedReceiverTypes().     
    * Only opening same type of receiver types is allowed. i.e. it is not allowed to first open as integrated receiver and then open usb type of receiver.
    * If user(s) wish to switch between different receiver types, one must first close all open sessions to the driver and then open with new receiver type.
    */    
    IMPORT_C TInt Open( const TDvbhReceiverType aReceiverType );

    /**
    * Closes a session to the receiver hardware/driver. If a session was open before this
    * method was called, then the reference count for keeping power to the hardware will be
    * decremented, like a call to RDvbhReceiver::PowerOff(), in case RDvbhReceiver::PowerOn()
    * has been called via the same RDvbhReceiver instance.  As a result, if there are 
    * no more open sessions to the receiver hardware/drivers after this method has been
    * called, then the receiver will be powered off.
    */    
    IMPORT_C void Close();

    /**
	* @pre Session has been previously opened with RDvbhReceiver::Open()
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Asynchronous method to initialize the power-up sequence on the receiver hardware, if it is not already powered on. A binary power reference count is maintained for each RDvbhReceiver instance.  
    * Powering on may take a longer time to complete in case of external receiver (e.g., Bluetooth receiver) since the connection with receiver is established during power-on sequence. 
    * Successful power-up sequence state transitions: EDvbhStateInactive => EDvbhStateBooting => EDvbhStateNoPlatform. 
    * Should the power-up sequence fails for some reason (e.g., no Bluetooth receiver available, USB receiver not connected) the receiver state goes to EDvbhStateNoHardware / EDvbhStateError when trying to power on.
	*
    * Completes with:
    * 			KErrNone: When the operation completed successfully;
	* 			KErrCancel: Power on cancelled; 
	* 			KErrGeneral: Error from the receiver;
	*           KErrLocked: receiver has been disabled with RDvbhReceiver::SetDisabled(); 
	* 			KErrHardwareNotAvailable: external receiver was not available (e.g. USB receiver was not connected, Bluetooth receiver connection failed); 
	*           or another system-wide error code.  Clients should accept completion with an unknown error code gracefully since new error code may be returned in the future.
    */
    IMPORT_C TInt PowerOn( TRequestStatus& aStatus );

    /**
    * Cancels ongoing Power On operation
    */    
    IMPORT_C void CancelPowerOn();

    /**
	* @post Either the receiver hardware is powered off, or the reference count for keeping power on is reduced.
	*
    * Asynchronous method to power off the receiver hardware, if it is not already powered off.
    *
    * Completes with:
    * 			KErrNone: When the operation completed successfully;
	* 			KErrCancel: Power off is cancelled; 
    *           or another system-wide error code.  Clients should accept completion with an unknown error code gracefully since new error code may be returned in the future.
    */    
    IMPORT_C void PowerOff( TRequestStatus& aStatus );

    /**
    * Cancels ongoing Power Off operation.
    */    
    IMPORT_C void CancelPowerOff();
    
    /**
    * @param  aDisable ETrue to request a transition to the Disabled state; EFalse to request a transition out of the Disabled state.
    *
    * Asynchronous method to force the receiver power-off despite there may be other sessions.
    * If called with ETrue, then receiver power will be forced off (if not off already) and 
    * will be kept off despite subsequent RDvbhReceiver::PowerOn() calls. If called with EFalse, 
    * then a previous RDvbhReceiver::SetDisabled(ETrue) will be cleared and if it was the last one; 
    * power can again be turned on by a call to RDvbhReceiver::PowerOn().
    *
    * Completes with:
    * 			KErrNone: When the operation completed successfully;
	* 			KErrCancel: SetDisabled is cancelled;
    *           or another system-wide error code.  Clients should accept completion with an unknown error code gracefully since new error code may be returned in the future.
    */    
    IMPORT_C void SetDisabled( TBool aDisable, TRequestStatus& aStatus );

    /**
    * Cancels ongoing SetDisabled operation. After cancellation receiver can be considered to not have power.
    */    
    IMPORT_C void CancelSetDisabled();

    /**
	* @param aScanConfiguration The new scan configuration. 
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; KErrNotSupported if aScanConfiguration is invalid; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see RDvbhReceiver::Scan()
	*
    * Sets a new scan configuration to be used for subsequent RDvbhReceiver::Scan() operations. 
    */    
    IMPORT_C TInt SetScanConfiguration( const TDvbhScanConfiguration& aScanConfiguration );
	
    /**
	* @param aScanConfiguration Updated with the current scan configuration. 
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @see RDvbhReceiver::Scan()
	*
    * Gets the current scan configuration.
    */	
    IMPORT_C TInt GetScanConfiguration( TDvbhScanConfiguration& aScanConfiguration );
	
    /**
	* @param aVersion Updated with the version number of the physical interface.
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves the version of the physical interface.    
    */	
    IMPORT_C TInt GetDvbhVersion( TVersion& aVersion );
	
    /**
	* @param aHardwareInfo Updated with the hardware information string.
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	*
    * Retrieves a string describing the hardware.  Different entries should be separated with ';'.
    */	
    IMPORT_C TInt GetHardwareInfo( TDvbhHardwareInfo& aHardwareInfo );

    /**
	* @param aObserver An instance of an MDvbhScanObserver implementation.
	* @param aStatus The status of the asynchronous request.
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @pre Session has been previously opened with RDvbhReceiver::Open(), RDvbhReceiver::PowerOn() has been called successfully and the receiver is in the NoPlatform or Ready state.
	* @see MDvbhScanObserver
	*
    * Starts a signal scan to discover the currently available platforms and 
    * networks according to the scan configuration set by RDvbhReceiver::SetScanConfiguration().
    * During the scan, progress reports & scan results will periodically be reported back to the
    * client via the passed in aObserver.
	*
    * Completes with:
    * 			KErrNone: When the operation completed successfully;
    * 			KErrNotReady: When state is not NoPlatform or Ready;
	* 			KErrAlreadyExists: When a Scan already pending;
	* 			KErrCancel: Scan cancelled;
	* 			KErrCorrupt: Corrupted data was received by the receiver;
	* 			KErrGeneral: Error from the receiver;
	* 			KErrInUse: There are active filters when Scan was called;
	* another system-wide error code    Clients should accept completion with unknown error codes gracefully since new error codes may be returned in the future.
    */    
    IMPORT_C TInt Scan(
        	  MDvbhScanObserver& aObserver,
        	  TRequestStatus& aStatus );

    /**
    * Stops a previously started scan operation.  Scan results received up to the point
    * when the scan was cancelled should still be considered valid.    
    */    
    IMPORT_C void CancelScan();

    /**
    * @param aNetwork Identifies the network where the platform belongs
    * @param aPlatform Identifies the new IP platform to set.
    * @param aStatus The request status variable to get the notification of completion.
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @pre Session has been previously opened with RDvbhReceiver::Open(), RDvbhReceiver::PowerOn() has been called successfully and the receiver is in the NoPlatform or Ready state.
    *
    * An asynchronous method to set the IP platform to use for DVB-H reception.
    * If user wishes to let receiver choose network, one should use network ID of -1
    *
    * Completes with:
    *			KErrNone: When the operation completed successfully;    
    *           KErrNotReady: When state is not NoPlatform or Ready;
    *           KErrAlreadyExists: When a SetPlatform is already pending;  
    *           KErrCancel: When SetPlatform was cancelled;
    *           KErrCorrupt: When corrupted data received from the receiver;
    *			KErrInUse: When there were active filters when the method was called;
    *           KErrGeneral: When there was some other error from the receiver;
	* or another system-wide error code. Clients should accept completion with unknown error codes gracefully since new error codes may be returned in the future.
    */    
    IMPORT_C TInt SetPlatform(
        const TDvbhNetwork& aNetwork,
        const TDvbhPlatform& aPlatform,
        TRequestStatus& aStatus );

    /**
    * Cancels an IP Platform set operation.
    */    
    IMPORT_C void CancelSetPlatform();

    /**
    * @param aDestAddress Identifies the IP data channel from which data is to be filtered. IPv4 addresses uses IPv4 mapped format.
    * @param aFilterId Updated with an ID to identify the created filter.
    * @param aStatus Request status variable to get the notification when the filter is removed due to an error or cancellation
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @pre Session has been previously opened with RDvbhReceiver::Open(), RDvbhReceiver::PowerOn() has been called successfully and the receiver is in the Ready or Receiving state.
    *
    * An asynchronous method used to create a new filter for the reception of IP data. User is expected to maintain active object for monitoring filter status.
    * Method is completed when user deletes the filter by calling RDvbhReceiver::CancelFilter, filter creation fails to receiver, or corrupted data has been 
    * received from receiver.
    *       
    * Completes with:
    *           KErrNotReady: When the state is not Ready or Receiving;
    *           KErrNoMemory: When there is not enough memory to perform the request;
    *           KErrGeneral: When the filter creation failed on the receiver or maximum number of filters allowed by the hardware/driver has already been allocated;
    *           KErrCancel: When the filter is cancelled;
    *           KErrCorrupt: When corrupted data is received from the receiver;
	* or another system-wide error code. Clients should accept completion with unknown error codes gracefully since new error codes may be returned in the future.
    */
    IMPORT_C TInt CreateFilter(
        const TIp6Addr& aDestAddress,
        TInt& aFilterId,
        TRequestStatus& aStatus );

    /**
    * @param aFilterId Identifies the filter to cancel, as returned by RDvbhReceiver::CreateFilter().    
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; KErrNotFound if the specified filter does not exist in this session; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.       . 
    * @see RDvbhReceiver::CreateFilter()
    *
    * Cancels a previously created filter.
    *
    */
    IMPORT_C TInt CancelFilter( TInt aFilterId );

    /**
    * @param aObserver An instance of an MDvbhDataObserver implementation.
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; KErrAlreadyExists if another IP data receiver has already been set; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.   
    * @see MDvbhDataObserver
    *
    * Sets an observer for receiving filtered IP data packets. Once set, the observer 
    * will receive IP packets for all active filters from any session.
    */
    IMPORT_C TInt ReceiveIPData( MDvbhDataObserver& aObserver );

    /**
	* @see RDvbhReceiver::ReceiveIPData()
	*
    * Stops the reception of IP data packets on the observer previously set with RDvbhReceiver::ReceiveIPData().
    */        
    IMPORT_C void CancelReceiveIPData();

    /**
    * @param aStatus The request status variable to get the notification of completion.
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
	* @pre Session has been previously opened with RDvbhReceiver::Open(), RDvbhReceiver::PowerOn() has been called successfully and the receiver is in the Ready or Receiving state.
	*   	
    * An ascynchronous method to update the network time. Once complete, network time has been synchronized. Network time itself can be retrieved from 
    * CDvbhReceiverInfo::GetNetworkTime or alternatively registering observer to get network time changes by using CDvbhReceiverInfo::SetNetworkTimeObserver.
    *
    * Completes with:
    *			KErrNone: When the operation completed successfully;
    *           KErrNotReady: When the state is not Ready or Receiving;
    *           KErrAlreadyExists: When an UpdateNetworkTime is already in progress;
    *           KErrCancel: When the UpdateNetworkTime operation has been cancelled;
	* or another system-wide error code. Clients should accept completion with unknown error codes gracefully since new error codes may be returned in the future.
    */
    IMPORT_C TInt UpdateNetworkTime( TRequestStatus& aStatus );

    /**
	* @see RDvbhReceiver::UpdateNetworkTime()
	*
    * Cancels a network time update.
    */    
    IMPORT_C void CancelUpdateNetworkTime();

    /**
    * @param aCommand The command identifier. Implementation (driver/hardware) specific.
    * @param aInputData The input data for the command. Implementation (driver/hardware) specific.
    * @param aOutputBuffer A buffer allocated by the caller to hold the output data. Output data is copied to the descriptor and the descriptor size is set upon successful completion of the command. The descriptor maximum size must be large enough to hold the command output. If it is not, the request is completed with KErrArgument.
    * @param aStatus The request status variable to get the notification of completion.       
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
    *
    * An asynchronous method that sends a custom command specific to driver and/or hardware implementation. 
    * The implementation interprets the message and acts accordingly.
    *
    * Completes with: 
    *			KErrNone: When the operation completed successfully;
    *           KErrArgument: When an illegal command is passed; 
    *           KErrAlreadyExists: When a CustomCommand is already in progress;
    *           KErrNoMemory: When there is not enough memory to perform the request; 
    *           KErrCancel: When the custom command is cancelled;
	* or another system-wide error code. Clients should accept completion with unknown error codes gracefully since new error codes may be returned in the future.
	*
	* Note: aInputData & aOutputBuffer should remain valid while this operation is outstanding, until it either
	* completes or is cancelled.
    */    
    IMPORT_C TInt CustomCommand(
        TInt aCommand,
        const TDesC8& aInputData,
        TDes8& aOutputBuffer,
        TRequestStatus& aStatus );

    /**
    * @param aStatus The request status variable of the operation to be cancelled.
	* @see RDvbhReceiver::CustomCommand()
	*
    * A synchronous method to cancel further processing of the custom command
    * related to the given TRequestStatus.
    */
    IMPORT_C void CancelCustomCommand( TRequestStatus &aStatus );

    /**
    * @param aCommand The command identifier. Implementation (driver/hardware) specific.
    * @param aInputData The input data for the command. Implementation (driver/hardware) specific.
	* @return KErrNone on success; KErrNotReady if RDvbhReceiver::Open() has not been called; or another system-wide error code.  Clients should accept unknown error codes gracefully since new error codes may be returned in the future.
    *	
    * A synchronous method to send a custom command specific to driver and hardware implementation. 
    * The implementation interprets the message and acts accordingly. No response is expected in this variant.
    *
    * Note: aInputData must remain valid for the life of this method.
    */
    IMPORT_C TInt CustomCommand( TInt aCommand, const TDesC8& aInputData );  

private:
    /*
    * Private dummy copy constructor to prevent copying.
    */
    RDvbhReceiver( const RDvbhReceiver& );

    /*
    * Private dummy assignment operator to prevent assignment.
    */    
    RDvbhReceiver& operator=( const RDvbhReceiver& );   

    /*
    * A leaving counterpart to the public Open method.  
    *
    * The intention is that the public Open() method may delegate to a TRAPed
    * OpenL().  OpenL() can perform all the operations required to
    * open the receiver, including possible leaving operations such as
    * the construction of iBody.
    */    
    void OpenL( const TDvbhReceiverType aReceiverType );

private:    	   
    /*
    * The body class to which RDvbhReceiver public methods should be delegated.
    *
    * RBody should be derived from an appropriate RHandleBase dervied class, 
    * such as RBusLogicalChannel or RSessionBase, depending on the implementation
    * requirements.
    */
	class RBody;
	RBody* iBody;
	};

#endif // DVBHRECEIVER_H

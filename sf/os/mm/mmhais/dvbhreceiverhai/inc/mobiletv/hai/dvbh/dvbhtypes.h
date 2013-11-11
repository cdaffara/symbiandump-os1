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
// Common DVB-H types and constants used by RDvbhReceiver & CDvbhReceiverInfo.
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef DVBHTYPES_H
#define DVBHTYPES_H

#ifdef __KERNEL_MODE__
	// Kernel-side
	#include <e32cmn.h>
	typedef TInt64 TTime;
#else
	// User-side
	#include <e32std.h> 
#endif

/**
* @publishedPartner
* @prototype
*
* The state of the receiver adaptation.
*
* This enum may be be extended in the future with the addition of new values.  Users should therefore handle unknown or unexpected values gracefully.
*/
enum TDvbhState
	{
	/**
	* Indicates that no receiver hardware is currently connected to the phone.
	* May transition to EDvbhStateInactive or EDvbhStateDisabled.
    * Transition out of this state happens when e.g. USB receiver is connected to the phone, 
    * or user initiates booting sequence with Bluetooth receiver by calling PowerOn (which tries to connect 
    * to the receiver accessory)
	*/
	EDvbhStateNoHardware,			// -> Inactive or Disabled
	
	/**
	* Indicates that the receiver is inactive.  May transition to EDvbhStateBooting via RDvbhReceiver::PowerOn().
	*/ 
    EDvbhStateInactive,   	 	  	// -> Booting
    
    /** 
    * Indicates that the receiver is booting.  Transitions to EDvbhStateNoPlatform if booting completes successfully.
    */
    EDvbhStateBooting,     		   	// -> NoPlatform
    
    /**
    * Indicates that the receiver has booted, but does not yet have an IP platform set.  May transition to
    * EDvbhStateScanning via RDvbhReceiver::Scan() or EDvbhStateSelectPlatform via RDvbhReceiver::SetPlatform().
    */
    EDvbhStateNoPlatform,   		// -> Scanning (scan) or SelectPlatform (set platform)
    
    /**
    * Indicates that the receiver is currently scanning for DVB-H signals.  Transitions to EDvbhStateNoPlatform once the scan completes successfully.
    */
    EDvbhStateScanning,     		// -> NoPlatform
    
    /**
    * Indicates that the receiver is in the process of selecting an IP platform.  Transitions to EDvbhStateReady if the selection completes successfuly.
    */
    EDvbhStateSelectPlatform, 		// -> Ready
    
    /**
    * Indicates that an IP platform has been set on the receiver.  May transition to EDvbhStateReceiving via RDvbhReceiver::CreateFilter() 
    * or EDvbhStateScanning via RDvbhReceiver::Scan().
    */
    EDvbhStateReady,         		// -> Receiving (set filter) or Scanning (scan)
    
    /**
    * Indicates that at least one filter was created on the receiver.  May transition to EDvbhStateReady once all filters have been removed via successive RDvbhReceiver::CancelFilter() calls.
    */
    EDvbhStateReceiving,      		// -> Ready (remove filter)
    
    /**
    * Indicates that the receiver has been forced to power down via a call to RDvbhReceiver::SetDisabled().  May transition to EDvbhStateInactive or EDvbhStateNoHardware by
    * calling RDvbhReceiver::SetDisabled(EFalse) 
    */
    EDvbhStateDisabled,       		// -> Inactive
    
    /**
    * Indicates that the receiver has encountered a fatal error. In this case receiver has powered itself down. May transition to EDvbhStateBooting 
    * by initiating a new power on sequence by calling RDvbhReceiver::PowerOn
    */
    EDvbhStateError           		// -> Booting
	};

/**
* @publishedPartner
* @prototype
*
* The battery state of external DVB-H receiver.
*
*/
enum TDvbhExtBatteryState
    {
    /**
    * Battery: 0 %
    */
    EDvbhExtBattery_0,
    /**
    * Battery: 10 %
    */
    EDvbhExtBattery_10,
    /**
    * Battery: 20 %
    */
    EDvbhExtBattery_20,
    /**
    * Battery: 30 %
    */
    EDvbhExtBattery_30,
    /**
    * Battery: 40 %
    */
    EDvbhExtBattery_40,
    /**
    * Battery: 50 %
    */
    EDvbhExtBattery_50,
    /**
    * Battery: 60 %
    */
    EDvbhExtBattery_60,
    /**
    * Battery: 70 %
    */
    EDvbhExtBattery_70,
    /**
    * Battery: 80 %
    */
    EDvbhExtBattery_80,
    /**
    * Battery: 90 %
    */
    EDvbhExtBattery_90,
    /**
    * Battery: 100 %
    */
    EDvbhExtBattery_100,
    /**
    * Charger is connected to external receiver
    */
    EDvbhExtBatteryCharging
    };

/**
* @publishedPartner
* @prototype
*
* The connection state of External DVB-H receiver.
*
*/
enum TDvbhExtConnectionState
    {
    /**
    * Accessory is not connected
    */
	EDvbhExtNotConnected,
    /**
    * Accessory is connected
    */
	EDvbhExtConnected
    };


/**
* @publishedPartner
* @prototype
*
* The antenna connection state of external DVB-H receiver.
*
*/
enum TDvbhExtAntennaConnectionState
    {
    /**
    * Accessory antenna is not connected
    */
	EDvbhExtAntennaNotConnected,
    /**
    * Accessory antenna is connected
    */
	EDvbhExtAntennaConnected
    };

/**
* @publishedPartner
* @prototype
*
* The type of receiver to be used.
*
* This enum may be be extended in the future with the addition of new values.  Users should therefore handle unknown or unexpected values gracefully.
*/
enum TDvbhReceiverType
    {
    /**
    * Integrated DVB-H Chip inside phone. DVB-H Kernel driver is used.
    */
	EDvbhReceiverIntegrated,
    /**
    * External Bluetooth receiver is used.
    */
	EDvbhReceiverBluetooth,
    /**
    * USB Accessory is used. 
    */
	EDvbhReceiverUsbAccessory
    };


/**
* @publishedPartner
* @prototype
*
* Device information about external DVB-H TV receiver
*
*/
class TDvbhAccessoryInfo
    {
public:
    /**
    * Product ID of external receiver
    */
    TUint16 iProductId;
    /**
    * Vendor ID of external receiver
    */
    TUint16 iVendorId;
    /**
    * Additional information about external receiver (reserverd for future use)
    */
    TUint32 iDevInfo;
    };

/**
* @publishedPartner
* @prototype
*
* The maximum size of a DVB-H Network Name string.  This value may
* require up to 8 bits, as per the specification DVB-H A038.
*/
const TInt KDvbhNetworkNameMaxSize = 256;

/**
* @publishedPartner
* @prototype
*
* The maximum size of a DVB-H IP Platform Name string.  This value may
* require up to 8 bits, as per the specification ETSI EN 301 192.
*/
const TInt KDvbhPlatformNameMaxSize = 256;

/**
* @publishedPartner
* @prototype
* @see RDvbhReceiver::GetHardwareInfo()
*
* The maximum size of a hardware information string that may be returned
* by RDvbhReceiver::GetHardwareInfo().
*/
const TInt KDvbhHWInfoMaxSize = 512;

/**
* @publishedPartner
* @prototype
*
* The normalized quality of a DVB-H signal.
*
* This enum may be be extended in the future with the addition of new values.  Users should therefore handle unknown or unexpected values gracefully.
*/
enum TDvbhSignalQuality
	{
	/**
	* There is no DVB-H signal.
	*/
	EDvbhSignalQualityNoSignal,
	
	/**
	* The DVB-H signal is of bad quality.
	*/ 
	EDvbhSignalQualityBad,
	
	/**
	* The DVB-H signal is of fair quality.
	*/ 	
	EDvbhSignalQualityFair,
	
	/**
	* The DVB-H signal is of good quality.
	*/ 
	EDvbhSignalQualityGood,
	
	/**
	* The DVB-H signal is of very good quality.
	*/ 
	EDvbhSignalQualityVeryGood,
	
	/**
	* The DVB-H signal is of excellent quality.
	*/ 
	EDvbhSignalQualityExcellent				
	};

/**
* @publishedPartner
* @prototype
*
* Used to represent a frequency in Hz.
*/
typedef TUint TDvbhFrequency;

/**
* @publishedPartner
* @prototype
* @see KDvbhHWInfoMaxSize.
* @see RDvbhReceiver::GetHardwareInfo()
*
* Used to hold a hardware information string returned by RDvbhReceiver::GetHardwareInfo().
*/
typedef TBuf8<KDvbhHWInfoMaxSize> TDvbhHardwareInfo;

/**
* @publishedPartner
* @prototype
*
* Used to represent a DVB-H network identifier. This may require
* up to 16-bits, as per the specifications ETSI EN 301 192 & DVB-H A038.
*/
typedef TInt TDvbhNetworkId;

/**
* @publishedPartner
* @prototype
*
* Used to represent a DVB-H cell identifier.  DVB-H specification A038
* states that elementary_cell_id & logical_cell_id require up to 6-bits each.
*/
typedef TInt TDvbhCellId;

/**
* @publishedPartner
* @prototype
*
* Used to identify a DVB-H network.
*/
class TDvbhNetwork
	{
public:
	/**
	* C++ Constructor.
	*/
    inline TDvbhNetwork();

	/**
	* The ID number of the network.
	*/
    TDvbhNetworkId                  iId;
    
    /**
    * The name of the network.
    */
    TBuf8<KDvbhNetworkNameMaxSize>  iName;
	};

/**
* @publishedPartner
* @prototype
* @see KDvbhInvalidPlatform
*
* Used to represent an DVB-H IP Platform identifier.  This may require
* up to 24-bits, as per the specification ETSI EN 301 192.
*
* The value KDvbhInvalidPlatform is reserved to represent an invalid IP platform Id.
*/
typedef TInt TDvbhPlatformId;

/**
* @publishedPartner
* @prototype
*
* A reserved TDvbhPlatformId value for representing an invalid IP platform Id.
*/
const TDvbhPlatformId KDvbhInvalidPlatform = -1;

/**
* @publishedPartner
* @prototype
*
* Used to identify an IP platform.
*/
class TDvbhPlatform
	{
public:
	/**
	* C++ Constructor.
	*/
    inline TDvbhPlatform();

	/**
	* The ID number of the IP platform.
	*/
    TDvbhPlatformId                 iId;
    
    /**
    * The name of the IP platform.
    */
    TBuf8<KDvbhPlatformNameMaxSize> iName;
	};

/**
* @publishedPartner
* @prototype
*
* Used to communicate the results of a signal scan.
*
*/
class TDvbhScanResult
	{
public:
	/**
	* Represents the status of the DVB-H signal on a frequency.
	*
	* This enum may be be extended in the future with the addition of new values.  Users should therefore handle unknown or unexpected values gracefully.
	*/
	enum TDvbhSignalStatus
		{
		/**
		* Indicates that no DVB-H signal was found on the frequency.
		*/
		ENoSignalFound		= 0x00,
		
		/**
		* Indicates that a DVB-H signal was found on the frequency.
		*/
		ESignalFound		= 0x01,
		
		/**
		* Indicates that a DVB-H signal and a valid DVB-H IP Platform were found on the frequency.
		*/
		EPlatformFound		= 0x02,
		
		/**
		* Indicates that a DVB-H signal and a valid DVB-H Network were found on the frequency.
		*/
		ENetworkFound		= 0x04
		};
public:
	/**
	* C++ Constructor.
	*/
    inline TDvbhScanResult();

	/**
	* The frequency (in Hz) to which this scan result applies.
	*/
    TDvbhFrequency  iFrequency;
    
	/**
	* The status of the signal on this frequency.  It represents a bitmap of
	* possible TDvbhSignalStatus values.  TDvbhSignalStatuses may be combined using 
	* bitwise OR unless noted.
	*
	* Note that if one or both of the ENoSignalFound or ENetworkFound bits are set,
	* then this implies that a signal was found, even if the ESignalFound has not been
	* set.  The ESignalFound bit is redundant in this case.
	*/
	TUint			iSignalStatus;

	/**
	* If an DVB-H IP Platform was found on the signal then it is reported here.
	*
	* Note: The contents of this member variable are valid only if the EPlatformFound bit
	* has been set in iSignalStatus.
	*/
    TDvbhPlatform   iPlatform;
    
	/**
	* If an DVB-H Network was found on the signal then it is reported here.
	*
	* Note: The contents of this member variable are valid only if the ENetworkFound bit
	* has been set in iSignalStatus.
	*/
    TDvbhNetwork    iNetwork;

	/**
	* A value between 0 and 100 representing how much of the specified frequency
	* range has been scanned.
	*
	* 0 represents 0% (no frequencies scanned yet).
	* 100 reprsents 100% (all frequencies scanned, RDvbhReceiver::Scan() operation about to complete).
	*/
    TUint           iProgress;
private:
	/**
	* @internalComponent
	* @prototype
	*
	* For Symbian use only.  Reserved for future expansion.
	*/
	TInt iReserved1;
	
	/**
	* @internalComponent
	* @prototype
	*
	* For Symbian use only.  Reserved for future expansion.
	*/
	TInt iReserved2;
	
	/**
	* @internalComponent
	* @prototype
	*
	* For Symbian use only.  Reserved for future expansion.
	*/
	TInt iReserved3; 
	};

/**
* @publishedPartner
* @prototype
*
* Used to hold performance metrics related to the receiver hardware.
*
* Note: The counts in this class are reset to 0 when Open() is called
* for the first time on an inactive receiver.
*/
class TDvbhPerformanceData
	{
public:
	/**
	* C++ Constructor.
	*/
    inline TDvbhPerformanceData();
 
	/**
    * Represents the number of currently active filters.
    */
    TInt    iFilterCount;
    
	/**
    * Represents the maximum number of filters that may be simultaneously
    * active on the RDvbhReceiver hardware.
    */
    TInt    iMaxFilters;
 
 	/**
    * Represents the cumulative received IP data count (bytes).
    */
    TInt64  iDataCount;
 
  	/**
    * Represents the cumulative total I/O write count (bytes)
    */
    TInt64  iWriteCount;
    
  	/**
    * Represents the cumulative total I/O write count (commands)
    */
    TInt    iWriteCommandCount;
    
  	/**
    * Represents the cumulative total I/O read count (bytes)
    */
    TInt64  iReadCount;
    
  	/**
    * Represents the cumulative total I/O read count (commands)
    */
    TInt    iReadCommandCount;
private:
	/**
	* @internalComponent
	* @prototype
	*
	* For Symbian use only.  Reserved for future expansion.
	*/
	TInt iReserved1;
	
	/**
	* @internalComponent
	* @prototype
	*
	* For Symbian use only.  Reserved for future expansion.
	*/
	TInt iReserved2;
	
	/**
	* @internalComponent
	* @prototype
	*
	* For Symbian use only.  Reserved for future expansion.
	*/
	TInt iReserved3;    
	};

/**
* @publishedPartner
* @prototype
*
* Used to represent the network time for an IP platform.
*/	
class TDvbhNetworkTime
	{
public:
	/**
	* C++ Constructor.
	*/
    inline TDvbhNetworkTime();

	/**
	* The network time as the number of microseconds since midnight, January 1st, 0 AD nominal Gregorian.
	*/
    TTime  iNetworkTime;
    
	/**
	* The tick count when the network time was stored.
	*/
    TUint32 iTerminalTimeStamp;
    
	/**
	* The DVB-H IP Platform whose network time this is.
	*/  
    TDvbhPlatformId iPlatformId;
	};

/**
* @publishedPartner
* @prototype
*
* Enumerates signal scan options.
*
* This enum may be be extended in the future with the addition of new values.  Users should therefore handle unknown or unexpected values gracefully.
*/
enum TDvbhScanOptions
	{
	/**
	* Indicates that the scan configuration supplied by the client is to be used in subsequent scans.
	*/
	EDvbhScanOptionNormal = 0x00,
	
	/**
	* Indicates that the implementation/hardware should use its own scan configuration in subsequent scans.
	*/
	EDvbhScanOptionFullScan = 0x01
	};

/**
* @publishedPartner
* @prototype
*
* Holds the parameters to be used for subsequent signal scans.
*/
class TDvbhScanConfiguration 
	{
public:
	/**
	* C++ Constructor.
	*/
    inline TDvbhScanConfiguration();

	/**
	* The lowest frequency of the range to be scanned, in Hz.
	*/
    TDvbhFrequency iScanStartFrequency;

	/**
	* The highest frequency of the range to be scanned, in Hz.
	*/
	TDvbhFrequency iScanEndFrequency;
	
	/**
	* The bandwidth between successive frequencies scanned, in Hz.
	*/
	TDvbhFrequency iSignalBandwidth;
	
	/**
	* Holds the options (as TDvbhScanOptions) for the scan. TDvbhScanOptions may be combined using bitwise OR unless noted.
	*/
    TUint  		   iScanOptions;
private:
	/**
	* @internalComponent
	* @prototype
	*
	* For Symbian use only.  Reserved for future expansion.
	*/
	TInt iReserved;
	};
	
#include <mobiletv/hai/dvbh/dvbhtypes.inl>

#endif // DVBHTYPES_H

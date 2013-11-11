// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\specific\ethernet.cpp
// PDD for the Ethernet under the windows emulator
// 
//

/**
 @file wins/specific/ethernet.cpp
*/


#include <ethernet.h>
#include "nk_priv.h"
#include "nk_plat.h"
#include <property.h>

#include "pcap.h"


/** @addtogroup enet Ethernet Drivers
 *  Kernel Ethernet Support
 */

/** @addtogroup enet_pdd Driver PDD's
 * @ingroup enet
 */

/** @addtogroup enet_byplatform Ethernet support by platform
 * @ingroup enet
 */

/** @addtogroup enet_wins WINS (Emulator) Ethernet support
 * @ingroup enet_byplatform
 */


// strings potentially written to ethernet.ini by netcards.exe
#define	KEpocIniEthSpeed10Mbps	"10Mbps"
#define	KEpocIniEthSpeed100Mbps	"100Mbps"

// entries in epoc.ini file:
#define	KEpocIniEthSpeedEntry	"ETHER_SPEED"
#define	KEpocIniEthNIFEntry	"ETHER_NIF"
#define	KEpocIniEthMACEntry	"ETHER_MAC"

#define KEthDrvPanicCategory "D32ETHER"

#define KLocalDriverNameMax				256

#define PCAP_FILTER "ether dst %x:%x:%x:%x:%x:%x or ether multicast and ether src not %x:%x:%x:%x:%x:%x"

_LIT(KPddName, "Ethernet.Wins");

// needs ldd version..
const TInt KMinimumLddMajorVersion=1;
const TInt KMinimumLddMinorVersion=0;
const TInt KMinimumLddBuild=122;

/** @addtogroup enet_windows Windows Emulator Ethernet Pdd
 * @ingroup enet_pdd
 * @ingroup enet_wins
 * @{
 */

/**
 * The Windows specific Ethernet physical device (factory) class 
 * @internalTechnology belongs to PDD which sits internally in kernel 
 */
class DDriverEthernet : public DPhysicalDevice
    {
public:

    /**
     * The constructor
     * Sets the drivers version number. Limits possible
	 * number of units to one only (unit 0)
     */
    DDriverEthernet();

    /**
     * Inherited from DPhysicalDevice.
	 * Install the driver by setting it's name
	 * @return KErrNone on success, other error code on failure
     */
    virtual TInt Install();

    /**
     * Inherited from DPhysicalDevice.
	 * Get the Capabilites of the driver
     * NOT supported but required as implementation of
	 * pure virtual in base class
     */
    virtual void GetCaps(TDes8 &aDes) const;

    /**
     * Inherited from DPhysicalDevice.
	 * Create a channel to a device
	 * @param aChannel a reference to a newly created channel object
	 * @param aUnit a unit for which the channel is being created
	 * @param anInfo pointer to a descriptor with additional info (may be NULL)
	 * @param aVer a requested version
     * @return KErrNone on success, other error code on failure
     */
    virtual TInt Create(DBase*& aChannel, TInt aUnit, 
						const TDesC8* anInfo, const TVersion &aVer);

    /**
     * Inherited from DPhysicalDevice.
	 * Validate that the info supplied would create a valid channel
	 * @param aUnit a unit for which validation is to be done
	 * @param anInfo pointer to a descriptor with additional info (may be NULL)
	 * @param aVer a version to be validated
     * @return KErrNone if valid, KErrNotSupported otherwise
	 */
    virtual TInt Validate(TInt aUnit, const TDesC8* anInfo, const TVersion &aVer);
	};


/**
 * The WINS specific Ethernet channel class for the libpcap library
 * @internalTechnology belongs to PDD which sits internally in kernel 
 */
class DEthernetWins : public DEthernet
    {

	/**
     * The Worker Thread
     * @param aDEthernetWins Pointer to this object
     * @return Should never return
     */
    friend TInt WorkThread(DEthernetWins * aDEthernetWins);

public:

	enum TWinsEtherPanic
	{
	EBadMacAddress = 1, // means bad MAC address in ini file or entry for it missing or ini file missing
	ENoNetInterface, // means entry for network interface name missing in ini file or ini file missing
	EPcapNull	// means Wpcap couldn't be initialised - potentially not installed or name of network interface in ini file wrong
	};

    /**
     * The Constructor.
     */
    DEthernetWins();

    /**
     * The Destructor.
     */
    ~DEthernetWins();

	/**
     * The Isr plays a role of the pcap callback.
     * Do NOT call the function directly
     * @param thisObject a pointer to this object
     * @param header the received packet's pcap header
     * @param pkt_data the received packet
	 * @post new packet is copied to LDD's RX buffers' queue
	 */
    static void Isr(u_char *thisObject,   
                     const struct pcap_pkthdr *header, 
                     const u_char *pkt_data);

    /**
     * The DoCreate Method.
     * Sets up the channel as part of the object creation
     * and retrieves the MAC address from epoc.ini file.
	 * Also creates wpcap handler and thread for wpcap loop.
	 * @pre epoc32\\data\\epoc.ini must exist with entries: "ETHER-NIF=..", "ETHER-MAC=..", "ETHER-SPEED=.."
	 * @param aUnit a unit for which the channel is being created
	 * @panic D32ETHER reason: (1) can't get proper MAC address (2) can't get
	 * network interface name (3) can't initialise wpcap
	 * @return KErrNone on success, other error code on failure
	 */
    TInt DoCreate(TInt aUnit); 

    /**
     * DEthernet implementation.
	 * Start the receiver.
     * Resumes pcap thread. Sets status to ready.
	 * @return KErrNone on success or suitable error code on failure
     */
    virtual TInt Start();

    /**
     * DEthernet implementation.
	 * Stop the receiver.
     * @param aMode possible values are: EStopNormal (=0), EStopEmergency (=1)
	 * @post pcap thread suspended, status set to not ready
     */
    virtual void Stop(TStopMode aMode);

    /**
     * DEthernet implementation.
	 * Validates a new configuration - should be called before Configure()
     * @param aConfig is the configuration to be validated
     * @return KErrNone if aConfig valid, KErrNotSupported otherwise
     * @see Configure()
     */
    virtual TInt ValidateConfig(const TEthernetConfigV01 &aConfig) const;

    /**
     * DEthernet implementation.
	 * Configure the PDD and pcap library
     * Reconfigure the library using the new configuration supplied.
	 * Sets pcap filter to read only frames with destination address set to
	 * broadcast, multicast or MAC addresss from defaultConfig.
     * This will not change the MAC address.
     * @param aConfig The new configuration
	 * @return KErrNone on success, suitable error code otherwise
     * @see ValidateConfig()
     * @see MacConfigure()
     */
    virtual TInt Configure(TEthernetConfigV01 &aConfig);

    /**
     * DEthernet implementation.
	 * Change the MAC address - writes new MAC address in defaultConfig.
	 * If new settings are to have any effect then pcap filter
	 * ought to be set again which is done by 'Configure()'
	 * @param aConfig a configuration structure containing the new MAC
     * @see Configure()
     */
    virtual void MacConfigure(TEthernetConfigV01 &aConfig);

	/**
     * DEthernet implementation.
	 * Get the current config from defaultConfig member varaiable
	 * which is assumed to be up to date.
     * Fills in the following fields:
     * The Transmit Speed
     * The Duplex Setting
     * The MAC address
     * @param aConfig is a TEthernetConfigV01 reference that will be filled in
     */
    virtual void GetConfig(TEthernetConfigV01 &aConfig) const;

	/**
     * DEthernet implementation.
	 * Dummy method, required as pure virtual in base class
     */
    virtual void CheckConfig(TEthernetConfigV01 &aConfig);

	/**
     * DEthernet implementation.
	 * Should query the capabilites.
     * NOT supported but required as pure virtual in base class
     */
    virtual void Caps(TDes8 &aCaps) const;

    /**
     * DEthernet implementation.
	 * Transmit data via wpcap
     * @param aBuffer reference to the data to be sent
     * @return KErrNone on success, other error code on failure
     */
    virtual TInt Send(TBuf8<KMaxEthernetPacket+32> &aBuffer);

    /**
     * DEthernet implementation.
	 * Retrieve data
     * Pull the received data out of the pcap library and into the supplied buffer. 
     * Need to be told if the buffer is OK 
     * @param aBuffer Reference to the buffer to be used to store the data in
     * @param okToUse Bool to indicate if the buffer is usable
     * @return KErrNone on success, other error code on failure
     */
    virtual TInt ReceiveFrame(TBuf8<KMaxEthernetPacket+32> &aBuffer, TBool okToUse);

	/**
     * DEthernet implementation.
	 * Disables all IRQ's 
     * @return The IRQ level before it was changed
     * @see RestoreIrqs()
     */
    virtual TInt DisableIrqs();

    /**
     * DEthernet implementation.
	 * Restore the IRQ's to the supplied level
	 * @param aIrq The level to set the irqs to.
     * @see DisableIrqs()
     */
    virtual void RestoreIrqs(TInt aIrq);

	/**
     * DEthernet implementation.
	 * Return the DFC Queue that this device should use
     * @param aUnit a channel's unit number (ignored - only one unit possible)
     * @return a DFC Queue to use
     */
    virtual TDfcQue* DfcQ(TInt aUnit);
    
		
private:
 	static TBool DEthernetWins::IsTcp(TDesC8 &aFrame);
 	static TInt DEthernetWins::GetTcpAckNumber(TDesC8 &frame);
 	static TInt DEthernetWins::GetTcpSeqNumber(TDesC8 &frame);
	/**
     * Read network interface to be used from configuration file. Panic if
	 * pre-conditions are not satisfied.
	 * @pre epoc32\\data\\epoc.ini must exist with entry: "ETHER-NIF=existing_nif_name"
	 * @post network interface name put in a member variable: iNetInterfaceName
	 * @panic D32ETHER reason: (2) can't get network interface name 
     */
	void  SetDriverName();

	/**
	 * Read MAC address from a configuration file and put it
	 * into defaultConfig member variable checking before if the
	 * one from the file is correct. Panic if pre-conditions are not satisfied
	 * (although in case when MAC address is improper).
	 * @pre epoc32\\data\\epoc.ini must exist with entry: "ETHER-MAC=proper_mac_address"
	 * @panic D32ETHER reason:  EBadMacAddress
	 * @return KErrNone on success (panics on failure)
	 */
	TInt SetMacAddress();

    
private:

    /**
     * The handle to the pcap interface - wpcap specific
     */
    pcap_t * iPcapPtr;

    /**
     * Saved received packet information - wpcap specific:
	 * pointer to a structure that holds general information about the packet:
	 * the time in which it was sniffed, the length of this packet, 
	 * and the length of his specific portion (in case it is fragmented)
     */
    const struct pcap_pkthdr * iPcapHeader; 

    /**
     * Saved receive packet data - wpcap specific
     */
    const u_char * iPcapPktData;

    /**
     * Id of the receiver - wpcap thread
     */
    unsigned long iWorkerThreadId;

	/**
     * Contains the handle to wpcap thread. 
     */
	HANDLE				iThreadHandle;

    /**
     * Stores the unit number (only one interface possible in 
	 * this implementation, so it will have value "0")
     */
    TInt  iUnit;
	
    /**
     * Is ETrue if the chip has been fully configured and is ready
	 * for receiving frames. Is set to ETrue in Start(), to EFalse
	 * in Stop(). Initialized in constructor as EFalse.
     */
    TBool iReady;

    /**
     * Contains the default/current configuration of the driver.
	 * Updated whenever configuration is to be changed.
     */
    TEthernetConfigV01 defaultConfig;

	/**
     * Contains the network interface name to be used 
	 * @see SetDriverName()
     */
	char	iNetInterfaceName[KLocalDriverNameMax];
	
    };

/** @} */ // End of wins ethernet pdd


 
DDriverEthernet::DDriverEthernet()
// Constructor
    {
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DDriverEthernet::DDriverEthernet()"));
    iUnitsMask=0x1;	// support unit 0 only
    iVersion=TVersion(KEthernetMajorVersionNumber,
                      KEthernetMinorVersionNumber,
                      KEthernetBuildVersionNumber);
	
	}


TInt DDriverEthernet::Install()
// Install the driver
    {
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DDriverEthernet::Install()"));
    return SetName(&KPddName);
    }


void GetWinsEthernetsCaps(TDes8 &aCaps, TInt aUnit=0)
    {
    __KTRACE_OPT(KHARDWARE, Kern::Printf("GetWinsEthernetsCaps(TDes8 &aCaps, TInt aUnit)"));
    TEthernetCaps capsBuf;
	
	aUnit=0;

    aCaps.FillZ(aCaps.MaxLength());
    aCaps=capsBuf.Left(Min(capsBuf.Length(),aCaps.MaxLength()));
    }

void PanicFromWinsEtherDrv(TInt aReason)
	{
	Kern::Fault(KEthDrvPanicCategory, aReason);
	}

void DDriverEthernet::GetCaps(TDes8 &aDes) const
// Return the driver's capabilities
    {
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DDriverEthernet::GetCaps(TDes8 &aDes) const"));
    GetWinsEthernetsCaps(aDes);	
    }


TInt DDriverEthernet::Create(DBase*& aChannel, 
                             TInt aUnit, 
                             const TDesC8* aInfo, 
                             const TVersion& aVer)
// Create a driver
    {
    __KTRACE_OPT(KHARDWARE, 
		Kern::Printf("DDriverEthernet::Create(DBase*& aChannel, TInt aUnit, const TDesC8* nInfo, const TVersion& aVer)"));
	
    TInt ret;

	ret = Validate( aUnit, aInfo, aVer);
	if ( KErrNone != ret )
		return ret;

	ret = KErrNoMemory;

	DEthernetWins* ptrPdd = new DEthernetWins;

	
	if ( ptrPdd )
        {
        ret = ptrPdd->DoCreate(aUnit);
        if ( ret != KErrNone)
			{
            delete ptrPdd;
			}	
		else
			aChannel = ptrPdd;
        }

	
    return ret;
    }


TInt DDriverEthernet::Validate(TInt aUnit, 
                               const TDesC8* /*aInfo*/, 
                               const TVersion& aVer)
//	Validate the requested configuration
    {
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DDriverEthernet::Validate(TInt aUnit, const TDesC8* /*aInfo*/, const TVersion& aVer)"));
    if ((!Kern::QueryVersionSupported(iVersion,aVer)) || 
        (!Kern::QueryVersionSupported(aVer,TVersion(KMinimumLddMajorVersion,
                                                    KMinimumLddMinorVersion,
                                                    KMinimumLddBuild))))
		{
        return KErrNotSupported;
		}
    
	if (aUnit != 0)
		{
        return KErrNotSupported;
		}

	return KErrNone;
    }


DEthernetWins::DEthernetWins()
// Constructor
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::DEthernetWins()"));

    iReady = EFalse;
    
	// set default configuration - must be set before DoCreate gets called
	defaultConfig.iEthSpeed  = KEthSpeedUnknown;
    defaultConfig.iEthDuplex = KEthDuplexUnknown;
							
	// MAC address initially set to NULL
	defaultConfig.iEthAddress[0] = 0; 
    defaultConfig.iEthAddress[1] = 0; 
    defaultConfig.iEthAddress[2] = 0; 
	defaultConfig.iEthAddress[3] = 0; 
    defaultConfig.iEthAddress[4] = 0; 
    defaultConfig.iEthAddress[5] = 0; 
	
	iNetInterfaceName[0] = '\0';

	// wpcap
	iPcapPtr = NULL;

	}

DEthernetWins::~DEthernetWins()
// Destructor
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::~DEthernetWins()"));

	if (iPcapPtr)
		{
		pcap_close(iPcapPtr);
		iPcapPtr = NULL;
		}
  
	}


TInt DEthernetWins::DoCreate(TInt aUnit)//, const TDesC8* /*anInfo*/)
// Sets up the PDD
	{
	__KTRACE_OPT(KHARDWARE, 
		Kern::Printf("DEthernetWins::DoCreate(TInt aUnit, const TDesC8* /*anInfo*/)"));
   
	iUnit = aUnit;

	TInt ret = KErrNone;

	SetMacAddress(); // will panic if can't get proper MAC address

	const char* speedProperty = Property::GetString(KEpocIniEthSpeedEntry);

	if( (NULL==speedProperty) ? 0 : (0 == strcmp( speedProperty, KEpocIniEthSpeed10Mbps )) )
		{
		defaultConfig.iEthSpeed = KEthSpeed10BaseT;
		}
	else if ( (NULL==speedProperty) ? 0 : (0 == strcmp( speedProperty, KEpocIniEthSpeed100Mbps )) )
		{
		defaultConfig.iEthSpeed = KEthSpeed100BaseTX;
		}

	SetDriverName();

	char errPcap[PCAP_ERRBUF_SIZE];

	if( iPcapPtr == NULL )
		{
		
		if((iPcapPtr = pcap_open_live( iNetInterfaceName,
									   2000, 
                                      1, 
                                      20, 
                                      errPcap)) != NULL)
			{
	 	    
			if ((iThreadHandle = CreateThread(NULL,
                                           0,
                                           (LPTHREAD_START_ROUTINE)WorkThread,
                                           this,
                                           CREATE_SUSPENDED,
                                           &iWorkerThreadId)) != NULL)
				{
    			ret = Configure(defaultConfig);
				}
			else
				{
				__KTRACE_OPT(KHARDWARE, Kern::Printf("CreateThread() failed to create worker thread"));
				ret = KErrBadHandle; 
				}
			}
		else
			{
		    __KTRACE_OPT(KHARDWARE, Kern::Printf("pcap_open_live() failed to open"));
			//ret = KErrCouldNotConnect;
			PanicFromWinsEtherDrv( EPcapNull );
			}
    
		}
	
    if(ret != KErrNone)
		{
		// Failed to init all so tidy up
		if( iPcapPtr )
			{
			pcap_close(iPcapPtr);
			iPcapPtr = NULL;
			}
		__KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::DoCreate() returned with ERROR"));
		return ret;
		}


    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::DoCreate %2x:%2x:%2x:%2x:%2x:%2x",
                                         defaultConfig.iEthAddress[0], 
                                         defaultConfig.iEthAddress[1],
                                         defaultConfig.iEthAddress[2], 
                                         defaultConfig.iEthAddress[3],
                                         defaultConfig.iEthAddress[4], 
                                         defaultConfig.iEthAddress[5]));

	return ret;
	}


void  DEthernetWins::SetDriverName()
	{
	__KTRACE_OPT(KHARDWARE, 
		Kern::Printf("DEthernetWins::SetDriverName()"));

	const char* property = Property::GetString(KEpocIniEthNIFEntry);

	__ASSERT_ALWAYS( (NULL != property),
		PanicFromWinsEtherDrv(ENoNetInterface) );

	memcpy( iNetInterfaceName, property, strlen(property) );
	}


TInt DEthernetWins::SetMacAddress()
// reads MAC address from epoc.ini file and writes into defaultConfig
	{
	
	__KTRACE_OPT(KHARDWARE, 
		Kern::Printf("DEthernetWins::SetMacAddress()"));

	TUint8 tempAddress[6];

	const char *tempBuffer = Property::GetString(KEpocIniEthMACEntry);

	__ASSERT_ALWAYS( ((tempBuffer != NULL) && (strlen(tempBuffer) >= 6)),
		PanicFromWinsEtherDrv(EBadMacAddress) );


	TBuf<20> validCharsLower(_L("0123456789abcdef"));
	TBuf<20> validCharsUpper(_L("0123456789ABCDEF"));
	TUint8 value;
	TUint8 upper=0;
	TChar c;
	TInt pos; 
	TInt i;
	for( i=0; i<6; i++)
		{
		c = tempBuffer[2*i];
		if(((pos = validCharsLower.Locate(c)) == KErrNotFound) &&
			((pos = validCharsUpper.Locate(c)) == KErrNotFound))
			{
			PanicFromWinsEtherDrv( EBadMacAddress );
			}
		upper = (TUint8)pos;
		c = tempBuffer[(2*i)+1];
		if( ((pos = validCharsLower.Locate(c)) == KErrNotFound) &&
			((pos = validCharsUpper.Locate(c)) == KErrNotFound) )
			{
			PanicFromWinsEtherDrv( EBadMacAddress );
			}

		value = (TUint8)pos;
		value = (TUint8)((upper<<4) | value);
		tempAddress[i] = value;
		}

	for( i=0; i<6; i++ )
		{
		defaultConfig.iEthAddress[i] = tempAddress[i];
		}

	return KErrNone;

	}


TDfcQue* DEthernetWins::DfcQ(TInt /*aUnit*/)
// Return the DFC queue to be used for this device
	{
    __KTRACE_OPT(KHARDWARE, 
		Kern::Printf("DEthernetWins::DfcQ(TInt )"));
    return Kern::DfcQue0();
	}


TInt DEthernetWins::Start()
// Start receiving frames
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::Start()"));

	TInt32 ret;
	
	// Start thread
	// ResumeThread() - from MSDN help:
	// This function decrements a thread’s suspend count. 
	// When the suspend count is decremented to zero, 
	// the execution of the thread is resumed. 
	// Return value: The thread’s previous suspend count indicates success. 0xFFFFFFFF indicates failure
	ret = ResumeThread( iThreadHandle );
	if( (0xFFFFFFFF == ret) )//|| (ret > 1) )
		return KErrGeneral;
	
    iReady = ETrue;

    return KErrNone;
	}


void DEthernetWins::Stop(TStopMode aMode)
// Stop receiving frames
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::Stop(TStopMode aMode)"));

	
    switch (aMode)
		{
        case EStopNormal:
        case EStopEmergency:
			SuspendThread(iThreadHandle);
			iReady = EFalse;
			break;
		default:
			SuspendThread(iThreadHandle);
			iReady = EFalse;
			break;
		}

	}


TInt DEthernetWins::ValidateConfig(const TEthernetConfigV01 &aConfig) const
// Validate a config structure.
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::ValidateConfig(const TEthernetConfigV01 &aConfig) const"));
    switch(aConfig.iEthSpeed)
        {
        case KEthSpeedUnknown:
		case KEthSpeedAuto: 
        case KEthSpeed10BaseT:
        case KEthSpeed100BaseTX:
			break;
        default:		
        	return KErrNotSupported;
        }

    switch(aConfig.iEthDuplex)
        {
        case KEthDuplexUnknown:
		case KEthDuplexAuto:
        case KEthDuplexHalf:
        case KEthDuplexFull:
			break;
        default:
        	return KErrNotSupported;
        }

    return KErrNone;
	}


void DEthernetWins::CheckConfig(TEthernetConfigV01& /*aConfig*/)
// dummy implementation of pure virtual function
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::CheckConfig(TEthernetConfigV01& aConfig)"));
	}

TInt DEthernetWins::Send(TBuf8<KMaxEthernetPacket+32> &aBuffer)
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::Send(TBuf8<KMaxEthernetPacket+32> &aBuffer)"));
    TUint16 * dataP = (TUint16 *)aBuffer.Ptr();
    TUint16 length = TUint16(aBuffer.Length());

	TInt ret;

    if (iPcapPtr)
	    {
		__KTRACE_OPT2(KHARDWARE, KSCRATCH, Kern::Printf("  >pdd tx: sending tcp seq=%u ack=%u", GetTcpSeqNumber(aBuffer), GetTcpAckNumber(aBuffer) ));
        if(pcap_sendpacket(iPcapPtr, (unsigned char *)dataP, length))
		    {
            __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::Send - pcap_sendpacket() failed "));

		    ret = KErrWrite; 
	        }
        else
		    {
            __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::Send: \n*** completed pcap_sendpacket() ***\n"));
            ret = KErrNone;
            }
		}
    else
		{
        __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::Send() pcap adapter NOT open\n"));
        ret = KErrNotReady; 
		}
    
    return ret;
	}


TInt DEthernetWins::DisableIrqs()
// Disable normal interrupts
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::DisableIrqs()"));
    return NKern::DisableInterrupts(1);
	}


void DEthernetWins::RestoreIrqs(TInt aLevel)
// Restore normal interrupts
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::RestoreIrqs(TInt aLevel)"));
    NKern::RestoreInterrupts(aLevel);
	}


void DEthernetWins::MacConfigure(TEthernetConfigV01 &aConfig)
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::MacConfigure(TEthernetConfigV01 &aConfig)"));
    defaultConfig.iEthAddress[0] = aConfig.iEthAddress[0]; 
	defaultConfig.iEthAddress[1] = aConfig.iEthAddress[1];
	defaultConfig.iEthAddress[2] = aConfig.iEthAddress[2];
	defaultConfig.iEthAddress[3] = aConfig.iEthAddress[3];
	defaultConfig.iEthAddress[4] = aConfig.iEthAddress[4];
	defaultConfig.iEthAddress[5] = aConfig.iEthAddress[5];
	}


TInt DEthernetWins::Configure(TEthernetConfigV01 & /*aConfig*/)
// Set a wpcap filter
	{

	__KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::Configure(TEthernetConfigV01 &aConfig)"));
	
    char errPcap[PCAP_ERRBUF_SIZE];
	TInt ret = KErrNone;
    char filter[128];
    struct bpf_program fcode;
    bpf_u_int32 SubNet,NetMask;

	//obtain the subnet
    pcap_lookupnet(	iNetInterfaceName, &SubNet, &NetMask, errPcap);

    sprintf(filter, 
            PCAP_FILTER, //pcap_filter, 
            defaultConfig.iEthAddress[0],
            defaultConfig.iEthAddress[1],
            defaultConfig.iEthAddress[2],
            defaultConfig.iEthAddress[3],
            defaultConfig.iEthAddress[4],
            defaultConfig.iEthAddress[5],
            defaultConfig.iEthAddress[0],
            defaultConfig.iEthAddress[1],
            defaultConfig.iEthAddress[2],
            defaultConfig.iEthAddress[3],
            defaultConfig.iEthAddress[4],
            defaultConfig.iEthAddress[5]);


	//compile the filter
    if( 0 != pcap_compile(iPcapPtr, &fcode, filter, 1, NetMask) )
		{
		// failed to compile:
		__KTRACE_OPT(KHARDWARE, Kern::Printf("pcap_compile() failed"));
		ret = KErrUnknown; 
		}
	else
		{
		// compiled successfully: set the filter
		if( 0 != pcap_setfilter(iPcapPtr, &fcode))
			{
			__KTRACE_OPT(KHARDWARE, Kern::Printf("pcap_setfilter() failed"));
			ret = KErrUnknown; 
			}
		}

    return ret; 
	}


void DEthernetWins::GetConfig(TEthernetConfigV01 &aConfig) const
// Get the current config from defaultConfig member
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::GetConfig(TEthernetConfigV01 &aConfig) const"));
    aConfig = defaultConfig;
	}


void DEthernetWins::Caps(TDes8 &aCaps) const
// return PDD's capabilites
    {
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::Caps(TDes8 &aCaps) const"));
    GetWinsEthernetsCaps(aCaps,iUnit); 
    }


void DEthernetWins::Isr( u_char *thisObject,  
                       	const struct pcap_pkthdr *header, 
                        const u_char *pkt_data)
// pcap callback fuction
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::Isr()"));

	DEthernetWins& thisPdd = *(DEthernetWins*) thisObject;
    	
	StartOfInterrupt();

	thisPdd.iPcapHeader = header;
	thisPdd.iPcapPktData = pkt_data;

    thisPdd.ReceiveIsr();
	
	EndOfInterrupt();

    return;
	}


TInt DEthernetWins::ReceiveFrame(TBuf8<KMaxEthernetPacket+32> &aBuffer, TBool okToUse)
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("DEthernetWins::ReceiveFrame(TBuf8<KMaxEthernetPacket+32> &aBuffer)"));

	// If no buffer available dump frame
    if(!okToUse)
		{
        return KErrGeneral;
		}

    aBuffer.Copy(iPcapPktData, iPcapHeader->len);
    aBuffer.SetLength(iPcapHeader->len);
	__KTRACE_OPT2(KHARDWARE, KSCRATCH, Kern::Printf("  >pdd rx: tcp seq=%u ack=%u", GetTcpSeqNumber(aBuffer), GetTcpAckNumber(aBuffer) ));
    
	return KErrNone;
	}

#ifdef _DEBUG
const TUint8 ETHER2_TYPE_IP_MSB = 0x08;
const TUint8 ETHER2_TYPE_IP_LSB = 0x00;
const TUint8 IP_TYPE_TCP        = 0x06;

TBool DEthernetWins::IsTcp(TDesC8 &aFrame)
	{
 	return (aFrame[12] == ETHER2_TYPE_IP_MSB && aFrame[13] == ETHER2_TYPE_IP_LSB && aFrame[23] == IP_TYPE_TCP);
	}

TInt DEthernetWins::GetTcpSeqNumber(TDesC8 &aFrame)
	{
 	TInt seqNum = 0;
 	if (IsTcp(aFrame))
 		seqNum = aFrame[38] << 24 | aFrame[39] << 16 | aFrame[40] << 8| aFrame[41];
 	return seqNum;
 	}

TInt DEthernetWins::GetTcpAckNumber(TDesC8 &aFrame)
 	{
 	TInt ackNum = 0;
 	if (IsTcp(aFrame))
 		ackNum = aFrame[42] << 24 | aFrame[43] << 16 | aFrame[44] << 8| aFrame[45];
 	return ackNum;
 	}
#endif

TInt WorkThread(DEthernetWins * aDEthernetWins)
	{
    __KTRACE_OPT(KHARDWARE, Kern::Printf("WorkThread(DEthernetWins * aDEthernetWins)"));

    return pcap_loop(aDEthernetWins->iPcapPtr, 0, DEthernetWins::Isr, (unsigned char *)aDEthernetWins);
	}

/**
 * @addtogroup enet_windows
 * @{
 */

/**
 * Real entry point from the Kernel: return a new driver
 * (Macro wrapping: EXPORT_C DPhysicalDevice *CreatePhysicalDevice() )
 */
DECLARE_STANDARD_PDD()
	{
	return new DDriverEthernet;
	}


/** @} */ // end of windows group

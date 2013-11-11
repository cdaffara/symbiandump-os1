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
// This contains implementation for CT_RDevUsbcClientData class



#include <f32file.h>
#include <d32usbc.h>
#include "T_RDevUsbcClientData.h" 



/*@{*/
//LITs to identify the commands
_LIT(KCmdNewL,									"NewL");
_LIT(KCmdUtilityLoadLogicalDevice, 				"LoadLogicalDevice");
_LIT(KCmdOpen, 					    			"Open");
_LIT(KCmdDeviceCaps,							"DeviceCaps");
_LIT(KCmdUtilitySetEndPoint, 					"SetEndPoint");
_LIT(KCmdEndpointCaps,							"EndpointCaps");
_LIT(KCmdSetInterface, 							"SetInterface");
_LIT(KCmdGetDeviceDescriptorSize,				"GetDeviceDescriptorSize");
_LIT(KCmdGetDeviceDescriptor,					"GetDeviceDescriptor");
_LIT(KCmdSetDeviceDescriptor,					"SetDeviceDescriptor");
_LIT(KCmdDestructor,							"~");
/*@}*/


/*@{*/
//LITs to read from the ini file
_LIT(KMode, 									"mode");
_LIT(KDirection, 								"direction");
_LIT(KBufferingMask, 							"bandwidthBufferingMask");
_LIT(KSupportsConnect,							"supportsConnect");
_LIT(KSupportsSelfPowered, 						"supportsSelfPowered");
_LIT(KSupportsRemoteWakeup, 					"supportsRemoteWakeup");
_LIT(KUSBVersionLow, 							"USBVersionLow");
_LIT(KUSBVersionHigh,							"USBVersionHigh");
_LIT(KVendorIDLow, 								"VendorIDLow");
_LIT(KVendorIDHigh, 							"VendorIDHigh");
_LIT(KProductIDLow, 							"ProductIDLow");
_LIT(KProductIDHigh, 							"ProductIDHigh");
_LIT(KReleaseNumberLow, 						"ReleaseNumberLow");
_LIT(KReleaseNumberHigh, 						"ReleaseNumberHigh");
_LIT(KDriver, 									"driver");
_LIT(KEXPECTED, 								"expected");
/*@}*/

/*@{*/
_LIT16(interfaceName, "USB interface");
/*@}*/

/*@{*/
_LIT(KResourceDMA, 							"ResourceDMA");
_LIT(KResourceDoubleBuffering,				"ResourceDoubleBuffering");
/*@}*/
const CDataWrapperBase::TEnumEntryTable CT_RDevUsbcClientData::iEUsbcEndpointResource[] = 
	{ 
	{ KResourceDMA,					EUsbcEndpointResourceDMA },		// Requests the use of DMA =0
	{ KResourceDoubleBuffering,		EUsbcEndpointResourceDoubleBuffering },	//Requests the use of double FIFO buffering = 1	
	};

/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_RDevUsbcClientData* CT_RDevUsbcClientData::NewL()
	{
	CT_RDevUsbcClientData*	ret=new (ELeave) CT_RDevUsbcClientData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
 * Protected constructor. First phase construction
 */
CT_RDevUsbcClientData::CT_RDevUsbcClientData()
	:
	iChannel(NULL),
	iInterface(NULL),
	iEndpointData(),
	iConfiguredEndpoints(0),
	iTxMaxPacketSize(0),
	iCaps(),
	iActiveCallback(NULL),
	iBuffer(NULL),
    iBuffer_verify(NULL),
    iDeviceDescriptorSize(0)
	{
	
	}

/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
void CT_RDevUsbcClientData::ConstructL()
	{
	iActiveCallback = CActiveCallback::NewL(*this);	
	}

/**
 * Public destructor
 */
CT_RDevUsbcClientData::~CT_RDevUsbcClientData()
	{		
	// Empty arrays and delete objects

	DestroyData();

	if (iActiveCallback)
		{
		delete iActiveCallback;
		iActiveCallback = NULL;
		}
	}

/**
 *  RunL is called when an asynchronous call has completed
 * 
 *  @param aActive - a pointer to an active object
 *  @param aIndex - Index of the asynchronous call
 * 
 */
void CT_RDevUsbcClientData::RunL(CActive* aActive, TInt aIndex)
	{
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::RunL"));
	DecOutstanding(); // One of the async calls has completed 
	TBool straySignal = EFalse; 
		  
	if(aActive == iActiveCallback)
		{
		INFO_PRINTF1(_L("Asynchronous task has completed. RunL  called"));
		}		
	else
		{ 
		ERR_PRINTF1(_L("Stray RunL signal"));
		SetBlockResult(EFail);
		straySignal = ETrue;		
		}
	
	if( !straySignal ) 
		{ 
		TInt err = aActive->iStatus.Int(); 
		if( err != KErrNone ) 
			{ 
			ERR_PRINTF2(_L("RunL Error %d"), err); 
			SetAsyncError( aIndex, err );		
			} 
		else
			{ 
			INFO_PRINTF1(_L("RunL  completed successfully"));
			}
		}
	INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::RunL"));
	}


/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_RDevUsbcClientData::GetObject()
	{
	return iChannel;
	}

/**
 * Returns the wrapper.
 * 
 * @return pointer to the Wrapper class
 */



/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 * @return					ETrue if the command is processed
 * @leave					System wide error
 */
TBool CT_RDevUsbcClientData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool ret=ETrue;
		
	if(aCommand == KCmdNewL )
		{
		DoCmdNewL();
		}
	else if(aCommand == KCmdUtilityLoadLogicalDevice)
		{
		DoCmdUtilityLoadLogicalDevice(aSection);
		}
	else if(aCommand == KCmdOpen)
		{
		DoCmdOpen();
		}
	else if(aCommand == KCmdDeviceCaps)
		{
		DoCmdDeviceCaps(aSection);
		}	
	else if(aCommand == KCmdGetDeviceDescriptorSize)
		{
		DoCmdGetDeviceDescriptorSize(aSection);
		}
	else if(aCommand == KCmdGetDeviceDescriptor)
		{
		DoCmdGetDeviceDescriptor(aSection);
		}
	else if(aCommand == KCmdSetDeviceDescriptor)
		{
		DoCmdSetDeviceDescriptor(aSection);
		}
	else if(aCommand == KCmdUtilitySetEndPoint)
		{
		DoCmdUtilitySetEndPointL(aSection);
		}	
	else if(aCommand == KCmdEndpointCaps)
		{
		DoCmdEndpointCaps();
		}
	else if(aCommand == KCmdSetInterface)
		{
		DoCmdSetInterface(aSection);
		}	
	else if(aCommand == KCmdDestructor)
		{
		DoCmdDestructor();
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		ret = EFalse;
		}
	
	return ret;
	}



/**
 * Creante an object of RDevUsbcClient
 */
void  CT_RDevUsbcClientData::DoCmdNewL()
	{
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::DoCmdNewL"));
	DestroyData();
	iChannel = new (ELeave) RDevUsbcClient();
	INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::DoCmdNewL"));
	}
/**
 * Loads Logical Device for USB.
 */
void CT_RDevUsbcClientData::DoCmdUtilityLoadLogicalDevice(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::DoCmdUtilityLoadLogicalDevice"));
	TInt error(KErrNone);
	TBool dataOk = ETrue;
	TPtrC lddName;
	TBufC<9> value;	
	if(!GetStringFromConfig(aSection, KDriver, lddName))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), & KDriver);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}	
	if(dataOk)
		{	
		value = lddName;		
		INFO_PRINTF2(_L("Driver: %S"), & value);
		error = User::LoadLogicalDevice(value);		
		
		if(error == KErrAlreadyExists)
			{
			INFO_PRINTF2(_L("USB - Logical device driver loaded with %d, error will be changed to KErrNone"), error);
			error = KErrNone;
			}
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("USB - Could not load logical device driver. Error %d"), error);
			}
		else
			{
			INFO_PRINTF2(_L("USB - Logical device driver loaded with %d"), error);
			}	
		SetError(error);
		}		
	INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::DoCmdUtilityLoadLogicalDevice"));
	}

/**
 * Opens a USB channel 
 */
void CT_RDevUsbcClientData::DoCmdOpen()
	{
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::DoCmdOpen"));
	// Open USB driver channel
	TInt error = iChannel->Open(USB::KUSBChannelOpenParameter);

	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("ERROR %d when opening channel"), error);
		}
	else
		{
		INFO_PRINTF2(_L("Channel open returns: %d "), error);
		}

	SetError(error);
	INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::DoCmdOpen"));
	}

/**
 * Reads device capabilities and verifies them
 * @param aSection		The section in the ini containing data for the command
 */
void CT_RDevUsbcClientData::DoCmdDeviceCaps(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::DoCmdDeviceCaps"));
	TBool supportsConnect;
	TBool dataOk = ETrue;
	if(!GetBoolFromConfig(aSection, KSupportsConnect, supportsConnect))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KSupportsConnect);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	
	TBool supportsSelfPowered;
	if(!GetBoolFromConfig(aSection, KSupportsSelfPowered, supportsSelfPowered))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KSupportsSelfPowered);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	
	TBool supportsRemoteWakeup;
	if(!GetBoolFromConfig(aSection, KSupportsRemoteWakeup, supportsRemoteWakeup))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KSupportsRemoteWakeup);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		TInt error = iChannel->DeviceCaps(iCaps);
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("Failed to read device caps [%d]"), error);
			SetError(error);
			}
		else
			{
			// Check that reported device capabilities are what they should be
			INFO_PRINTF2(_L("Device supports %d endpoints"), iCaps().iTotalEndpoints );
			INFO_PRINTF2(_L("Device supports Software-Connect: %s"), iCaps().iConnect ? _S("yes") : _S("no"));
			if(!iCaps().iConnect && supportsConnect)
				{
				ERR_PRINTF1(_L("Device supports Software-Connect feature IS NOT SUPPORTED"));
				SetBlockResult(EFail);
				}

			INFO_PRINTF2(_L("Device is Self-Powered: %s"), iCaps().iSelfPowered ? _S("yes") : _S("no"));
			if(!iCaps().iSelfPowered && supportsSelfPowered)
				{
				ERR_PRINTF1(_L("Device is Self-Powered feature IS NOT SUPPORTED"));
				SetBlockResult(EFail);
				}

			INFO_PRINTF2(_L("Device supports Remote-Wakeup:	%s"),iCaps().iRemoteWakeup ? _S("yes") : _S("no"));
			if(!iCaps().iRemoteWakeup && supportsRemoteWakeup)
				{
				ERR_PRINTF1(_L("Device supports Remote-Wakeup feature IS NOT SUPPORTED"));
				SetBlockResult(EFail);
				}
			}		
		}
	INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::DoCmdDeviceCaps"));
	}

/**
 * Reads device endpoint capabilities and prints them
 */
void CT_RDevUsbcClientData::DoCmdEndpointCaps()
	{
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::DoCmdEndpointCaps"));
	TPtr8 endpointData(reinterpret_cast<TUint8*>(iEndpointData), sizeof(iEndpointData), sizeof(iEndpointData));
	TInt error = iChannel->EndpointCaps(endpointData);

	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to read endpoint caps [%d]"), error);
		SetError(error);
		}
	else
		{
		INFO_PRINTF1(_L("User endpoint capabilities:"));
		for (TInt i = 0; i < iCaps().iTotalEndpoints; i++)
			{
			const TUsbcEndpointCaps* caps = &iEndpointData[i].iCaps;
			INFO_PRINTF4(_L("Endpoint %02d: MaxPackeSize=%s TypeDirM = 0x%08x "),
						i + 1,
						(caps->iSizes & KUsbEpSizeCont ) ? _S("Continuous") :
						((caps->iSizes & KUsbEpSize8) ? _S("8 bytes") :
						((caps->iSizes & KUsbEpSize16) ? _S("16 bytes") :
						((caps->iSizes & KUsbEpSize32) ? _S("32 bytes") :
						((caps->iSizes & KUsbEpSize64) ? _S("64 bytes") :
						((caps->iSizes & KUsbEpSize128) ? _S("128 bytes") :
						((caps->iSizes & KUsbEpSize256) ? _S("256 bytes") :
						((caps->iSizes & KUsbEpSize512) ? _S("512 bytes") :
						((caps->iSizes & KUsbEpSize1023) ? _S("1023 bytes") :
						((caps->iSizes & KUsbEpSize1024) ? _S("1024 bytes") :
						_S("Unknown")))))))))),
						caps->iTypesAndDir);
			}
		}
	INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::DoCmdEndpointCaps"));
	}


/*
 * Inits USB Device Descriptors - Get the device descriptor size
 */

void CT_RDevUsbcClientData::DoCmdGetDeviceDescriptorSize(const TTEFSectionName& aSection)
	{	
	// Read the size of the descriptors and create buffer to store them.
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::DoCmdGetDeviceDescriptorSize"));
    TInt expected;
    if(!GetIntFromConfig(aSection, KEXPECTED, expected))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KEXPECTED);
		SetBlockResult(EFail);
		}
    else
        {
        iDeviceDescriptorSize = 0;
    	INFO_PRINTF1(_L("Initializing device descriptors"));
        INFO_PRINTF1(_L("void CT_RDevUsbcClientData::GetDeviceDescriptorSize() BEGIN"));
    	iChannel->GetDeviceDescriptorSize(iDeviceDescriptorSize);	
        
        if(expected != iDeviceDescriptorSize)
            {
            ERR_PRINTF3(_L("Expected size != actual size, expected = %d, actual = %d "),expected, iDeviceDescriptorSize);
            SetBlockResult(EFail);
            }
        }
        // initialize buffer which used to verify.
        
            
    INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::DoCmdGetDeviceDescriptorSize"));
	}

/**
 * Inits USB Device Descriptors - Get device descriptor
 */

void CT_RDevUsbcClientData::DoCmdGetDeviceDescriptor(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::DoCmdGetDeviceDescriptor"));
    
    //initialize descriptor buffer.
    TRAPD(error, iBuffer = HBufC8::NewL(iDeviceDescriptorSize));
    if(error != KErrNone || !iBuffer)
    	{
    	ERR_PRINTF2(_L("Failed to initialize device descriptor [%d]"),error);
    	SetError(error);
    	}
    TRAPD(error2, iBuffer_verify = HBufC8::NewL(iDeviceDescriptorSize));
    if(error2 != KErrNone || !iBuffer_verify)
    	{
    	ERR_PRINTF2(_L("Failed to initialize device descriptor [%d]"),error2);
    	SetError(error2);
    	}  
    
	TInt err;	
	TPtr8 devDescrPtr = iBuffer->Des();
	// Read the current descriptors.	
	err = iChannel->GetDeviceDescriptor(devDescrPtr);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to get the device descriptor [%d]"),err);
		SetError(err);
		}   
        
    // Read descriptors from ini and compare.
    TInt USBVersionLow;
	TBool dataOk = ETrue;
	if(!GetIntFromConfig(aSection, KUSBVersionLow, USBVersionLow))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KUSBVersionLow);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt USBVersionHigh;
	if(!GetIntFromConfig(aSection, KUSBVersionHigh, USBVersionHigh))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KUSBVersionHigh);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt VendorIDLow;
	if(!GetIntFromConfig(aSection, KVendorIDLow, VendorIDLow))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KVendorIDLow);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt VendorIDHigh;
	if(!GetIntFromConfig(aSection, KVendorIDHigh, VendorIDHigh))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KVendorIDHigh);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	// product ID for test device
	TInt ProductIDLow;
	if(!GetIntFromConfig(aSection, KProductIDLow, ProductIDLow))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KProductIDLow);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt ProductIDHigh;
	if(!GetIntFromConfig(aSection, KProductIDHigh, ProductIDHigh))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KProductIDHigh);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	// release number
	TInt ReleaseNumberLow;
	if(!GetIntFromConfig(aSection, KReleaseNumberLow, ReleaseNumberLow))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KReleaseNumberLow);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt ReleaseNumberHigh;
	if(!GetIntFromConfig(aSection, KReleaseNumberHigh, ReleaseNumberHigh))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KReleaseNumberHigh);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}			
	// Device descriptor data is presented in bytes (Little endian)
	// defined in the .ini file
	// The following presents numeric values device uses to introduce
	// herself to USB host in enumeration process
	if(dataOk)
		{
		TPtr8 devDescrPtr_verify = iBuffer_verify->Des();
		err = iChannel->GetDeviceDescriptor(devDescrPtr_verify);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Failed to get the device descriptor [%d]"),err);
			SetError(err);
			}   
		// USB version
		devDescrPtr_verify[USB::KSpecOffset] = USBVersionLow;
		devDescrPtr_verify[USB::KSpecOffset + 1] = USBVersionHigh;

		devDescrPtr_verify[USB::KVendorOffset] = VendorIDLow;
		devDescrPtr_verify[USB::KVendorOffset + 1] = VendorIDHigh;

		// product ID for test device
		devDescrPtr_verify[USB::KProductIDOffset] = ProductIDLow;
		devDescrPtr_verify[USB::KProductIDOffset + 1] = ProductIDHigh;

		// Set release number
		devDescrPtr_verify[USB::KReleaseOffset] = ReleaseNumberLow;
		devDescrPtr_verify[USB::KReleaseOffset + 1] = ReleaseNumberHigh;

        if(devDescrPtr != devDescrPtr_verify)
            {
            ERR_PRINTF1(_L("Expected DeviceDescriptor is not the same as actual. "));
            SetBlockResult(EFail);
            }
		}	
        
        delete iBuffer;
		iBuffer = NULL;
        delete iBuffer_verify;
		iBuffer_verify = NULL;

	INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::DoCmdGetDeviceDescriptor"));
	}

/**
 * Inits USB Device Descriptors - Set device descriptor
 * @param aSection - The section in the ini containing data for the command
 */
void CT_RDevUsbcClientData::DoCmdSetDeviceDescriptor(const TTEFSectionName& aSection)
	{
	// USB version
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::DoCmdSetDeviceDescriptor"));
    
    iDeviceDescriptorSize = 0;
    INFO_PRINTF1(_L("Initializing device descriptors"));
    iChannel->GetDeviceDescriptorSize(iDeviceDescriptorSize);	
    INFO_PRINTF2(_L("Device descriptor size = %d"), iDeviceDescriptorSize);
    
    //initialize descriptor buffer.
    TRAPD(err, iBuffer = HBufC8::NewL(iDeviceDescriptorSize));
    if(err != KErrNone || !iBuffer)
    	{
    	ERR_PRINTF2(_L("Failed to initialize device descriptor [%d]"),err);
    	SetError(err);
    	}
    
	TInt error;
	TInt USBVersionLow;
	TBool dataOk = ETrue;
	if(!GetIntFromConfig(aSection, KUSBVersionLow, USBVersionLow))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KUSBVersionLow);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt USBVersionHigh;
	if(!GetIntFromConfig(aSection, KUSBVersionHigh, USBVersionHigh))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KUSBVersionHigh);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}

	TInt VendorIDLow;
	if(!GetIntFromConfig(aSection, KVendorIDLow, VendorIDLow))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KVendorIDLow);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt VendorIDHigh;
	if(!GetIntFromConfig(aSection, KVendorIDHigh, VendorIDHigh))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KVendorIDHigh);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	// product ID for test device
	TInt ProductIDLow;
	if(!GetIntFromConfig(aSection, KProductIDLow, ProductIDLow))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KProductIDLow);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt ProductIDHigh;
	if(!GetIntFromConfig(aSection, KProductIDHigh, ProductIDHigh))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KProductIDHigh);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	// release number
	TInt ReleaseNumberLow;
	if(!GetIntFromConfig(aSection, KReleaseNumberLow, ReleaseNumberLow))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KReleaseNumberLow);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt ReleaseNumberHigh;
	if(!GetIntFromConfig(aSection, KReleaseNumberHigh, ReleaseNumberHigh))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KReleaseNumberHigh);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}			
	// Device descriptor data is presented in bytes (Little endian)
	// defined in the .ini file
	// The following presents numeric values device uses to introduce
	// herself to USB host in enumeration process
	if(dataOk)
		{
		TPtr8 devDescrPtr = iBuffer->Des();
		devDescrPtr.FillZ(devDescrPtr.MaxLength());

		// USB version
		devDescrPtr[USB::KSpecOffset] = USBVersionLow;
		devDescrPtr[USB::KSpecOffset + 1] = USBVersionHigh;

		devDescrPtr[USB::KVendorOffset] = VendorIDLow;
		devDescrPtr[USB::KVendorOffset + 1] = VendorIDHigh;

		// product ID for test device
		devDescrPtr[USB::KProductIDOffset] = ProductIDLow;
		devDescrPtr[USB::KProductIDOffset + 1] = ProductIDHigh;

		// Set release number
		devDescrPtr[USB::KReleaseOffset] = ReleaseNumberLow;
		devDescrPtr[USB::KReleaseOffset + 1] = ReleaseNumberHigh;

		INFO_PRINTF1(_L("Setting device descriptors"));
		// Store the descriptors.
		error = iChannel->SetDeviceDescriptor(devDescrPtr);
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("Failed to set the device descriptor [%d]"),error);
			delete iBuffer;
			SetError(error);
			}
		else
			{
			delete iBuffer;
			iBuffer = NULL;
			}
		
		INFO_PRINTF1(_L("Device descriptors set"));
		}	
	INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::DoCmdSetDeviceDescriptor"));
	}

/**
 * Sets USB EndPoints
 * @param aSection - The section in the ini containing data for the command
 */

void CT_RDevUsbcClientData::DoCmdUtilitySetEndPointL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::DoCmdUtilitySetEndPointL"));
	TPtrC16 mode;
	TBool dataOk = ETrue;
	if(!GetStringFromConfig(aSection, KMode, mode))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMode);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TPtrC16 direction;
	if(!GetStringFromConfig(aSection, KDirection, direction))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDirection);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		TInt error(KErrNone);	
		if(mode == USB::KEPBulk)
			{
			INFO_PRINTF1(_L("Selected mode is KEPBulk"));
			if(direction == USB::KEPIn)
				{
				error = SetEndpoint(KUsbEpTypeBulk, KUsbEpDirIn);
				if (error != KErrNone)
					{
					ERR_PRINTF2(_L("Failed to set end point direction to KEPIn with error [%d]"), error);
					SetBlockResult(EFail);					
					}
				}
			else if(direction == USB::KEPOut)
				{
				error = SetEndpoint(KUsbEpTypeBulk, KUsbEpDirOut);
				if (error != KErrNone)
					{
					ERR_PRINTF2(_L("Failed to set end point direction to KEPOut with error [%d]"), error);
					SetBlockResult(EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("USB - INCORRECT direction %S"), direction.AllocL());
				SetBlockResult(EFail);
				}
			}
		else if(mode == USB::KEPInterrupt)
			{
			INFO_PRINTF1(_L("Selected mode is KEPInterrupt"));
			if(direction == USB::KEPIn)
				{
				error = SetEndpoint(KUsbEpTypeInterrupt, KUsbEpDirIn);
				if (error != KErrNone)
					{
					ERR_PRINTF2(_L("Failed to set end point direction to KEPIn with error [%d]"), error);
					SetBlockResult(EFail);
					}
				}
			else if(direction == USB::KEPOut)
				{
				error = SetEndpoint(KUsbEpTypeInterrupt, KUsbEpDirOut);
				if (error != KErrNone)
					{
					ERR_PRINTF2(_L("Failed to set end point direction to KEPOut with error [%d]"), error);
					SetBlockResult(EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("USB - INCORRECT direction %S"), direction.AllocL());
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("USB - Mode not supported %S"), mode.AllocL());
			SetBlockResult(EFail);
			}
		}			
	INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::DoCmdUtilitySetEndPointL"));
	}


/**
 * Sets a new endpoint for interface handled by this channel
 * @param aEndpointType - USB endpoint type
 * @param aEndpointDirection - USB endpoint direction	
 */
TInt CT_RDevUsbcClientData::SetEndpoint(TUint aEndpointType, TUint aEndpointDirection)
	{
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::SetEndpoint"));
	if(iConfiguredEndpoints == KMaxEndpointsPerClient)
		{
		ERR_PRINTF2(_L("Maximum number of endpoints [%d] for clients reached "), KMaxEndpointsPerClient);
		SetBlockResult(EFail);
		return KErrArgument;
		}

	// search for the endpoints that match the settings user has given.
	for (TInt i = 0; i < iCaps().iTotalEndpoints; ++i)
		{
		const TUsbcEndpointCaps* caps = &iEndpointData[i].iCaps;
		const TInt maximumPacketSize = caps->MaxPacketSize();

		// Check if endpoint in question matches with parameters we want to use for configuring an endpoint
		if ( (caps->iTypesAndDir & (aEndpointType | aEndpointDirection)) == (aEndpointType | aEndpointDirection) )
			{
			INFO_PRINTF3(_L("Configuring %s endpoint with direction %s"),
				(aEndpointType == KUsbEpTypeControl) ? _S("Control") :
				((aEndpointType == KUsbEpTypeIsochronous) ? _S("Isochronous") :
				((aEndpointType == KUsbEpTypeBulk) ? _S("Bulk") :
				((aEndpointType == KUsbEpTypeInterrupt) ? _S("Interrupt") :
				_S("Unknown")))),
				(aEndpointDirection == KUsbEpDirIn) ? _S("In") :
				((aEndpointDirection == KUsbEpDirOut) ? _S("Out") :
				((aEndpointDirection == KUsbEpDirBidirect) ? _S("Bidirectional") :
				_S("Unknown")))
				);

			iInterface().iEndpointData[iConfiguredEndpoints].iType = aEndpointType;
			iInterface().iEndpointData[iConfiguredEndpoints].iDir = aEndpointDirection;
			iInterface().iEndpointData[iConfiguredEndpoints].iSize = maximumPacketSize;
			iTxMaxPacketSize = maximumPacketSize;

			iConfiguredEndpoints++;

			break;
			}
		}

	INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::SetEndpoint"));
	return KErrNone;
	}



/**
 * Sets interface with endpoints configured with CT_RDevUsbcClientData::SetEndpoint
 * Note that endpoints must be set before calling this method
 * 
 * @param aBandwithwidthBufferingMask TInt contains IN and OUT bandwidth buffering masks combined 
 *									 using binary OR operation
 *			NOTE: IT IS TEST CASE SCRIPTERS RESPONSIBILITY TO CHECK VALID (or invalid for negative cases) MASKS
 */
void CT_RDevUsbcClientData::DoCmdSetInterface(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::DoCmdSetInterface"));
	TInt bandwidthBufferingMask;
	TBool dataOk = ETrue;
	if(!GetIntFromConfig(aSection, KBufferingMask, bandwidthBufferingMask))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KBufferingMask);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		INFO_PRINTF2(_L("bandwidthBufferingMask %d"), bandwidthBufferingMask);
		INFO_PRINTF2(_L("Configuring the interface settings with %d endpoints"), iConfiguredEndpoints);

		if( iConfiguredEndpoints == 0 )
			{
			ERR_PRINTF1(_L("No endpoints have been set! Set endpoints before configuring interface"));
			SetBlockResult(EFail);
			}
		else
			{
			HBufC16* allocatedName = 0;
			TRAPD(error, allocatedName = interfaceName().AllocL());
		    if (error != KErrNone)
		        {
		        ERR_PRINTF2(_L("AllocL failed [%d]"), error);
		        SetBlockResult(EFail);
		        }
		    else
		    	{
		    	iInterface().iString = allocatedName;
				iInterface().iTotalEndpointsUsed = iConfiguredEndpoints;
				iInterface().iClass.iClassNum = 0xff;
				iInterface().iClass.iSubClassNum = 0xff;
				iInterface().iClass.iProtocolNum = 0xff;
				INFO_PRINTF2(_L("Using bandwidth buffering mask 0x%08x"), bandwidthBufferingMask);

				error = iChannel->SetInterface(0, iInterface, bandwidthBufferingMask);

				delete allocatedName;
				allocatedName = NULL;
				
				if (error != KErrNone)
					{
					ERR_PRINTF2(_L("Failed to set the interface [%d]"), error);
					SetError(error);
					}
				INFO_PRINTF2(_L("SetInterface returns [%d]"), error);
		    	}			
			}	    
		}
	INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::DoCmdSetInterface"));
	}




/**
 * Tell test class to stop waiting 
 * @param result from callback
 */
void CT_RDevUsbcClientData::Signal(TInt aResult)
	{
	INFO_PRINTF2(_L("Signal %d"),aResult);
	}


/**
 * Destroyer
 */

void  CT_RDevUsbcClientData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START*CT_RDevUsbcClientData::DoCmdDestructor"));
	DestroyData();
	INFO_PRINTF1(_L("*END*CT_RDevUsbcClientData::DoCmdDestructor"));
	}

void CT_RDevUsbcClientData::DestroyData()
	{
	if(iChannel)
		{
		iChannel->Close();
		delete iChannel;
		iChannel = NULL;
		}
	}

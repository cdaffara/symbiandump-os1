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
// This contains T_RDevUsbcClientData.h

#ifndef T_RDEVUSBCLIENT_DATA_H
#define T_RDEVUSBCLIENT_DATA_H


// User Includes
#include "DataWrapperBase.h"
#include "USBConstants.h"


// System includes
#include <e32base.h> // CActive
#include <d32usbc.h> // RDevUsbcClient
#include <e32debug.h>
#include <f32file.h>
#include <e32cmn.h>



/**
 * CT_RDevUsbcClientData class
 */
class CT_RDevUsbcClientData : public CDataWrapperBase
	{
public:
	~CT_RDevUsbcClientData();
	static CT_RDevUsbcClientData* NewL();

	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void RunL(CActive* aActive, TInt aIndex);
	void DoCancel();
	void Signal(TInt aResult);		
	void DestroyData();
protected:
	CT_RDevUsbcClientData();

	void ConstructL();

private:
    void DoCmdNewL();
	void DoCmdUtilityLoadLogicalDevice(const TTEFSectionName& aSection);
	void DoCmdOpen();
	void DoCmdDeviceCaps(const TTEFSectionName& aSection);
	void DoCmdGetDeviceDescriptorSize(const TTEFSectionName& aSection);
	void DoCmdGetDeviceDescriptor(const TTEFSectionName& aSection);
	void DoCmdSetDeviceDescriptor(const TTEFSectionName& aSection);
	void DoCmdUtilitySetEndPointL(const TTEFSectionName& aSection);
	void DoCmdSetInterface(const TTEFSectionName& aSection);			
	void DoCmdDestructor();
	void DoCmdEndpointCaps(); 
    /**
	 * Returns current Testclass 
	 */
	TInt SetEndpoint(TUint aEndpointType, TUint aEndpointDirection);	
	
private:
/**
	* Provides handle to USB Driver channel
	* USB Interface is configured using this channel
	*/	
	RDevUsbcClient *iChannel;
	
	/**
	* Package buffer for a TUsbcInterfaceInfo object. This buffer
	* is filled prior to a call to RDevUsbcClient::SetInterface
	*/
	TUsbcInterfaceInfoBuf iInterface;
	
	/** Data structures for endpoint data, KUsbcMaxEndpoints is defined in d32usbc.h 
	 requires both endpoint data container and descripter the data is stored to*/
	TUsbcEndpointData iEndpointData[KUsbcMaxEndpoints];	
			
	/** 
	* Number of currently set endpoints, 
	* maximum is defined in d32usbc.h KMaxEndpointsPerClient
	*/
	TInt iConfiguredEndpoints;
			

	/**
	* Maximum packet size for sending.
	*/
	TInt iTxMaxPacketSize;
	
	/**
	 * Capabilities
	 */
	TUsbDeviceCaps iCaps;	
	
	/**
	 * Active Callback for asynchronous functions
	 */
	CActiveCallback* iActiveCallback;
		
	/**
	 * Buffer for init device.
	 */
	HBufC8* iBuffer;	
    HBufC8* iBuffer_verify;
   	/**
	 * Descriptor size.
	 */ 
    TInt iDeviceDescriptorSize;
	/**
	 * Enum for the resources of the Endpoint
	 */
	static const TEnumEntryTable iEUsbcEndpointResource[];	
	
	};


#endif // T_RDEVUSBCLIENT_DATA_H


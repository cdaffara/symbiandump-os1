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
//


#ifndef __MMFCONTROLLEREXTENDEDDATA_H__
#define __MMFCONTROLLEREXTENDEDDATA_H__

#include <mmf/server/mmfdatasource.h>
#include <mmf/server/mmfdatasink.h>
#include <mmf/common/mmfcontrollerframework.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <mmfplugininterfaceuids.hrh>

#include <caf/caf.h>

/**
@internalComponent
*/
const TUid KUidInterfaceMMFDataSinkHolder = {0x101F76D4};
/**
@internalComponent
*/
const TUid KUidInterfaceMMFDataSourceHolder = {0x101F76D5};
/**
@internalComponent
*/
const TUid KUidMMFControllerExtendedDataHolder = {0x102834AC};

/**
@internalComponent

Create a const TUid with the same values as the controller
ECOM plugin interface UID
*/
const TUid KUidInterfaceMMFController = {KMmfUidPluginInterfaceController};

/**
@internalComponent

IPC messages for interface KUidInterfaceMMFController in the controller framework.
*/
enum TMMFControllerMessages
	{
	EMMFControllerAddDataSource,
	EMMFControllerAddDataSink,
	EMMFControllerRemoveDataSource,
	EMMFControllerRemoveDataSink,
	EMMFControllerReset,
	EMMFControllerPrime,
	EMMFControllerPlay,
	EMMFControllerPause,
	EMMFControllerStop,
	EMMFControllerGetPosition,
	EMMFControllerSetPosition,
	EMMFControllerGetDuration,
	EMMFControllerGetNumberOfMetaDataEntries,
	EMMFControllerGetSizeOfMetaDataEntry,//use internally
	EMMFControllerGetMetaDataEntry,
	EMMFControllerSetPrioritySettings,
	EMMFControllerCancelAddDataSource,
	EMMFControllerCancelAddDataSink,
	EMMFControllerAddFileHandleDataSource,
	EMMFControllerAddFileHandleDataSink,
	EMMFControllerSourceSinkInitDataPreload,
	EMMFControllerAddFileHandleDataSourceWithInitData,
	EMMFControllerAddFileHandleDataSinkWithInitData
	};

/**
@internalComponent

The controller proxy session.

Only one session can be connected to a controller proxy server.

*/
NONSHARABLE_CLASS(CMMFControllerExtendedData) : public CMMFObject
	{
public:
	CMMFControllerExtendedData();
	
	~CMMFControllerExtendedData();
	
	// Set/Get the source/sink initialization data used by CMMFController
	// Ownership of aSourceSinkInitData is transferred (NULL pointer is allow)
	// If source/sink initialization data is already set, previous one will be destroyed
	void SetSourceSinkInitData(HBufC8* aSourceSinkInitData);
	HBufC8* SourceSinkInitData() const;
	void ResetSourceSinkInitData();

	// Set/Get the client thread ID used by CMMFController
	void SetClientThreadId(TThreadId aClientThreadId);
	TThreadId ClientThreadId() const;
	
	// Set/Get the Secure DRM mode of CMMFController
	void SetSecureDrmMode(TBool aSecureDrmMode);
	TBool SecureDrmMode() const;
		
	//from CMMFObject
	void HandleRequest(TMMFMessage& aMessage);
private:
	/**
	Extended data uses by CMMFController: Source/Sink initialization data
	It serves as a temporary storage for Source/Sink initialization
	Client should request the server to use and cleanup this variable ASAP
	once it is being loaded
	*/
	HBufC8*	iSourceSinkInitData;
	
	/**
	Extended data uses by CMMFController: The client thread Id
	*/
	TThreadId iClientThreadId;
	
	/**
	Extended data uses by CMMFController: Controller Scecure DRM Mode
	*/
	TBool iSecureDrmMode;
	};

#endif //__MMFCONTROLLEREXTENDEDDATA_H__

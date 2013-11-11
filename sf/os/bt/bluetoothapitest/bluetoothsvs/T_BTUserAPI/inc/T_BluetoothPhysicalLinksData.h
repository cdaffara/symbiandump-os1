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


#if (!defined __T_BLUETOOTH_PHYSICAL_LINKS_DATA_H__)
#define __T_BLUETOOTH_PHYSICAL_LINKS_DATA_H__
	
// User Includes
#include "DataWrapperBase.h"

// EPOC includes
#include <bt_sock.h>
#include <bttypes.h>

class CT_BluetoothPhysicalLinksData : public CDataWrapperBase, private MBluetoothPhysicalLinksNotifier
	{
public:
	/**
	* Public destructor
	*/
	~CT_BluetoothPhysicalLinksData();	

	/**
	* Two phase constructor
	*/
	static CT_BluetoothPhysicalLinksData* NewL();

	/**
	* Process a command read from the ini file
	*
	* @param aCommand			The command to process
	* @param aSection			The section in the ini containing data for the command
	* @param aAsyncErrorIndex	Command index for async calls to return errors to
	*
	* @return					ETrue if the command is processed
	*
	* @leave					System wide error
	*/
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	/**
	* Return a pointer to the object that the data wraps
	*
	* @return	pointer to the object that the data wraps
	*/
	virtual TAny*	GetObject();

	/**
	* Set the object that the data wraps
	*
	* @param	aObject object that the wrapper is testing
	*
	* @leave	KErrNotSupported if the the function is not supported
	*/
	virtual void	SetObjectL(TAny* aObject);

	/**
	* The object will no longer be owned by this
	*
	* @leave	KErrNotSupported if the the function is not supported
	*/
	virtual void	DisownObjectL();
	
	inline virtual TCleanupOperation CleanupOperation();

protected:
	CT_BluetoothPhysicalLinksData();
	void ConstructL();

private:
	void DestroyData();
	
	//	MBluetoothPhysicalLinksNotifier implementation
	virtual void	HandleCreateConnectionCompleteL(TInt aErr);
	virtual void	HandleDisconnectCompleteL(TInt aErr);
	virtual void	HandleDisconnectAllCompleteL(TInt aErr);
	
	// Implementation of DoCommandL for each of the commands
	inline void	DoCmdNewL(const TDesC& aSection);
	inline void	DoCmdNewLC(const TDesC& aSection);
	inline void	DoCmdCreateConnection(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdCancelCreateConnection();
	inline void	DoCmdDisconnect(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdDisconnectAll(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void	DoCmdBroadcast(const TDesC& aSection);
	inline void	DoCmdReadRaw(const TDesC& aSection);
	inline void	DoCmdEnumerate(const TDesC& aSection);
	inline void	DoCmdDestructor();
	inline void	DoCmdMBPLN_ExtensionInterfaceL(const TDesC& aSection);
	
	static void CleanupOperation(TAny* aAny);
	
private:
	CBluetoothPhysicalLinks*	iData;
	TInt						iErrForCreateConnection;
	TInt						iErrForDisconnect;
	TInt						iErrForDisconnectAll;
	TInt						iDelayAfterCreateConnection;
	TBTDevAddr					iBTDeveAddr;
	TInt						iCreateIndex;
	TInt						iDisconnectIndex;
	TInt						iDisconnectAllIndex;
	};

#endif /* __T_BLUETOOTH_PHYSICAL_LINKS_DATA_H__*/

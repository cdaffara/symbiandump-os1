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


#if (!defined __T_BT_DEV_ADDRDATA_H__)
#define __T_BT_DEV_ADDRDATA_H__

// Classes to get to protected members
#include "T_TRfcommSockAddrChild.h"
#include "T_TBTSockAddrChild.h"

// User includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <w32std.h>
#include <e32std.h>
#include <es_sock.h> // TSockAddr address 
#include <bt_sock.h> // Bluetooth address classes


class CT_BTDevAddrData: public CDataWrapperBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_BTDevAddrData*	NewL();

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
	* Set the internal iBTDevAddr object using the copy constructor
	*/	

	void CopyObjectL(TAny* aObjectPtr);

	/**
	* Destructor
	*/
	~CT_BTDevAddrData();
	
	const TBTDevAddr*	GetAddress();
	
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

	//	MT_MMActiveCallback implementation
	virtual void			RunL(CActive* aActive, const TInt aIndex);
	virtual void			DoCancel(CActive* aActive, const TInt aIndex);
	inline void				RunGetDeviceAddress(const TInt aIndex);

	inline virtual TCleanupOperation CleanupOperation();

protected:
	/**
	* Constructor. First phase construction
	*/
	CT_BTDevAddrData();

	/**
	* Second phase construction
	*
	* @leave	system wide error
	*/
	void	ConstructL();
   	
private:
	// Implementation of DoCommandL for each of the commands
	inline void TBTDAConstruct(const TDesC& aSection);
	inline void TBTDASetAddress(const TDesC& aSection);

	inline void OpenHostResolver(const TDesC& aSection);
	inline void SetHostNameL(const TDesC& aSection);
	inline void SetDiscoverable(const TDesC& aSection);
	inline void GetDeviceAddress(const TDesC& aSection, const TInt aIndex);


	/**
	* Delete iTBTSockAddr and set to NULL
	*/
	void Cleanup();	

	static void CleanupOperation(TAny* aAny);

private:
	// An object of TBTDevAddr class in order to retain state between 
	// calls
    TBTDevAddr*				iBTDevAddr; 

	CActiveCallback*		iActive;

	TNameEntry				iBtNameEntry;
	RHostResolver			iHostResolver;
	TInquirySockAddr		iAddr;
	TPtrC					iDeviceName;
	TBool					iHostResolverOpen;

	};

#endif /* __T_BT_DEV_ADDRDATA_H__ */

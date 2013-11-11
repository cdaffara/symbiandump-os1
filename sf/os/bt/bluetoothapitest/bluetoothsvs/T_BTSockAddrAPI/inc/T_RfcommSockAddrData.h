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


#if (!defined __T_BT_SOCK_ADDR_API_TRFCOMMSOCKADDR_DATA_H__)
#define __T_BT_SOCK_ADDR_API_TRFCOMMSOCKADDR_DATA_H__

//	User Includes
#include "DataWrapperBase.h"
// Classes to get to protected members
#include "T_TRfcommSockAddrChild.h"

//	EPOC includes
#include <w32std.h>
#include <e32std.h>
#include <es_sock.h> // TSockAddr address 
#include <bt_sock.h> // Bluetooth address classes


class CT_TRfcommSockAddrData: public CDataWrapperBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_TRfcommSockAddrData*	NewL();
	
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
	* Destructor
	*/
	~CT_TRfcommSockAddrData();

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
	/**
	* Protected constructor. First phase construction
	*/
	CT_TRfcommSockAddrData();
	
		/**
	* Return a pointer to the object that the data wraps
	*
	* \return pointer to the object that the data wraps
	*/
	virtual TAny*	GetObject();
private:	
	// Implementation of DoCommandL for each of the commands
	inline void TRfcommSockAddrConstruct(const TDesC& aSection);
	inline void TRfcommSockAddrCastFromTSockAddr(const TDesC& aSection);
	inline void TRfcommSockAddrSetBTAddr(const TDesC& aSection);
	inline void TRfcommSockAddrBTAddr(const TDesC& aSection);
	inline void TRfcommSockAddrSetSecurity(const TDesC& aSection);
	inline void TRfcommSockAddrBTSecurity(const TDesC& aSection);
	inline void TRfcommSockAddrSetFamily(const TDesC& aSection);
	inline void TRfcommSockAddrFamily(const TDesC& aSection);
	inline void TRfcommSockAddrSetPort(const TDesC& aSection);
	inline void TRfcommSockAddrPort(const TDesC& aSection);
	inline void TRfcommSockAddrCmpPort(const TDesC& aSection);
	inline void	TRfcommSockAddrPROTECTEDSetUserLen(const TDesC& aSection);
	inline void TRfcommSockAddrGetUserLen(const TDesC& aSection);
	inline void TRfcommSockAddrPROTECTEDUserPtr();
	inline void TRfcommSockAddrPROTECTEDEndBTSockAddrPtr();

	/**
	* Delete iTBTSockAddr if owned and set to 0
	*/
	void Cleanup();	
	
	static void CleanupOperation(TAny* aAny);

private:
	// An object of iT_TRfcommSockAddrChild class in order to retain state between calls
	// T_TRfcommSockAddrChild is a wrapper around the TRfcommSockAddr to reach protected methods

    T_TRfcommSockAddrChild* iTRfcommSockAddr;
    // Identifies if the iTRfcommSockAddr is owned, needed due to cast
    TBool iTRfcommSockAddrOwned;
    
	};
#endif /* __T_BT_SOCK_ADDR_API_TRFCOMMSOCKADDR_DATA_H__ */

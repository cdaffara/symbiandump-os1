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


#if (!defined __T_BT_SOCK_ADDR_DATA_H__)
#define __T_BT_SOCK_ADDR_DATA_H__

//	User Includes
#include "DataWrapperBase.h"
// Classes to get to protected members
#include "T_TRfcommSockAddrChild.h"
#include "T_TBTSockAddrChild.h"

//	EPOC includes
#include <w32std.h>
#include <e32std.h>
#include <es_sock.h> // TSockAddr address 
#include <bt_sock.h> // Bluetooth address classes


class CT_BTSockAddrData: public CDataWrapperBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_BTSockAddrData*	NewL();
	
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
	~CT_BTSockAddrData();
	
	/**
	* Set object from outside so that it can be verified
	*/	
	void SetObject(T_TBTSockAddrChild* aTBTSockAddrChild, TBool aTransferOwnership);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_BTSockAddrData();
	
		/**
	* Return a pointer to the object that the data wraps
	*
	* \return pointer to the object that the data wraps
	*/
	virtual TAny*	GetObject();
private:	
	// Implementation of DoCommandL for each of the commands
	inline void TBTSockAddrConstruct(const TDesC& aSection);
	inline void TBTSockAddrCastFromTSockAddr(const TDesC& aSection);
	inline void TBTSockAddrSetBTAddr(const TDesC& aSection);
	inline void TBTSockAddrBTAddr(const TDesC& aSection);
	inline void TBTSockAddrSetSecurity(const TDesC& aSection);
	inline void TBTSockAddrBTSecurity(const TDesC& aSection);
	inline void TBTSockAddrSetFamily(const TDesC& aSection);
	inline void TBTSockAddrFamily(const TDesC& aSection);
	inline void TBTSockAddrSetPort(const TDesC& aSection);
	inline void TBTSockAddrPort(const TDesC& aSection);
	inline void TBTSockAddrCmpPort(const TDesC& aSection);
	inline void TBTSockAddrPROTECTEDSetUserLen(const TDesC& aSection);
	inline void TBTSockAddrGetUserLen(const TDesC& aSection);
	inline void TBTSockAddrPROTECTEDUserPtr();
	inline void TBTSockAddrPROTECTEDEndBTSockAddrPtr();

	/**
	* Delete iTBTSockAddr if owned and set to 0
	*/
	void Cleanup();	

private:
	// An object of T_TBTSockAddrChild class in order to retain state between calls       
	// T_TBTSockAddrChild is a wrapper around the TBTSockAddr to reach protected methods
    T_TBTSockAddrChild*	iTBTSockAddr;
    // Identifies if the iTBTSockAddr is owned, needed due to cast
    TBool iTBTSockAddrOwned;
    
	};

#endif /* __T_BT_SOCK_ADDR_DATA_H__ */

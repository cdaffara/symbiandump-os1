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


#if (!defined __T_SOCK_ADDR_DATA_H__)
#define __T_SOCK_ADDR_DATA_H__

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


class CT_SockAddrData: public CDataWrapperBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_SockAddrData*	NewL();
	
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
	~CT_SockAddrData();

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_SockAddrData();
	
	/**
	* Delete iTSockAddr if owned and set to 0
	*/
	void Cleanup();	
	/**
	* Return a pointer to the object that the data wraps
	*
	* \return pointer to the object that the data wraps
	*/
	virtual TAny*	GetObject();	

private:
	// An object of TSockAddr class in order to retain state between calls    
    TSockAddr*               iTSockAddr;    
	};
	
#endif /* __T_SOCK_ADDR_DATA_H__ */

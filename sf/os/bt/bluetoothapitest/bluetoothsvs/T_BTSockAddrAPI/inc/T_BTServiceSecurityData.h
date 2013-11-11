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


#if (!defined __T_BT_SERVICE_SECURITYDATA_H__)
#define __T_BT_SERVICE_SECURITYDATA_H__

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


class CT_BTServiceSecurityData: public CDataWrapperBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_BTServiceSecurityData*	NewL();

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
	* Set the internal iBTServiceSecurity object using the copy constructor
	*/	

	void CopyObjectL(TAny* aObjectPtr);

	/**
	* Destructor
	*/
	~CT_BTServiceSecurityData();

	inline virtual TCleanupOperation CleanupOperation();

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_BTServiceSecurityData();

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

private:
	// Implementation of DoCommandL for each of the commands
	inline void TBTSSConstruct(const TDesC& aSection);
	inline void TBTSSSetAuthentication(const TDesC& aSection);
	inline void TBTSSAuthenticationRequired(const TDesC& aSection);
	inline void TBTSSSetAuthorisation(const TDesC& aSection);
	inline void TBTSSAuthorisationRequired(const TDesC& aSection);
	inline void TBTSSSetDenied(const TDesC& aSection);
	inline void TBTSSDenied(const TDesC& aSection);
	inline void TBTSSSetEncryption(const TDesC& aSection);
	inline void TBTSSEncryptionRequired(const TDesC& aSection);
	inline void TBTSSSetPasskeyMinLength(const TDesC& aSection);
	inline void TBTSSPasskeyMinLength(const TDesC& aSection);
	inline void TBTSSSetUid(const TDesC& aSection);
	inline void	TBTSSUid(const TDesC& aSection);

	/**
	* Delete iTBTSockAddr and set to 0
	*/
	void Cleanup();	

	static void CleanupOperation(TAny* aAny);

private:
	// An object of TBTServiceSecurity class in order to retain state between 
	// calls
    TBTServiceSecurity*		iBTServiceSecurity; 
	};

#endif /* __T_BT_SERVICE_SECURITYDATA_H__ */

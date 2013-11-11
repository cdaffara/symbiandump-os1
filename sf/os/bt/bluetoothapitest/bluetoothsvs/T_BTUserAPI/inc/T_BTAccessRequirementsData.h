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


#if (!defined __T_BT_ACCESS_REQUIREMENTS_DATA_H__)
#define __T_BT_ACCESS_REQUIREMENTS_DATA_H__
	
// User Includes
#include "DataWrapperBase.h"

// EPOC includes
#include <bt_sock.h>

class CT_BTAccessRequirementsData : public CDataWrapperBase
	{
public:
	/**
	* Public destructor
	*/
	~CT_BTAccessRequirementsData();

	/**
	* Two phase constructor
	*/
	static CT_BTAccessRequirementsData*	NewL();

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
	CT_BTAccessRequirementsData();
	void ConstructL();

private:
	/**
	* Helper methods
	*/
	void DestroyData();
	
	inline void	DoCmdTBTAccessRequirements();
	inline void DoCmdSetAuthentication(const TDesC& aSection);
	inline void DoCmdSetAuthorisation(const TDesC& aSection);
	inline void DoCmdSetEncryption(const TDesC& aSection);
	inline void DoCmdSetDenied(const TDesC& aSection);
	inline void DoCmdSetPasskeyMinLength(const TDesC& aSection);
	inline void DoCmdAuthenticationRequired(const TDesC& aSection);
	inline void DoCmdAuthorisationRequired(const TDesC& aSection);
	inline void DoCmdEncryptionRequired(const TDesC& aSection);
	inline void DoCmdDenied(const TDesC& aSection);
	inline void DoCmdPasskeyMinLength(const TDesC& aSection);
	
	static void CleanupOperation(TAny* aAny);
	
private:
	TBTAccessRequirements*	iData;
	};

#endif /* __T_BT_ACCESS_REQUIREMENTS_DATA_H__*/

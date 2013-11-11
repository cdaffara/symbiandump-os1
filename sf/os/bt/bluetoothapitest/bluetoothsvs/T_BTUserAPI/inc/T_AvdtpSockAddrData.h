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


#if (!defined __T_AVDTP_SOCKADDR_DATA_H__)
#define __T_AVDTP_SOCKADDR_DATA_H__
	
// User Includes
#include "DataWrapperBase.h"

class CT_AvdtpSockAddrData : public CDataWrapperBase
	{
public:
	/**
	* Public destructor
	*/
	~CT_AvdtpSockAddrData();
	
	/**
	* Two phase constructor
	*/
	static CT_AvdtpSockAddrData*	NewL();

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
	* Set object from outside so that it can be verified
	*/	
	void SetObject(TAvdtpSockAddr* aAvdtpSockAddr, TBool aTransferOwnership);

	/**
	* The object will no longer be owned by this
	*
	* @leave	KErrNotSupported if the the function is not supported
	*/
	virtual void	DisownObjectL();

	inline virtual TCleanupOperation CleanupOperation();
		
protected:
	CT_AvdtpSockAddrData();
	void ConstructL();

private:
	void DestroyData();
	
	inline void DoCmdnewL(const TDesC& aSection);
	inline void DoCmdCast(const TDesC& aSection);
	inline void DoCmdSetSession(const TDesC& aSection);
	inline void DoCmdSession(const TDesC& aSection);
	inline void DoCmdSetSEID(const TDesC& aSection);
	inline void DoCmdSEID(const TDesC& aSection);
	inline void DoCmdFamily(const TDesC& aSection);
	inline void DoCmdGetUserLen(const TDesC& aSection);
	inline void DoCmdPort(const TDesC& aSection);
	inline void DoCmdBTAddr(const TDesC& aSection);
	inline void DoCmdBTSecurity(const TDesC& aSection);
	
	static void CleanupOperation(TAny* aAny);

private:
	TAvdtpSockAddr* iData;
	
    // Identifies if the data is owned, needed due to cast
    TBool iDataOwned;
	};

#endif /* __T_AVDTP_SOCKADDR_DATA_H__*/

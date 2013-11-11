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


#if (!defined __T_INQUIRY_SOCKADDR__DATA_H__)
#define __T_INQUIRY_SOCKADDR__DATA_H__
	
// User Includes
#include "DataWrapperBase.h"

// Classes to get to protected members
#include "T_InquirySockAddrChild.h"

// EPOC includes
#include <bt_sock.h>
#include <es_sock.h>

/**
 * Test Socket address class used for inquiries
 *
 */
class CT_InquirySockAddrData : public CDataWrapperBase
	{
public:
	/**
	* Public destructor
	*/
	~CT_InquirySockAddrData();

	/**
	* Two phase constructor
	*/
	static CT_InquirySockAddrData*	NewL();

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
	CT_InquirySockAddrData();
	void ConstructL();

private:
	void DestroyData();
	
	inline void	DoCmdTInquirySockAddr(const TDesC& aSection);
	inline void DoCmdBTAddr(const TDesC& aSection);
	inline void DoCmdSetBTAddr(const TDesC& aSection);
	inline void DoCmdCast(const TDesC& aSection);
	inline void DoCmdMajorServiceClass(const TDesC& aSection);
	inline void DoCmdSetMajorServiceClass(const TDesC& aSection);
	inline void DoCmdMajorClassOfDevice(const TDesC& aSection);
	inline void DoCmdSetMajorClassOfDevice(const TDesC& aSection);
	inline void DoCmdMinorClassOfDevice(const TDesC& aSection);
	inline void DoCmdSetMinorClassOfDevice(const TDesC& aSection);
	inline void DoCmdIAC(const TDesC& aSection);
	inline void DoCmdSetIAC(const TDesC& aSection);
	inline void DoCmdAction(const TDesC& aSection);
	inline void DoCmdSetAction(const TDesC& aSection);
	inline void DoCmdVersion(const TDesC& aSection);
	inline void DoCmdPROTECTEDFormatTypeField(const TDesC& aSection);

	//RHostResolver
	inline void DoCmdOpenHostResolver(const TDesC& aSection);
	inline void DoCmdGetByAddress(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void DoCmdSetHostName(const TDesC& aSection);
	inline void SetDiscoverable(const TDesC& aSection);
	
	static void CleanupOperation(TAny* aAny);
	
	virtual void RunL(CActive* aActive, TInt aIndex);
	virtual void DoCancel(CActive* aActive, TInt aIndex);
	inline void RunGetDeviceAddress(TInt aIndex);
	inline void DoCancelGetDeviceAddress();
	
private:
	T_InquirySockAddrChild*		iData;
	TBool						iTInquirySockAddrOwned;
	TInquirySockAddr 			iSockAddr;
	TBTDevAddr* 				iBTDevAddr;
		
	//RHostResolver
	CActiveCallback*			iActive;
	RHostResolver				iHostResolver;
	TBool						iHostResolverOpen;
	TNameEntry					iBtNameEntry;
	TPtrC						iDeviceName;
	};

#endif /* __T_INQUIRY_SOCKADDR__DATA_H__ */

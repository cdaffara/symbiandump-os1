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


#if	(!defined __T_DATA_RSDP_H__ )
#define	__T_DATA_RSDP__H__

//	EPOC Includes
#include <btsdp.h>

// User includes
#include "DataWrapperBase.h"

/**
 * Test	Active Notification	class
 *
 */
class CT_DataRSdp :	public CDataWrapperBase
	{
public:
	/**
	* Two phase	constructor
	*/
	static CT_DataRSdp*	NewL();

	/**
	* Public destructor
	*/
	~CT_DataRSdp();

	/**
	* Process a	command	read from the ini file
	*
	* @param aDataWrapper	test step requiring	command	to be processed
	* @param aCommand	the	command	to process
	* @param aSection		the	entry in the ini file requiring	the	command	to be processed
	*
	* @return ETrue	if the command is processed
	*/
	virtual	TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	/**
	* Return a pointer to the object that the data wraps
	*
	* @return	pointer	to the object that the data	wraps
	*/
	virtual	TAny*	GetObject()	{ return iRsdp; }

	/**
	* Set the object that the data wraps
	*
	* @param	aObject	object that	the	wrapper	is testing
	*
	*/
	virtual	void	SetObjectL(TAny* aAny);

	/**
	* The object will no longer be owned by this
	*
	* @leave	KErrNotSupported if the the function is not supported
	*/
	virtual void	DisownObjectL();

	inline virtual TCleanupOperation CleanupOperation();
	
protected:
	/**
	* Protected	constructor. First phase construction
	*/
	CT_DataRSdp();

	/**
	* Second phase construction
	*/
	void	ConstructL();

private:

	static void CleanupOperation(TAny* aAny);

	/**
	* Helper methods
	*/
	void		DestroyData();	
	inline	void DoCmdConstructor();
	inline	void DoCmdConnect();
	inline	void DoCmdClose();
	inline	void DoCmdVersion(const TDesC& aSection);
	inline	void DoCmdResourceCountMarkStart();
	inline	void DoCmdResourceCountMarkEnd();
	inline	void DoCmdResourceCount(const TDesC& aSection);	

private:
	RSdp*						iRsdp;
	};

#endif /* __T_DATA_RSDP_H__*/

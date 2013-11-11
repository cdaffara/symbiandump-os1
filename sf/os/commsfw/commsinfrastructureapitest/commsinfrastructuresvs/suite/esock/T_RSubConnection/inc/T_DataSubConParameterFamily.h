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


#ifndef __T_DATA_SUBCONPARAMETERFAMILY__
#define __T_DATA_SUBCONPARAMETERFAMILY__

//	User Includes
#include "T_DataCommsBase.h"

//	EPOC includes
#include <es_sock.h>



class CT_DataSubConParameterFamily: public CT_DataCommsBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_DataSubConParameterFamily*	NewL();
	
	/**
	* Public destructor
	*/
	~CT_DataSubConParameterFamily();	
	
	/**
	* Process a command read from the ini file
	*
	* \param aTestStep	test step requiring command to be processed
	* \param aCommand	the command to process
	* \param aEntry		the entry in the ini file requiring the command to be processed
	*
	* \return ETrue if the command is processed
	*/
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	


protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_DataSubConParameterFamily();
	
		/**
	* Return a pointer to the object that the data wraps
	*
	* \return pointer to the object that the data wraps
	*/
	virtual TAny*	GetObject();

private:
	// DoCommandL delegated test methods
	void							DoCmdConstructorL( const TDesC& aSection );
	void							DoCmdDestructor();	

private:
	CSubConParameterFamily*         iSubConParameterFamily;    
	};
	
_LIT(KDataSubConParameterFamily,	"CSubConParameterFamily");

#endif /* __T_DATA_SUBCONPARAMETERFAMILY__ */

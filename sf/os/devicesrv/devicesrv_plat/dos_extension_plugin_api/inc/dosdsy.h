/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: ?Description
*
*/


#ifndef __DOSDSY_H__
#define __DOSDSY_H__

#include <e32base.h>
#include <bttypes.h> // THCIConnHandle, TBDAddress
#include <DosSvrServices.h> // TBTDeviceType

class CDosExtensionBase;

/**
* Abstract class that defines the mandatory functions to be written in 
* the plug-in when creating the licensee specific Factory class.
* The Factory class is responsible for creating and delivering services 
* to the Dos Server on its demand.
*/
class MDosFactoryBaseDSY
{
public:
	/**
	* The implementation should create a licensee specific Extension Service.
	* @return A pointer to the newly created object.
	*/
	virtual CDosExtensionBase* NewExtensionServiceL()=0;

	
};

/**
* Abstract class that defines the mandatory functions to be written in 
* the plug-in when creating the licensee specific Extension service class.
* The Extension service is responsible for completing the RDosExtension 
* requests.
*/
class MDosExtensionBaseDSY
{
public:

	/**
	* Implements a raw function call. The implementation is supposed to interpret the parameters and
	* satisfy the requests.
	* @param aFunc Unique integer among the extended functions what function we are calling.
	* @param aParameter A pointer to the parameter.
	* @param aParLength Length of the parameter.
	* @param aParameterModified ETrue if the content of aParameter has changed, EFalse otherwise
	* @return Symbian error code.
	*/
	virtual TInt CallFunctionL(TInt aFunc , TAny *aParameter , TInt aParLength, TBool& aParameterModified)=0;

	/**
	* Implements a raw function call. The implementation is supposed to interpret the parameters and
	* satisfy the requests. The implemtation should also complete the message.
	* @param aFunc Unique integer among the extended functions what function we are calling.
	* @param aParameter A pointer to the parameter.
	* @param aParLength Length of the parameter.
	* @param aMessage The message to be completed.
	* @return Symbian error code.
	*/
	virtual void CallFunctionAndCompleteL(TInt aFunc , TAny *aParameter , TInt aParLength, const RMessage2& aMessage)=0;
};

#endif //__DOSDSY_H__

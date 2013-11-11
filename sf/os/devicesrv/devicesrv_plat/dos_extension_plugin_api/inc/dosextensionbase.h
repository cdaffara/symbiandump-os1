/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*    This is the definition for CDosExtensionBase class.
*
*/



#ifndef __DOSEXTENSIONBASE_H__
#define __DOSEXTENSIONBASE_H__

#include "dosdsy.h"
#include "dosservice.h"

/**
* Server side object to used to extend the functionality of the Dos Server.
* It corresponds with RDosExtension class in the client side and the plug-in
* side Extension service derives from this,
*/
class CDosExtensionBase : public CDosService, public MDosExtensionBaseDSY
{
private:
	/**
	* Processes the message sent by the client.
	* @param aMessage Client-server message.
	* @return Symbian error code.
	*/
#ifdef __ARMCC__
	IMPORT_C virtual TInt ExecuteMessageL(const RMessage2& aMessage);
#else
	IMPORT_C TInt ExecuteMessageL(const RMessage2& aMessage);
#endif

protected:
	/**
	* Completes a non-autocompleted request.
	* @param aMessage The message to complete.
	* @param aError Symbian error code.
	* @param aPar A pointer to the parameter.
	* @param aParLength Length of the parameter.
	* @param aParameterModified ETrue indicates that the parameters were modified and the new values
	* need to be copied into the client's thread
	*/
	IMPORT_C void CompleteRequest(const RMessage2& aMessage, TInt aError, TAny* aPar=NULL, TInt aParLength=0, TBool aParameterModified=EFalse) const;
};

#endif //__DOSEXTENSIONBASE_H__

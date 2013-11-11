// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "messageutils.h"

_LIT(KServerMessageUtilsWrite16, "RMessageWrite16");
_LIT(KServerMessageUtilsWrite8, "RMessageWrite8");
_LIT(KServerMessageUtilsRead16, "RMessageRead16");
_LIT(KServerMessageUtilsRead8, "RMessageRead8");

/** 
write data in buffer to message by index

@param aMessage A reference to RMessage2 object
@param aIndex Index of message object
@param aBuf 16 bits data buffer to write to message object
@return Symbian standard error code
@internalTechnology
@released
 */
TInt MessageUtils::Write(const RMessage2& aMessage, const TInt aIndex, const TDesC16& aBuf)
	{
	TInt err = aMessage.Write(aIndex, aBuf); // assume a zero offset in the client side buffer
	if(err)
		{
		PanicClient(aMessage, KServerMessageUtilsWrite16, err);
		}
	// also return the error code so that a server can cleanup is required...
	return err;
	}
	
/** 
write data in buffer to message by index

@param aMessage A reference to RMessage2 object
@param aIndex Index of message object
@param aBuf 8 bits data buffer to write to message object
@return Symbian standard error code
@internalTechnology
@released
 */
 TInt MessageUtils::Write(const RMessage2& aMessage, const TInt aIndex, const TDesC8& aBuf)
	{
	TInt err = aMessage.Write(aIndex, aBuf); // assume a zero offset in the client side buffer
	if(err)
		{
		PanicClient(aMessage, KServerMessageUtilsWrite8, err);
		}
	// also return the error code so that a server can cleanup is required...
	return err;
	}

/** 
read data to buffer from message by index

@param aMessage A reference to RMessage2 object
@param aIndex Index of message object
@param aBuf 16 bits data buffer to store data from message object
@return Symbian standard error code
@internalTechnology
@released
 */	
TInt MessageUtils::Read(const RMessage2& aMessage, const TInt aIndex, TDes16& aBuf)
	{
	TInt err = aMessage.Read(aIndex, aBuf); // assume a zero offset in the buffer
	if(err)
		{
		PanicClient(aMessage, KServerMessageUtilsRead16, err);
		}
	// also return the error code so that a server can cleanup is required...
	return err;
	}

/** 
read data to buffer from message by index

@param aMessage A reference to RMessage2 object
@param aIndex Index of message object
@param aBuf 8 bits data buffer to store data from message object
@return Symbian standard error code
@internalTechnology
@released
 */	
TInt MessageUtils::Read(const RMessage2& aMessage, const TInt aIndex, TDes8& aBuf)
	{
	TInt err = aMessage.Read(aIndex, aBuf); // assume a zero offset in the buffer
	if(err)
		{
		PanicClient(aMessage, KServerMessageUtilsRead8, err);
		}	
	// also return the error code so that a server can cleanup is required...
	return err;
	}
	
/** 
Complete the message by sending a panic to client

@param aMessage A reference to RMessage2 object
@param aCat Panic category
@param aReason Reason to panic the client
@internalTechnology
@released
 */	
void MessageUtils::PanicClient(const RMessage2& aMessage, const TDesC& aCat, const TInt aReason)
	{
	aMessage.Panic(aCat, aReason); // will also complete the RMessage
	}
	

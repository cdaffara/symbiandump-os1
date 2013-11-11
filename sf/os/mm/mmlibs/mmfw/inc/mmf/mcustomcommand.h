//mcustomcommand.h

/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @publishedPartner
 @released
*/

#ifndef __MCUSTOMCOMMAND_H
#define __MCUSTOMCOMMAND_H

#include <e32std.h>
#include <mmf/common/mmfcontrollerframework.h>

class MCustomCommand
	{
public:
	/*
	Send a synchronous custom command to server side.
	@param  aDestination. The destination of the custom command.
	@param  aFunction. Custom command. Meaning is dependent on Custom Interface in use.
	@param  aDataTo1. A buffer of data to be supplied to the receiver - meaning of contents dependent on CI in use.
	@param  aDataTo2. A 2nd buffer of data to be supplied to the receiver - meaning of contents dependent on CI in use.
	@param  aDataFrom. Response data from the server side CI - meaning of contents dependent on CI in use.
	@return Result of the custom command. KErrNone if successful, otherwise one of the system wide error codes.
	*/
	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2,
	                                     TDes8&                       aDataFrom) = 0;
	/*
  	Send a synchronous custom command to server side.
	@param  aDestination. The destination of the custom command.
	@param  aFunction. Custom command. Meaning is dependent on Custom Interface in use.
	@param  aDataTo1. A buffer of data to be supplied to the receiver - meaning of contents dependent on CI in use.
	@param  aDataTo2. A 2nd buffer of data to be supplied to the receiver - meaning of contents dependent on CI in use.
	@return  Result of the custom command. KErrNone if successful, otherwise one of the system wide error codes.
	*/
	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2) = 0;

	/*
        Send an asynchronous custom command to server side.
	@param  aDestination. The destination of the custom command.
	@param  aFunction. Custom command. Meaning is dependent on Custom Interface in use.
	@param  aDataTo1. A buffer of data to be supplied to the receiver - meaning of contents dependent on CI in use.
	@param  aDataTo2.A 2nd buffer of data to be supplied to the receiver - meaning of contents dependent on CI in use.
	@param  aDataFrom. Response data from the server side CI - meaning of contents dependent on CI in use.
	@param  aStatus. Indicates the completion status of the request.
	@return  Result of the custom command.  KErrNone if successful, otherwise one of the system wide error codes.
	*/
	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TDes8&                      aDataFrom,
	                                      TRequestStatus&             aStatus) = 0;
	/*
	Send an asynchronous custom command to server side.
	@param  aDestination. The destination of the custom command.
	@param  aFunction. Custom command. Meaning is dependent on Custom Interface in use.
	@param  aDataTo1. A buffer of data to be supplied to the receiver - meaning of contents dependent on CI in use.
	@param  aDataTo2. A 2nd buffer of data to be supplied to the receiver - meaning of contents dependent on CI in use.
	@param  aStatus. Indicates the completion status of the request.
	@return Result of the custom command. KErrNone if successful, otherwise one of the system wide error codes.
	*/
	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TRequestStatus&             aStatus) = 0;

	};

#endif

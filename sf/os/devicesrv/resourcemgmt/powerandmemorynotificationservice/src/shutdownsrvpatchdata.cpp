// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>

//all the constant value needs to be in a separate file
//Care should be taken not to define the exported data in the same source file in which
//they are referred, lest the compiler inline's it. If inlined, the data cannot be patched.
//Hence, the definition of data must be put in a separate source file and they must be 
//referred in other source files by using 'extern' qualifier


/**
Patchable constant used in the shutdown server. 
Used to calculate the shutdown timer with KShtdwnTimeoutClient.
It represents the base timeout value in microseconds.

The timeout is calculated as:
KShtdwnTimeoutBase + KShtdwnTimeoutClient * numClients,
where numClients is the number of connecting clients.

If the timeout value is;
-1, the timer is not used, the server will wait for all client's registration 
    before executing the shutdown;
0,  the timer expires immediately, the server will execute the shutdown immediately, 
    not waiting for any client's re-registration.
other value greater than 0, the server will wait a certain period of time for the clients' registration 
    and execute the shutdown if the timer expires or all clients re-register.
    
Licensees who wish to alter these patchable constants will need to define 
these ROM build-time macros in their product HRH or IBY files.

The new values must follow the rule:
KShtdwnTimeoutBase >= -1 and
KShtdwnTimeoutClient >= 0.

Otherwise, the server panics with KErrNotSupported. 

@internalComponent
@see KShtdwnTimeoutClient
*/
EXPORT_C extern const TInt32 KShtdwnTimeoutBase = 1000000;

/**
Patchable data used in the shutdown server. 
Used to calculate the shutdown timer with KShtdwnTimeoutBase.
It represents the timeout value for each client in microseconds.
@internalComponent
@see KShtdwnTimeoutBase
*/
EXPORT_C extern const TInt32 KShtdwnTimeoutClient = 500000;

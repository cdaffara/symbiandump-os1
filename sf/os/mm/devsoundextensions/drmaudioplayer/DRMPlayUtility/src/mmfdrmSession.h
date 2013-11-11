/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   DRM audio player session class
*
*/


#ifndef __RDRMSESSION_H
#define __RDRMSESSION_H

#include <e32std.h>


class RDrmSession : public RSessionBase
    {
	// Functions
public:
	
	RDrmSession();	
 	~RDrmSession();
	
	//Sync Db server request
	TInt  Connect();
	void  Disconnect();
	//TInt  SendDrmMessage();
	TVersion Version() const;

	//Sync send no data
	TInt Send(TInt aFunction);
	//Sync send with data
	TInt Send(TInt aFunction,const TIpcArgs& aArgs);
	//Async send no data
	void Send(TInt aFunction,TRequestStatus& aStatus);
	//Async send with data
	void Send(TInt aFunction,const TIpcArgs& aArgs,TRequestStatus& aStatus);

private:
    };

#endif

// End of file

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

#ifndef __SUSADAPTIONSESSION_H__
#define __SUSADAPTIONSESSION_H__

#include <e32base.h>


class CSsmAdaptationServer;

/**
 CSusAdaptionSession maintains session with the client
 @internalComponent
 @released
 */
class CSsmAdaptationSession : public CSession2
	{
public:
	static CSsmAdaptationSession* NewL();
	void ServiceL(const RMessage2 &aMessage);
	void ServiceError(const RMessage2 &aMessage, TInt aError);
	~CSsmAdaptationSession();

private:

	CSsmAdaptationSession();
	CSsmAdaptationServer* AdaptionServer() const;
	};

#endif

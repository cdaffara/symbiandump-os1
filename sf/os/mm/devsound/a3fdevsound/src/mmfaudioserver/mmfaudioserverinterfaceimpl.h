/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MMFAUDIOSERVERINTERFACEIMPL_H
#define MMFAUDIOSERVERINTERFACEIMPL_H

//  INCLUDES
#include <e32std.h>
#include "mmfaudioserverinterface.h"

class CMMFAudioServer;

// CLASS DECLARATION

class CAudioSvrLoaderImpl : public CAudioSvrLoader
	{
	friend class CAudioSvrLoader;
public:
	~CAudioSvrLoaderImpl();
protected:
	void ConstructL();
	CAudioSvrLoaderImpl();
private:
	
	CMMFAudioServer* iServer;
	};

#endif // MMFAUDIOSERVERINTERFACE_H

// End of File

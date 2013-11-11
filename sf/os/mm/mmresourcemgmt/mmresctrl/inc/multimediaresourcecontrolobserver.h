//multimediaresourcecontrolobserver.h

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



/**
 @file
 @internalTechnology 
*/

#ifndef M_MULTIMEDIARESOURCECONTROLOBSERVER_H
#define M_MULTIMEDIARESOURCECONTROLOBSERVER_H

class MLogicalChain;

class MMultimediaResourceControlObserver
	{

public:

	virtual void ReceiveResourceUpdate(MLogicalChain *aMessage, TInt aError) = 0;

	virtual void ReceiveResourceResponse(MLogicalChain *aMessage, TInt aError) = 0;

	virtual void ReceivePreemptionUpdate(MLogicalChain *aMessage, TInt aError) = 0;
	
	virtual void CanResume() = 0;
	};

#endif // M_MULTIMEDIARESOURCECONTROLOBSERVER_H

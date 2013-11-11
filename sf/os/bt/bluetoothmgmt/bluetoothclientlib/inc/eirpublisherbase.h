// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _EIRPUBLISHERBASE_H
#define _EIRPUBLISHERBASE_H 

#include <bluetooth/eirpublisher.h>
#include <e32base.h>

class CEirPublisherBase : public CBase, public MEirPublisherNotifier
	{
public:

	IMPORT_C ~CEirPublisherBase();

protected:
	IMPORT_C CEirPublisherBase(TEirTag aTag);
	IMPORT_C void ConstructL();

protected:
	CEirPublisher* iPublisher; 
	TEirTag iTag;
	};
	
#endif	//_EIRPUBLISHERBASE_H

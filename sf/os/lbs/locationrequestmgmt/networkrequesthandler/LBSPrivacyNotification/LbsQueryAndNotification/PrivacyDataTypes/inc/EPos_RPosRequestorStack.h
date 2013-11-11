/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef RPOSREQUESTORSTACK_H
#define RPOSREQUESTORSTACK_H

#include <e32base.h>
#include <s32strm.h>

class CPosRequestor;


/**
Array of CPosRequestor objects.

This class inherits from RPointerArray which means the client can use
all the array functions from that class.

@see CPosRequestor
@publishedPartner
@deprecated
@capabilities None
*/
class RPosRequestorStack : public RPointerArray<CPosRequestor>
    {
public:
    IMPORT_C void InternalizeL(RReadStream& aStream);
    IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
    };

#endif      // RPOSREQUESTORSTACK_H

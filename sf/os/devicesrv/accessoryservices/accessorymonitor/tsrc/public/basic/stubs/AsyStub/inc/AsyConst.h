/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Used as an reference implementation
*
*/


#ifndef ASYCONSTS_H
#define ASYCONSTS_H

const TUid    KTFAccessoryTestProperty     = {0x10200c70};
const TUint32 KTFAccessoryMethod           =  0x00001001;

struct TTFAccessoryPublishAndSubscribe
      {
      TTFASYReference iMethod;
      TInt64 iParam1;
      RArray<TUint32> iCapsArray;
      };

#endif //  ASYCONSTS_H
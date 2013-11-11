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



#ifndef INTGPSPSY1_PUBLICS
#define INTGPSPSY1_PUBLICS

#include <lbsfieldids.h>

/** HPositioneGenericInfo Field IDs used to request behavior
    or information from the PSY */
enum TIntGpsPsy1PositionCommandFieldIds
{
    KIntGpsPsy1FieldStart =                   EPositionFieldProprietaryFieldsBegin + 300,
    KIntGpsPsy1ConfigPsyUid,                 /** The UID of PSY to be configured. if the
                                                 UID dose not match, PSY shall complete request
                                                 with KErrNotSupported*/
                                        
    KIntGpsPsy1ConfigItemsNumber,            /** Number of config items after this field */
};

#endif // INTGPSPSY1_PUBLICS

/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
@internalComponent
@released
*/

#ifndef NETWORKPRIVACYPANICDECLARATION_H
#define NETWORKPRIVACYPANICDECLARATION_H

#include <epos_networkprivacypanic.h>


#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
GLDEF_C void Panic(TPosNetworkPrivacyPanic aPanic);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

#endif // NETWORKPRIVACYPANICDECLARATION_H
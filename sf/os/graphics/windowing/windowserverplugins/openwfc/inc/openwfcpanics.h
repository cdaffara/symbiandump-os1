// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// openwfpanics.h: panic definition for OpenWf Interop
//
//

#ifndef OPENWFPANICS_H
#define OPENWFPANICS_H
enum TOpenWfcPanic // used for OpenWf errors
	{	//low values are line numbers.
	EPanicSetOrderError						=10001,
	EPanicUnexpectedErrorElementConstruct,
	EPanicWfcNoDevForScreen,
	EPanicWfcDeviceNotCreated,
	EPanicWfcContextNotCreated,
	EPanicWfcStartupErrorUnexpected,
	EPanicWfcStreamError,
	EPanicWfcElementNotRemovedOnShutdown,
	EPanicWfcSourceLookupOnElementRemoval,

	EPanicWfcInvalidRotation,
	EPanicWfcBadAttribute,
	EPanicWfcNoStreamSurface,
	EPanicWfcThreadManagerNotInitialised,
	EPanicWfcThreadManagerCannotQueueJob,
	EPanicUnexpectedErrorElementScene,
	EPanicUnexpectedErrorElementRemoval,
	EPanicMethodNotImplemented,

	};

void Panic(TOpenWfcPanic aPanic);

#ifdef _DEBUG
#define OPENWFC_ASSERT_DEBUG(c, p) \
	do { \
		if(!(c)) \
			{ \
			Panic(p); \
			} \
		} while (EFalse)
#else
#define OPENWFC_ASSERT_DEBUG(c, p)
#endif //_DEBUG

#define OPENWFC_ASSERT_ALWAYS(c, p) \
	do { \
		if(!(c)) \
			{ \
			Panic(p); \
			} \
		} while (EFalse)

#endif /*OPENWFPANICS_H_*/

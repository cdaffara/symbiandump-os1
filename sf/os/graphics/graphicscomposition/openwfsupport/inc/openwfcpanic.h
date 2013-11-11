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
// openwfcpanic.h: panic definition for OpenWf Interop
// 
//

#ifndef OPENWFCPANIC_H
#define OPENWFCPANIC_H

enum TOpenWfcPanic // used for OpenWf errors
	{
	EOwfPanicInvalidCallStreamMap  = 1000000,		//An attempt was made to construct the stream map singleton
	EOwfPanicInvalidHasMap,			// The stream map is not available
	EOwfPanicSurfaceStreamBufferNotLocked, //An attempt was made to use or release a surface buffer while it's not locked
	EOwfPanicSurfaceStreamBufferIndexOutOfBounds, //Out of bounds indexing of surface buffer
	EOwfUnexpectedErrorInObservers,    // Unexpected errors during observers processing 
	EOwfDestroyedWithOpenReaders,      // The stream is destroyed while readers are still using it
    EOwfInvalidSUSDisplayedParameter,  // Invalid timestamp detected in the displayed parameter
	EOwfInvalidSUSDisplayedXTimesParameter, // Invalid parameters relative to DisplayedXTimes observer
    EOwfSymbianStreamBadArgument,       // Bad argument is detected in the Symbian Streams APIs
    EOwfSymbianStreamUnexpectedError,   // Unexpected error encountered in Symbian Stream APIs
    EOwfSymbianStreamInvalidBufferIndexError, // Attempt to operate with an invalid buffer
    EOwfSymbianUnexpectedObserverId,    // Unexepected Observer Id
	};

void Panic(TOpenWfcPanic aPanic);

#ifdef _DEBUG
#define WFCI_ASSERT_DEBUG(c, p) \
	do { \
		if(!(c)) \
			{ \
			Panic(p); \
			} \
		} while (EFalse)
#else
#define WFCI_ASSERT_DEBUG(c, p)
#endif //_DEBUG

#define WFCI_ASSERT_ALWAYS(c, p) \
	do { \
		if(!(c)) \
			{ \
			Panic(p); \
			} \
		} while (EFalse)

#endif /* OPENWFCPANIC_H */

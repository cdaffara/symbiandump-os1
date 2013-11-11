// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements TGsmuPanic and Panic()
// 
//

/**
 @file
*/

#ifndef __GSMUMAIN_H__
#define __GSMUMAIN_H__

#include "smsstacklog.h"
#include <e32base.h>

/**
 *  @internalComponent
 */
enum TGsmuPanic
	{
//  GSMUELEM.CPP
	KGsmuPanicCommandDataLengthTooLong=0x00,
	KGsmuPanicNoTelematicInterworking,
	KGsmuPanicNoTelematicDevice,
	KGsmuPanicNoShortMessageType,
	KGsmuPanicNotPossibleToSetNumberPlanIdentification,
	KGsmuPanicNoNumberPlanIdentificationDefined,
	KGsmuPanicNumQuarterHoursOutOfRange,
	KGsmuPanicValidityPeriodOutOfRange,
	KGsmuPanicInformationElementIndexOutOfRange,
	KGsmuPanicUserDataHeaderTooLong,
	KGsmuPanicUnsupportedAlphabet,
	KGsmuPanicUserDataBodyTooLong,
	KGsmuPanicCommandDataBufferTooLong,
	KGsmuPanicTextCompressed,
	KGsmuPanicNotSupportedWithDCSBits7To4,
	KGsmuPanicUnsupportedValidityPeriodFormat,
	KGsmuPanicConversionRetriedOut,
	KGsmuPanicDeprecatedMethod,
	KGsmuPanicMethodBodyNotImplemented,
	KGsmuPanicUnknownInformationElement,
//  GSMUPDU.CPP
	KGsmuPanicUnsupportedPduType=0x20,
	KGsmuPanicDataCodingSchemeNotPresent,
	KGsmuPanicMessageNotConcatenated,
	KGsmuPanicConcatenatedMessageReferenceOutOfRange,
	KGsmuPanicNumConcatenatedMessagePDUsOutOfRange,
	KGsmuPanicConcatenatedMessagePDUIndexOutOfRange,
	KGsmuPanicProtocolIdentifierNotPresent,
	KGsmuPanicParameterIndicatorNotPresent,
	KGsmuPanicUserDataNotPresent,
	KGsmuPanicToFromAddressNotPresent,
	KGsmuPanicNotRPError,
	KGsmuPanicPortOutOfRange,
	KGsmuPanicSetStatusReportRequestNotSupportedForCommandType,
//  GSMUMSG.CPP
	KGsmuPanicNotDecoded=0x40,
	KGsmuPanicTextNotPresent,
	KGsmuPanicFunctionNotSupportedWithTextCompression,
	KGsmuPanicSmsArrayNotEmpty,
	KGsmuPanicWrongNumberOfMessagePDUs,
	KGsmuPanicGsmCompressionDllAlreadyExists,
	KGsmuPanicTextTooLong,
	KGsmuPanicCompressionNotSupported,
	KGsmuPanicUnsupportedForPDUType,
	KGsmuPanicBadOptimizationOptions,
	KGSMUPanicUserTimeZoneOffsetOutOfRange,
	KGsmuPanicInconsistentMessageAttributes,
//  GSMUBUF
	KGsmuPanicTextHeaderTooLong=0x60,
	KGsmuPanicBufferNotReset,
//  GSMUSTORE
	KGsmuPanicSARStoreAlreadyOpen=0x80,
	KGsmuPanicSARStoreEntryArrayNotReset,
	KGsmuPanicSARStoreNotOpen,
	KGsmuPanicSAREntryDataStreamIdChanged,
	KGsmuPanicSAREntryDataStreamIdNotSet,
	KGsmuPanicSARStoreTransaction,
//	GSMUSAR
	KGsmuPanicIllegalSegmentSize=0x90,
	KGsmuPanicSegmentBufferTooSmall,
//	GSMUPRIV
	KGsmuPanicPackStartBitInvalid=0xa0,
	KGsmuPanicPackAlphabetInvalid,
//  GSMUIEOPERATIONS
	KGsmuPanicUnknownInformationElement1=0xb0,
	KGsmuPanicUnknownInformationElement2,
	KGsmuPanicUnknownInformationElement3,
	KGsmuPanicUnknownInformationElement4,
	KGsmuPanicUnknownInformationElement5,
	KGsmuPanicUnknownInformationElement6,
	KGsmuPanicUnknownInformationElement7,
	KGsmuPanicUnknownInformationElement8,
	KGsmuPanicUnknownInformationElement9,
	KGsmuPanicUnknownInformationElement10,
	KGsmuPanicUnknownInformationElement11,
	KGsmuPanicMethodBodyNotImplemented1,
//  GSMUINONEOPERATIONS
	KGsmuPanicVersionNotSupported=0xc0
	};

/**
 *  @internalComponent
 */
GLDEF_C void Panic(TGsmuPanic aPanic);


#endif // __GSMUMAIN_H__

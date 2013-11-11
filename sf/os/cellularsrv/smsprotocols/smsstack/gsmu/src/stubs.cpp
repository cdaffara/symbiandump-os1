// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements stub panics for deprecated functions.
// 
//

/**
 @file
*/

#include "Gsmumain.h"

#ifndef __EABI__

#include "smsstacklog.h"

// Was TGsmCompressionSettings::CharacterGroupID(TInt &) const
EXPORT_C void PanicStub1()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was TGsmCompressionSettings::CharacterSet(TGsmCompressionSettings::TGsmCharacterSet &) const
EXPORT_C void PanicStub2()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsMessage::CompressionSettings(TGsmCompressionSettings &) const
EXPORT_C void PanicStub3()
    {
    BULLSEYE_OFF
    // Ignore in code coverage - deprecated method
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsMessage::CompressionSupportedL(RFs &)
EXPORT_C void PanicStub4()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsMessage::DecodeMessagePDUsL(CArrayFix<TSms> const &, TInt)
EXPORT_C void PanicStub5()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsPDU::EncodeMessagePDUL(TSms &, TInt) const
EXPORT_C void PanicStub6()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsMessage::EncodeMessagePDUsL(CArrayFix<TSms> &, TInt, TInt)
EXPORT_C void PanicStub7()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was TGsmCompressionSettings::ExternalizeL(RWriteStream &) const
EXPORT_C void PanicStub8()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was TGsmCompressionSettings::HuffmanInitializationID(TInt &) const
EXPORT_C void PanicStub9()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was TGsmCompressionSettings::InternalizeL(RReadStream &)
EXPORT_C void PanicStub10()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was TGsmCompressionSettings::KeywordDictionaryID(TInt &) const
EXPORT_C void PanicStub11()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsMessage::NewL(CSmsPDU::TSmsPDUType, CSmsBufferBase *, TInt)
EXPORT_C void PanicStub12()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsMessage::NewL(TSms const &, CSmsBufferBase *, TInt, TInt, TInt)
EXPORT_C void PanicStub13()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsPDU::NewL(TSms const &, CCnvCharacterSetConverter &, RFs &, TInt, TInt, TInt)
EXPORT_C void PanicStub14()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSARStore::OpenL(TDesC16 const &, TUid)
EXPORT_C void PanicStub15()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsPDU::ParsedServiceCenterAddress(TGsmTelNumber &) const
EXPORT_C void PanicStub16()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsPDU::ParsedToFromAddress(TGsmTelNumber &) const
EXPORT_C void PanicStub17()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was TGsmCompressionSettings::PunctuatorID(TInt &) const
EXPORT_C void PanicStub18()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was TGsmCompressionSettings::SetCharacterGroupID(TInt, TInt)
EXPORT_C void PanicStub19()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was TGsmCompressionSettings::SetCharacterSet(TInt, TGsmCompressionSettings::TGsmCharacterSet)
EXPORT_C void PanicStub20()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsMessage::SetCompressionSettings(TGsmCompressionSettings const &)
EXPORT_C void PanicStub21()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSARStore::SetExtraStreamIdL(TStreamId)
EXPORT_C void PanicStub22()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was TGsmCompressionSettings::SetHuffmanInitializationID(TInt, TInt)
EXPORT_C void PanicStub23()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was TGsmCompressionSettings::SetKeywordDictionaryID(TInt, TInt)
EXPORT_C void PanicStub24()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsPDU::SetParsedServiceCenterAddressL(TGsmTelNumber const &)
EXPORT_C void PanicStub25()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSmsPDU::SetParsedToFromAddressL(TGsmTelNumber const &)
EXPORT_C void PanicStub26()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was TGsmCompressionSettings::SetPunctuatorID(TInt, TInt)
EXPORT_C void PanicStub27()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CGsmCompressionBase::CGsmCompressionBase(RFs &)
EXPORT_C void PanicStub28()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was TGsmCompressionSettings::TGsmCompressionSettings(void)
EXPORT_C void PanicStub29()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CGsmCompressionBase::Reserved(void)
EXPORT_C void PanicStub30()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSARStore::CloseFile(void)
EXPORT_C void PanicStub31()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

// Was CSARStore::OpenFileLC(void)
EXPORT_C void PanicStub32()
    {
    // Ignore in code coverage - deprecated method
    BULLSEYE_OFF
    Panic(KGsmuPanicDeprecatedMethod);
    BULLSEYE_RESTORE
    }

#endif // __EABI__

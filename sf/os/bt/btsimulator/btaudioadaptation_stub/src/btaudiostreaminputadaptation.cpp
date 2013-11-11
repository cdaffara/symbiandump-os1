/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains stub implementation of CBTAudioStreamInputAdaptation class, 
*                which implements BT Audio Adaptation API. 
*
*/


#include <e32std.h> 

#include "btaudiostreaminputadaptation.h"
#include "btaudioadaptationdebug.h"

CBTAudioStreamInputAdaptation::CBTAudioStreamInputAdaptation(MBTAudioStreamObserver& /*aStreamObserver*/, MBTAudioErrorObserver& /*aErrorObserver*/) 
    {
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::CBTAudioStreamInputAdaptation() ->")));
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::CBTAudioStreamInputAdaptation() <-")));
    }

void CBTAudioStreamInputAdaptation::ConstructL()
    {
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::ConstructL() ->")));
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::ConstructL() <-")));
    }

// ---------------------------------------------------------------------------
// Static factory method for the abstract base class. 
// This just calls the factory method of the concrete implementation class. 
// ---------------------------------------------------------------------------
//
EXPORT_C CBTAudioStreamInputBase* CBTAudioStreamInputBase::NewL(MBTAudioStreamObserver& aStreamObserver, MBTAudioErrorObserver& aErrorObserver)
    {
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioInput::NewL() ->")));
    CBTAudioStreamInputBase* audioInput = (CBTAudioStreamInputAdaptation::NewL(aStreamObserver, aErrorObserver)); 
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioInput::NewL() <-")));
    return audioInput; 
    }

// ---------------------------------------------------------------------------
// Static factory method. 
// ---------------------------------------------------------------------------
//
CBTAudioStreamInputAdaptation* CBTAudioStreamInputAdaptation::NewL(MBTAudioStreamObserver& aStreamObserver, MBTAudioErrorObserver& aErrorObserver)
    {
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::NewL() ->")));
    CBTAudioStreamInputAdaptation* self = new (ELeave) CBTAudioStreamInputAdaptation(aStreamObserver, aErrorObserver);
    CleanupStack::PushL(self); 
    self->ConstructL();
    CleanupStack::Pop(self);
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::NewL() <-")));
    return self;
    }

CBTAudioStreamInputAdaptation::~CBTAudioStreamInputAdaptation()
    {
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::~CBTAudioStreamInputAdaptation() ->")));
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::~CBTAudioStreamInputAdaptation() <-")));
    }

// ---------------------------------------------------------------------------
// From class CBTAudioStreamInputBase.
// Returns KErrNotSupported always - this is the stub implementation. 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CBTAudioStreamInputAdaptation::Start()
    {
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::Start() ->")));
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::Start() <-")));
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// From class CBTAudioStreamInputBase.
// Returns a NULL pointer always - this is the stub implementation. 
// ---------------------------------------------------------------------------
//
EXPORT_C TAny* CBTAudioStreamInputAdaptation::EncoderInterface(TUid /*aInterfaceId*/)
    {
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::EncoderInterface() ->")));
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::EncoderInterface() <-")));
    return (TAny *) 0;
    }

// ---------------------------------------------------------------------------
// From class CBTAudioStreamInputBase.
// Doesn't do anything - this is the stub implementation. 
// ---------------------------------------------------------------------------
//
EXPORT_C void CBTAudioStreamInputAdaptation::BufferEmptied( const TDesC8& /*aBuffer*/ )
    {
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::BufferEmptied() ->")));
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::BufferEmptied() <-")));
    }

// ---------------------------------------------------------------------------
// From class CBTAudioStreamInputBase.
// Doesn't do anything - this is the stub implementation. 
// ---------------------------------------------------------------------------
//
EXPORT_C void CBTAudioStreamInputAdaptation::Stop()
    {
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::Stop() ->")));
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::Stop() <-")));
    }

// ---------------------------------------------------------------------------
// From class CBTAudioStreamInputBase.
// Returns KErrNotSupported always - this is the stub implementation. 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CBTAudioStreamInputAdaptation::SetFormat(TFourCC /*aDesiredFourCC*/)
    {
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::SetFormat() ->")));
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::SetFormat() <-")));
    return KErrNotSupported; 
    }

// ---------------------------------------------------------------------------
// From class CBTAudioStreamInputBase.
// Returns KErrNotSupported always - this is the stub implementation. 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CBTAudioStreamInputAdaptation::Connect() 
    {
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::Connect() ->")));
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::Connect() <-")));
    return KErrNotSupported; 
    }

// ---------------------------------------------------------------------------
// From class CBTAudioStreamInputBase.
// Doesn't do anything - this is the stub implementation. 
// ---------------------------------------------------------------------------
//
EXPORT_C void CBTAudioStreamInputAdaptation::Disconnect()
    {
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::Disconnect() ->")));
    BT_AUDIO_ADAPTATION_TRACE_OPT( KPRINTFTRACE, FLOG(_L("[BTAudioAdaptation]\t CBTAudioStreamInputAdaptation::Disconnect() <-")));
    }

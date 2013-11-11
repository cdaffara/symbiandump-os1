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
// Mmf\Common\MmfMidi.h
// 
//

#ifndef __MMF_COMMON_MMFMIDI_H__
#define __MMF_COMMON_MMFMIDI_H__

// Standard EPOC32 includes
#include <e32base.h>

// Public Media Server includes
#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfutilities.h>

#include <midiclientutility.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfmidiconfig.h>
#endif

/**
MIDI event usued by a MIDI controller to signal to a 
client that a MIDI source open request has completed.

@publishedAll
@released
*/
const TUid KMMFEventCategoryMidiOpenDataSourceComplete = {0x101F8F85};

/**
MIDI event used by a MIDI controller to signal to a 
client that play of a MIDI resource has commenced.

@publishedAll
@released
*/
const TUid KMMFEventCategoryMidiPlaying = {0x101F8F86};

/**
MIDI event used by a MIDI controller to signal to 
a client that play of a MIDI resource has commenced 
but not all SP-MIDI requirements can be met.

@publishedAll
@released
*/
const TUid KMMFEventCategoryMidiPlaybackIncompatible = {0x101F8F87};

/**
MIDI event used by a MIDI controller to signal to a client that play
of a MIDI resource has commenced but due to system resource constraints
and SP-MIDI content constraints, there is no audible MIDI content
during playback.

@publishedAll
@released
*/
const TUid KMMFEventCategoryMidiPlaybackSilent = {0x101F8F88};

/**
MIDI event used by a MIDI controller to signal to
a client that play of a MIDI resource has completed.

@publishedAll
@released
*/
const TUid KMMFEventCategoryMidiPlayingComplete = {0x101F8F89};

/**
MIDI event used by a MIDI controller to signal to
a client a sync update event. The frequency of this event is
set by the client with SetSyncUpdateCallbackIntervalL.

@publishedAll
@released
*/
const TUid KMMFEventCategoryMidiSyncUpdate = {0x101F8F8A};

/**
MIDI event used by a MIDI controller to signal to
a client that any currently opened resources have been closed.

@publishedAll
@released
*/
const TUid KMMFEventCategoryMidiClose = {0x101F8F8B};

/**
MIDI event used by a MIDI controller to signal to a client that the 
MIDI controller has been primed and it's ready to play.

@publishedAll
@released
*/
const TUid KMMFEventCategoryMidiPrime = {0x101F8F8C};

/**
MIDI event used by a MIDI controller to signal to a client that the MIDI
resource has encountered a tempo change message in the MIDI data stream.

@publishedAll
@released
*/
const TUid KMMFEventCategoryTempoChanged = {0x101F8F8D};

/**
MIDI event used by a MIDI controller to signal to a client that the MIDI
resource has encountered a volume change message in the MIDI data stream.

@publishedAll
@released
*/
const TUid KMMFEventCategoryVolumeChanged = {0x101F8F8E};

/**
MIDI event used by a MIDI controller to signal to a client that the 
MIDI resource has encountered a mute message in the MIDI data stream.

@publishedAll
@released
*/
const TUid KMMFEventCategoryMuteChanged = {0x101F8F8F};

/**
MIDI event used by a MIDI controller to signal to a client that the polyphony
of the MIDI engine has changed.

@publishedAll
@released
*/
const TUid KMMFEventCategoryPolyphonyChanged = {0x101F8F90};

/**
MIDI event used by a MIDI controller to signal to a client that
an instrument has been changed on a channel.

@publishedAll
@released
*/
const TUid KMMFEventCategoryInstrumentChanged = {0x101F8F91};

/**
MIDI event used by a MIDI controller to signal to a client that a timed
meta data entry has been found in the MIDI data stream.

@publishedAll
@released
*/
const TUid KMMFEventCategoryMetaDataEntryFound  = {0x101F8F92};

/**
MIDI event used by a MIDI controller to signal to a client that a 
MIP message has been found in the MIDI data stream.

@publishedAll
@released
*/
const TUid KMMFEventCategoryMipMessageReceived  = {0x101F8F93};

#endif

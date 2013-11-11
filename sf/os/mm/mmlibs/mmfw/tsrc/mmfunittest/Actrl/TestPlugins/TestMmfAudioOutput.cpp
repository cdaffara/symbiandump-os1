// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "MMFAudioOutput.h"
#include <mmf/server/mmfdatabuffer.h>
#include <mmfaudiopolicyproxy.h>
#include <ImplementationProxy.h>
#include <mmffourcc.h>

#include "TestMMFAudioOutput.h"

// special ack event id
const TUid KPrimeTestId = { 0x101F7853 };
const TUid KPlayTestId = { 0x11111111 };

/*

*/

void CTestMMFAudioOutput::SinkPrimeL()
	{
	//[ send a prime event to the test code ]
	TMMFEvent primeEvent( KPrimeTestId, KErrNone );
	SendEventToClient( primeEvent );
	//[ now actually do the prime ]
    CMMFAudioOutput::SinkPrimeL();
	}

/*

*/
void CTestMMFAudioOutput::SinkPlayL()
	{
	//[ send an event back to the test code that we are playing]
	TMMFEvent playEvent( KPlayTestId, KErrNone );
	SendEventToClient( playEvent );
	// [ actually do the play now]
    CMMFAudioOutput::SinkPlayL();
	}

/*


*/

MDataSink* CTestMMFAudioOutput::NewSinkL()
	{
	CTestMMFAudioOutput* self = new (ELeave) CTestMMFAudioOutput ;
	return STATIC_CAST( MDataSink*, self ) ;
	}




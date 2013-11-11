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
// MMFTestDataSink.h
// 
//

#ifndef __MMFTESTDATASINK_H__
#define __MMFTESTDATASINK_H__

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <e32std.h>

#include <e32hal.h>
#include <e32svr.h>
#include <ecom/implementationproxy.h>

#include "MMFTestDataSinkUIDs.hrh"

// Public Media Server includes
#include <mmf/server/mmfdatasink.h>
//#include <mmfdataSink.h>
#include <mmf/server/mmfdatapath.h>
#include <mmf/common/mmffourcc.h>
//#include <mmfdataSinksink.hrh>
//#include <mmfAudioIOInterfaceUIDs.hrh>

//#include <mmf/server/mmfcodec.h>
//#include <mmf/server/mmfdatabuffer.h>
//#include <mmfpaniccodes.h>

//const TUint KAudioOutputDefaultFrameSize = 0x1000;

//void Panic(TInt aPanicCode);

class MMFTestDataSink : public MDataSink
{
public:
	static MMFTestDataSink* NewL(TUid aType);
	static MMFTestDataSink* NewLC(TUid aType);

	virtual void ConstructSinkL( const TDesC8& /*aInitData*/ );

	virtual TUid DataSinkType() const;
	virtual TFourCC SinkDataTypeCode(TMediaId /*aMediaId*/);
	virtual TInt SetSinkDataTypeCode(TFourCC /*aSinkFourCC*/, TMediaId /*aMediaId*/);
	virtual void EmptyBufferL(CMMFBuffer* /*aBuffer*/, MDataSource* /*aSupplier*/, TMediaId /*aMediaId*/);
	virtual void BufferFilledL(CMMFBuffer* /*aBuffer*/);
	virtual TBool CanCreateSinkBuffer();
	virtual CMMFBuffer* CreateSinkBufferL(TMediaId /*aMediaId*/, TBool &aReference);
	virtual TInt SinkThreadLogon(MAsyncEventHandler& /*aEventHandler*/);
	virtual void SinkThreadLogoff();
	virtual void NegotiateL(MDataSource& /* aDataSource*/);
	virtual void SinkPrimeL();
	virtual void SinkPlayL();
	virtual void SinkPauseL();
	virtual void SinkStopL();
	virtual void SetSinkPrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/);

private:
	MMFTestDataSink(TUid aType);
	~MMFTestDataSink(void);
	void ConstructL (void);
};

#endif

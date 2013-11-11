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
// MMFTestDataSourceB.h
// 
//

#ifndef __MMFTestDataSourceB_H__
#define __MMFTestDataSourceB_H__

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <e32std.h>

#include <e32hal.h>
#include <e32svr.h>
#include <ecom/implementationproxy.h>

// Public Media Server includes
#include <mmf/server/mmfdatasink.h>
#include <mmf/server/mmfdatasource.h>
#include <mmf/server/mmfdatapath.h>
#include <mmf/server/mmfdatasourcesink.hrh>
#include <mmf/plugin/mmfaudioiointerfaceuids.hrh>

#include <mmf/common/mmffourcc.h>

#include "MMFTestDataSourceBUIDs.hrh"

class MMFTestDataSourceB : public MDataSource
{
public:
	static MMFTestDataSourceB* NewL(TUid aType);
	static MMFTestDataSourceB* NewLC(TUid aType);

	virtual void ConstructSourceL(  const TDesC8& /*aInitData */);

	virtual TUid DataSourceType() const;
	virtual TFourCC SourceDataTypeCode(TMediaId /*aMediaId*/);
	virtual TInt SetSourceDataTypeCode(TFourCC /*aSourceFourCC*/, TMediaId /*aMediaId*/);
	virtual void FillBufferL(CMMFBuffer* /*aBuffer*/, MDataSink* /*aConsumer*/,TMediaId /*aMediaId*/);
	virtual void BufferEmptiedL(CMMFBuffer* /*aBuffer*/); //called by MDataSink to pass back emptied buffer to the source
	virtual TBool CanCreateSourceBuffer();
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &/*aReference*/);

	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& /*aSinkBuffer*/, TBool &aReference);

	virtual TInt SourceThreadLogon(MAsyncEventHandler& /*aEventHandler*/);
	virtual void SourceThreadLogoff();
	virtual void NegotiateSourceL(MDataSink& /* aDataSink*/); //called if source setup depends on sink
	virtual TBool SourceSampleConvert();
	virtual void SourcePrimeL();
	virtual void SourcePlayL();
	virtual void SourcePauseL();
	virtual void SourceStopL();
	virtual void SetSourcePrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/);

private:
	MMFTestDataSourceB(TUid aType);
	~MMFTestDataSourceB(void);
	void ConstructL (void);
};

#endif

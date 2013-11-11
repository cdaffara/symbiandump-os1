// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TS_CMMFTESTTERMINATIONCONTROLLER_H
#define TS_CMMFTESTTERMINATIONCONTROLLER_H

#include <e32std.h>
#include <e32base.h>


#include <ecom/implementationproxy.h>

#include <mmf/common/mmfcontroller.h>

#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include "cmmftestterminationcontrolleruids.hrh"
#include "TS_Codes.h"

class CMMFTestTerminationController : public CMMFController						   
{
public:
	static CMMFTestTerminationController* NewL(void);
	static CMMFTestTerminationController* NewLC(void);

//implement CMMFController ecom plugin interface
	virtual void AddDataSourceL(MDataSource& aDataSource);
	virtual void AddDataSinkL(MDataSink& aDataSink);
	virtual void RemoveDataSourceL(MDataSource& aDataSource);
	virtual void RemoveDataSinkL(MDataSink& aDataSink);
	virtual void RemoveSourceSinkL();
	virtual void ResetL();
	virtual void PrimeL();
	virtual void PlayL();
	virtual void PauseL();
	virtual void StopL();
	virtual TTimeIntervalMicroSeconds PositionL() const;
	virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
	virtual TTimeIntervalMicroSeconds DurationL() const;
	virtual void CustomCommand(TMMFMessage& aMessage);
	virtual void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);
	virtual void GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries);
	virtual CMMFMetaDataEntry* GetMetaDataEntryL(TInt aIndex);

private:
	CMMFTestTerminationController(void);
	~CMMFTestTerminationController(void);
	void ConstructL(void);
};

#endif

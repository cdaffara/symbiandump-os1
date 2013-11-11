// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TS_CMMFTESTCONSTRUCTCONTROLLER_H
#define TS_CMMFTESTCONSTRUCTCONTROLLER_H

#include <e32std.h>
#include <e32base.h>


#include <ecom/implementationproxy.h>

#include <mmf/common/mmfcontroller.h>

#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include "cmmftestconstructcontrolleruids.hrh"
#include "../../TS_Codes.h"

class CMMFTestConstructController : public CMMFController						   
{
public:
	static CMMFTestConstructController* NewL(void);
	static CMMFTestConstructController* NewLC(void);

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

protected:
	CMMFTestConstructController(void);
	virtual ~CMMFTestConstructController(void);
	virtual void ConstructL(void);
};

class CMMFTestConstructController2 : public CMMFTestConstructController
{
public:
	static CMMFTestConstructController2* NewL(void);
	static CMMFTestConstructController2* NewLC(void);

protected:
	CMMFTestConstructController2(void);
	~CMMFTestConstructController2(void);
	void ConstructL(void);
};

class CMMFTestConstructController3 : public CMMFTestConstructController
{
public:
	class CDummyMMFObject : public CMMFObject
		{
	public:
		static CDummyMMFObject* NewL();
		~CDummyMMFObject();
		void HandleRequest(TMMFMessage& aMessage);
	private:
		CDummyMMFObject();
		};

public:
	static CMMFTestConstructController3* NewL(void);
	static CMMFTestConstructController3* NewLC(void);
	
protected:
	CMMFTestConstructController3(void);
	~CMMFTestConstructController3(void);
	void ConstructL(void);
};

#endif

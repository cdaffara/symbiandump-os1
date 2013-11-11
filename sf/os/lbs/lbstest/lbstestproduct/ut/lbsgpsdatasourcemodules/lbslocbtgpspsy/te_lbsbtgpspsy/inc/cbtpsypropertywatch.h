/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Example CTestStep derived implementation
*
*/



/**
 @file cbtpsypropertywatch.h
 @internalTechnology
*/

#ifndef CBTPSYPROPERTYWATCH_H_
#define CBTPSYPROPERTYWATCH_H_
#include <e32base.h>
#include <e32cons.h>
#include <e32property.h>
#include <BtGpsPsyPrivatePSKeys.h>
#include "Te_LbsBtGpsPsyStepBase.h"
#include "Te_LbsBtGpsPsyDefs.h"

/**
Subscribes to an integer property using the active object framework
 */
class CBTPSYPropertyWatch : public CActive
	{
	enum {EPriority=0};
public:
	static CBTPSYPropertyWatch* NewL();
	void WatchL();
	~CBTPSYPropertyWatch();
	
protected:
	CBTPSYPropertyWatch();
	void ConstructL();
	void RunL();
	void DoCancel();
public:
	virtual TInt CheckProperty();
	virtual void PrintPropertyL();
	void CBTPSYPropertyWatch::PrintPropertyForThreadL( const TDesC& aText);
	void CBTPSYPropertyWatch::PrintPropertyForThreadL( const TDesC& aText, const TDesC& aFile );
	void IssueRequest();
protected:
	// A handle to the subscribed property
	RProperty			iProperty;
	TRequestStatus		iPropertyWatchStatus;
	};

class CBTPSYPropertyWatchArray : public CBase
	{
public:
	static CBTPSYPropertyWatchArray* NewL();
	static CBTPSYPropertyWatchArray* NewLC();
	~CBTPSYPropertyWatchArray();
	void WatchL();
private:
	CBTPSYPropertyWatchArray();
	void ConstructL();

private:
	CArrayPtrFlat<CBTPSYPropertyWatch>* iPropertyWatchArray;
	};

class CBtGpsPsyBatteryStateWatch : public CBTPSYPropertyWatch
	{
public:
	static CBtGpsPsyBatteryStateWatch* NewL();
	~CBtGpsPsyBatteryStateWatch();
private:
	CBtGpsPsyBatteryStateWatch();
	void ConstructL();
public:
	TInt CheckProperty();
	void PrintPropertyL();
	};

class CBtGpsPsyBatteryLevelWatch : public CBTPSYPropertyWatch
	{
public:
	static CBtGpsPsyBatteryLevelWatch* NewL();
	~CBtGpsPsyBatteryLevelWatch();
private:
	CBtGpsPsyBatteryLevelWatch();
	void ConstructL();
public:
	TInt CheckProperty();
	void PrintPropertyL();
	};

class CBtGpsPsyExtAntennaWatch : public CBTPSYPropertyWatch
	{
public:
	static CBtGpsPsyExtAntennaWatch* NewL();
	~CBtGpsPsyExtAntennaWatch();
private:
	CBtGpsPsyExtAntennaWatch();
	void ConstructL();
public:
	TInt CheckProperty();
	void PrintPropertyL();
	};

class CBtGpsPsyExtPowerWatch : public CBTPSYPropertyWatch
	{
public:
	static CBtGpsPsyExtPowerWatch* NewL();
	~CBtGpsPsyExtPowerWatch();
private:
	CBtGpsPsyExtPowerWatch();
	void ConstructL();
public:
	TInt CheckProperty();
	void PrintPropertyL();
	};

class CBtGpsPsyStateWatch : public CBTPSYPropertyWatch
	{
public:
	static CBtGpsPsyStateWatch* NewL();
	~CBtGpsPsyStateWatch();
private:
	CBtGpsPsyStateWatch();
	void ConstructL();
public:
	TInt CheckProperty();
	void PrintPropertyL();
	};

#endif /*CBTPSYPROPERTYWATCH_H_*/

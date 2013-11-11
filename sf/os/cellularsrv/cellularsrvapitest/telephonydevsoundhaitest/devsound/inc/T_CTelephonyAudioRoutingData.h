/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/


#ifndef T_CTELEPHONYAUDIOROUTINGDATA_H_
#define T_CTELEPHONYAUDIOROUTINGDATA_H_

//User includes
#include "DataWrapperBase.h"

//Epoc includes
#include <mtelephonyaudioroutingobserver.h>//MTelephonyAudioRoutingObserver
#include <telephonyaudiorouting.h>//CTelephonyAudioRouting
/** 
 * This class tests the CTelephonyAudioRouting API's 
 */

class CT_CTelephonyAudioRoutingData : public CDataWrapperBase, public MTelephonyAudioRoutingObserver
	{
public:
	~CT_CTelephonyAudioRoutingData();
	static CT_CTelephonyAudioRoutingData* NewL();
	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	
private:
	CT_CTelephonyAudioRoutingData();
	// void ConstructL();
	void DestroyData();

public:
	/**
	 * From MAudioOutputObserver
	 */
	void AvailableOutputsChanged( CTelephonyAudioRouting& /*aTelephonyAudioRouting*/);
	void OutputChanged( CTelephonyAudioRouting& /*aTelephonyAudioRouting*/);
	void SetOutputComplete( CTelephonyAudioRouting& /*aTelephonyAudioRouting*/, TInt aError);
    
protected:
	void DoCmdNewL();
	void DoCmdDestructor();
	void DoCmdAvailableOutputs();
	void DoCmdOutput();
	void DoCmdPreviousOutput();
	void DoCmdSetShowNote(const TTEFSectionName& aSection);
	void DoCmdGetShowNote();
	void DoCmdSetOutputL(const TTEFSectionName& aSection);
  
private:
	/**
	 * Wrapped object
	 */
	CTelephonyAudioRouting* iTelephonyAudioRouting;	
	/**
	 * Current output configured
	 */
	CTelephonyAudioRouting::TAudioOutput iOutput;
	/**
	 * Enum for output routing
	 */
	static const TEnumEntryTable iEnumOutput[];
	};

#endif /*T_CTELEPHONYAUDIOROUTING_H_*/

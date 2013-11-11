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
// Declares the internal observer classes used by CDvbhReceiverInfo.
// 
//

/**
 @file
 @internalComponent
 @prototype
*/

#ifndef DVBHRECEIVERINFOOBSERVER_H
#define DVBHRECEIVERINFOOBSERVER_H

#include "dvbhstubcommon.h"
#include <e32property.h>
#include <e32cons.h>

class MDvbhStateObserver;
class MDvbhSignalQualityObserver;
class MDvbhPlatformObserver;
class MDvbhNetworkTimeObserver;
class MDvbhFrequencyObserver;
class MDvbhCellIdObserver;
class MDvbhNetworkIdObserver;
class MDvbhExtBatteryStateObserver;
class MDvbhExtConnectionObserver;
class MDvbhExtAntennaConnectionObserver;

/**
* Interface to abstract the callback to be called when an RProperty changes. 
*/
class MPropertyNotifier
	{
public:
	/**
	* Callback to be called when an RProperty changes.
	* Implementations will probably want to call the appropriate callback on an external observer object.
	* @param aProperty A handle to the RProperty that has just changed.
	*/
	virtual void NotifyPropertyChanged(RProperty& aProperty) = 0;
	
	/**
	* Called to get the key of the property of interest.
	* @return the key value.
	*/
	virtual TInt GetPropertyKey() const = 0;
	};

/**
* CDvbhPropertyObserver is the class used to observer changes to an RProperty.
*
* To observer changes to a particular RProperty, users must call 
* CDvbhPropertyObserver::SetObserver() passing in an MPropertyNotifier 
* implementation corresponding to the RProperty of interest.  
* When the RProperty changes, CDvbhPropertyObserver::RunL() will call 
* MPropertyNotifier::NotifyPropertyChanged() of the implementation. 
*/
NONSHARABLE_CLASS(CDvbhPropertyObserver) : public CActive
	{
public:
	CDvbhPropertyObserver();
	~CDvbhPropertyObserver();
	TInt SetObserver(MPropertyNotifier* aPropertyNotifier);

private: 
	void RunL();
	void DoCancel();
	
private:
	MPropertyNotifier* iPropertyNotifier; //Not owned
	RProperty iProperty;
	};

/**
* Thin template base class for the RProperty notifiers.
*/
NONSHARABLE_CLASS(TDvbhNotifierBase)
	{
protected:
	TDvbhNotifierBase();
	void SetExternalObserver(TAny* aObserver);
protected:
	TAny* iExternalObserver; //Not owned
	};

/**
* Template on which RProperty Notifiers are based.  Contains all the
* code common to a Notifier for any RProperty.
*
* The template parameter OBSERVER must be one of the observer M-classes declard in dvbhreceiverinfo.h.
*/
template<class OBSERVER>
NONSHARABLE_CLASS(TDvbhNotifier) : protected TDvbhNotifierBase
	{
public:
	inline void SetExternalObserver(OBSERVER* aObserver);
	};
	
/**
* The state notifier for CDvbhReceiverInfo::CPrivateData. 
*/	
NONSHARABLE_CLASS(TDvbhStateNotifier) : public TDvbhNotifier<MDvbhStateObserver>,
										public MPropertyNotifier
	{
public:
	virtual TInt GetPropertyKey() const;
	virtual void NotifyPropertyChanged(RProperty& aProperty);
	};

/**
* The SignalQuality notifier for CDvbhReceiverInfo::CPrivateData. 
*/	
NONSHARABLE_CLASS(TDvbhSignalQualityNotifier) : public TDvbhNotifier<MDvbhSignalQualityObserver>,
												public MPropertyNotifier
	{
public:
	virtual TInt GetPropertyKey() const;
	virtual void NotifyPropertyChanged(RProperty& aProperty);
	};

/**
* The Platform notifier for CDvbhReceiverInfo::CPrivateData. 
*/	
NONSHARABLE_CLASS(TDvbhPlatformNotifier) : public TDvbhNotifier<MDvbhPlatformObserver>,
										   public MPropertyNotifier
	{
public:
	virtual TInt GetPropertyKey() const;
	virtual void NotifyPropertyChanged(RProperty& aProperty);
	};

/**
* The NetworkTime notifier for CDvbhReceiverInfo::CPrivateData. 
*/	
NONSHARABLE_CLASS(TDvbhNetworkTimeNotifier) : public TDvbhNotifier<MDvbhNetworkTimeObserver>,
											  public MPropertyNotifier
	{
public:
	virtual TInt GetPropertyKey() const;
	virtual void NotifyPropertyChanged(RProperty& aProperty);
	};

/**
* The Frequency notifier for CDvbhReceiverInfo::CPrivateData. 
*/	
NONSHARABLE_CLASS(TDvbhFrequencyNotifier) : public TDvbhNotifier<MDvbhFrequencyObserver>,
											public MPropertyNotifier
	{
public:
	virtual TInt GetPropertyKey() const;
	virtual void NotifyPropertyChanged(RProperty& aProperty);
	};

/**
* The CellId notifier for CDvbhReceiverInfo::CPrivateData. 
*/	
NONSHARABLE_CLASS(TDvbhCellIdNotifier) : public TDvbhNotifier<MDvbhCellIdObserver>,
										 public MPropertyNotifier
	{
public:
	virtual TInt GetPropertyKey() const;
	virtual void NotifyPropertyChanged(RProperty& aProperty);
	};

/**
* The NetworkId notifier for CDvbhReceiverInfo::CPrivateData. 
*/	
NONSHARABLE_CLASS(TDvbhNetworkIdNotifier) : public TDvbhNotifier<MDvbhNetworkIdObserver>,
											public MPropertyNotifier
	{
public:
	virtual TInt GetPropertyKey() const;
	virtual void NotifyPropertyChanged(RProperty& aProperty);
	};

/**
* The battery state for CDvbhReceiverInfo::CPrivateData. 
*/	
NONSHARABLE_CLASS(TDvbhExtBatteryStateNotifier) : public TDvbhNotifier<MDvbhExtBatteryStateObserver>,
											     public MPropertyNotifier
	{
public:
	virtual TInt GetPropertyKey() const;
	virtual void NotifyPropertyChanged(RProperty& aProperty);
	};

/**
* The connection state for CDvbhReceiverInfo::CPrivateData. 
*/	
NONSHARABLE_CLASS(TDvbhExtConnectionStateNotifier) : public TDvbhNotifier<MDvbhExtConnectionObserver>,
											         public MPropertyNotifier
	{
public:
	virtual TInt GetPropertyKey() const;
	virtual void NotifyPropertyChanged(RProperty& aProperty);
	};
    
/**
* The connection state for CDvbhReceiverInfo::CPrivateData. 
*/	
NONSHARABLE_CLASS(TMtvAccConnectionStateNotifier) : public TDvbhNotifier<MDvbhExtConnectionObserver>,
											        public MPropertyNotifier
	{
public:
	virtual TInt GetPropertyKey() const;
	virtual void NotifyPropertyChanged(RProperty& aProperty);
	};

/**
* The antenna connection state for CDvbhReceiverInfo::CPrivateData. 
*/	
NONSHARABLE_CLASS(TMtvAccAntennaConnectionStateNotifier) : public TDvbhNotifier<MDvbhExtAntennaConnectionObserver>,
											               public MPropertyNotifier
	{
public:
	virtual TInt GetPropertyKey() const;
	virtual void NotifyPropertyChanged(RProperty& aProperty);
	};


#include "dvbhreceiverinfoobserver.inl"
 
#endif //DVBHRECEIVERINFOOBSERVER_H






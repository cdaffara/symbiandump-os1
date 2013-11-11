/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef _LCFPSYDUMMY1_H_
#define _LCFPSYDUMMY1_H_


#include <lbs/epos_cpositioner.h>


class CLcfPsyDummy1: public CPositioner
{
public:
	static CLcfPsyDummy1* NewL(TAny* aConstructionParameters);
	CLcfPsyDummy1::~CLcfPsyDummy1();

private:
	void        ConstructL(TAny* aConstructionParameters);

public: // from CPositioner

	void        NotifyPositionUpdate(TPositionInfoBase& aPosInfo, 
							  TRequestStatus& aStatus);
	void        CancelNotifyPositionUpdate();
    TBool       TrackingOverridden() const;
    void        StartTrackingL(const TTimeIntervalMicroSeconds& aInterval);
    void        StopTracking();
    void        ServiceL(RMessage2& aMessage);

	void        GetPositionInfoL(TPositionInfoBase& aPosInfo);
	void        GetBasicPositionInfoL(TPositionInfoBase& aPosInfo);
	void        GetGenericPositionInfoL(TPositionInfoBase& aPosInfo);


private: // member variables

	TRequestStatus* iStatusPtr;

};




#endif

// End of file

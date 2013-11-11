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



#ifndef _LCFPSYDUMMY3_H_
#define _LCFPSYDUMMY3_H_


#include <lbs/epos_cpositioner.h>


class CLcfPsyDummy3: public CPositioner
{
public:
	static CLcfPsyDummy3* NewL(TAny* aConstructionParameters);
	CLcfPsyDummy3::~CLcfPsyDummy3();

private:
	void ConstructL(TAny* aConstructionParameters);

public: // from CPositioner

	void NotifyPositionUpdate(TPositionInfoBase& aPosInfo, 
							  TRequestStatus& aStatus);

	void GetPositionInfoL(TPositionInfoBase& aPosInfo);

	void GetBasicPositionInfoL(TPositionInfoBase& aPosInfo);

	void GetGenericPositionInfoL(TPositionInfoBase& aPosInfo);

	void GetPositionCourseInfoL(TPositionInfoBase& aPosInfo);

	void GetPositionSatelliteInfoL(TPositionInfoBase& aPosInfo);

	void CancelNotifyPositionUpdate();

private: // member variables

	TRequestStatus* iStatusPtr;

};




#endif

// End of file

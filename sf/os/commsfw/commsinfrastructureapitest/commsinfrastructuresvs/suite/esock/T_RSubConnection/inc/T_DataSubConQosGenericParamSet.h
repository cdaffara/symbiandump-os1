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


#ifndef __T_DATA_SUBCONQOSGENERICPARAMSET__
#define __T_DATA_SUBCONQOSGENERICPARAMSET__

//	User Includes
#include "T_DataCommsBase.h"

//	EPOC includes
#include <cs_subconparams.h>



class CT_DataSubConQosGenericParamSet: public CT_DataCommsBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_DataSubConQosGenericParamSet*	NewL();
	
	/**
	* Public destructor
	*/
	~CT_DataSubConQosGenericParamSet();	
	
	/**
	* Process a command read from the ini file
	*
	* \param aTestStep	test step requiring command to be processed
	* \param aCommand	the command to process
	* \param aEntry		the entry in the ini file requiring the command to be processed
	*
	* \return ETrue if the command is processed
	*/
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	


protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_DataSubConQosGenericParamSet();
	
		/**
	* Return a pointer to the object that the data wraps
	*
	* \return pointer to the object that the data wraps
	*/
	virtual TAny*	GetObject();

private:
	// DoCommandL delegated test methods
	void				DoCmdConstructorL( const TDesC& aSection );
	void				DoCmdDestructor();
	void				DoCmdGetDownLinkAveragePacketSize( const TDesC& aSection );
	void				DoCmdGetDownLinkBandwidth( const TDesC& aSection );
	void				DoCmdGetDownLinkDelay( const TDesC& aSection );
	void				DoCmdGetDownLinkDelayVariation( const TDesC& aSection );
	void				DoCmdGetDownLinkMaximumBurstSize( const TDesC& aSection );
	void				DoCmdGetDownLinkMaximumPacketSize( const TDesC& aSection );
	void				DoCmdGetDownLinkPriority( const TDesC& aSection );
	void				DoCmdGetHeaderMode( const TDesC& aSection );
	void				DoCmdGetName( const TDesC& aSection );
	void				DoCmdGetUpLinkAveragePacketSize( const TDesC& aSection );
	void				DoCmdGetUpLinkBandwidth( const TDesC& aSection );
	void				DoCmdGetUpLinkDelay( const TDesC& aSection );
	void				DoCmdGetUpLinkDelayVariation( const TDesC& aSection );
	void				DoCmdGetUpLinkMaximumBurstSize( const TDesC& aSection );
	void				DoCmdGetUpLinkMaximumPacketSize( const TDesC& aSection );
	void				DoCmdGetUpLinkPriority( const TDesC& aSection );
	void				DoCmdSetDownLinkAveragePacketSize( const TDesC& aSection );
	void				DoCmdSetDownLinkBandwidth( const TDesC& aSection );
	void				DoCmdSetDownLinkDelay( const TDesC& aSection );
	void				DoCmdSetDownLinkDelayVariation( const TDesC& aSection );
	void				DoCmdSetDownLinkMaximumBurstSize( const TDesC& aSection );
	void				DoCmdSetDownLinkMaximumPacketSize( const TDesC& aSection );
	void				DoCmdSetDownLinkPriority( const TDesC& aSection );
	void				DoCmdSetHeaderMode( const TDesC& aSection );
	void				DoCmdSetName( const TDesC& aSection );
	void				DoCmdSetUpLinkAveragePacketSize( const TDesC& aSection );
	void				DoCmdSetUpLinkBandwidth( const TDesC& aSection );
	void				DoCmdSetUpLinkDelay( const TDesC& aSection );
	void				DoCmdSetUpLinkDelayVariation( const TDesC& aSection );
	void				DoCmdSetUpLinkMaximumBurstSize( const TDesC& aSection );
	void				DoCmdSetUpLinkMaximumPacketSize( const TDesC& aSection );
	void				DoCmdSetUpLinkPriority( const TDesC& aSection );

private:
	CSubConQosGenericParamSet*         iSubConQosGenericParamSet;    
	};
	
_LIT(KDataSubConQosGenericParamSet,	"CSubConQosGenericParamSet");

#endif /* __T_DATA_SUBCONQOSGENERICPARAMSET__ */

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
// This file is part of the EPOC Emulator Variant Base Port
// Hardware Configuration Repository Platform Specific Layer (PSL) 
//


// -- INCLUDES ----------------------------------------------------------------


#include "hcr_debug.h"

#include "hcr_hai.h"



// -- GLOBALS -----------------------------------------------------------------


GLREF_C HCR::SRepositoryCompiled gRepository;


// -- CLASSES- ----------------------------------------------------------------


class HCRVariant : public HCR::MVariant
    {    
    
public:

    HCRVariant();
    virtual ~HCRVariant();
    
public:    
    
    TInt Initialise();
    
    TBool IgnoreCoreImgRepository();
	TInt GetCompiledRepositoryAddress( TAny* & aAddr);
    TInt GetOverrideRepositoryAddress( TAny* & aAddr);
    
    };
    
    


// -- METHODS -----------------------------------------------------------------


HCRVariant::HCRVariant()
    {
    HCR_FUNC("HCRVariant");
    }


HCRVariant::~HCRVariant()
    {
    HCR_FUNC("~HCRVariant");
    }


TInt HCRVariant::Initialise()
    {
    HCR_FUNC("HCRVariant::Initialise");
    
    HCR_TRACE_RETURN(KErrNone);
    }
    
    
TInt HCRVariant::GetCompiledRepositoryAddress( TAny* & aAddr)
    {
    HCR_FUNC("HCRVariant::GetCompiledRepositoryAddress");
        
    aAddr = static_cast<TAny*>(&gRepository);
    HCR_TRACE_RETURN(KErrNone);
    }

TBool HCRVariant::IgnoreCoreImgRepository()
    {
    HCR_FUNC("HCRVariant::IgnoreCoreImgRepository");
        
    HCR_TRACE_RETURN(EFalse);
    }

TInt HCRVariant::GetOverrideRepositoryAddress( TAny* &)
    {
    HCR_FUNC("HCRVariant::GetRAMRepositoryAddress");
    
    // OverrideRepository not supported in EPOC Emulator version
    
    HCR_TRACE_RETURN(KErrNotSupported);
    }

 
// -- ENTRY POINTS ------------------------------------------------------------


GLDEF_C HCR::MVariant* CreateHCRVariant()
    {
    HCR_FUNC("CreateHCRVariant");

    return new HCRVariant;
    }

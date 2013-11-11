Copyright (C) 2004 Symbian Software Ltd. All Rights Reserved.

BUILDING alternative schemas:

1. The base schema is stored in com/symbian/commdb/base_schema, and supports record-level ReadOnly / Hidden
and table-level ReadOnly properties.
XLST tranformation is used to create schemas from this base schema as required.
Note: You must have an XSLT software installed. The default coversion scripts use MSXLS.
MSXLS should be installed, and the PATH environment variabl updated to inlude it.
 
2. To build the CXMLED XSchema which support ReadOnly / Hidden, run build-hidden_ro_support.bat
3. To build the CXLMED XSchema with not Hidden / ReadOnly support, run  build-regular.bat
Note: Please make sure that the schema was built correctly by examining the resulting XLM/ XSD files.


4. To export the schema with Hidden / ReadOnly support, run "bldmake bldfiles" and then "abld export"
from the hidden_readonly_support\group directory.  
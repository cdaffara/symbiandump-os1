The schema is used by the Symbian CommDB Editor to outline which tables will exist and their format.

The schema table entries must be matched to the CED dbdefs.h.

Currently some IFDEF exist in dbdefs (c++) which can not be reproduced in the schema (xml) so CED will not be
able to accept table entries that exist in the schema but have been IFDEF out in the dbdefs.h file.

Hence now the schema will be divided into a folder for each release.

A table should only be inserted into a release schema if it will definitely be included by the dbdefs.h and not an IFDEF.

At MS4 when the licensee receives the build the schema should be checked against the dbdefs.h and the Symbian CommDB Editor packaged and uploaded to the developer site.

Add new schemas as required.  Also adding to the bld.inf so files exported to epoc32/tools/

last update - Agnelo Vaz - 10 August 2006
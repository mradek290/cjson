
#ifndef CJSON_IMPL_GUARD
#define CJSON_IMPL_GUARD

#include "cjson.h"

cjsonObject* cjson__ExtractObject( char*, char**, hmap*, cjsonError* );
cjsonArray*  cjson__ExtractArray( char*, char**, hmap*, cjsonError* );

_Bool cjson__ExtractAddress( const void* v, unsigned n ){
    return *((const unsigned long long*)v) & (1ull << n);
}

_Bool cjson__CompareAddress( const void* a, const void* b ){
    return *((const unsigned long long*)a) == *((const unsigned long long)b);
}

cjson_StringAddressMapping* cjson__Address2String( hmap* map, void* adr ){
    return (cjson__StringAddressMapping*) hmapGet( map, &adr );
}

_Bool cjson__StringWriteBackDelete( void* v, void* ex ){

    //TODO

    return 1;
}

_Bool cjson__StringWriteBack( void* v, void* ex ){

    //TODO

    return 1;
}

hmap* cjson__ExpungeStrings( char* input ){

    //TODO

    return 0;
}

cjsonError cjson__DeilmCheck( const char* input, unsigned input_sz ){

    //TODO

    return cjsonerr_Unkown;
}

cjsonDataField* cjsonParse( char* input, unsigned input_sz, cjsonError* error ){

    //TODO
    
    return 0;
}

void cjsonFree( cjsonDataField* datafield ){

    //TODO

}

cjsonObject* cjsonObjectInit(){

}

cjsonArray* cjsonArrayInit( unsigned sz ){

}

cjsonDataField* cjsonGetObjectField( cjsonObject* obj, const char* fieldname ){

    //TODO

    return 0;
}

cjsonDataField* cjsonGetArrayData( cjsonArray* obj, unsigned n ){
    void* adr = ((_Bool*)arr) + sizeof(jsonArray) + n * sizeof(cjsonDataField);
    return (cjsonDataField*)adr;
}

#endif

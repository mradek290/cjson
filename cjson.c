
#ifndef CJSON_IMPL_GUARD
#define CJSON_IMPL_GUARD

#include "cjson.h"

cjsonObject* cjson__ExtractObject( char*, char**, hmap*, cjsonError* );
cjsonArray*  cjson__ExtractArray( char*, char**, hmap*, cjsonError* );

_Bool cjson__ExtractAddress( const void* v, unsigned n ){
    return *((const unsigned long long*)v) & (1ull << n);
}

_Bool cjson__CompareAddress( const void* a, const void* b ){
    return *((const unsigned long long*)a) == *((const unsigned long long*)b);
}

cjson__StringAddressMapping* cjson__Address2String( hmap* map, void* adr ){
    return (cjson__StringAddressMapping*) hmapGet( map, &adr );
}

_Bool cjson__ExtractObjectName( const void* v, unsigned n ){
    const char* str = *((const char**)v);
    return str[n/8] & (1ull << (n%8));
}

_Bool cjson__CompareObjectName( const void* a, const void* b ){
    const char* sa = *((const char**)a);
    const char* sb = *((const char**)b);
    unsigned ssa = strlen(sa);
    unsigned ssb = strlen(sb);
    return ssa == ssb ? !memcmp(sa,sb,ssa) : 0;
}

void cjson__ObjectAddField( cjsonObject* obj, cjsonObjectField* field ){

    cjsonObjectField* adr = (cjsonObjectField*) hmapGet( obj->Fields, &(field->Name) );
    *adr = *field;
    obj->Elements++;
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

    cjsonObject* obj = (cjsonObject*) malloc( sizeof(cjsonObject) );
    obj->Elements = 0;
    obj->Fields = hmapCreate(
        sizeof(cjsonObjectField),
        cjson__ExtractObjectName,
        cjson__CompareObjectName
    );

    return obj;
}

cjsonArray* cjsonArrayInit( unsigned sz ){

    cjsonArray* arr = (cjsonArray*) malloc(
        sizeof(cjsonArray) + sz * sizeof(cjsonDataField)
    );

    arr->Elements = sz;
    return arr;
}

cjsonDataField* cjsonGetObjectField( cjsonObject* obj, const char* fieldname ){

    if( !hmapExist(obj->Fields, &fieldname) ) return 0;
    cjsonObjectField* adr = (cjsonObjectField*) hmapGet(obj->Fields, &fieldname);
    return &(adr->Field);
}

cjsonDataField* cjsonGetArrayData( cjsonArray* arr, unsigned n ){
    void* adr = ((_Bool*)arr) + sizeof(cjsonArray) + n * sizeof(cjsonDataField);
    return (cjsonDataField*)adr;
}

const char* cjsonGetTypeName( cjsonType jt ){
    switch(jt){
        case cjsontype_Invalid : return "Invalid Type";
        case cjsontype_Object  : return "Object";
        case cjsontype_Array   : return "Array";
        case cjsontype_String  : return "String";
        case cjsontype_Integer : return "Integer";
        case cjsontype_Double  : return "Double";
        case cjsontype_Bool    : return "Boolean";
        case cjsontype_Null    : return "Null";
        default : return "Error: Invalid argument passed to cjsonGetTypeName";
    }
}

#endif

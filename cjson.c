
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

    cjson__StringAddressMapping* sam = (cjson__StringAddressMapping*)v;
    memcpy(
        sam->Address,
        sam->String,
        cfstrSize(sam->String)
    );

    cfstrFree(sam->String);
    return 1;
}

_Bool cjson__StringWriteBack( void* v, void* ex ){

    cjson__StringAddressMapping* sam = (cjson__StringAddressMapping*)v;
    memcpy(
        sam->Address,
        sam->String,
        cfstrSize(sam->String)
    );

    return 1;
}

hmap* cjson__ExpungeStrings( char* input ){

    char* p0 = strchr( input, cjson__strdelim0 );
    if( !p0++ ){
        return hmapCreate(
            sizeof(cjson__StringAddressMapping),
            cjson__ExtractAddress,
            cjson__CompareAddress
        );
    }

    char* p1 = strchr( p0, cjson__strdelim0 );
    while( p1 && p1[-1] == cjson__escapechr )
        p1 = strchr( p1+1, cjson__escapechr );

    if( !p1 ) return 0; //String lacking closing " found

    hmap* map = hmapCreate(
        sizeof(cjson__StringAddressMapping),
        cjson__ExtractAddress,
        cjson__CompareAddress
    );

    while(1){

        cjson__StringAddressMapping* sam = cjson__Address2String( map, p0 );
        sam->Address = p0;
        sam->String = cfstrCreateSz( p0, p1-p0 );
        memset( p0, cjson__unmarkchr, p1-p0 );

        p0 = strchr( p1+1, cjson__strdelim0 );
        if( !p0++ ) return map;

        p1 = strchr( p0, cjson__strdelim0 );
        while( p1 && p1[-1] == cjson__escapechr ){
            
            unsigned escapes = 0;
            char* pesc = p1-1;
            while( *pesc-- == cjson__escapechr ) ++escapes;
            if( escapes & 1 ){
                p1 = strchr( p1 + 1, cjson__strdelim0 );
            }else{
                break;
            }
        }

        if( !p1 ){
            hmapFree( map, cjson__StringWriteBackDelete, 0 );
            return 0;
        }
    }

}

cjsonError cjson__DeilmCheck( char* input, unsigned input_sz ){

    const char* end = input + input_sz;
    signed checksum0 = 0;
    signed checksum1 = 0;

    while( input < end && checksum0 >= 0 && checksum1 >= 0 ){
        switch(*input++){

            case cjson__objdelim0 :
                checksum0++;
                continue;

            case cjson__objdelim1 :
                checksum0--;
                continue;

            case cjson__arrdelim0 :
                checksum1++;
                continue;

            case cjson__arrdelim1 :
                checksum1--;
                continue;

            default : continue;
        }
    }

    if( checksum0 ) return cjsonerr_ObjDelim;
    if( checksum1 ) return cjsonerr_ArrDelim;
    return cjsonerr_NoError;
}

cjsonDataField* cjsonParse( char* input, unsigned input_sz, cjsonError* error ){

     cjsonError e = cjsonerr_Unkown;
    if( !error ){
        error = &e;
    }

    hmap* stringmap = cjson__ExpungeStrings(input);
    if( !stringmap ){
        *error = cjsonerr_StrDelim;
        return 0;
    }
    
    *error = cjson__DeilmCheck(input,input_sz);
    if( *error != cjsonerr_NoError ){
        hmapFree( stringmap, cjson__StringWriteBackDelete, 0 );
        return 0;
    }

    char* obj0 = strchr(input, cjson__objdelim0 );
    char* arr0 = strchr(input, cjson__arrdelim0 );
    if( obj0 || arr0 ){

        char* dummy;
        cjsonDataField* dat = (cjsonDataField*) malloc(sizeof(cjsonDataField));
        
        if( !arr0 || obj0 < arr0 ){

            dat->Type = cjsontype_Object;
            dat->Value.Object = cjson__ExtractObject(
                obj0, &dummy, stringmap, error
            );

            hmapFree( stringmap, cjson__StringWriteBack, 0 );
            return dat;
        }

        if( !obj0 || arr0 < obj0 ){

            dat->Type = cjsontype_Array;
            dat->Value.Array = cjson__ExtractArray(
                arr0, &dummy, stringmap, error
            );

            hmapFree( stringmap, cjson__StringWriteBack, 0 );
            return dat;
        }

        //This branch should never execute but just in case...
        free(dat);
        *error = cjsonerr_Unkown;
        hmapFree( stringmap, cjson__StringWriteBackDelete, 0 );
        return 0;

    }else{

        //we got some wierd input which is either complete garbage
        //or raw data without any array or objects
        *error = cjsonerr_BadData;
        hmapFree( stringmap, cjson__StringWriteBackDelete, 0 );
        return 0;
    }
}

void cjson__ExtractNumeral( char* start, char** end, cjsonDataField* cjdf ){

    char* stop = start;
    _Bool isfloating = 0;
    while(1){

        switch(*stop++){

            case '0' :
            case '1' :
            case '2' :
            case '3' :
            case '4' :
            case '5' :
            case '6' :
            case '7' :
            case '8' :
            case '9' :
            case '-' :
                continue;

            case '.' :
                isfloating = 1;
                continue;

            default : break;
        }

        break;
    }

    char* dummy;
    if( isfloating ){
        cjdf->Type = cjsontype_Double;
        cjdf->Value.Double = strtod(start, &dummy);
    }else{
        cjdf->Type = cjsontype_Integer;
        cjdf->Value.Integer = strtoll(start, &dummy, 10); 
    }

    *end = stop-1;
}

cjsonObject* cjson__ExtractObject( char* start, char** end, hmap* strings, cjsonError* e ){

    cjsonObject* obj = cjsonObjectInit();
    if( *e != cjsonerr_NoError ){
        return obj;
    }

    cjsonObjectField field;
    while(1){

        switch(*start++){

            case 0:
                *e = cjsonerr_BadData;

            case cjson__objdelim1 :
                *end = start;
                return obj;

            case cjson__strdelim0 :
                break;

            default : continue;
        }

        field.Name = cjson__Address2String(strings,start)->String;
        field.Field.Type = cjsontype_Invalid;
        start += 1 + cfstrSize(field.Name);

        while(1){
            switch(*start++){
            
                case 0 : //random null terminator??
                case cjson__objdelim1 :
                    *e = cjsonerr_BadData;
                    *end = start-1;
                    return obj;

                case 'n' :
                case 'N' :
                    field.Field.Type = cjsontype_Null;
                    cjson__ObjectAddField(obj, &field);
                    break;

                case 't' :
                case 'T' :
                    field.Field.Type = cjsontype_Bool;
                    field.Field.Value.Boolean = 1;
                    cjson__ObjectAddField(obj, &field);
                    break;

                case 'f' :
                case 'F' :
                    field.Field.Type = cjsontype_Bool;
                    field.Field.Value.Boolean = 0;
                    cjson__ObjectAddField(obj, &field);
                    break;

                case '-' :
                case '.' :
                case '0' :
                case '1' :
                case '2' :
                case '3' :
                case '4' :
                case '5' :
                case '6' :
                case '7' :
                case '8' :
                case '9' :
                    cjson__ExtractNumeral(start-1, &start, &(field.Field) );
                    cjson__ObjectAddField(obj, &field );
                    break;

                case cjson__objdelim0 :
                    field.Field.Type = cjsontype_Object;
                    field.Field.Value.Object = cjson__ExtractObject(
                        start-1, &start, strings, e
                    );
                    cjson__ObjectAddField(obj, &field );
                    break;

                case cjson__arrdelim0 :
                    field.Field.Type = cjsontype_Array;
                    field.Field.Value.Array = cjson__ExtractArray(
                        start, &start, strings, e
                    );
                    cjson__ObjectAddField(obj, &field);
                    break;

                case cjson__strdelim0 :
                    field.Field.Type = cjsontype_String;
                    field.Field.Value.String = cjson__Address2String(strings,start)->String;
                    cjson__ObjectAddField(obj, &field);
                    start += 1 + cfstrSize(field.Field.Value.String);
                    break;

                default : continue;
            }

            break;
        }
    }
}

cjson__ArrayNode* cjson__ArrayNodeInit( unsigned n ){

    unsigned mem_sz = sizeof(cjson__ArrayNode) + n * sizeof(cjsonDataField);
    cjson__ArrayNode* nd = (cjson__ArrayNode*) malloc(mem_sz);
    nd->Elements = 0;
    nd->Capacity = n;
    return nd;
}

cjsonDataField* cjson__ArrayNodeNext( cjson__ArrayNode** adr ){

    cjson__ArrayNode* nd = *adr;
    if( nd->Elements == nd->Capacity ){

        cjson__ArrayNode* newnode = cjson__ArrayNodeInit(
            __hmapGrowth(nd->Capacity)
        );

        newnode->Next = nd;
        *adr = newnode;
        nd = newnode;
    }

    unsigned slot = nd->Elements++;
    void* value = (_Bool*)(nd+1) + slot * sizeof(cjsonDataField);
    return (cjsonDataField*) value;
}

cjsonArray* cjson__ArrayNodeSolidify( cjson__ArrayNode* nd, unsigned total ){

    cjsonArray* arr = cjsonArrayInit(total);
    unsigned slots = 0;

    while( nd ){

        memcpy(
            cjsonGetArrayData(arr,slots),
            nd + 1,
            sizeof(cjsonDataField) * nd->Elements
        );
        
        slots += nd->Elements;
        cjson__ArrayNode* t = nd;
        nd = nd->Next;
        free(t);
    }

    return arr;
}

cjsonArray* cjson__ExtractArray( char* start, char** end, hmap* strings, cjsonError* e ){

    if( *e != cjsonerr_NoError ){
        return cjsonArrayInit(0);
    }

    unsigned total = 0;
    cjson__ArrayNode* nd = cjson__ArrayNodeInit(cjson__default_arraynode_sz);
    cjsonDataField* field;
    nd->Next = 0;

    while(1){
        switch(*start++){

            case 0 : //surprise null?!
                *end = start-1;

            case cjson__objdelim1 :
                *e = cjsonerr_BadData;
                return cjson__ArrayNodeSolidify(nd,total);

            case cjson__arrdelim1 :
                *end = start;
                return cjson__ArrayNodeSolidify(nd,total);

            case 'n' :
            case 'N' :
                ++total;
                field = cjson__ArrayNodeNext(&nd);
                field->Type = cjsontype_Null;
                break;

            case 't' :
            case 'T' :
                ++total;
                field = cjson__ArrayNodeNext(&nd);
                field->Type = cjsontype_Bool;
                field->Value.Boolean = 1;
                break;

            case 'f' :
            case 'F' :
                ++total;
                field = cjson__ArrayNodeNext(&nd);
                field->Type = cjsontype_Bool;
                field->Value.Boolean = 0;
                break;

            case '-' :
            case '.' :
            case '0' :
            case '1' :
            case '2' :
            case '3' :
            case '4' :
            case '5' :
            case '6' :
            case '7' :
            case '8' :
            case '9' :
                ++total;
                cjson__ExtractNumeral(
                    start-1, &start, cjson__ArrayNodeNext(&nd)
                );
                break;

            case cjson__objdelim0 :
                ++total;
                field = cjson__ArrayNodeNext(&nd);
                field->Type = cjsontype_Object;
                field->Value.Object = cjson__ExtractObject(
                    start-1, &start, strings, e
                );
                break;

            case cjson__arrdelim0 :
                ++total;
                field = cjson__ArrayNodeNext(&nd);
                field->Type = cjsontype_Array;
                field->Value.Array = cjson__ExtractArray(
                    start, &start, strings, e
                );
                break;

            case cjson__strdelim0 :
                ++total;
                field = cjson__ArrayNodeNext(&nd);
                field->Type = cjsontype_String;
                field->Value.String = cjson__Address2String(strings,start)->String;
                start += 1 + cfstrSize(field->Value.String);
                break;

            default : continue;
        }
    }
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

const char* cjsonGetErrorName( cjsonError je ){
    switch(je){
        case cjsonerr_NoError  : return "No Error";
        case cjsonerr_ObjDelim : return "Object encapsulation failure";
        case cjsonerr_ArrDelim : return "Array encapsulation failure";
        case cjsonerr_StrDelim : return "String encapsulation failure";
        case cjsonerr_BadData  : return "Data Inconsistency";
        case cjsonerr_Unkown   : return "Unkown";
        default : return "Error: Invalid argument passed to cjsonGetErrorName";
    }
}

#endif

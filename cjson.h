
#ifndef CJSON_HEADER_GUARD
#define CJSON_HEADER_GUARD

#ifdef CJSON_DLL_EXPORT
#define CJSON_DECLSPEC __declspec(dllexport)
#else
#define CJSON_DECLSPEC
#endif

#include <stdio.h>
#include "hmap/hmap.c"
#include "comfystring/comfystr.c"

#define cjson__objdelim0 '{'
#define cjson__objdelim1 '}'
#define cjson__arrdelim0 '['
#define cjson__arrdelim1 ']'
#define cjson__strdelim0 '\"'
#define cjson__escapechr '\\'
#define cjson__unmarkchr 7
#define cjson__default_arraynode_sz 16

typedef enum {
    cjsonerr_NoError = 0,
    cjsonerr_ObjDelim,
    cjsonerr_StrDelim,
    cjsonerr_ArrDelim,
    cjsonerr_BadData,
    cjsonerr_Unkown
} cjsonError;

typedef enum cjson__Type {
    cjsontype_Invalid = 0,
    cjsontype_Object,
    cjsontype_Array,
    cjsontype_String,
    cjsontype_Integer,
    cjsontype_Double,
    cjsontype_Bool,
    cjsontype_Null
} cjsonType;

struct cjson__Object;
struct cjson__Array;
union  cjson__Value;

typedef struct cjson__ArrayNodeTag {
    unsigned Elements;
    unsigned Capacity;
    struct cjson__ArrayNodeTag* Next;
    //struct cjson__Value* Data; //Implicit!
} cjson__ArrayNode;

typedef struct {
    void* Address;
    char* String;
    _Bool Owning;
} cjson__StringAddressMapping;

typedef union cjson__Value {
    _Bool Boolean;
    double Double;
    long long Integer;
    char* String;
    struct cjson__Array* Array;
    struct cjson__Object* Object;
} cjsonValue;

typedef struct {
    union cjson__Value Value;
    enum  cjson__Type Type;
} cjsonDataField;

typedef struct {
    char* Name;
    cjsonDataField Field;
} cjsonObjectField;

typedef struct cjson__Object {
    hmap* Fields;
    unsigned Elements;
} cjsonObject;

typedef struct cjson__Array {
    unsigned Elements;
} cjsonArray;

typedef _Bool (*cjsonObjectLambda)( cjsonObjectField*, void* );

CJSON_DECLSPEC cjsonDataField* cjsonInitRoot();
CJSON_DECLSPEC cjsonObject* cjsonInitObject();
CJSON_DECLSPEC cjsonArray*  cjsonInitArray(unsigned);

CJSON_DECLSPEC cjsonDataField* cjsonGetObjectField( cjsonObject*, const char* );
CJSON_DECLSPEC cjsonDataField* cjsonGetArrayData( cjsonArray*, unsigned );

/*-----------*/void cjsonEnumerateObject( cjsonObject*, cjsonObjectLambda, void* );
CJSON_DECLSPEC void cjsonObjectAddField( cjsonObject*, cjsonObjectField* );

CJSON_DECLSPEC cjsonDataField* cjsonParse( char*, unsigned, cjsonError* );
CJSON_DECLSPEC char* cjsonSerialize( cjsonDataField* );

CJSON_DECLSPEC void cjsonFree( cjsonDataField* );

CJSON_DECLSPEC const char* cjsonGetTypeName( cjsonType );
CJSON_DECLSPEC const char* cjsonGetErrorName( cjsonError );

//------------------------------------------------------------------------

#ifdef CJSON_DLL_EXPORT

CJSON_DECLSPEC void cjsonCleanUpString( char* );

CJSON_DECLSPEC cjsonType cjsonGetDataType( cjsonDataField* );
CJSON_DECLSPEC long long cjsonGetDataAsInt( cjsonDataField* );
CJSON_DECLSPEC double cjsonGetDataAsDouble( cjsonDataField* );
CJSON_DECLSPEC _Bool cjsonGetDataAsBool( cjsonDataField* );
CJSON_DECLSPEC char* cjsonGetDataAsString( cjsonDataField* );
CJSON_DECLSPEC cjsonObject* cjsonGetDataAsObject( cjsonDataField* );
CJSON_DECLSPEC cjsonArray*  cjsonGetDataAsArray( cjsonDataField* );

CJSON_DECLSPEC void cjsonSetDataAsInt( cjsonDataField*, long long );
CJSON_DECLSPEC void cjsonSetDataAsDouble( cjsonDataField*, double );
CJSON_DECLSPEC void cjsonSetDataAsBool( cjsonDataField*, _Bool );
CJSON_DECLSPEC void cjsonSetDataAsString( cjsonDataField*, const char* );
CJSON_DECLSPEC void cjsonSetDataAsObject( cjsonDataField*, cjsonObject* );
CJSON_DECLSPEC void cjsonSetDataAsArray( cjsonDataField*, cjsonArray* );

CJSON_DECLSPEC unsigned cjsonGetArraySize( cjsonArray* );
CJSON_DECLSPEC unsigned cjsonGetObjectElementCount( cjsonObject* );
CJSON_DECLSPEC cjsonObjectField* cjsonGetObjectFields( cjsonObject* );
CJSON_DECLSPEC cjsonObjectField* cjsonAccessObjectFields( cjsonObjectField*, unsigned );

CJSON_DECLSPEC char* cjsonGetObjectFieldName( cjsonObjectField* );
CJSON_DECLSPEC cjsonType cjsonGetObjectFieldType( cjsonObjectField* );
CJSON_DECLSPEC cjsonDataField* cjsonGetObjectFieldData( cjsonObjectField* );
#endif

#endif

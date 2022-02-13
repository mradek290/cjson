
#ifndef CJSON_HEADER_GUARD
#define CJSON_HEADER_GUARD

#include "hmap/hmap.c"
#include "comfystring/comfystr.c"

#define cjson__objdelim0 '{'
#define cjson__objdelim1 '}'
#define cjson__arrdelim0 '['
#define cjson__arrdelim1 ']'
#define cjson__strdelim0 '\"'
#define cjson__escapechr '\\'
#define cjson__unmarkchr 7

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

typedef struct {
    void* Address;
    char* String;
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

cjsonObject* cjsonObjectInit();
cjsonArray*  cjsonArrayInit(unsigned);

cjsonDataField* cjsonGetObjectField( cjsonObject*, const char* );
cjsonDataField* cjsonGetArrayData( cjsonArray*, unsigned );

cjsonDataField* cjsonParse( char*, unsigned, cjsonError* );

void cjsonFree( cjsonDataField* );

const char* cjsonGetTypeName( cjsonType );

#endif

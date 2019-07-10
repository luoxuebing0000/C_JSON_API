#ifndef __datatype_def_h__
#define __datatype_def_h__

typedef unsigned int U32;
enum BOOL {TRUE, FALSE};
enum json_flag { JSON_NONE, JSON_BOOL, JSON_STR, JSON_ARR, JSON_OBJ, JSON_NUM };
enum json_split { NO_COMMA, WITH_COMMA};// ÓÐÎÞ¶ººÅ
typedef struct _tag_array array;
typedef struct _tag_keyvalue keyvalue;
typedef struct _tag_object object;
typedef struct _tag_json Json;

struct _tag_array
{
	Json** elems;
	U32 count;
};

struct _tag_keyvalue
{
	char* key;
	Json* val;
};

struct _tag_object
{
	keyvalue* kvs;
	U32 count;
};

struct _tag_json
{
	enum json_flag type;
	union
	{
		double num;
		enum BOOL bol;
		char* str;
		array arr;
		object obj;
	};
};

#endif

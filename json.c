#define _CRT_SECURE_NO_WARNINGS
#define __STDC_LIMIT_MACROS

#include "json.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <string.h>


enum json_split { NO_COMMA = 0, WITH_COMMA };// 有无逗号
typedef struct _tag_array array;
typedef struct _tag_keyvalue keyvalue;
typedef struct _tag_object object;

/**
 *  打印传入的数值
 *  @param num 要打印的数值
 *  @return  无
 */
static void print_num(double num);

/**
 *  打印传入的布尔类型对应的字符串
 *  @param bol 若为TRUE，则打印"true"，若为FALSE，则为"false"
 *  @return  无
 */
static void print_bool(enum BOOL bol);

/**
 *  打印传入的字符串
 *  @param str 要打印的字符串首地址
 *  @return  无
 */
static void print_str(const char* str);

/**
 *  将json转换成yaml格式，并按照给定的deep深度控制打印格式
 *  @param 
 *    -json 必须为JSON_OBJ类型，非JSON_OBJ类型无法转成yaml格式
 *    -deep 深度，打印格式需使用deep控制打印的空格数
 *  @return  无
 */
static void json_to_yaml_print_deep(const Json* json, int deep);

/**
 *  将json对象转成yaml格式时遇到JSON_ARR时使用
 *  @param 
 *    -arr 要打印成yaml格式的JSON_ARR对象
 *    -deep 深度，打印格式需使用deep控制打印的空格数
 *  @return  无
 */
static void yaml_print_arr(const array* arr, int deep);

/**
 *  将json对象转成yaml格式时遇到JSON_OBJ时使用
 *  @param 
 *    -obj 要打印成yaml格式的JSON_OBJ对象
 *    -deep 深度，打印格式需使用deep控制打印的空格数
 *  @return  无
 */
static void yaml_print_obj(const object* obj, int deep);

/**
 *  打印deep空格
 *  @param deep 打印的空格数
 *  @return  无
 */
static void yaml_space_print(int deep);

//char* json_to_yaml_str_deep(const Json* json, int deep);
//char* yaml_str_arr(const array* arr, int deep);
//char* yaml_str_obj(const object* obj, int deep);
//char* yaml_str_num(double num);
//char* yaml_str_bool(enum BOOL bol);
//char* yaml_str_str(const char* str);
//char* yaml_str_cat(char* str1, char* str2);
//char* yaml_space_str(int deep);
/**
 *  清空一个Json变量数据
 *  @param json 要释放的Json变量的地址，清空Json变量存储的数据
 *  @return  创建的Json对象的首地址，使用malloc申请内存，在函数外面进行内存释放
 */
static void json_clear(Json* json);

/**
 * 递归打印json数据
 *  @param
 *      -json 通用类型的json对象
 *      -deep 当前递归深度，用于打印table键
 *      -flag 标志，用来决定json最后一个value后面有没有逗号
 *  @return  无
 */
static void json_print_val_deep(Json* json, int deep, enum json_split flag);

/**
 * 释放array数组中的数据（回收内存）
 *  @param
 *      -arr 通用类型的array类型变量
 *  @return  无
 */
static void json_free_array(array* arr);

/**
 * 释放str存储的数据（回收内存）
 *  @param
 *      -str 字符串的地址，使用二级地址，避免野指针
 *  @return  无
 */
static void json_free_str(char** str);

/**
 * 释放object中的数据（回收内存）
 *  @param
 *      -obj 通用的obj类型地址
 *  @return  无
 */
static void json_free_object(object* obj);

/**
 * 打印传入的自定义的数组对象存储的数据，
 *  @param
 *      -arr 自定义的array类型变量
 *  @return  无
 */
static void json_print_arr(const array* arr, int* deep);

/**
 * 打印传入的自定义的object类型变量存储的数据，
 *  @param
 *      -obj 自定义的object类型变量地址
 *      -deep 打印的table个数
 *  @return  无
 */
static void json_print_obj(const object* obj, int* deep);

/**
 * 打印deep个table键，
 *  @param
 *      -deep 打印的table个数
 *  @return  无
 */
static void format_print_tbl(int deep);

/**
 * 格式控制，控制是否需要打印 ","
 *  @param
 *      -flag 若为 WITT_COMMA，则打印逗号，若为NO_COMMA，则不打印逗号
 *  @return  无
 */
static void format_control(enum json_split flag);

/**
 * JSON_ARR类型修改值处理函数
 *  @param
 *      -json 要修改的JSON_ARR json变量
 *      -key  通过key查找到要修改的val值
 *      -val  要修改成的val值
 *  @return  返回 0 则代表成功，返回负数，则表示失败
 */
static int json_type_arr_set_val(Json* json, const char* key, Json* val);

/**
 * JSON_OBJ类型修改值处理函数
 *  @param
 *      -json 要修改的JSON_OBJ json变量
 *      -key  通过key查找到要修改的val值
 *      -val  要修改成的val值
 *  @return  返回 0 则代表成功，返回负数，则表示失败
 */
static int json_type_obj_set_val(Json* json, const char* key, Json* val);

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
	struct _tag_json* parent_node;
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

/**
 *  创建一个Json变量
 *  @return  创建的Json变量的首地址，使用malloc申请内存，在函数外面进行内存释放
 */
Json* json_create()
{
	int ret = 0;
	// 申请内存并判断
	Json* tmp = (Json*)calloc(1, sizeof(Json));
	if (tmp == NULL)
	{
		ret = -1;
		printf("func json_create() error: calloc memory error : %d\n", ret);
		return NULL;
	}
	tmp->parent_node = tmp;
	tmp->type = JSON_NONE;
	return tmp;
}

/**
 *  销毁一个Json变量
 *  @param json 要销毁的Json变量的地址，在此处进行内存释放
 *  @return 返回空值
 */
void json_destroy(Json* json)
{
	if (json == NULL)
		return;
	if (json->type != JSON_NONE)
	{
		json_clear(json);
	}
	free(json);
}

/**
 *  清空一个Json变量数据
 *  @param json 要释放的Json变量的地址，清空Json变量存储的数据
 *  @return  创建的Json对象的首地址，使用malloc申请内存，在函数外面进行内存释放
 */
static void json_clear(Json* json)
{
	assert(json != NULL);
	switch (json->type)
	{
	case JSON_ARR:
		json_free_array(&json->arr);
		break;
	case JSON_STR:
		json_free_str(&json->str);
		break;
	case JSON_OBJ:
		json_free_object(&json->obj);
		break;
	default:
		break;
	}
	memset(json, 0, sizeof(Json));
	json->parent_node = json;
	json->type = JSON_NONE;
}

/**
 *  向Json变量添加一个json格式键值对
 *  @param
 *      -json 要添加数据的json变量
 *      -key  要添加的键名
 *      -val  要添加的值
 *  @return  创建的Json对象的首地址，使用malloc申请内存，在函数外面进行内存释放
 */
int json_obj_add_member(Json* json, const char* key, Json* val)
{
	assert(json != NULL);
	assert(key != NULL);
	assert(val != NULL);

	int ret = 0;

	if (val->parent_node != NULL)
	{
		ret = -1;
		return ret;
	}

	// 判断json的类型，如果为OBJ类型或者为NONE类型则添加成员，若不是则提示操作失败
	if (json->type != JSON_OBJ && json->type != JSON_NONE)
	{
		ret = -2;
		printf("func json_obj_add_member() failed: json类型是非OBJ类型 %d\n", ret);
		return ret;
	}

	if (json->type == JSON_NONE)
	{
		json->type = JSON_OBJ;
	}

	keyvalue* kvs_tmp = NULL;
	if (json->obj.count == 0)
	{
		kvs_tmp = (keyvalue*)calloc(1, sizeof(keyvalue));
		if (kvs_tmp == NULL)
		{
			printf("func json_obj_add_member() error: calloc memory error\n");
		}
	}
	else
	{
		// 申请动态扩容的keyvalue的内存并初始化
		kvs_tmp = (keyvalue*)realloc(json->obj.kvs, (json->obj.count + 1) * sizeof(keyvalue));
		if (kvs_tmp == NULL)
		{
			ret = -3;
			printf("func json_obj_add_member() error: realloc memory error : %d\n", ret);
			return ret;
		}
	}
	json->obj.kvs = kvs_tmp;
	// 在keyvalue中最后一个添加key和val
	// TODO
	json->obj.kvs[json->obj.count].key = strdup(key);
	json->obj.kvs[json->obj.count].val = val;
	json->obj.count++;
	val->parent_node = json;
	return ret;
}

/**
 *  新建一个int型的json变量
 *  @param interger  整形值
 *  @return  创建的整形json对象
 */
Json* json_new_num(double num)
{
	int ret = 0;
	//定义一个Json变量并申请内存
	Json* json_ret = (Json*)calloc(1, sizeof(Json));
	if (json_ret == NULL)
	{
		ret = -1;
		printf("func json_new_num error: calloc memory error %d\n", ret);
		return NULL;
	}
	json_ret->parent_node = NULL;
	json_ret->type = JSON_NUM;
	json_ret->num = num;
	return json_ret;
}

/**
 *  新建一个bool型的json变量
 *  @param b  布尔值 true or false
 *  @return  创建的布尔型json对象
 */
Json* json_new_bool(enum BOOL b)
{
	int ret = 0;
	//定义一个Json变量并申请内存 
	Json* json_ret = (Json*)calloc(1, sizeof(Json));
	if (json_ret == NULL)
	{
		ret = -1;
		printf("func json_new_num error: calloc memory error %d\n", ret);
		return NULL;
	}
	json_ret->parent_node = NULL;
	json_ret->type = JSON_BOOL;
	json_ret->bol = b;
	return json_ret;
}

/**
 *  新建一个字符串型的json变量
 *  @param str  字符串
 *  @return  创建的字符串型json对象
 */
Json* json_new_str(const char* str)
{
	assert(str != NULL);
	int ret = 0;
	//定义一个Json变量并申请内存 
	Json* json_ret = (Json*)calloc(1, sizeof(Json));
	if (json_ret == NULL)
	{
		ret = -1;
		printf("func json_new_num error: calloc memory error %d\n", ret);
		return NULL;
	}
	json_ret->parent_node = NULL;
	json_ret->type = JSON_STR;
	char* str_tmp = strdup(str);
	if (str_tmp == NULL)
	{
		ret = -2;
		printf("func json_new_num error: calloc memory error %d\n", ret);
		goto END;
	}
	json_ret->str = str_tmp;
	return json_ret;
END:
	if (json_ret != NULL)
		free(json_ret);
	return NULL;
}

Json* json_new_array()
{
	int ret = 0;
	//定义一个Json变量并申请内存 
	Json* json_ret = (Json*)calloc(1, sizeof(Json));
	if (json_ret == NULL)
	{
		ret = -1;
		printf("func json_new_num error: calloc memory error %d\n", ret);
		return NULL;
	}
	json_ret->parent_node = NULL;
	json_ret->type = JSON_ARR;
	return json_ret;
}
Json* json_new_object()
{
	int ret = 0;
	//定义一个Json变量并申请内存 
	Json* json_ret = (Json*)calloc(1, sizeof(Json));
	if (json_ret == NULL)
	{
		ret = -1;
		printf("func json_new_num error: calloc memory error %d\n", ret);
		return NULL;
	}
	json_ret->parent_node = NULL;
	json_ret->type = JSON_OBJ;
	return json_ret;
}

/**
 *  向数组类型json对象中在指定位置中添加一个值
 *  @param
 *      -json 数组类型的json对象
 *      -idx  数组的索引
 *      -val  要添加的值
 *  @return  创建的对象类型的json对象
 */
int json_arr_add_elem(Json* json, U32 idx, Json* val)
{
	assert(json != NULL);
	assert(val != NULL);
	assert(idx >= 0);
	assert(json->type == JSON_ARR || json->type == JSON_NONE);

	int ret = 0;
	if (json->type == JSON_NONE)
	{
		json->type = JSON_ARR;
	}

	if (val->parent_node != NULL)
	{
		ret = -1;
		return ret;
	}

	// idx范围判断
	assert(json->arr.count <= idx);
	// if (json->arr.count < idx)
	// {
	// 	ret = -2;
	// 	printf("func json_arr_add_elem() failed: idx超过范围 %d\n", ret);

	// }

	// 扩容
	Json** json_tmp = NULL;
	if (json->arr.count == 0)
	{
		json_tmp = (Json**)calloc(1, sizeof(Json*));
		if (json_tmp == NULL)
		{
			fprintf(stderr, "func json_arr_add_elem() error: malloc memory error\n");
			return ret;
		}
	}
	else
	{
		json_tmp = (Json**)realloc(json->arr.elems, (json->arr.count + 1) * sizeof(Json*));
		if (json_tmp == NULL)
		{
			ret = -2;
			printf("func json_arr_add_elem() error: malloc memory error %d\n", ret);
			return ret;
		}

		for (int i = (int)json->arr.count; i > (int)idx; i--)
		{
			json_tmp[i] = json_tmp[i - 1];
		}
	}
	json_tmp[json->arr.count] = val;
	json->arr.elems = json_tmp;
	json->arr.count++;
	val->parent_node = json;
	return ret;
}

/**
 *  通过key获取指定的json变量的值
 *  @param
 *      -json 通用类型的json对象
 *      -key  传入的key值
 *  @return  获取的指定的json对象
 */
Json* json_get_val(Json* json, const char* key)
{
	assert(json != NULL);
	assert(key != NULL);

	return NULL;
}

/**
 *  打印指定的json对象中的数据
 *  @param
 *      -json 通用类型的json对象
 *  @return  无
 */
void json_print_val(Json* json)
{
	assert(json != NULL);
	int deep = 0;
	json_print_val_deep(json, deep, NO_COMMA);

}

/**
 * 递归打印json数据
 *  @param
 *      -json 通用类型的json对象
 *      -deep 当前递归深度，用于打印table键
 *      -flag 标志，用来决定json最后一个value后面有没有逗号
 *  @return  无
 */
static void json_print_val_deep(Json* json, int deep, enum json_split flag)
{
	assert(json != NULL);
	assert(deep >= 0);
	switch (json->type)
	{
	case JSON_NUM:
		print_num(json->num);
		format_control(flag);
		break;
	case JSON_BOOL:
		print_bool(json->bol);
		format_control(flag);
		break;
	case JSON_STR:
		print_str(json->str);
		format_control(flag);
		break;
	case JSON_ARR:
		json_print_arr(&json->arr, &deep);
		format_control(flag);
		break;
	case JSON_OBJ:
		json_print_obj(&json->obj, &deep);
		format_control(flag);
		break;
	default:
		break;
	}
	return;
}

/**
 * 释放array数组中的数据（回收内存）
 *  @param
 *      -arr 通用类型的array类型变量
 *  @return  无
 */
static void json_free_array(array* arr)
{
	assert(arr != NULL);
	for (U32 i = 0; i < arr->count; i++)
	{
		json_destroy(arr->elems[i]);
		arr->elems[i] = NULL;
	}
	free(arr->elems);
}

/**
 * 释放str存储的数据（回收内存）
 *  @param
 *      -str 字符串的地址，使用二级地址，避免野指针
 *  @return  无
 */
static void json_free_str(char** str)
{
	assert(str != NULL);
	char* tmp = *str;
	if (tmp == NULL)
		return;
	//printf("free str: %s\n",*str);
	free(*str);
	*str = NULL;
}

/**
 * 释放object中的数据（回收内存）
 *  @param
 *      -obj 通用的obj类型地址
 *  @return  无
 */
static void json_free_object(object* obj)
{
	assert(obj != NULL);
	for (U32 i = 0; i < obj->count; i++)
	{
		json_free_str(&obj->kvs[i].key);
		json_destroy(obj->kvs[i].val);
		obj->kvs[i].val = NULL;
	}
	free(obj->kvs);
}


/**
 * 打印传入的自定义的数组对象存储的数据，
 *  @param
 *      -arr 自定义的array类型变量
 *  @return  无
 */
static void json_print_arr(const array* arr, int* deep)
{
	assert(arr != NULL);
	assert(deep != NULL);
	printf("[\n");
	(*deep)++;
	for (U32 i = 0; i < arr->count; i++)
	{
		format_print_tbl(*deep);
		if (i != arr->count - 1)
			json_print_val_deep(arr->elems[i], *deep, WITH_COMMA);
		else
			json_print_val_deep(arr->elems[i], *deep, NO_COMMA);
	}
	format_print_tbl(*deep - 1);
	printf("]");
}

/**
 * 打印传入的自定义的object类型变量存储的数据，
 *  @param
 *      -obj 自定义的object类型变量地址
 *      -deep 打印的table个数
 *  @return  无
 */
static void json_print_obj(const object* obj, int* deep)
{
	assert(obj != NULL);
	assert(deep != NULL);
	printf("{\n");
	(*deep)++;

	for (U32 i = 0; i < obj->count; i++)
	{
		format_print_tbl(*deep);
		printf("\"%s\": ", obj->kvs[i].key);
		if (i == obj->count - 1)
			json_print_val_deep(obj->kvs[i].val, *deep, NO_COMMA);
		else
			json_print_val_deep(obj->kvs[i].val, *deep, WITH_COMMA);
	}
	format_print_tbl(*deep - 1);
	printf("}");
}

/**
 * 打印deep个table键，
 *  @param
 *      -deep 打印的table个数
 *  @return  无
 */
static void format_print_tbl(int deep)
{
	assert(deep >= 0);
	for (int i = 0; i < deep; i++)
	{
		printf("\t");
	}
}

static void format_control(enum json_split flag)
{
	if (flag == WITH_COMMA)
	{
		printf(",\n");
	}
	else
	{
		printf("\n");
	}
}

/**
 * 修改json对象存储的key对应的value值
 *  @param
 *      -json 要修改的json对象
 *      -key  要查找的key值
 *      -val 要替换的value值
 *  @return
 *       0: 修改成功
 *      -1: json是一个value值或者为空，不存在key值
 */
int json_set_val(Json* json, const char* key, Json* val)
{
	assert(json != NULL);
	assert(key != NULL);
	assert(val != NULL);
	int ret = 0;
	// 判断json存储的类型
	if (json->type != JSON_ARR && json->type != JSON_OBJ)
	{
		ret = -1;
		printf("func json_set_val error: json中无key值 %d\n", ret);
		return ret;
	}
	if (json->type == JSON_ARR) // 若json的类型为数组类型
	{
		json_type_arr_set_val(json, key, val);
	}
	else // json的类型为OBJ类型
	{
		json_type_obj_set_val(json, key, val);
	}
	return ret;
}

/**
 * JSON_ARR类型修改值处理函数
 *  @param
 *      -json 要修改的JSON_ARR json变量
 *      -key  通过key查找到要修改的val值
 *      -val  要修改成的val值
 *  @return  返回 0 则代表成功，返回负数，则表示失败
 */
static int json_type_arr_set_val(Json* json, const char* key, Json* val)
{
	assert(json != NULL);
	assert(key != NULL);
	assert(val != NULL);
	assert(json->type == JSON_ARR);
	int ret = 0;

	for (U32 i = 0; i < json->arr.count; i++)
	{
		if (json->arr.elems[i]->type == JSON_NONE ||
			json->arr.elems[i]->type == JSON_NUM ||
			json->arr.elems[i]->type == JSON_STR ||
			json->arr.elems[i]->type == JSON_BOOL)
		{
			ret = -1;
			continue;
		}
		else if (json->arr.elems[i]->type == JSON_ARR)
		{
			ret = json_type_arr_set_val(json->arr.elems[i], key, val);
		}
		else if (json->arr.elems[i]->type == JSON_OBJ)
		{
			ret = json_type_obj_set_val(json->arr.elems[i], key, val);
		}
		if (ret == 0)
			return ret;
	}
	return ret;
}

/**
 * JSON_OBJ类型修改值处理函数
 *  @param
 *      -json 要修改的JSON_OBJ json变量
 *      -key  通过key查找到要修改的val值
 *      -val  要修改成的val值
 *  @return  返回 0 则代表成功，返回负数，则表示失败
 */
static int json_type_obj_set_val(Json* json, const char* key, Json* val)
{
	assert(json != NULL);
	assert(key != NULL);
	assert(val != NULL);
	assert(json->type == JSON_OBJ);
	int ret = 0;
	for (U32 i = 0; i < json->obj.count; i++)
	{
		if (strcmp(json->obj.kvs[i].key, key) == 0)
		{
			json->obj.kvs[i].val = val;
		}
		else if (json->obj.kvs[i].val->type == JSON_NONE ||
			json->obj.kvs[i].val->type == JSON_NUM ||
			json->obj.kvs[i].val->type == JSON_STR ||
			json->obj.kvs[i].val->type == JSON_BOOL)
		{
			ret = -1;
			continue;
		}
		else if (json->obj.kvs[i].val->type == JSON_ARR)
		{
			ret = json_type_arr_set_val(json->obj.kvs[i].val, key, val);
		}
		else // JSON_OBJ
		{
			ret = json_type_obj_set_val(json->obj.kvs[i].val, key, val);
		}
		if (ret == 0)
			return ret;
	}
	return ret;
}

/**
 * 讲json格式数据转换成yaml格式并打印到控制台
 *  @param
 *      -json 要求：必须是json格式对象才可以(JSON_OBJ)，否则报错
 *  @return  无
 */
void json_to_yaml_print(const Json* json)
{
	assert(json != NULL);
	// 必须为json格式数据
	assert(json->type == JSON_OBJ);
	int deep = 0;
	json_to_yaml_print_deep(json, deep);
}

/**
 * 讲json格式数据转换成yaml格式并使用字符串传出
 *  @param
 *      -json 要求：必须是json格式对象才可以(JSON_OBJ)，否则报错
 *  @return  传出一个字符串，内存由里面管理，使用后须在外面释放内存
 *  @其他说明：
 *      函数功能已经实现，但是内存申请失败后申请的内存没有释放，存在内存泄露，所以注释了
 */
//char* json_to_yaml_str(const Json* json)
//{
//	assert(json != NULL);
//	assert(json->type == JSON_OBJ);
//	int deep = 0;
//	char* str = json_to_yaml_str_deep(json, deep);
//	if (str == NULL)
//		fprintf(stderr, "func json_to_yaml_str_deep error -1\n");
//	return str;
//}

/**
 *  将json转换成yaml格式，并按照给定的deep深度控制打印格式
 *  @param 
 *    -json 必须为JSON_OBJ类型，非JSON_OBJ类型无法转成yaml格式
 *    -deep 深度，打印格式需使用deep控制打印的空格数
 *  @return  无
 */
static  void json_to_yaml_print_deep(const Json* json, int deep)
{
	assert(json != NULL);
	assert(deep >= 0);
	// 通过json的类型，分别使用对应类型的函数处理json数据
	switch (json->type)
	{
	case JSON_NUM:
		print_num(json->num);
		break;
	case JSON_BOOL:
		print_bool(json->bol);
		break;
	case JSON_STR:
		print_str(json->str);
		break;
	case JSON_ARR:
		yaml_print_arr(&json->arr, deep);
		break;
	case JSON_OBJ:
		yaml_print_obj(&json->obj, deep);
		break;
	default:
		break;
	}
}

/**
 *  将json对象转成yaml格式时遇到JSON_ARR时使用
 *  @param 
 *    -arr 要打印成yaml格式的JSON_ARR对象
 *    -deep 深度，打印格式需使用deep控制打印的空格数
 *  @return  无
 */
static  void yaml_print_arr(const array* arr, int deep)
{
	assert(arr != NULL);
	//assert(deep <= __INT_MAX__);
	// 循环处理arr对象存储的数据  array{Json **val; U32 count}
	for (U32 i = 0; i < arr->count; i++)
	{
		// yaml格式数据遇到数组，需要使用”- “标记，需要特殊处理 
		if (arr->elems[i]->type != JSON_ARR)
		{
			printf("\n");
			yaml_space_print(deep);
			printf("- ");
			// 若数据存储的数据中为对象，则需要换行打印，
			if (arr->elems[i]->type == JSON_OBJ)
			{
				printf("\n");
			}
		}

		// 根据JSON数据对象总存储JSON类型，分别使用对应的函数处理
		switch (arr->elems[i]->type)
		{
		case JSON_OBJ:
			yaml_print_obj(&arr->elems[i]->obj, deep + 1);
			break;
		case JSON_ARR:
			printf("\n");
			yaml_print_arr(&arr->elems[i]->arr, deep + 1);
			break;
		case JSON_NUM:
			print_num(arr->elems[i]->num);
			break;
		case JSON_BOOL:
			print_bool(arr->elems[i]->bol);
			break;
		case JSON_STR:
			print_str(arr->elems[i]->str);
			break;
		default:
			break;
		}
	}
}

/**
 *  将json对象转成yaml格式时遇到JSON_OBJ时使用
 *  @param 
 *    -obj 要打印成yaml格式的JSON_OBJ对象
 *    -deep 深度，打印格式需使用deep控制打印的空格数
 *  @return  无
 */
static  void yaml_print_obj(const object* obj, int deep)
{
	assert(obj != NULL);
	U32 len = obj->count;
	// 循环处理obj存储的kvs数组  (keyvalue {char *key;Json* val})
	for (U32 i = 0; i < len; i++)
	{
		// 打印空格
		yaml_space_print(deep);
		// 输出key值
		printf("%s: ", obj->kvs[i].key);
		// 通过判断json->kvs[i].val的类型，分别使用对应的函数处理json格式数据到yaml格式转换并打印
		switch (obj->kvs[i].val->type)
		{
		case JSON_OBJ:
			printf("\n");
			yaml_print_obj(&obj->kvs[i].val->obj, deep + 1);
			break;
		case JSON_ARR:
			yaml_print_arr(&obj->kvs[i].val->arr, deep + 1);
			break;
		case JSON_NUM:
			print_num(obj->kvs[i].val->num);
			break;
		case JSON_BOOL:
			print_bool(obj->kvs[i].val->bol);
			break;
		case JSON_STR:
			print_str(obj->kvs[i].val->str);
			break;
		default:
			break;
		}
		if (i != len - 1)
			printf("\n");
	}
}

/**
 *  打印deep空格
 *  @param deep 打印的空格数
 *  @return  无
 */
static void yaml_space_print(int deep)
{
	// 循环打印deep个双空格
	for (int i = 0; i < deep; i++)
		printf("  ");
}

/**
 *  打印传入的数值
 *  @param num 要打印的数值
 *  @return  无
 */
static void print_num(double num)
{
	printf("%lf", num);
}
static void print_bool(enum BOOL bol)
{
	assert(bol == TRUE || bol == FALSE);
	if (bol == TRUE)
	{
		printf("true");
	}
	else
	{
		printf("false");
	}
}

/**
 *  打印传入的字符串
 *  @param str 要打印的字符串首地址
 *  @return  无
 */
static void print_str(const char* str)
{
	// 如果str为空字符串，则打印null以替代
	if (str == NULL)
	{
		printf("null");
		return;
	}
	else
	{
		printf("%s", str);
	}
}


//char* json_to_yaml_str_deep(const Json* json, int deep)
//{
//	assert(json != NULL);
//	assert(deep >= 0);
//	char* tmp = NULL;
//	switch (json->type)
//	{
//	case JSON_NUM:
//		tmp = yaml_str_num(json->num);
//		break;
//	case JSON_BOOL:
//		tmp = yaml_str_bool(json->bol);
//		break;
//	case JSON_STR:
//		tmp = yaml_str_str(json->str);
//		break;
//	case JSON_ARR:
//		tmp = yaml_str_arr(&json->arr, deep);
//		break;
//	case JSON_OBJ:
//		tmp = yaml_str_obj(&json->obj, deep);
//		break;
//	default:
//		break;
//	}
//	return tmp;
//}
//char* yaml_str_arr(const array* arr, int deep)
//{
//	assert(arr != NULL);
//	//assert(deep <= __INT_MAX__);
//	char* ret_str = (char*)calloc(4096, 1); // 40k
//	if (ret_str == NULL)
//	{
//		fprintf(stderr, "func yaml_str_arr error: alloc memory error: -1\n");
//		return NULL;
//	}
//	char* tmp_str = NULL;
//	for (U32 i = 0; i < arr->count; i++)
//	{
//		if (arr->elems[i]->type != JSON_ARR)
//		{
//			sprintf(ret_str + strlen(ret_str), "\n");
//			tmp_str = yaml_space_str(deep);
//			if (tmp_str == NULL)
//			{
//				fprintf(stderr, "func yaml_str_arr error: yaml_space_str error -3\n");
//				goto END;
//			}
//			sprintf(ret_str + strlen(ret_str), "%s- ", tmp_str);
//			if (arr->elems[i]->type == JSON_OBJ)
//			{
//				sprintf(ret_str + strlen(ret_str), "\n");
//			}
//			free(tmp_str);
//			tmp_str = NULL;
//		}
//		switch (arr->elems[i]->type)
//		{
//		case JSON_OBJ:
//			tmp_str = yaml_str_obj(&arr->elems[i]->obj, deep + 1);
//			if (tmp_str == NULL)
//			{
//				fprintf(stderr, "func yaml_str_arr error: yaml_str_obj error -4\n");
//				goto END;
//			}
//			sprintf(ret_str + strlen(ret_str), "%s", tmp_str);
//			free(tmp_str);
//			tmp_str = NULL;
//			break;
//		case JSON_ARR:
//			tmp_str = yaml_str_arr(&arr->elems[i]->arr, deep + 1);
//			if (tmp_str == NULL)
//			{
//				fprintf(stderr, "func yaml_str_arr error: yaml_str_arr error -5\n");
//				goto END;
//			}
//			sprintf(ret_str + strlen(ret_str), "\n%s", tmp_str);
//			free(tmp_str);
//			tmp_str = NULL;
//			break;
//		case JSON_NUM:
//			tmp_str = yaml_str_num(arr->elems[i]->num);
//			if (tmp_str == NULL)
//			{
//				fprintf(stderr, "func yaml_str_arr error: yaml_str_num error -6\n");
//				goto END;
//			}
//			sprintf(ret_str + strlen(ret_str), "%s", tmp_str);
//			free(tmp_str);
//			tmp_str = NULL;
//			break;
//		case JSON_BOOL:
//			tmp_str = yaml_str_bool(arr->elems[i]->bol);
//			if (tmp_str == NULL)
//			{
//				fprintf(stderr, "func yaml_str_arr error: yaml_str_bool error -7\n");
//				goto END;
//			}
//			sprintf(ret_str + strlen(ret_str), "%s", tmp_str);
//			free(tmp_str);
//			tmp_str = NULL;
//			break;
//		case JSON_STR:
//			tmp_str = yaml_str_str(arr->elems[i]->str);
//			if (tmp_str == NULL)
//			{
//				fprintf(stderr, "func yaml_str_arr error: yaml_str_str error -8\n");
//				goto END;
//			}
//			sprintf(ret_str + strlen(ret_str), "%s", tmp_str);
//			free(tmp_str);
//			tmp_str = NULL;
//			break;
//		default:
//			break;
//		}
//		//printf("%s\n", ret_str);
//	}
//	return ret_str;
//END:
//	// 内存处理
//	// TODO
//	return NULL;
//}
//
//char* yaml_str_obj(const object* obj, int deep)
//{
//
//	assert(obj != NULL);
//	//assert(deep <= INT_MAX);
//	U32 len = obj->count;
//	char* ret_str = (char*)calloc(4096, 1);
//	if (ret_str == NULL)
//	{
//		fprintf(stderr, "func yaml_str_obj error: calloc memory error -1\n");
//		goto END;
//	}
//	char* tmp_str = NULL;
//	for (U32 i = 0; i < len; i++)
//	{
//		tmp_str = yaml_space_str(deep);
//		if (tmp_str == NULL)
//		{
//			fprintf(stderr, "func yaml_str_obj error: yaml_space_print error -1\n");
//			goto END;
//		}
//		sprintf(ret_str + strlen(ret_str), "%s%s: ", tmp_str, obj->kvs[i].key);
//		free(tmp_str);
//		tmp_str = NULL;
//		switch (obj->kvs[i].val->type)
//		{
//		case JSON_OBJ:
//			tmp_str = yaml_str_obj(&obj->kvs[i].val->obj, deep + 1);
//			if (tmp_str == NULL)
//			{
//				fprintf(stderr, "func yaml_str_obj error: yaml_str_obj error -2\n");
//				goto END;
//			}
//			sprintf(ret_str + strlen(ret_str), "\n%s", tmp_str);
//			free(tmp_str);
//			tmp_str = NULL;
//			break;
//		case JSON_ARR:
//			tmp_str = yaml_str_arr(&obj->kvs[i].val->arr, deep + 1);
//			if (tmp_str == NULL)
//			{
//				fprintf(stderr, "func yaml_str_obj error: yaml_str_arr error -3\n");
//				goto END;
//			}
//			sprintf(ret_str + strlen(ret_str), "%s", tmp_str);
//			free(tmp_str);
//			tmp_str = NULL;
//			break;
//		case JSON_NUM:
//			tmp_str = yaml_str_num(obj->kvs[i].val->num);
//			if (tmp_str == NULL)
//			{
//				fprintf(stderr, "func yaml_str_obj error: yaml_str_num error -4\n");
//				goto END;
//			}
//			sprintf(ret_str + strlen(ret_str), "%s", tmp_str);
//			free(tmp_str);
//			tmp_str = NULL;
//			break;
//		case JSON_BOOL:
//			tmp_str = yaml_str_bool(obj->kvs[i].val->bol);
//			if (tmp_str == NULL)
//			{
//				fprintf(stderr, "func yaml_str_obj error: yaml_str_bool error -5\n");
//				goto END;
//			}
//			sprintf(ret_str + strlen(ret_str), "%s", tmp_str);
//			free(tmp_str);
//			tmp_str = NULL;
//			break;
//		case JSON_STR:
//			tmp_str = yaml_str_str(obj->kvs[i].val->str);
//			if (tmp_str == NULL)
//			{
//				fprintf(stderr, "func yaml_str_obj error: yaml_str_str error -6\n");
//				goto END;
//			}
//			sprintf(ret_str + strlen(ret_str), "%s", tmp_str);
//			free(tmp_str);
//			tmp_str = NULL;
//			break;
//		default:
//			break;
//		}
//		if (i != len - 1)
//		{
//			sprintf(ret_str + strlen(ret_str), "\n");
//		}
//	}
//	return ret_str;
//
//END:
//	// 处理失败后申请的内存
//	// TODO
//	return NULL;
//}
//char* yaml_str_num(double num)
//{
//	//assert(num <= __DBL_MAX__);
//	char* tmp = (char*)calloc(100, 1);
//	if (tmp == NULL)
//	{
//		fprintf(stderr, "func yaml_str_num error: calloc memory error\n");
//		return NULL;
//	}
//	sprintf(tmp, "%lf", num);
//	return tmp;
//}
//char* yaml_str_bool(enum BOOL bol)
//{
//	assert(bol == TRUE || bol == FALSE);
//	char* tmp = NULL;
//	if (bol == TRUE)
//	{
//		tmp = strdup("true");
//	}
//	else
//	{
//		tmp = strdup("false");
//	}
//	if (tmp == NULL)
//	{
//		fprintf(stderr, "func yaml_str_bool error: strdup error -1\n");
//	}
//	return tmp;
//}
//char* yaml_str_str(const char* str)
//{
//	assert(str != NULL);
//	char* tmp = strdup(str);
//	if (tmp == NULL)
//	{
//		fprintf(stderr, "func yaml_str_str error: strdup error -1\n");
//	}
//	return tmp;
//}
//
//char* yaml_str_cat(char* str1, char* str2)
//{
//	assert(str1 != NULL);
//	assert(str2 != NULL);
//	int len = strlen(str1) + strlen(str2);
//	char* ret = (char*)malloc(len + 1);
//	if (ret == NULL)
//	{
//		fprintf(stderr, "func yaml_str_cat error: malloc memory error -1\n");
//		return NULL;
//	}
//	memset(ret, 0, len + 1);
//	char* tmp_ret = strcat(ret, str1);
//	if (tmp_ret == NULL)
//	{
//		free(ret);
//		fprintf(stderr, "func yaml_str_cat error: malloc memory error -2\n");
//		return NULL;
//	}
//	tmp_ret = strcat(ret, str2);
//	if (tmp_ret == NULL)
//	{
//		free(ret);
//		fprintf(stderr, "func yaml_str_cat error: malloc memory error -3\n");
//		return NULL;
//	}
//	free(str1);
//	free(str2);
//	return ret;
//}
//
//char* yaml_space_str(int deep)
//{
//	//assert(deep <= __INT_MAX__);
//	char* ret_str = (char*)calloc(deep * 2 + 1, 1);
//	if (ret_str == NULL)
//	{
//		fprintf(stderr, "func yaml_space_str error: calloc momory error\n");
//		return NULL;
//	}
//	for (int i = 0; i < deep; i++)
//	{
//		sprintf(ret_str + strlen(ret_str), "  ");
//	}
//	return ret_str;
//}
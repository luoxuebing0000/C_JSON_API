#include "json.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <string.h>

#define _CRT_SECURE_NO_WARNINGS

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
	if(json == NULL)
		return;
	if (json->type == JSON_NONE)
	{
		free(json);
	}
	else
	{
		json_clear(json);
	}
}

/**
 *  清空一个Json变量数据
 *  @param json 要释放的Json变量的地址，清空Json变量存储的数据
 *  @return  创建的Json对象的首地址，使用malloc申请内存，在函数外面进行内存释放
 */
void json_clear(Json* json)
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
	memset(json,0,sizeof(Json));
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
	// 申请动态扩容的keyvalue的内存并初始化
	keyvalue* kvs_tmp = (keyvalue*)realloc(json->obj.kvs, (json->obj.count + 1) * sizeof(keyvalue));
	if(kvs_tmp == NULL)
	{
		ret = -3;
		printf("func json_obj_add_member() error: realloc memory error : %d\n", ret);
		return ret;
	}
	json->obj.kvs = kvs_tmp;
	// 在keyvalue中最后一个添加key和val
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
	char *str_tmp = strdup(str);
	if(str_tmp == NULL)
	{
		ret = -2;
		printf("func json_new_num error: calloc memory error %d\n", ret);
		goto END;
	}
	json_ret->str = str_tmp;
	return json_ret;
END:
	if(json_ret != NULL)
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
int json_arr_add_elem(Json* json, int idx, Json* val)
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
	Json** json_tmp = (Json**)realloc(json->arr.elems, (json->arr.count + 1) * sizeof(int));
	if(json_tmp == NULL)
	{
		ret = -2;
		printf("func json_arr_add_elem() error: malloc memory error %d\n", ret);
		return ret;
	}
	json->arr.elems = json_tmp;

	for (int i = json->arr.count; i >= idx; i--)
	{
		json->arr.elems[i] = json->arr.elems[i-1];
	}
	json->arr.elems[idx] = val;
	json->arr.count++;
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
	json_print_val_deep(json, deep,NO_COMMA);
	
}

/**
 * 递归打印json数据
 *  @param
 *      -json 通用类型的json对象
 *      -deep 当前递归深度，用于打印table键
 *      -flag 标志，用来决定json最后一个value后面有没有逗号
 *  @return  无
 */
void json_print_val_deep(Json* json, int deep, enum json_split flag)
{
	assert(json != NULL);
	assert(deep >= 0);
	switch (json->type)
	{
	case JSON_NUM:
		json_print_num(json->num);
		format_control(flag);
		break;
	case JSON_BOOL:
		json_print_bool(json->bol);
		format_control(flag);
		break;
	case JSON_STR:
		json_print_str(json->str);
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
void json_free_array(array* arr)
{
	assert(arr != NULL);
	for (int i = 0; i < arr->count; i++)
	{
		json_destroy(arr->elems[i]);
		arr->elems[i] = NULL;
	}
}

/**
 * 释放str存储的数据（回收内存）
 *  @param
 *      -str 字符串的地址，使用二级地址，避免野指针
 *  @return  无
 */
void json_free_str(char** str)
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
void json_free_object(object* obj)
{
	assert(obj != NULL);
	for (int i = 0; i < obj->count; i++)
	{
		json_free_str(&obj->kvs[i].key);
		json_destroy(obj->kvs[i].val);
		obj->kvs[i].val = NULL;
	}
}

/**
 * 打印传入的数
 *  @param
 *      -num 要输出的数
 *  @return  无
 */
void json_print_num(double num)
{
	printf("%lf",num);
}

/**
 * 打印传入的bool类型的字符串，
 *  @param
 *      -b 打印的数根据b判断为true 或 false
 *  @return  无
 */
void json_print_bool(enum BOOL b)
{
	if(b == TRUE)
	{
		printf("true");
	}
	else
	{
		printf("false");
	}
}

/**
 * 打印传入的字符串，
 *  @param
 *      -b 打印的数为传入的字符串
 *  @return  无
 */
void json_print_str(const char* str)
{
	assert(str != NULL);
	printf("\"%s\"",str);
}

/**
 * 打印传入的自定义的数组对象存储的数据，
 *  @param
 *      -arr 自定义的array类型变量
 *  @return  无
 */
void json_print_arr(const array* arr, int* deep)
{
	assert(arr != NULL);
	assert(deep != NULL);
	printf("[\n");
	(*deep)++;
	for (int i = 0; i < arr->count; i++)
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
void json_print_obj(const object* obj, int* deep)
{
	assert(obj != NULL);
	assert(deep != NULL);
	printf("{\n");
	(*deep)++;

	for (int i = 0; i < obj->count; i++)
	{
		format_print_tbl(*deep);
		printf("\"%s\": ", obj->kvs[i].key);
		if(i == obj->count-1)
			json_print_val_deep(obj->kvs[i].val, *deep,NO_COMMA);
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
void format_print_tbl(int deep)
{
	assert(deep >= 0);
	for (int i = 0; i < deep; i++)
	{
		printf("\t");
	}
}

void format_control(enum json_split flag)
{
	if(flag == WITH_COMMA)
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
int json_set_val(Json *json,const char *key,Json *val)
{
	assert(json != NULL);
	assert(key != NULL);
	assert(val != NULL);
	int ret = 0;
	// 判断json存储的类型
	if(json->type != JSON_ARR && json->type != JSON_OBJ)
	{
		ret = -1;
		printf("func json_set_val error: json中无key值%d\n",ret);
		return ret;
	}
	
	if(json->type == JSON_ARR) // 若json的类型为数组类型
	{
		json_type_arr_set_val(json, key, val);
	}
	else // json的类型为OBJ类型
	{
		json_type_obj_set_val(json, key, val);
	}
	return ret;
}


int json_type_arr_set_val(Json* json, const char* key, Json* val)
{
	assert(json != NULL);
	assert(key != NULL);
	assert(val != NULL);
	assert(json->type == JSON_ARR);
	int ret = 0;

	for (int i = 0; i < json->arr.count; i++)
	{
		if (json->arr.elems[i]->type == JSON_NONE ||
			json->arr.elems[i]->type == JSON_NUM  ||
			json->arr.elems[i]->type == JSON_STR  ||
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
int json_type_obj_set_val(Json* json, const char* key, Json* val)
{
	assert(json != NULL);
	assert(key != NULL);
	assert(val != NULL);
	assert(json->type == JSON_OBJ);
	int ret = 0;
	for (int i = 0; i < json->obj.count; i++)
	{
		if (strcmp(json->obj.kvs[i].key, key) == 0)
		{
			json->obj.kvs[i].val = val;
		}
		else if (json->obj.kvs[i].val->type == JSON_NONE ||
				 json->obj.kvs[i].val->type == JSON_NUM  ||
				 json->obj.kvs[i].val->type == JSON_STR  ||
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

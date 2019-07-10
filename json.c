#include "json.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <string.h>

#define _CRT_SECURE_NO_WARNINGS

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
	tmp->type = JSON_NONE;
	return tmp;
}

/**
 *  销毁一个Json变量
 *  @param json 要销毁的Json变量的地址，在此处进行内存释放
 *  @return 返回空值
 */
void json_destroy(Json** json)
{
	assert(json != NULL);
	json_clear(*json);
	free(*json);
	*json = NULL; // 避免野指针
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
	//assert(json != val);

	int ret = 0;

	// 添加自己的时候
	if(json == val)
	{
		ret = -1;
		printf("不能添加自己 %d\n", ret);
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
	json->obj.kvs = (keyvalue*)realloc(json->obj.kvs, (json->obj.count + 1) * sizeof(keyvalue));
	if (json->obj.kvs == NULL)
	{
		ret = -3;
		printf("func json_obj_add_member() error: realloc memory error : %d\n", ret);
		return ret;
	}

	// 在keyvalue中最后一个添加key和val
	json->obj.kvs[json->obj.count].key = strdup(key);
	json->obj.kvs[json->obj.count].val = val;
	json->obj.count++;
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
		goto END;
	}
	json_ret->type = JSON_NUM;
	json_ret->num = num;
	return json_ret;
END:
	return NULL;
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
		goto END;
	}
	json_ret->type = JSON_BOOL;
	json_ret->bol = b;
	return json_ret;
END:
	return NULL;
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
	json_ret->type = JSON_STR;
	json_ret->str = strdup(str);
	return json_ret;
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
	int ret = 0;
	if (json->type != JSON_ARR && json->type != JSON_NONE)
	{
		ret = -1;
		printf("func json_arr_add_elem() failed: json类型是非ARR类型 %d\n", ret);
		goto END;
	}
	if (json->type == JSON_NONE)
	{
		json->type = JSON_ARR;
	}

	// idx范围判断
	if (json->arr.count < idx)
	{
		ret = -2;
		printf("func json_arr_add_elem() failed: idx超过范围 %d\n", ret);

	}

	// 扩容
	json->arr.elems = (Json**)realloc(json->arr.elems, (json->arr.count + 1) * sizeof(int));
	if (json->arr.elems == NULL)
	{
		ret = -3;
		printf("func json_arr_add_elem() error: malloc memory error %d\n", ret);
		goto END;
	}

	for (int i = json->arr.count; i >= idx; i--)
	{
		json->arr.elems[i] = json->arr.elems[i-1];
	}
	json->arr.elems[idx] = val;
	json->arr.count++;
	return ret;
END:
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
	json_print_val_deep(json, deep,WITH_COMMA);
	
}

void json_print_val_deep(Json* json, int deep, enum json_split flag)
{
	assert(json != NULL);
	assert(deep >= 0);
	switch (json->type)
	{
	case JSON_NUM:
		json_print_num(json->num, flag);
		break;
	case JSON_BOOL:
		json_print_bool(json->bol, flag);
		break;
	case JSON_STR:
		json_print_str(json->str, flag);
		break;
	case JSON_ARR:
		json_print_arr(&json->arr, &deep, flag);
		break;
	case JSON_OBJ:
		json_print_obj(&json->obj, &deep, flag);
		break;
	default:
		break;
	}
	return;
}


void json_free_array(array* arr)
{
	assert(arr != NULL);
	for (int i = 0; i < arr->count; i++)
	{
		json_destroy(&arr->elems[i]);
	}
}

void json_free_str(char** str)
{
	assert(str != NULL);
	char* tmp = *str;
	if (tmp == NULL)
		return;
	printf("free str: %s\n",*str);
	free(*str);
	*str = NULL;
}

void json_free_object(object* obj)
{
	assert(obj != NULL);
	for (int i = 0; i < obj->count; i++)
	{
		json_free_str(&obj->kvs[i].key);
		json_destroy(&obj->kvs[i].val);
	}
}





void json_print_num(double num, enum json_split flag)
{
	if (flag == WITH_COMMA)
		printf("%lf,\n", num);
	else
		printf("%lf\n", num);
}

void json_print_bool(enum BOOL b, enum json_split flag)
{
	if (b == TRUE && flag == WITH_COMMA)
	{
		printf("TRUE,\n");
	}
	else if(b == TRUE && flag == NO_COMMA)
	{
		printf("TRUE\n");
	}
	else if (b == FALSE && flag == WITH_COMMA)
	{
		printf("FALSE,\n");
	}
	else
	{
		printf("FALSE\n");
	}
}

void json_print_str(const char* str, enum json_split flag)
{
	assert(str != NULL);
	if (flag == WITH_COMMA)
		printf("\"%s\",\n", str);
	else
		printf("\"%s\"\n", str);
}

void json_print_arr(const array* arr, int* deep, enum json_split flag)
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
	if (flag == WITH_COMMA)
		printf("],\n");
	else
		printf("]\n");
}

void json_print_obj(const object* obj, int* deep, enum json_split flag)
{
	assert(obj != NULL);
	assert(deep != NULL);
	printf("{\n");
	if (*deep == 0)
		flag = NO_COMMA;
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
	if (flag == WITH_COMMA)
		printf("},\n");
	else
		printf("}\n");

}

void format_print_tbl(int deep)
{
	assert(deep >= 0);
	for (int i = 0; i < deep; i++)
	{
		printf("\t");
	}
}
#ifndef __JSON_H__
#define __JSON_H__

#include "datatype_def.h"

/**
 *  ����һ��Json����
 *  @return  ������Json�������׵�ַ��ʹ��malloc�����ڴ棬�ں�����������ڴ��ͷ�
 */
Json* json_create();

/**
 *  ����һ��Json����
 *  @param json Ҫ���ٵ�Json�����ĵ�ַ���ڴ˴������ڴ��ͷ�
 *  @return ���ؿ�ֵ
 */
void json_destroy(Json** json);

/**
 *  ���һ��Json��������
 *  @param json Ҫ�ͷŵ�Json�����ĵ�ַ�����Json�����洢������
 *  @return  ������Json������׵�ַ��ʹ��malloc�����ڴ棬�ں�����������ڴ��ͷ�
 */
void json_clear(Json* json);

/**
 *  ��Json�������һ��json��ʽ��ֵ��
 *  @param 
 *      -json Ҫ������ݵ�json����
 *      -key  Ҫ��ӵļ���
 *      -val  Ҫ��ӵ�ֵ
 *  @return  ������Json������׵�ַ��ʹ��malloc�����ڴ棬�ں�����������ڴ��ͷ�
 */
int json_obj_add_member(Json* json, const char* key, Json* val);

/**
 *  �½�һ��int�͵�json����
 *  @param interger  ����ֵ
 *  @return  ����������json����
 */
Json* json_new_num(double num);

/**
 *  �½�һ��bool�͵�json����
 *  @param b  ����ֵ true or false
 *  @return  �����Ĳ�����json����
 */
Json* json_new_bool(enum BOOL b);

/**
 *  �½�һ���ַ����͵�json����
 *  @param str  �ַ���
 *  @return  �������ַ�����json����
 */
Json* json_new_str(const char* str);

/**
 *  �½�һ�������͵�json����
 *  @param arr  �Զ�������
 *  @return  ������������json����
 */
//Json* json_new_array(const array* arr);

/**
 *  �½�һ���������͵�json����
 *  @param obj  ����
 *  @return  �����Ķ������͵�json����
 */
//Json* json_new_object(const object* obj);


Json* json_new_array();
Json* json_new_object();

/**
 *  ����������json��������ָ��λ�������һ��ֵ
 *  @param 
 *      -json �������͵�json����
 *      -idx  ���������
 *      -val  Ҫ��ӵ�ֵ
 *  @return  0 �ɹ� -1 ʧ��
 */
int json_arr_add_elem(Json* json, int idx, Json* val);

/**
 *  ͨ��key��ȡָ����json������ֵ
 *  @param
 *      -json ͨ�����͵�json����
 *      -key  �����keyֵ
 *  @return  ��ȡ��ָ����json�������������򷵻�NULL
 */
Json* json_get_val(Json* json, const char* key);

/**
 *  ��ӡָ����json�����е�����
 *  @param
 *      -json ͨ�����͵�json����
 *  @return  ��
 */
void json_print_val(Json* json);

void json_print_val_deep(Json* json,int deep, enum json_split flag);

void json_free_array(array *arr);

void json_free_str(char** str);

void json_free_object(object* obj);


void json_print_num(double num, enum json_split flag);

void json_print_bool(enum BOOL b, enum json_split flag);

void json_print_str(const char* str, enum json_split flag);

void json_print_arr(const array* arr, int* deep, enum json_split flag);

void json_print_obj(const object* obj, int* deep, enum json_split flag);

void format_print_tbl(int deep);

#endif
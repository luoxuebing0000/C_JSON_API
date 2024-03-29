#ifndef __JSON_H__
#define __JSON_H__

typedef unsigned int U32;
enum BOOL { TRUE, FALSE };
enum json_flag { JSON_NONE, JSON_BOOL, JSON_STR, JSON_ARR, JSON_OBJ, JSON_NUM };
typedef struct _tag_json Json;

/**
 *  创建一个Json变量
 *  @return  创建的Json变量的首地址，使用malloc申请内存，在函数外面进行内存释放
 */
Json* json_create();

enum json_flag json_type(Json* json);

/**
 *  销毁一个Json变量
 *  @param json 要销毁的Json变量的地址，在此处进行内存释放
 *  @return 返回空值
 */
void json_destroy(Json* json);



/**
 *  向Json变量添加一个json格式键值对
 *  @param 
 *      -json 要添加数据的json变量
 *      -key  要添加的键名
 *      -val  要添加的值
 *  @return  创建的Json对象的首地址，使用malloc申请内存，在函数外面进行内存释放
 */
int json_obj_add_member(Json* json, const char* key, Json* val);

/**
 *  新建一个int型的json变量
 *  @param interger  整形值
 *  @return  创建的整形json对象
 */
Json* json_new_num(double num);

/**
 *  新建一个bool型的json变量
 *  @param b  布尔值 true or false
 *  @return  创建的布尔型json对象
 */
Json* json_new_bool(enum BOOL b);

/**
 *  新建一个字符串型的json变量
 *  @param str  字符串
 *  @return  创建的字符串型json对象
 */
Json* json_new_str(const char* str);


 /**
  *  新建一个数组型的json变量
  *  @param arr  自定义数组
  *  @return  创建的数组型json对象
  */
Json* json_new_array();

 /**
  *  新建一个对象类型的json变量
  *  @param obj  对象
  *  @return  创建的对象类型的json对象
  */
Json* json_new_object();

/**
 *  向数组类型json对象中在指定位置中添加一个值
 *  @param 
 *      -json 数组类型的json对象
 *      -idx  数组的索引
 *      -val  要添加的值
 *  @return  0 成功 -1 失败
 */
int json_arr_add_elem(Json* json, U32 idx, Json* val);

/**
 *  通过key获取指定的json变量的值
 *  @param
 *      -json 通用类型的json对象
 *      -key  传入的key值
 *  @return  获取的指定的json对象，若不存在则返回NULL
 */
Json* json_get_val(Json* json, const char* key);

/**
 *  打印指定的json对象中的数据
 *  @param
 *      -json 通用类型的json对象
 *  @return  无
 */
void json_print_val(Json* json);

/**
 * 修改json对象存储的key对应的value值
 *  @param
 *      -json 要修改的json对象
 *      -key  要查找的key值
 *      -val 要替换的value值
 *  @return  无
 */
int json_set_val(Json *json,const char *key,Json *val);

/**
 * 讲json格式数据转换成yaml格式并打印到控制台
 *  @param
 *      -json 要求：必须是json格式对象才可以(JSON_OBJ)，否则报错
 *  @return  无
 */
void json_to_yaml_print(const Json* json);


/**
 * 讲json格式数据转换成yaml格式并使用字符串传出
 *  @param
 *      -json 要求：必须是json格式对象才可以(JSON_OBJ)，否则报错
 *  @return  传出一个字符串，内存由里面管理，使用后须在外面释放内存
 *  @其他说明：
 *       函数功能已经实现，但是内存申请失败后申请的内存没有释放，存在内存泄露，所以注释了
 */
//char *json_to_yaml_str(const Json* json);

#endif
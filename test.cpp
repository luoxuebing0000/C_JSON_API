#include<gtest/gtest.h>
#include "json.h"

TEST(json_create_test,return_test)
{
  Json* json = json_create();
  EXPECT_EQ(json->type,JSON_NONE);
}

TEST(json_destroy_test,json_destroy_func_test)
{
  Json *root = json_create();
  json_destroy(&root);
  ASSERT_TRUE(root == NULL);

  root = json_create();
	Json* basic_obj = json_new_object();
	Json* enable_val = json_new_bool(TRUE);
	Json* ip_val = json_new_str("200.200.3.61");
	Json* port_val = json_new_num(389);
	Json* timeout_val = json_new_num(10);
	Json* basedn_val = json_new_str("aaa");
	Json* fd_val = json_new_num(-1);
	Json* maxcnt_val = json_new_str("10000000000000000000000000000000000033333333333");

	// "dns": ["200.200.0.1", "200.0.0.254"]
	Json* dns_arr_index_0 = json_new_str("200.200.0.1");
	Json* dns_arr_index_1 = json_new_str("200.0.0.254");
	Json* dns_arr_0 = json_new_array();
	json_arr_add_elem(dns_arr_0, 0, dns_arr_index_0);
	json_arr_add_elem(dns_arr_0, 1, dns_arr_index_1);

	// 向basic_obj中添加键值
	json_obj_add_member(basic_obj, "enable", enable_val);
	json_obj_add_member(basic_obj, "ip", ip_val);
	json_obj_add_member(basic_obj, "port", port_val);
	json_obj_add_member(basic_obj, "timeout", timeout_val);
	json_obj_add_member(basic_obj, "basedn", basedn_val);
	json_obj_add_member(basic_obj, "fd", fd_val);
	json_obj_add_member(basic_obj, "maxcnt", maxcnt_val);
	json_obj_add_member(basic_obj, "dns", dns_arr_0);

	// json对象的添加
	json_obj_add_member(root, "basic", basic_obj);

  json_destroy(&root);
  ASSERT_TRUE(root == NULL);
}


TEST(json_clear_test,param_test)
{
  Json *root = json_create();
  json_clear(root);
  ASSERT_TRUE(root->type == JSON_NONE);

  root = json_create();
	Json* basic_obj = json_new_object();
	Json* enable_val = json_new_bool(TRUE);
	Json* ip_val = json_new_str("200.200.3.61");
	Json* port_val = json_new_num(389);
	Json* timeout_val = json_new_num(10);
	Json* basedn_val = json_new_str("aaa");
	Json* fd_val = json_new_num(-1);
	Json* maxcnt_val = json_new_str("10000000000000000000000000000000000033333333333");

	// "dns": ["200.200.0.1", "200.0.0.254"]
	Json* dns_arr_index_0 = json_new_str("200.200.0.1");
	Json* dns_arr_index_1 = json_new_str("200.0.0.254");
	Json* dns_arr_0 = json_new_array();
	json_arr_add_elem(dns_arr_0, 0, dns_arr_index_0);
	json_arr_add_elem(dns_arr_0, 1, dns_arr_index_1);

	// 向basic_obj中添加键值
	json_obj_add_member(basic_obj, "enable", enable_val);
	json_obj_add_member(basic_obj, "ip", ip_val);
	json_obj_add_member(basic_obj, "port", port_val);
	json_obj_add_member(basic_obj, "timeout", timeout_val);
	json_obj_add_member(basic_obj, "basedn", basedn_val);
	json_obj_add_member(basic_obj, "fd", fd_val);
	json_obj_add_member(basic_obj, "maxcnt", maxcnt_val);
	json_obj_add_member(basic_obj, "dns", dns_arr_0);

	// json对象的添加
	json_obj_add_member(root, "basic", basic_obj);

  json_clear(root);
  ASSERT_TRUE(root->type == JSON_NONE);
}

TEST(json_obj_add_member_test,param_ret_test)
{
  Json *root = json_create();
  // add str json test
  Json* json_str_name = json_new_str("huanan");
  int ret = json_obj_add_member(root,"name",json_str_name);
  ASSERT_TRUE(ret == 0);
  ASSERT_TRUE(root->type == JSON_OBJ);
  ASSERT_TRUE(root->obj.kvs[0].val->type == JSON_STR);
  ASSERT_TRUE(strcmp(root->obj.kvs[0].val->str,"huanan") == 0);
  ASSERT_TRUE(strcmp(root->obj.kvs[0].key,"name") == 0);
  ASSERT_TRUE(root->obj.count == 1);
  
  // add num json test
  Json* json_num_val = json_new_num(32.8);
  ret = json_obj_add_member(root,"db",json_num_val);
  ASSERT_TRUE(ret == 0);
  ASSERT_TRUE(root->obj.kvs[1].val->type == JSON_NUM);
  ASSERT_TRUE(root->obj.kvs[1].val->num == 32.8);
  ASSERT_TRUE(strcmp(root->obj.kvs[1].key,"db") == 0);
  ASSERT_TRUE(root->obj.count == 2);

  // add bool json test
  Json* json_bool_val = json_new_bool(TRUE);
  ret = json_obj_add_member(root,"enable",json_bool_val);
  ASSERT_TRUE(ret == 0);
  ASSERT_TRUE(root->obj.kvs[2].val->type == JSON_BOOL);
  ASSERT_TRUE(root->obj.kvs[2].val->bol == TRUE);
  ASSERT_TRUE(strcmp(root->obj.kvs[2].key,"enable") == 0);
  ASSERT_TRUE(root->obj.count == 3);

  // add array json test
  Json* dns_arr_index_0 = json_new_str("200.200.0.1");
	Json* dns_arr_index_1 = json_new_str("200.0.0.254");
	Json* dns_arr_0 = json_new_array();
	json_arr_add_elem(dns_arr_0, 0, dns_arr_index_0);
	json_arr_add_elem(dns_arr_0, 1, dns_arr_index_1);
  ret = json_obj_add_member(root,"dns",dns_arr_0);
  ASSERT_TRUE(ret == 0);
  ASSERT_TRUE(root->obj.kvs[3].val->type == JSON_ARR);
  ASSERT_TRUE(root->obj.kvs[3].val->arr.count == 2);
  ASSERT_TRUE(root->obj.kvs[3].val->arr.elems[0]->type == JSON_STR);
  ASSERT_TRUE(root->obj.kvs[3].val->arr.elems[1]->type == JSON_STR);
  ASSERT_TRUE(strcmp(root->obj.kvs[3].val->arr.elems[0]->str,"200.200.0.1") == 0);
  ASSERT_TRUE(strcmp(root->obj.kvs[3].val->arr.elems[1]->str,"200.0.0.254") == 0);
  ASSERT_TRUE(strcmp(root->obj.kvs[3].key,"dns") == 0);
  ASSERT_TRUE(root->obj.count == 4);

  // add object json test
  Json *json_obj_val = json_new_object();
  ret = json_obj_add_member(root,"obj",json_obj_val);
  ASSERT_TRUE(ret == 0);
  ASSERT_TRUE(root->obj.kvs[4].val->type == JSON_OBJ);
  ASSERT_TRUE(strcmp(root->obj.kvs[4].key,"obj") == 0);
  ASSERT_TRUE(root->obj.count == 5);
}


int main(int argc,char **argv){
  testing::InitGoogleTest(&argc,argv);
  return RUN_ALL_TESTS();
}

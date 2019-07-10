#include "json.h"

void Test()
{
	Json* root = json_create();
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

	Json* advance_obj = json_new_object();

	/*"dns": [
			{"name":"huanan", "ip": "200.200.0.1"},
			{"name":"huabei", "ip": "200.0.0.254"}],*/
	Json* dns_arr_1 = json_new_array();
	// {"name":"huanan", "ip": "200.200.0.1"}
	Json* dns_obj_index_0 = json_new_object();
	Json* dns_obj_name_index_0_val = json_new_str("huanan");
	Json* dns_obj_ip_index_0_val = json_new_str("200.200.0.1");
	json_obj_add_member(dns_obj_index_0, "name", dns_obj_name_index_0_val);
	json_obj_add_member(dns_obj_index_0, "ip", dns_obj_ip_index_0_val);

	// {"name":"huabei", "ip": "200.0.0.254"}
	Json* dns_obj_index_1 = json_new_object();
	Json* dns_obj_name_index_1_val = json_new_str("huabei");
	Json* dns_obj_ip_index_1_val = json_new_str("200.0.0.254");
	json_obj_add_member(dns_obj_index_1, "name", dns_obj_name_index_1_val);
	json_obj_add_member(dns_obj_index_1, "ip", dns_obj_ip_index_1_val);

	json_arr_add_elem(dns_arr_1, 0, dns_obj_index_0);
	json_arr_add_elem(dns_arr_1, 1, dns_obj_index_1);

	// "portpool": [130,131,132]
	Json* portpool_arr = json_new_array();
	Json* portpool_arr_index_0 = json_new_num(130);
	Json* portpool_arr_index_1 = json_new_num(131);
	Json* portpool_arr_index_2 = json_new_num(132);
	json_arr_add_elem(portpool_arr, 0, portpool_arr_index_0);
	json_arr_add_elem(portpool_arr, 1, portpool_arr_index_1);
	json_arr_add_elem(portpool_arr, 2, portpool_arr_index_2);

	// "url": "http://200.200.0.4/main"
	Json* url_val = json_new_str("http://200.200.0.4/main");
	// "path": "/etc/sinfors"
	Json* path_val = json_new_str("/etc/sinfors");

	// "value": 3.14
	Json* value_val = json_new_num((int)3.14);

	// 实现advance_obj的插入
	json_obj_add_member(advance_obj, "dns", dns_arr_1);
	json_obj_add_member(advance_obj, "portpool", portpool_arr);
	json_obj_add_member(advance_obj, "url", url_val);
	json_obj_add_member(advance_obj, "path", path_val);
	json_obj_add_member(advance_obj, "value", value_val);

	// json对象的添加
	json_obj_add_member(root, "basic", basic_obj);
	json_obj_add_member(basic_obj,"json_test",advance_obj);
	json_obj_add_member(root, "advance", advance_obj);

	//json_obj_add_member(root, "root", root);  // bug，不处理会导致死循环

	// 打印
	 json_print_val(root);

	// json_print_val(portpool_arr);

	// 释放内存
	json_destroy(root);
	// json_clear(root);

}

int main(void)
{
	Test();

	return 0;
}
#include "dc.h"

struct dict_object * ta_vendor = NULL;
struct dict_object * ta_appli = NULL;
struct dict_object * ta_cmd_r = NULL;
struct dict_object * ta_cmd_a = NULL;

//real
struct dict_object * ta_avp_optype = NULL; //start, stop, update
struct dict_object * ta_avp_userid = NULL; //
struct dict_object * ta_avp_tenantid = NULL;
struct dict_object * ta_avp_requestQuota = NULL;
struct dict_object * ta_avp_usedQuota = NULL;
struct dict_object * ta_avp_grantedQuota = NULL;

//system
struct dict_object * ta_sess_id = NULL;
struct dict_object * ta_origin_host = NULL;
struct dict_object * ta_origin_realm = NULL;
struct dict_object * ta_dest_host = NULL;
struct dict_object * ta_dest_realm = NULL;
struct dict_object * ta_user_name = NULL;
struct dict_object * ta_res_code = NULL;

int ta_dict_init(void)
{
    TRACE_DEBUG(FULL, "Initializing the dictionary for test");
    
    /* Create the Test Vendor */
    {
        struct dict_vendor_data data;
        data.vendor_id = ta_conf->vendor_id;
        data.vendor_name = "app_test vendor";
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_VENDOR, &data, NULL, &ta_vendor));
    }
    
    /* Create the Test Application */
    {
        struct dict_application_data data;
        data.application_id = ta_conf->appli_id;
        data.application_name = "app_test application";
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_APPLICATION, &data, ta_vendor, &ta_appli));
    }
    
    /* Create the Test-Request & Test-Answer commands */
    {
        struct dict_cmd_data data;
        data.cmd_code = ta_conf->cmd_id;
        data.cmd_name = "Test-Request";
        data.cmd_flag_mask = CMD_FLAG_PROXIABLE | CMD_FLAG_REQUEST;
        data.cmd_flag_val  = CMD_FLAG_PROXIABLE | CMD_FLAG_REQUEST;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_COMMAND, &data, ta_appli, &ta_cmd_r));
        data.cmd_name = "Test-Answer";
        data.cmd_flag_val  = CMD_FLAG_PROXIABLE;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_COMMAND, &data, ta_appli, &ta_cmd_a));
    }
    
    //create ta_optype //in all
    {
        struct dict_avp_data data;
        data.avp_code = 400000;
        data.avp_vendor = ta_conf->vendor_id;
        data.avp_name = "OpType-AVP";
        data.avp_flag_mask = AVP_FLAG_VENDOR;
        data.avp_flag_val = AVP_FLAG_VENDOR;
        data.avp_basetype = AVP_TYPE_INTEGER32;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_AVP, &data, NULL, &ta_avp_optype));
    }
    //create user_id //in req, optype=start
    {
        struct dict_avp_data data;
        data.avp_code = 400001;
        data.avp_vendor = ta_conf->vendor_id;
        data.avp_name = "UserId-AVP";
        data.avp_flag_mask = AVP_FLAG_VENDOR;
        data.avp_flag_val = AVP_FLAG_VENDOR;
        data.avp_basetype = AVP_TYPE_OCTETSTRING;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_AVP, &data, NULL, &ta_avp_userid));
    }
    //create requestQuota //in req, optype!=stop
    {
        struct dict_avp_data data;
        data.avp_code = 400002;
        data.avp_vendor = ta_conf->vendor_id;
        data.avp_name = "requestQuota-AVP";
        data.avp_flag_mask = AVP_FLAG_VENDOR;
        data.avp_flag_val = AVP_FLAG_VENDOR;
        data.avp_basetype = AVP_TYPE_UNSIGNED64;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_AVP, &data, NULL, &ta_avp_requestQuota));
    }
    //create usedQuota //in req, optype!=start
    {
        struct dict_avp_data data;
        data.avp_code = 400003;
        data.avp_vendor = ta_conf->vendor_id;
        data.avp_name = "usedQuota-AVP";
        data.avp_flag_mask = AVP_FLAG_VENDOR;
        data.avp_flag_val = AVP_FLAG_VENDOR;
        data.avp_basetype = AVP_TYPE_UNSIGNED64;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_AVP, &data, NULL, &ta_avp_usedQuota));
    }
    //create grantedQuota //in rsp, optype!=stop
    {
        struct dict_avp_data data;
        data.avp_code = 400004;
        data.avp_vendor = ta_conf->vendor_id;
        data.avp_name = "grantedQuota-AVP";
        data.avp_flag_mask = AVP_FLAG_VENDOR;
        data.avp_flag_val = AVP_FLAG_VENDOR;
        data.avp_basetype = AVP_TYPE_UNSIGNED64;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_AVP, &data, NULL, &ta_avp_grantedQuota));
    }
    //create tenant_id //in req, optype=start
    {
        struct dict_avp_data data;
        data.avp_code = 400005;
        data.avp_vendor = ta_conf->vendor_id;
        data.avp_name = "TenantId-AVP";
        data.avp_flag_mask = AVP_FLAG_VENDOR;
        data.avp_flag_val = AVP_FLAG_VENDOR;
        data.avp_basetype = AVP_TYPE_OCTETSTRING;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_AVP, &data, NULL, &ta_avp_tenantid));
    }
    /* Now resolve some other useful AVPs */
    CHECK_FCT( fd_dict_search( fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Session-Id", &ta_sess_id, ENOENT) );
    CHECK_FCT( fd_dict_search( fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Origin-Host", &ta_origin_host, ENOENT) );
    CHECK_FCT( fd_dict_search( fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Origin-Realm", &ta_origin_realm, ENOENT) );
    CHECK_FCT( fd_dict_search( fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Destination-Host", &ta_dest_host, ENOENT) );
    CHECK_FCT( fd_dict_search( fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Destination-Realm", &ta_dest_realm, ENOENT) );
    CHECK_FCT( fd_dict_search( fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "User-Name", &ta_user_name, ENOENT) );
    CHECK_FCT( fd_dict_search( fd_g_config->cnf_dict, DICT_AVP, AVP_BY_NAME, "Result-Code", &ta_res_code, ENOENT) );
    
    /* Create the rules for Test-Request and Test-Answer */
    {
        struct dict_rule_data data;
        data.rule_min = 1;
        data.rule_max = 1;
        
        /* Session-Id is in first position */
        data.rule_avp = ta_sess_id;
        data.rule_position = RULE_FIXED_HEAD;
        data.rule_order = 1;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_r, NULL));
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_a, NULL));
        
        data.rule_position = RULE_REQUIRED;
        data.rule_avp = ta_avp_optype;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_r, NULL));
        //data.rule_avp = ta_avp_userid;
        //CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_r, NULL));
        //data.rule_avp = ta_avp_requestQuota;
        //CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_r, NULL));
        //data.rule_avp = ta_avp_usedQuota;
        //CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_r, NULL));
        //data.rule_avp = ta_avp_grantedQuota;
        //CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_a, NULL));
        
        /* idem for Origin Host and Realm */
        data.rule_avp = ta_origin_host;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_r, NULL));
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_a, NULL));
        
        data.rule_avp = ta_origin_realm;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_r, NULL));
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_a, NULL));
        
        /* And Result-Code is mandatory for answers only */
        data.rule_avp = ta_res_code;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_a, NULL));
        
        /* And Destination-Realm for requests only */
        data.rule_avp = ta_dest_realm;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_r, NULL));
        
        /* And Destination-Host optional for requests only */
        data.rule_position = RULE_OPTIONAL;
        data.rule_min = 0;
        data.rule_avp = ta_dest_host;
        CHECK_FCT(fd_dict_new( fd_g_config->cnf_dict, DICT_RULE, &data, ta_cmd_r, NULL));
    }
    
    return 0;
}

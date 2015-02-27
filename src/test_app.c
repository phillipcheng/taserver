/* 
 * Test application for freeDiameter.
 */

#include "dc.h"

const int DC_CLIENT=1;
const int DC_SERVER=2;

/* Initialize the configuration */
struct ta_conf * ta_conf = NULL;
static struct ta_conf _conf;
static pthread_t ta_stats_th = (pthread_t)NULL;


int ta_conf_handle(char * conffile){
    //fill in the global ta_conf
    if (conffile!=NULL){
        ta_conf->mode = DC_SERVER;
    }
    return 1;
}

static int ta_conf_init(void)
{
	ta_conf = &_conf;
	memset(ta_conf, 0, sizeof(struct ta_conf));
	
	/* Set the default values */
	ta_conf->vendor_id  = 999999;		/* Dummy value */
	ta_conf->appli_id   = 0xffffff;	/* dummy value */
	ta_conf->cmd_id     = 0xfffffe;	/* Experimental */
    ta_conf->mode = DC_CLIENT;
	ta_conf->dest_realm = strdup(fd_g_config->cnf_diamrlm);
	ta_conf->dest_host  = NULL;
	
	/* Initialize the mutex */
	CHECK_POSIX( pthread_mutex_init(&ta_conf->stats_lock, NULL) );
	
	return 0;
}

static void ta_conf_dump(void)
{
	if (!TRACE_BOOL(INFO))
		return;
	fd_log_debug( "------- app_test configuration dump: ---------");
	fd_log_debug( " Vendor Id .......... : %u", ta_conf->vendor_id);
	fd_log_debug( " Application Id ..... : %u", ta_conf->appli_id);
	fd_log_debug( " Command Id ......... : %u", ta_conf->cmd_id);
	fd_log_debug( " Destination Realm .. : %s", ta_conf->dest_realm ?: "- none -");
	fd_log_debug( " Destination Host ... : %s", ta_conf->dest_host ?: "- none -");
	fd_log_debug( "------- /app_test configuration dump ---------");
}


/* entry point */
int ta_entry(char * conffile)
{
    ta_conf_init();
	
	/* Parse configuration file */
	if (conffile != NULL) {
		ta_conf->mode = DC_SERVER;
	}
	
	TRACE_DEBUG(INFO, "Extension Test_App initialized with configuration: '%s'", conffile);
	ta_conf_dump();
	
	/* Install objects definitions for this test application */
    CHECK_FCT( ta_dict_init() );

    /* Start the signal handler thread */
    if (ta_conf->mode==DC_SERVER){
        CHECK_FCT( ta_serv_init() );
    }else{
        CHECK_FCT( ta_cli_init() );
    }
    
	/* Advertise the support for the test application in the peer */
	CHECK_FCT( fd_disp_app_support ( ta_appli, ta_vendor, 1, 0 ) );
	
	return 0;
}

/* Unload */
void fd_ext_fini(void)
{
    if (ta_conf->mode==DC_SERVER){
        ta_serv_fini();
    }else{
        ta_cli_fini();
    }
    
	CHECK_FCT_DO( fd_thr_term(&ta_stats_th), );
	CHECK_POSIX_DO( pthread_mutex_destroy(&ta_conf->stats_lock), );
}

EXTENSION_ENTRY("test_app", ta_entry);

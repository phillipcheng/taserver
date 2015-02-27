//
//  dc.h
//  trplugin
//
//  Created by Cheng Yi on 1/31/15.
//  Copyright (c) 2015 Cheng Yi. All rights reserved.
//

#ifndef trplugin_dc_h
#define trplugin_dc_h

#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>
#include <freeDiameter/extension.h>

#include <signal.h>
#include <getopt.h>
#include <locale.h>

typedef enum { false, true } bool;
typedef enum {d_start=1, d_update=2, d_stop=3} d_req_type;//diameter request op type


extern const unsigned long DUMMY_INIT_QUOTA;

extern const unsigned int DIAMETER_SUCCESS;
extern const unsigned int DIAMETER_UNKNOWN_SESSION_ID;//for update and stop
extern const unsigned int DIAMETER_AUTHORIZATION_REJECTED; //maps to user not found for start

/* The module configuration */
struct ta_conf {
    uint32_t	vendor_id;	/* default 999999 */
    uint32_t	appli_id;	/* default 123456 */
    uint32_t	cmd_id;		/* default 234567 */
    char 	*	dest_realm;	/* default local realm */
    char 	*	dest_host;	/* default NULL */
    char 	*	user_name;	/* default NULL */
    struct ta_stats {
        unsigned long long	nb_echoed; /* server */
        unsigned long long	nb_sent;   /* client */
        unsigned long long	nb_recv;   /* client */
        unsigned long long	nb_errs;   /* client */
        unsigned long		shortest;  /* fastest answer, in microseconds */
        unsigned long		longest;   /* slowest answer, in microseconds */
        unsigned long		avg;       /* average answer time, in microseconds */
    } stats;
    pthread_mutex_t		stats_lock;
};
extern struct ta_conf * ta_conf;

typedef struct {
    uint64_t leftQuota;
    char* sid;//diameter session id
}UsageServerSession;
UsageServerSession * usageserver_session_alloc();
void usageserver_session_free(UsageServerSession *data);

/* Parse the configuration file */
int ta_conf_handle(char * conffile);

int ta_serv_init(void);
void ta_serv_fini(void);

/* Initialize dictionary definitions */
int ta_dict_init(void);

int dcinit(int argc, char * argv[]);

int ta_entry(char * conffile);

extern const char* DEBUG_NAME;

/* Some global variables for dictionary */
extern struct dict_object * ta_vendor;
extern struct dict_object * ta_appli;
extern struct dict_object * ta_cmd_r;
extern struct dict_object * ta_cmd_a;
//
extern struct dict_object * ta_avp_optype; //start, stop, update
extern struct dict_object * ta_avp_userid; //
extern struct dict_object * ta_avp_tenantid;
extern struct dict_object * ta_avp_requestQuota;
extern struct dict_object * ta_avp_usedQuota;
extern struct dict_object * ta_avp_grantedQuota;
extern struct dict_object * ta_avp;
extern struct dict_object * ta_avp_long;

extern struct dict_object * ta_sess_id;
extern struct dict_object * ta_origin_host;
extern struct dict_object * ta_origin_realm;
extern struct dict_object * ta_dest_host;
extern struct dict_object * ta_dest_realm;
extern struct dict_object * ta_user_name;
extern struct dict_object * ta_res_code;

extern struct session_handler * ta_cli_reg;

#endif


/* Install the dispatch callbacks */

#include "dc.h"

const unsigned long DUMMY_INIT_QUOTA = 3*1024*1024;
static struct disp_hdl * ta_hdl_fb = NULL; /* handler for fallback cb */
static struct disp_hdl * ta_hdl_tr = NULL; /* handler for Test-Request req cb */
struct session_handler * ta_cli_reg = NULL;

UsageServerSession * usageserver_session_alloc(){
    UsageServerSession* s = malloc(sizeof(UsageServerSession));
    s->leftQuota=0;
    return s;
}
void usageserver_session_free(UsageServerSession *data){
    if (data->sid!=NULL){
        free(data->sid);
    }
    free(data);
}

/* Default callback for the application. */
static int ta_fb_cb( struct msg ** msg, struct avp * avp, struct session * sess, void * opaque, enum disp_action * act)
{
	/* This CB should never be called */
	TRACE_ENTRY("%p %p %p %p", msg, avp, sess, act);
	
	fd_log_debug("Unexpected message received!");
	
	return ENOTSUP;
}

/* Callback for incoming Test-Request messages */
static int ta_tr_cb( struct msg ** msg, struct avp * avp, struct session * sess, void * opaque, enum disp_action * act)
{
	struct msg *ans, *req;
    struct avp * src = NULL;
    struct avp_hdr * hdr = NULL;
    UsageServerSession* mi;
	
	TRACE_ENTRY("%p %p %p %p", msg, avp, sess, act);
	
	if (msg == NULL)
		return EINVAL;
	
	/* Value of Origin-Host */
    fprintf(stderr, "Request received from ");
    CHECK_FCT( fd_msg_search_avp ( *msg, ta_origin_host, &src) );
    if (src) {
        CHECK_FCT( fd_msg_avp_hdr( src, &hdr ) );
        fprintf(stderr, "'%.*s'", (int)hdr->avp_value->os.len, hdr->avp_value->os.data);
    } else {
        fprintf(stderr, "no_Origin-Host");
    }
    fprintf(stderr, ", replying...\n");
	
    //get or create the session/session state

    req = *msg;
	/* Create answer header */
	CHECK_FCT( fd_msg_new_answer_from_req ( fd_g_config->cnf_dict, msg, 0 ) );
    ans = *msg;

    d_req_type reqType;

    //get the op type
    CHECK_FCT( fd_msg_search_avp ( req, ta_avp_optype, &src) );
    CHECK_FCT( fd_msg_avp_hdr( src, &hdr )  );
    reqType = hdr->avp_value->i32;
    if (reqType==d_start){
        //create the session state
        mi= usageserver_session_alloc();
        mi->leftQuota = DUMMY_INIT_QUOTA;
    }else{
        //get the session state
        fd_sess_state_retrieve(ta_cli_reg, sess, &mi);
    }

    if (reqType!=d_start){//for update, stop, need to update the leftQuota
        //update the left quota in session state, according to the used quota avp in req
        CHECK_FCT( fd_msg_search_avp ( req, ta_avp_usedQuota, &src) );
        CHECK_FCT( fd_msg_avp_hdr( src, &hdr )  );
        uint64_t usedQuota = hdr->avp_value->u64;
        if (mi->leftQuota>=usedQuota){
            mi->leftQuota-=usedQuota;
        }else{
            fprintf(stderr, "fatal, the used should not be larger then the granted last time.");
            mi->leftQuota=0;
        }
    }
    if (reqType!=d_stop){//for start, update, need to reply with grantedQuota
        //set the granted quota AVP according to requested quota and left quota in session state
        CHECK_FCT( fd_msg_search_avp ( req, ta_avp_requestQuota, &src) );
        CHECK_FCT( fd_msg_avp_hdr( src, &hdr )  );
        uint64_t reqAmt = hdr->avp_value->u64;
        uint64_t grantAmt=0;
        if (mi->leftQuota>=reqAmt){
            grantAmt = reqAmt;
        }else{
            grantAmt = mi->leftQuota;
        }
        hdr->avp_value->u64 = grantAmt;
        CHECK_FCT( fd_msg_avp_new ( ta_avp_grantedQuota, 0, &avp ) );
        CHECK_FCT( fd_msg_avp_setvalue( avp, hdr->avp_value ) );
        CHECK_FCT( fd_msg_avp_add( ans, MSG_BRW_LAST_CHILD, avp ) );
        fprintf(stderr, "add granted quota avp, %llu.\n", grantAmt);
    }
    fprintf(stderr, "session:leftQuota:%llu\n", mi->leftQuota);
    
	/* Set the Origin-Host, Origin-Realm, Result-Code AVPs */
	CHECK_FCT( fd_msg_rescode_set( ans, "DIAMETER_SUCCESS", NULL, NULL, 1 ) );
    
    //save the session state
    fd_sess_state_store(ta_cli_reg, sess, &mi);
    
    /* Send the answer */
	CHECK_FCT( fd_msg_send( msg, NULL, NULL ) );

	return 0;
}

int ta_serv_init(void)
{
    CHECK_FCT( fd_sess_handler_create(&ta_cli_reg, (void *)free, NULL, NULL) );
    
	struct disp_when data;
	
	TRACE_DEBUG(FULL, "Initializing dispatch callbacks for test");
	
	memset(&data, 0, sizeof(data));
	data.app = ta_appli;
	data.command = ta_cmd_r;
	
	/* fallback CB if command != Test-Request received */
	CHECK_FCT( fd_disp_register( ta_fb_cb, DISP_HOW_APPID, &data, NULL, &ta_hdl_fb ) );
	
	/* Now specific handler for Test-Request */
	CHECK_FCT( fd_disp_register( ta_tr_cb, DISP_HOW_CC, &data, NULL, &ta_hdl_tr ) );
	
	return 0;
}

void ta_serv_fini(void)
{
	if (ta_hdl_fb) {
		(void) fd_disp_unregister(&ta_hdl_fb, NULL);
	}
	if (ta_hdl_tr) {
		(void) fd_disp_unregister(&ta_hdl_tr, NULL);
	}
	
	return;
}

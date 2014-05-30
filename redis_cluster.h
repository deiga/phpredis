#ifndef REDIS_CLUSTER_H
#define REDIS_CLUSTER_H

#include "cluster_library.h"
#include <php.h>
#include <stddef.h>

/* Redis cluster hash slots and N-1 which we'll use to find it */
#define REDIS_CLUSTER_SLOTS 16384
#define REDIS_CLUSTER_MOD   (REDIS_CLUSTER_SLOTS-1)

/* Get attached object context */
#define GET_CONTEXT() \
    (redisCluster*)zend_object_store_get_object(getThis() TSRMLS_CC)

/* Command building/processing is identical for every command */
#define CLUSTER_BUILD_CMD(name, c, cmd, cmd_len, slot) \
    redis_##name##_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU, c->flags, &cmd, &cmd_len, &slot)

#define CLUSTER_PROCESS_REQUEST(cmdname, resp_func) \
    redisCluster *c = GET_CONTEXT(); \
    char *cmd; \
    int cmd_len; \
    short slot; \
    if(redis_##cmdname##_cmd(INTERNAL_FUNCTION_PARAM_PASSTHRU,c->flags, &cmd, \
                             &cmd_len, &slot)==FAILURE) \
    { \
        RETURN_FALSE; \
    } \
    if(cluster_send_command(c,slot,cmd,cmd_len TSRMLS_CC)<0 || \
                            c->err_state!=0) \
    { \
        efree(cmd); \
        RETURN_FALSE; \
    } \
    resp_func(INTERNAL_FUNCTION_PARAM_PASSTHRU, c); 
    


/* Send a request to our cluster, and process the response */
/*#define CLUSTER_PROCESS_REQUEST(c, slot, cmd, cmd_len, resp_func) \
    if(cluster_send_command(c,slot,cmd,cmd_len TSRMLS_CC)<0 || \
       c->err_state!=0) || \
    { \
        efree(cmd); \
        RETURN_FALSE; \
    } \
    efree(cmd);
*/

/* For the creation of RedisCluster specific exceptions */
PHPAPI zend_class_entry *rediscluster_get_exception_base(int root TSRMLS_DC);

/* Create cluster context */
zend_object_value create_cluster_context(zend_class_entry *class_type 
                                         TSRMLS_DC);

/* Free cluster context struct */
void free_cluster_context(void *object TSRMLS_DC);

/* Inittialize our class with PHP */
void init_rediscluster(TSRMLS_D);

/* RedisCluster method implementation */
PHP_METHOD(RedisCluster, __construct);
PHP_METHOD(RedisCluster, get);
PHP_METHOD(RedisCluster, set);

#endif
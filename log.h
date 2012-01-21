#ifndef conflux_log_H
#define conflux_log_H
#define TRUE 1
#define FALSE 0
#include <stdio.h>

#undef AUDITFILE
#define AUDITFILE "conflux_audit"

#define LOGDIR "/tmp"

#define CONFLUX_MAX_LOGLEN 1024
#define URDWRMASK 0664

#define CONFLUX_DEBUG_FILE   "EPM_DEBUG_FILE"
#define DEF_CONFLUX_DBG_FILE "/tmp/conflux_debug"
#define PATH_MAX 1024
FILE *dbg_file;
int CONFLUX_DEBUG;

#define AUDIT_FILE_LOG 1
#define DEBUG_FILE_LOG 2

#define AUDIT_LOG         AUDIT_FILE_LOG, __FILE__, __LINE__ 
#define DEBUG_LOG         DEBUG_FILE_LOG, __FILE__, __LINE__

#define conflux_log(confluxlog) _conflux_log(confluxlog, __FILE__, __LINE__)
#define conflux_type_elog(confluxlog) _conflux_type_elog(confluxlog, __FILE__, __LINE__)
#define conflux_debug_log(confluxlog) _conflux_debug_log(confluxlog, __FILE__, __LINE__)

extern char confluxlog[CONFLUX_MAX_LOGLEN];
extern char logDirStr[PATH_MAX];
extern FILE *dbg_file; 

/*Macros*/
#define SPRINT(buff,fs,a) do{snprintf(buff,sizeof(buff),"Value of %s="fs,#a,a);conflux_log(buff);}while(0)

typedef struct conflux_dbg_entry
{
    char *var;         /* debug env */
    int  *val_holder;  /* Value of debug var */
    char *outfile_var; /* env describing out file */
    FILE **outfile_fp;    /* outfile */
} conflux_dbg_entry;

void read_log_dir_env();
void _conflux_log(char *,const char *, int);
void _conflux_debug_log( char *,const char *, int );
void _conflux_file_log(char *fname, char *log,const char *filename, int line);
void _conflux_type_elog(char *log,const char *filename, int line);
void init_debug_file();

void conflux_common_log(unsigned long,const char *,int,const char*,...); 


typedef struct conflux_st_entry
{
    char *st_log_flag;         		/* debug env */
    int  *val_holder;  		/* Value of debug var */
}conflux_st_entry;


#endif


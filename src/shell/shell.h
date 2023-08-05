#pragma once
#include "db_list.h"

#define VT_ESC "\033"
#define VT_CLEAR VT_ESC "[H" VT_ESC "[J"
#define VT_BLINK VT_ESC "[5m"
#define VT_BOLD VT_ESC  "[1m"
#define VT_NORMAL VT_ESC "[m"



#define VT_GREEN VT_ESC "[32m"
#define VT_RED VT_ESC "[31m"
#define VT_BLUE VT_ESC "[34m"
#define VT_COLOR_END VT_ESC "[39m"


#define ADD_CMD(_name,_function, _help_string) \
  cmd_t _var##_function __attribute__((section(".cmd_list"))) = {#_name,_function,_help_string}  

#define CMDDATA __attribute__((section(".cmd_list")))


typedef void (*cmd_func_t)(void);
typedef void (*cmd_func_normal)(int ,char*[]);

typedef struct {
    const char* name;
    cmd_func_t func;
    const char* descr;
    int   pass_param;
} cmd_t;


typedef void (*shell_command_cb)(int ,char*[]);
struct shell_command_s {
    list_node_t node;
    const char*  name;   
    const char*  descr;     
    shell_command_cb cb;
};  


extern cmd_t cmd_table[];

int run_shell();
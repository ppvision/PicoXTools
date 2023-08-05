#include "shell.h"
#include "cc.h"
#include "common/base_def.h"
#include "io.h"
#include "pico/stdio.h"

#if LIB_PICO_STDIO_USB
#include "pico/stdio_usb.h"
#endif
#if LIB_PICO_STDIO_UART
#include "pico/stdio_uart.h"
#endif



#include "pico/version.h"
#include "readln.h"
#include "tar.h"
#include "vi.h"
#include "ymodem.h"
#include <ctype.h>
#include <errno.h>
#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/time.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

// #define COPYRIGHT "\u00a9" // for UTF8
#define COPYRIGHT "(c)" // for ASCII
#define MAX_ARGS 16


__attribute__((section(".cmd_end"))) static volatile cmd_t cmd_end_ = {
    NULL, NULL, NULL};


static uint32_t screen_x = 80, screen_y = 24;
static lfs_file_t file;
static buf_t cmd_buffer, curdir = "/";
buf_t result;


static bool run = true;

int argc;
char* argv[MAX_ARGS + 1];


#define pop_argument(argc , argv,n)  argc-=n; argv+=n

extern bool mounted ;
extern bool check_mount(bool need);
extern char* full_path(const char* name);
// used by Vi
void get_screen_xy(uint32_t* x, uint32_t* y) {
    *x = screen_x>80?80:screen_x;
    *y = screen_y>40?40:screen_y;

}

static void echo_key(char c) {
    tty_putchar(c);
    if (c == '\r')
        tty_putchar('\n');
}


static void vi_cmd(void) {
    if (check_mount(true))
        return;
    // vi(argc, argv);
    vi(argc - 1, argv + 1);
}

static void clear_cmd(void) { strcpy(result, VT_CLEAR "\n"); }

static void reboot_cmd(void) {
    // release any resources we were using
    if (mounted) {
        savehist();
        fs_unmount();
    }
    watchdog_reboot(0, 0, 1);
}

#if LIB_PICO_STDIO_USB
static void usbboot_cmd(void) {
    // release any resources we were using
    if (mounted) {
        savehist();
        fs_unmount();
    }
    reset_usb_boot(0, 0);
}
#endif

static void quit_cmd(void) {
    printf("\nare you sure (Y/n) ? ");
    fflush(stdout);
    char c = tty_getchar();
    tty_putchar('\n');
    if (c != 'y' && c != 'Y' && c != '\r')
        return;
    // release any resources we were using
    if (mounted) {
        savehist();
        fs_unmount();
    }
    printf("\nbye!\n");
    sleep_ms(1000);
    exit(0);
}

static char art_ppvsion_str[]={
    0x20,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x20,0x20,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,
    0x5f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x5f,0x5f,0x5f,0x5f,0x5f,
    0x5f,0x5f,0x5f,0x5f,0x20,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x20,0x5f,0x5f,0x5f,
    0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x20,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x20,0x20,
    0x5f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0xa,0x28,0x20,0x20,0x5f,0x5f,0x5f,0x5f,
    0x20,0x29,0x28,0x20,0x20,0x5f,0x5f,0x5f,0x5f,0x20,0x29,0x7c,0x5c,0x20,0x20,0x20,
    0x20,0x20,0x2f,0x7c,0x5c,0x5f,0x5f,0x20,0x20,0x20,0x5f,0x5f,0x2f,0x28,0x20,0x20,
    0x5f,0x5f,0x5f,0x5f,0x20,0x5c,0x5c,0x5f,0x5f,0x20,0x20,0x20,0x5f,0x5f,0x2f,0x28,
    0x20,0x20,0x5f,0x5f,0x5f,0x20,0x20,0x29,0x28,0x20,0x28,0x20,0x20,0x20,0x20,0x2f,
    0x7c,0xa,0x7c,0x20,0x28,0x20,0x20,0x20,0x20,0x29,0x7c,0x7c,0x20,0x28,0x20,0x20,
    0x20,0x20,0x29,0x7c,0x7c,0x20,0x29,0x20,0x20,0x20,0x28,0x20,0x7c,0x20,0x20,0x20,
    0x29,0x20,0x28,0x20,0x20,0x20,0x7c,0x20,0x28,0x20,0x20,0x20,0x20,0x5c,0x2f,0x20,
    0x20,0x20,0x29,0x20,0x28,0x20,0x20,0x20,0x7c,0x20,0x28,0x20,0x20,0x20,0x29,0x20,
    0x7c,0x7c,0x20,0x20,0x5c,0x20,0x20,0x28,0x20,0x7c,0xa,0x7c,0x20,0x28,0x5f,0x5f,
    0x5f,0x5f,0x29,0x7c,0x7c,0x20,0x28,0x5f,0x5f,0x5f,0x5f,0x29,0x7c,0x7c,0x20,0x7c,
    0x20,0x20,0x20,0x7c,0x20,0x7c,0x20,0x20,0x20,0x7c,0x20,0x7c,0x20,0x20,0x20,0x7c,
    0x20,0x28,0x5f,0x5f,0x5f,0x5f,0x5f,0x20,0x20,0x20,0x20,0x7c,0x20,0x7c,0x20,0x20,
    0x20,0x7c,0x20,0x7c,0x20,0x20,0x20,0x7c,0x20,0x7c,0x7c,0x20,0x20,0x20,0x5c,0x20,
    0x7c,0x20,0x7c,0xa,0x7c,0x20,0x20,0x5f,0x5f,0x5f,0x5f,0x5f,0x29,0x7c,0x20,0x20,
    0x5f,0x5f,0x5f,0x5f,0x5f,0x29,0x28,0x20,0x28,0x20,0x20,0x20,0x29,0x20,0x29,0x20,
    0x20,0x20,0x7c,0x20,0x7c,0x20,0x20,0x20,0x28,0x5f,0x5f,0x5f,0x5f,0x5f,0x20,0x20,
    0x29,0x20,0x20,0x20,0x7c,0x20,0x7c,0x20,0x20,0x20,0x7c,0x20,0x7c,0x20,0x20,0x20,
    0x7c,0x20,0x7c,0x7c,0x20,0x28,0x5c,0x20,0x5c,0x29,0x20,0x7c,0xa,0x7c,0x20,0x28,
    0x20,0x20,0x20,0x20,0x20,0x20,0x7c,0x20,0x28,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x5c,0x20,0x5c,0x5f,0x2f,0x20,0x2f,0x20,0x20,0x20,0x20,0x7c,0x20,0x7c,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x29,0x20,0x7c,0x20,0x20,0x20,0x7c,0x20,0x7c,
    0x20,0x20,0x20,0x7c,0x20,0x7c,0x20,0x20,0x20,0x7c,0x20,0x7c,0x7c,0x20,0x7c,0x20,
    0x5c,0x20,0x20,0x20,0x7c,0xa,0x7c,0x20,0x29,0x20,0x20,0x20,0x20,0x20,0x20,0x7c,
    0x20,0x29,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x5c,0x20,0x20,0x20,0x2f,0x20,
    0x20,0x5f,0x5f,0x5f,0x29,0x20,0x28,0x5f,0x5f,0x5f,0x2f,0x5c,0x5f,0x5f,0x5f,0x5f,
    0x29,0x20,0x7c,0x5f,0x5f,0x5f,0x29,0x20,0x28,0x5f,0x5f,0x5f,0x7c,0x20,0x28,0x5f,
    0x5f,0x5f,0x29,0x20,0x7c,0x7c,0x20,0x29,0x20,0x20,0x5c,0x20,0x20,0x7c,0xa,0x7c,
    0x2f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x7c,0x2f,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x5c,0x5f,0x2f,0x20,0x20,0x20,0x5c,0x5f,0x5f,0x5f,0x5f,0x5f,
    0x5f,0x5f,0x2f,0x5c,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x29,0x5c,0x5f,0x5f,0x5f,
    0x5f,0x5f,0x5f,0x5f,0x2f,0x28,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x29,0x7c,0x2f,
    0x20,0x20,0x20,0x20,0x29,0x5f,0x29,0xa,0x0};

//\nPico Shell " PSHELL_GIT_TAG ", 

static void version_cmd(void) {

    printf(VT_BLUE);
    tty_puts(art_ppvsion_str);
    printf(VT_COLOR_END);
    printf("LittleFS v%d.%d, Vi " VI_VER ", SDK v%d.%d.%d ",
           LFS_VERSION >> 16, LFS_VERSION & 0xffff, PICO_SDK_VERSION_MAJOR, PICO_SDK_VERSION_MINOR,
           PICO_SDK_VERSION_REVISION);
#if !defined(NDEBUG)
    printf("gcc %s\n", __VERSION__);
#endif
}

#define READ_TM_OUT (100*1000*2)

static bool cursor_pos(uint32_t* x, uint32_t* y) {
    int rc = false;
    *x = 80;
    *y = 24;
    do {
        printf(VT_ESC "[6n");
        tty_fflush(stdout);
        int k = getchar_timeout_us(READ_TM_OUT);
        if (k == PICO_ERROR_TIMEOUT)
            break;
        char* cp = cmd_buffer;
        while (cp < cmd_buffer + sizeof cmd_buffer) {
            k = getchar_timeout_us(READ_TM_OUT);
            if (k == PICO_ERROR_TIMEOUT)
                break;
            *cp++ = k;
        }
        if (cp == cmd_buffer)
            break;
        if (cmd_buffer[0] != '[')
            break;
        *cp = 0;
        if (cp - cmd_buffer < 5)
            break;
        char* end;
        uint32_t row, col;
        if (!isdigit(cmd_buffer[1]))
            break;
        errno = 0;
        row = strtoul(cmd_buffer + 1, &end, 10);
        if (errno)
            break;
        if (*end != ';' || !isdigit(end[1]))
            break;
        col = strtoul(end + 1, &end, 10);
        if (errno)
            break;
        if (*end != 'R')
            break;
        if (row < 1 || col < 1 || (row | col) > 0x7fff)
            break;
        *x = col;
        *y = row;
        rc = true;
    } while (false);
    return rc;
}


static void set_translate_crlf(bool enable) {
    stdio_driver_t* driver;
#if LIB_PICO_STDIO_UART
    driver = &stdio_uart;
#endif
#if LIB_PICO_STDIO_USB
    driver = &stdio_usb;
#endif
    stdio_set_translate_crlf(driver, enable);
}

static bool screen_size(void) {
    int rc = false;
    screen_x = 80;
    screen_y = 24;
    uint32_t cur_x, cur_y;
    do {
        set_translate_crlf(false);
        if (!cursor_pos(&cur_x, &cur_y))
            break;
        printf(VT_ESC "[999;999H");
        if (!cursor_pos(&screen_x, &screen_y))
            break;
        if (cur_x > screen_x)
            cur_x = screen_x;
        if (cur_y > screen_y)
            cur_y = screen_y;
        printf("\033[%d;%dH", cur_y, cur_x);
        tty_fflush(stdout);
        rc = true;
    } while (false);
    set_translate_crlf(true);
    return rc;
}

static void resize_cmd(void) { screen_size(); }




void parse_cmd(void) {
    // read line into buffer
    char* cp = cmd_buffer;
    char* cp_end = cp + sizeof(cmd_buffer);
    char prompt[128];
    snprintf(prompt, sizeof(prompt), VT_BOLD "%s: " VT_NORMAL, full_path(""));
    int prompt_len = strlen(prompt);

    cp = dgreadln(cmd_buffer, mounted, prompt);
    bool not_last = true;
    for (argc = 0; not_last && (argc < MAX_ARGS); argc++) {
        while ((*cp == ' ') || (*cp == ',') || (*cp == '='))
            cp++; // skip blanks
        if ((*cp == '\r') || (*cp == '\n'))
            break;
        argv[argc] = cp; // start of string
        while ((*cp != ' ') && (*cp != ',') && (*cp != '=') && (*cp != '\r') && (*cp != '\n'))
            cp++; // skip non blank
        if ((*cp == '\r') || (*cp == '\n'))
            not_last = false;
        *cp++ = 0; // terminate string
    }
    argv[argc] = NULL;
}

static void cc_cmd(void) {
    if (check_mount(true))
        return;
    cc(0, argc, argv);
}

static void tar_cmd(void) {
    if (check_mount(true))
        return;
    tar(argc, argv);
}




static void yput_cmd(void){
    if (check_mount(true))
        return;
    ymodem_rx();
}
#if 0


static int xmodem_rx_cb(uint8_t* buf, uint32_t len) {
    if (fs_file_write(&file, buf, len) != len)
        printf("error writing file\n");
}

static int xmodem_tx_cb(uint8_t* buf, uint32_t len) { return fs_file_read(&file, buf, len); }



static void xput_cmd(void) {
    if (check_mount(true))
        return;
    if (check_name())
        return;
    if (fs_file_open(&file, full_path(argv[1]), LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC) <
        LFS_ERR_OK) {
        strcpy(result, "Can't create file");
        return;
    }
    set_translate_crlf(false);
    xmodemReceive(xmodem_rx_cb);
    set_translate_crlf(true);
    busy_wait_ms(3000);
    sprintf(result, "\nfile transfered, size: %d", fs_file_seek(&file, 0, LFS_SEEK_END));
    fs_file_close(&file);
}




static void xget_cmd(void) {
    if (check_mount(true))
        return;
    if (check_name())
        return;
    if (fs_file_open(&file, full_path(argv[1]), LFS_O_RDONLY) < LFS_ERR_OK) {
        strcpy(result, "Can't open file");
        return;
    }
    set_translate_crlf(false);
    xmodemTransmit(xmodem_tx_cb);
    set_translate_crlf(true);
    fs_file_close(&file);
}




// clang-format off
/*

{"xget",    xget_cmd,       "get a file (xmodem)"},
{"xput",    xput_cmd,       "put a file (xmodem)"},

*/

#endif

extern cmd_t fs_cmd_table[];

cmd_t cmd_table[] = {
    {"clear",   clear_cmd,      "clear the screen"},
    {"cc",      cc_cmd,         "compile & run C source file. cc -h for help"},
    {"quit",    quit_cmd,       "shutdown the system"},
    {"reboot",  reboot_cmd,     "restart the system"},
    {"resize",  resize_cmd,     "establish screen dimensions"},
    {"tar",     tar_cmd,        "manage tar archives"},
	{"version", version_cmd,    "display pico shell's version"},
    {"vi",      vi_cmd,         "edit file(s) with vi"},
    {"yput",    yput_cmd,       "put a file (ymodem)"},
    // {"DAP_Link",probe_main,     "CMSIS-DAP."},
    {0,0,0}
};


cmd_t *full_cmd_table[] CMDDATA ={
    cmd_table,
    fs_cmd_table,
    NULL
};


// clang-format on

extern unsigned int __CMD_TABLE_START__;
extern unsigned int __CMD_TABLE_END__;
static const cmd_t *__pcmd_table     = (cmd_t *)&__CMD_TABLE_START__;
static const cmd_t *__pcmd_table_end = (cmd_t *)&__CMD_TABLE_END__;

// ADD_CMD(clear,      clear_cmd,      "clear the screen");
// ADD_CMD(cc,         cc_cmd,         "compile & run C source file. cc -h for help");
// ADD_CMD(quit,       quit_cmd,       "shutdown the system");
// ADD_CMD(reboot,     reboot_cmd,     "restart the system");
// ADD_CMD(resize,     resize_cmd,     "establish screen dimensions");
// ADD_CMD(tar,        tar_cmd,        "manage tar archives");
// ADD_CMD(version,    version_cmd,    "display pico shell's version");
// ADD_CMD(vi,         vi_cmd,         "edit file(s) with vi");
// ADD_CMD(yput,       yput_cmd,       "put a file (ymodem)");
ADD_CMD(DAP_Link,   probe_main,     "To enter the CMSIS-DAP ; reboot is required to exit.");


cmd_t * lookup_cmd(char * str_cmd_nm){
    for(int group=0;full_cmd_table[group];group++){
        cmd_t *p_group = full_cmd_table[group];
        for (int i = 0; p_group && p_group[i].name; i++){
            if (strcmp(str_cmd_nm, p_group[i].name) == 0) 
                return  &p_group[i];
        }
    }
    int n_num = __pcmd_table_end - __pcmd_table;//
    for(int i = 0;i< n_num;i++){
        if (strcmp(str_cmd_nm, __pcmd_table[i].name) == 0) 
            return  (cmd_t *)&__pcmd_table[i];
    }
    return NULL;
}

static void help(void) {
    printf("\n");
    for(int group=0;full_cmd_table[group];group++){
        cmd_t *p_group = full_cmd_table[group];
        for (int i = 0; p_group && p_group[i].name; i++)
            printf(VT_BLUE "%9s" VT_COLOR_END " - %s\n", p_group[i].name, p_group[i].descr);
    }

    int n_num = __pcmd_table_end - __pcmd_table;//
    for(int i = 0;i< n_num;i++){
        printf(VT_BLUE "%9s" VT_COLOR_END " - %s\n",__pcmd_table[i].name, __pcmd_table[i].descr);
    }
}




static bool run_as_cmd(const char* dir) {
    char* tfn;
    if (strlen(dir) == 0)
        tfn = full_path(argv[0]);
    else {
        if (argv[0][0] == '/')
            return false;
        tfn = argv[0];
    }
    char* fn = malloc(strlen(tfn) + 6);
    strcpy(fn, dir);
    strcat(fn, tfn);
    char buf[4];
    if (fs_getattr(fn, 1, buf, sizeof(buf)) != sizeof(buf)) {
        free(fn);
        return false;
    }
    if (strcmp(buf, "exe")) {
        free(fn);
        return false;
    }
    argv[0] = fn;
    cc(1, argc, argv);
    free(fn);
    return true;
}


int run_shell(){
	bool detected = screen_size();
    printf(VT_CLEAR);
    tty_fflush(stdout);
    // printf("\n" VT_BOLD "Shell" VT_NORMAL " - Copyright " COPYRIGHT " \n");
    char buf[16];
#if defined(VGABOARD_SD_CLK_PIN)
    strcpy(buf, "sd card");
#else
    strcpy(buf, "internal flash");
#endif
    version_cmd();

/*
    char *console="PPVision";
    printf("\nboard: " PICO_BOARD ", console: %s [%u X %u], filesystem: %s\n\n"
           "enter command or hit ENTER for help\n",
           console, screen_x, screen_y, buf);
*/
    printf("enter command or hit ENTER for help\n");
    if (!detected) {
        printf("\nYour terminal does not respond to standard VT100 escape sequences"
               "\nsequences. The editor will likely not work at all!");
        tty_fflush(stdout);
    }

    if (fs_load() != LFS_ERR_OK) {
        printf("Can't access filesystem device! Aborting.\n");
        exit(-1);
    }
    if (fs_mount() != LFS_ERR_OK) {
        printf("The flash file system appears corrupt or unformatted!\n"
               " would you like to format it (Y/n) ? ");
        tty_fflush(stdout);
        char c = getchar();
        while (c != 'y' && c != 'Y' && c != 'N' && c != 'n' && c != '\r')
            c = getchar();
        tty_putchar(c);
        if (c != '\r')
            echo_key('\r');
        tty_putchar('\n');
        if (c == 'y' || c == 'y')
            if (fs_format() != LFS_ERR_OK)
                printf("Error formating file system!\n");
            else {
                if (fs_mount() != LFS_ERR_OK)
                    printf("Error formating file system!\n");
                else {
                    printf("file system formatted and mounted\n");
                    mounted = true;
                }
            }
    } else {
        printf(VT_GREEN "File system automatically mounted !\n"VT_COLOR_END);
        mounted = true;
    }
	while (run) {
        printf("\n" VT_BOLD "%s: " VT_NORMAL, full_path(""));
        tty_fflush(stdout);
        parse_cmd();
        result[0] = 0;
        bool found = false;
        if (argc) {
            if (argc ==1 && strstr(argv[0], "\033[1m")) {
                continue ;
            }
            if(!strcmp(argv[0], "help")) {
                help();
                continue;
            }
            if (!strcmp(argv[0], "q")) {
                quit_cmd();
                continue;
            }

            cmd_t * p_command = lookup_cmd(argv[0]);
            if(p_command){
                if(p_command->pass_param){
                    ((cmd_func_normal)p_command->func)(argc,argv);
                }else{
                    p_command->func();
                }
                if (result[0]) printf("\n%s\n", result);
                found = true;
                continue;
            }

            /*int group=0;
            for(;full_cmd_table[group];group++){
                cmd_t *p_group = full_cmd_table[group];
                for (int i = 0; p_group&&p_group[i].name; i++){
                    // printf("CMD: [%d,%d]%s\n",group,i,p_group[i].name);
                    if (strcmp(argv[0], p_group[i].name)== 0) {
                        if(p_group[i].pass_param){
                            ((cmd_func_normal)p_group[i].func)(argc,argv);
                        }else{
                            p_group[i].func();
                        }
                        if (result[0]) printf("\n%s\n", result);
                        found = true;
                        break;
                    }
                }
                if(found) break;
            }*/
            if (!found) {
                if (!run_as_cmd("") && !run_as_cmd("/bin/")){
                    printf("\nunknown command '%s'. hit ENTER for help\n", argv[0]);
                }
            }
        } else{
            help();
        }
    }
}
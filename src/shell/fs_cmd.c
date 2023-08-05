#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include "common/base_def.h"
#include "shell.h"
#include "io.h"

bool mounted = false;
static buf_t path;
static buf_t curdir = "/";;
extern buf_t result;
extern void parse_cmd(void) ;
extern int argc;
extern char** argv;

bool check_mount(bool need) {
    if (mounted == need)
        return false;
    sprintf(result, "filesystem is %smounted", (need ? "not " : ""));
    return true;
}

static bool check_name() {
    if (argc > 1)
        return false;
    strcpy(result, "missing file or directory name");
    return true;
}

char* full_path(const char* name) {
    if (name == NULL)
        return NULL;
    if (name[0] == '/') {
        strcpy(path, name);
        return path;
    }
    if (strncmp(name, "./", 2) == 0)
        name += 2;
    strcpy(path, curdir);
    if (strncmp(name, "../", 3) != 0) {
        if (name[0])
            strcat(path, name);
    } else {
        name += 3; // root doen't have a parent
        char* cp = strrchr(path, '/');
        if (cp != NULL)
            *cp = 0;
        cp = strrchr(path, '/');
        if (cp != NULL)
            *(cp + 1) = 0;
        strcat(path, name);
    }
    return path;
}

int check_from_to_parms(char** from, char** to, int copy) {
    *from = NULL;
    *to = NULL;
    int rc = 1;
    do {
        if (argc < 3) {
            strcpy(result, "need two names");
            break;
        }
        bool from_is_dir = false;
        bool to_is_dir = false;
        bool to_exists = false;
        *from = strdup(full_path(argv[1]));
        if (*from == NULL) {
            strcpy(result, "no memory");
            break;
        }
        struct lfs_info info;
        if (fs_stat(*from, &info) < LFS_ERR_OK) {
            sprintf(result, "%s not found", *from);
            break;
        }
        from_is_dir = info.type == LFS_TYPE_DIR;
        *to = strdup(full_path(argv[2]));
        if (*to == NULL) {
            strcpy(result, "no memory");
            break;
        }
        if (fs_stat(*to, &info) == LFS_ERR_OK) {
            to_is_dir = info.type == LFS_TYPE_DIR;
            to_exists = 1;
        }
        if (copy && from_is_dir) {
            strcpy(result, "can't copy a directory");
            break;
        }
        if (to_exists && to_is_dir) {
            char* name = strrchr(*from, '/') + 1;
            bool append_slash = (*to)[strlen(*to) - 1] == '/' ? false : true;
            int l = strlen(*to) + strlen(name) + 1;
            if (append_slash)
                l++;
            char* to2 = malloc(l);
            if (!to2) {
                strcpy(result, "no memory");
                break;
            }
            strcpy(to2, *to);
            if (append_slash)
                strcat(to2, "/");
            strcat(to2, name);
            free(*to);
            *to = to2;
        }
        rc = 0;
    } while (0);
    if (rc) {
        if (*from)
            free(*from);
        if (*to)
            free(*to);
    }
    return rc;
}




static void mv_cmd(int argc, char* argv[]) {
    char* from;
    char* to;
    if (check_from_to_parms(&from, &to, 0))
        return;
    struct lfs_info info;
    if (fs_rename(from, to) < LFS_ERR_OK)
        sprintf(result, "could not move %s to %s", from, to);
    else
        sprintf(result, "%s moved to %s", from, to);
    free(from);
    free(to);
}

static void cp_cmd(int argc, char* argv[]) {
    char* from;
    char* to;
    char* buf = NULL;
    if (check_from_to_parms(&from, &to, 1))
        return;
    lfs_file_t in, out;
    bool in_ok = false, out_ok = false;
    do {
        buf = malloc(4096);
        if (buf == NULL) {
            strcpy(result, "no memory");
            break;
        }
        if (fs_file_open(&in, from, LFS_O_RDONLY) < LFS_ERR_OK) {
            sprintf(result, "error opening %s", from);
            break;
        }
        in_ok = true;
        if (fs_file_open(&out, to, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC) < LFS_ERR_OK) {
            sprintf(result, "error opening %s", from);
            break;
        }
        out_ok = true;
        int l = fs_file_read(&in, buf, 4096);
        while (l > 0) {
            if (fs_file_write(&out, buf, l) != l) {
                sprintf(result, "error writing %s", to);
                break;
            }
            l = fs_file_read(&in, buf, 4096);
        }
    } while (false);
    if (in_ok)
        fs_file_close(&in);
    if (out_ok)
        fs_file_close(&out);
    if (buf) {
        if (out_ok && fs_getattr(from, 1, buf, 4) == 4 && strcmp(buf, "exe") == 0)
            fs_setattr(to, 1, buf, 4);
        free(buf);
    }
    if (!result[0])
        sprintf(result, "file %s copied to %s", from, to);
    free(from);
    free(to);
}

static void cat_cmd(int argc, char* argv[]) {
    if (check_mount(true))
        return;
    if (check_name())
        return;
    lfs_file_t file;
    if (fs_file_open(&file, full_path(argv[1]), LFS_O_RDONLY) < LFS_ERR_OK) {
        strcpy(result, "error opening file");
        return;
    }
    int l = fs_file_seek(&file, 0, LFS_SEEK_END);
    fs_file_seek(&file, 0, LFS_SEEK_SET);
    char buf[256];
    while (l) {
        int l2 = l;
        if (l2 > sizeof(buf))
            l2 = sizeof(buf);
        if (fs_file_read(&file, buf, l2) != l2) {
            sprintf(result, "error reading file");
            break;
        }
        for (int i = 0; i < l2; ++i)
            putchar(buf[i]);
        l -= l2;
    }
    fs_file_close(&file);
}


static void mkdir_cmd(int argc, char* argv[]) {
    if (check_mount(true))
        return;
    if (check_name())
        return;
    if (fs_mkdir(full_path(argv[1])) < LFS_ERR_OK) {
        strcpy(result, "Can't create directory");
        return;
    }
    sprintf(result, "%s created", full_path(argv[1]));
}

static char rmdir_path[256];

static bool clean_dir(char* name) {
    int path_len = strlen(rmdir_path);
    if (path_len)
        strcat(rmdir_path, "/");
    strcat(rmdir_path, name);
    lfs_dir_t dir_f;
    if (fs_dir_open(&dir_f, rmdir_path) < LFS_ERR_OK) {
        printf("can't open %s directory\n", rmdir_path);
        return false;
    }
    struct lfs_info info;
    while (fs_dir_read(&dir_f, &info) > 0)
        if (info.type == LFS_TYPE_DIR && strcmp(info.name, ".") && strcmp(info.name, ".."))
            if (!clean_dir(info.name)) {
                fs_dir_close(&dir_f);
                return false;
            }
    fs_dir_rewind(&dir_f);
    while (fs_dir_read(&dir_f, &info) > 0) {
        if (info.type == LFS_TYPE_REG) {
            int plen = strlen(rmdir_path);
            strcat(rmdir_path, "/");
            strcat(rmdir_path, info.name);
            if (fs_remove(rmdir_path) < LFS_ERR_OK) {
                printf("can't remove %s", rmdir_path);
                fs_dir_close(&dir_f);
                return false;
            }
            printf("%s removed\n", rmdir_path);
            rmdir_path[plen] = 0;
        }
    }
    fs_dir_close(&dir_f);
    if (fs_remove(rmdir_path) < LFS_ERR_OK) {
        sprintf(result, "can't remove %s", rmdir_path);
        return false;
    }
    printf("%s removed\n", rmdir_path);
    rmdir_path[path_len] = 0;
    return true;
}

static void rm_cmd(int argc, char* argv[]) {
    if (check_mount(true))
        return;
    if (check_name())
        return;
    bool recursive = false;
    if (strcmp(argv[1], "-r") == 0) {
        if (argc < 3) {
            strcpy(result, "specify a file or directory name");
            return;
        }
        recursive = true;
        argv[1] = argv[2];
    }
    // lfs won't remove a non empty directory but returns without error!
    struct lfs_info info;
    char* fp = full_path(argv[1]);
    if (fs_stat(fp, &info) < LFS_ERR_OK) {
        sprintf(result, "%s not found", full_path(argv[1]));
        return;
    }
    int isdir = 0;
    if (info.type == LFS_TYPE_DIR) {
        isdir = 1;
        lfs_dir_t dir;
        fs_dir_open(&dir, fp);
        int n = 0;
        while (fs_dir_read(&dir, &info))
            if ((strcmp(info.name, ".") != 0) && (strcmp(info.name, "..") != 0))
                n++;
        fs_dir_close(&dir);
        if (n) {
            if (recursive) {
                rmdir_path[0] = 0;
                clean_dir(fp);
                return;
            } else
                sprintf(result, "directory %s not empty", fp);
            return;
        }
    }
    if (fs_remove(fp) < LFS_ERR_OK)
        strcpy(result, "Can't remove file or directory");
    sprintf(result, "%s %s removed", isdir ? "directory" : "file", fp);
}

static void mount_cmd(void) {
    if (check_mount(false))
        return;
    if (fs_mount() != LFS_ERR_OK) {
        strcpy(result, "Error mounting filesystem");
        return;
    }
    mounted = true;
    strcpy(result, "mounted");
}

static void unmount_cmd(void) {
    if (check_mount(true))
        return;
    if (fs_unmount() != LFS_ERR_OK) {
        strcpy(result, "Error unmounting filesystem");
        return;
    }
    mounted = false;
    strcpy(result, "unmounted");
}

static void format_cmd(void) {
    if (check_mount(false))
        return;
    printf("are you sure (y/N) ? ");
    fflush(stdout);
    parse_cmd();
    if ((argc == 0) || ((argv[0][0] | ' ') != 'y')) {
        strcpy(result, "user cancelled");
        return;
    }
    if (fs_format() != LFS_ERR_OK)
        strcpy(result, "Error formating filesystem");
    strcpy(result, "formatted");
}

static void disk_space(uint64_t n, char* buf) {
    double d = n;
    static const char* suffix[] = {"B", "KB", "MB", "GB", "TB"};
    char** sfx = (char**)suffix;
    while (d >= 1000.0) {
        d /= 1000.0;
        sfx++;
    }
    sprintf(buf, "%.1f%s", d, *sfx);
}

static void status_cmd(void) {
    if (check_mount(true))
        return;
    struct fs_fsstat_t stat;
    fs_fsstat(&stat);
    const char percent = 37;
    char total_size[32], used_size[32];
    disk_space((int64_t)stat.block_count * stat.block_size, total_size);
    disk_space((int64_t)stat.blocks_used * stat.block_size, used_size);
#ifndef NDEBUG
    printf("\ntext size 0x%x (%d), bss size 0x%x (%d)", stat.text_size, stat.text_size,
           stat.bss_size, stat.bss_size);
#endif
    sprintf(result,
            "\ntotal blocks %d, block size %d, used %s out of %s, %1.1f%c "
            "used.\n",
            (int)stat.block_count, (int)stat.block_size, used_size, total_size,
            stat.blocks_used * 100.0 / stat.block_count, percent);
}

static void ls_cmd(void) {
    if (check_mount(true))
        return;
    int show_all = 0;
    char** av = argv;
    if ((argc > 1) && (strcmp(av[1], "-a") == 0)) {
        argc--;
        av++;
        show_all = 1;
    }
    if (argc > 1)
        full_path(av[1]);
    else
        full_path("");
    lfs_dir_t dir;
    if (fs_dir_open(&dir, path) < LFS_ERR_OK) {
        strcpy(result, "not a directory");
        return;
    }
    printf("\n");
    struct lfs_info info;
    while (fs_dir_read(&dir, &info) > 0)
        if (strcmp(info.name, ".") && strcmp(info.name, ".."))
            if (info.type == LFS_TYPE_DIR)
                if ((info.name[0] != '.') || show_all)
                    printf(" %7d [%s]\n", info.size, info.name);
    fs_dir_rewind(&dir);
    while (fs_dir_read(&dir, &info) > 0)
        if (strcmp(info.name, ".") && strcmp(info.name, ".."))
            if (info.type == LFS_TYPE_REG)
                if ((info.name[0] != '.') || show_all)
                    printf(" %7d %s\n", info.size, info.name);
    fs_dir_close(&dir);
}

static void cd_cmd(int argc, char* argv[]) {
    if (check_mount(true))
        return;
    if (argc < 2) {
        strcpy(path, "/");
        goto cd_done;
    }
    if (strcmp(argv[1], ".") == 0)
        goto cd_done;
    if (strcmp(argv[1], "..") == 0) {
        if (strcmp(curdir, "/") == 0) {
            strcpy(result, "not a directory");
            return;
        }
        strcpy(path, curdir);
        char* cp = strrchr(path, '/');
        if (cp == NULL)
            cp = curdir;
        *cp = 0;
        cp = strrchr(path, '/');
        if (cp != NULL)
            *(cp + 1) = 0;
        goto cd_done;
    }
    full_path(argv[1]);
    lfs_dir_t dir;
    if (fs_dir_open(&dir, path) < LFS_ERR_OK) {
        strcpy(result, "not a directory");
        return;
    }
    fs_dir_close(&dir);
cd_done:
    strcpy(curdir, path);
    if (curdir[strlen(curdir) - 1] != '/')
        strcat(curdir, "/");
    sprintf(result, "changed to %s", curdir);
}





cmd_t fs_cmd_table[] = {
    {"cat",     (cmd_func_t)cat_cmd,        "display a text file",1},
    {"rm",      (cmd_func_t)rm_cmd,         "remove a file or directory. -r for recursive",1},
    {"ls",      ls_cmd,         "list a directory, -a to show hidden files"},
    {"cd",      (cmd_func_t)cd_cmd,         "change directory",1},
    {"cp",      (cmd_func_t)cp_cmd,         "copy a file",1},
    {"mkdir",   (cmd_func_t)mkdir_cmd,      "create a directory",1},
    {"mv",      (cmd_func_t)mv_cmd,         "rename a file or directory",1},
    {"format",  format_cmd,     "format the filesystem"},
    {"mount",   mount_cmd,      "mount the filesystem"},
    {"unmount", unmount_cmd,    "unmount the filesystem"},
    {"status",  status_cmd,     "display the filesystem status"},
    {0,0,0}
};


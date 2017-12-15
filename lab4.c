//
// Created by jerry on 17-12-11.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>

#define NORMAL_COLOR  "\x1B[0m"
#define GREEN  "\x1B[32m"
#define BLUE  "\x1B[34m"

void print_info(char *path) {
    // printf("%s%s\n", GREEN, path);
    struct stat info;
    lstat(path, &info);

    // permission of file
    mode_t perm = info.st_mode;

    // number of hard links of file
    unsigned nlink = (unsigned) info.st_nlink;

    // owner and group of file
    char owner[64], group[64];
    struct passwd *pw = getpwuid(info.st_uid);
    struct group *gp = getgrgid(info.st_gid);
    if (!gp || !pw)
        return;
    strcpy(owner, pw->pw_name);
    strcpy(group, gp->gr_name);

    // size of file
    size_t size_of_file = (size_t) info.st_size;

    // date of last modification
    time_t last_modified_time = info.st_mtime;

    // print permissions
    // printf( (S_ISDIR(perm)) ? "d" : S_ISLNK(perm) ? "l" : "-");
    if (S_ISDIR(perm))
        printf("%sd", GREEN);
    else if (S_ISLNK(perm))
        printf("%sl", BLUE);
    else printf("%s-", BLUE);
    printf( (perm & S_IRUSR) ? "r" : "-");
    printf( (perm & S_IWUSR) ? "w" : "-");
    printf( (perm & S_IXUSR) ? "x" : "-");
    printf( (perm & S_IRGRP) ? "r" : "-");
    printf( (perm & S_IWGRP) ? "w" : "-");
    printf( (perm & S_IXGRP) ? "x" : "-");
    printf( (perm & S_IROTH) ? "r" : "-");
    printf( (perm & S_IWOTH) ? "w" : "-");
    printf( (perm & S_IXOTH) ? "x" : "-");

    // print hard links
    printf("%2u", nlink);

    // print owner and group
    printf(" %s %s", owner, group);

    // print size of file
    printf("%7lu", size_of_file);

    // print current time
    char format_time[50];
    char *time = ctime(&last_modified_time);
    memcpy(format_time, time, strlen(time) - 1);
    memcpy(format_time + strlen(time) - 1, ".", 1);
    printf(" %s", format_time);

    // print file name
    char name[255];
    char* p = strrchr(path, '/');
    if (p != NULL)
        strcpy(name, p + 1);
    else
        strcpy(name, path);
    printf(" %s\n", name);

    printf("%s", NORMAL_COLOR);
}

void print_dir(char *path) {

    static char *dir_paths[2048];
    int n = 0;
    struct stat statbuf;
    lstat(path, &statbuf);

    DIR *d = opendir(path);
    if (S_ISREG(statbuf.st_mode)) {
        print_info(path);
        return;
    }
    else if (d == NULL)    // cannot open dir
        return;
    struct dirent* dir;
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type != DT_DIR) {    // not a directory
            // printf("%s%s\n", BLUE, dir->d_name);
            char this_path[255];
            sprintf(this_path, "%s/%s", path, dir->d_name);
            print_info(this_path);
            continue;
        }

        if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
            continue;

        char next_path[255];
        sprintf(next_path, "%s/%s", path, dir->d_name);
        print_info(next_path);
        // print_dir(next_path);
        dir_paths[n] = (char*)malloc(strlen(next_path) + 1);
        strcpy(dir_paths[n], next_path);
        ++n;
    }

    // recursively print directories
    while (n-- > 0) {
        printf("\n%s:\n", dir_paths[n]);
        print_dir(dir_paths[n]);
    }
    closedir(d);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        print_dir(".");
        return 0;
    }
    printf("%s\n", NORMAL_COLOR);
    print_dir(argv[1]);
    printf("%s\n", NORMAL_COLOR);
    return 0;
}
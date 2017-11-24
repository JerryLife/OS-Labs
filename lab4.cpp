//
// Created by jerry on 17-11-23.
//

#include <iostream>
#include "boost/filesystem.hpp"
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>

using namespace std;
namespace fs = boost::filesystem;

void print_dir(string dir) {
    fs::directory_iterator end_itr;
    fs::path p(dir);
    for (fs::directory_iterator itr(p); itr != end_itr; ++itr) {
        if (fs::is_regular_file(itr->path())) {
            // get information of all files

            // file name
            string cur_file = itr->path().string();
            cur_file = cur_file.substr(2, cur_file.size());

            // permission
            fs::file_status s = status(itr->path());
            unsigned perm = s.permissions();

            // number of hardlinks
            unsigned nlink = (unsigned)fs::hard_link_count(itr->path());

            // owner and group of file
            struct stat info;
            stat(cur_file.c_str(), &info);
            struct passwd *pw = getpwuid(info.st_uid);
            struct group *gp = getgrgid(info.st_gid);
            string owner = string(pw->pw_name);
            string group = string(gp->gr_name);

            // size of file
            size_t size_of_file = fs::file_size(itr->path());

            // date of last modification
            time_t last_modified_time = fs::last_write_time(itr->path());

            /////////////////// Format ////////////////////////////////
            // print permissions
            printf("-");
            printf( (perm & fs::owner_read) ? "r" : "-");
            printf( (perm & fs::owner_write) ? "w" : "-");
            printf( (perm & fs::owner_exe) ? "x" : "-");
            printf( (perm & fs::group_read) ? "r" : "-");
            printf( (perm & fs::group_write) ? "w" : "-");
            printf( (perm & fs::group_exe) ? "x" : "-");
            printf( (perm & fs::others_read) ? "r" : "-");
            printf( (perm & fs::others_write) ? "w" : "-");
            printf( (perm & fs::others_exe) ? "x" : "-");

            // print hard links
            printf("%2u", nlink);

            // print owner and group
            printf(" %s %s", owner.c_str(), group.c_str());

            // print size of file
            printf("%7lu", size_of_file);

            // print current time
            string str_time = ctime(&last_modified_time);
            printf(" %s", str_time.substr(0, str_time.size() - 1).c_str());

            // print file name
            printf(" %s", cur_file.c_str());

            printf("\n");

        } else if (is_directory(itr->path())) {
            cout << itr->path().string() << endl;
            print_dir(itr->path().string());
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        print_dir(".");
    }
    else if (argc > 2) {
        cout << "Too many arguments" << endl;
    }
    else {
        print_dir(argv[1]);
    }

    return 0;
}
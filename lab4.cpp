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
            string cur_file = itr->path().string();

            // permission
            fs::file_status s = status(itr->path());
            char c_perm[50];
            sprintf(c_perm, "%o", s.permissions());
            string perm = string(c_perm);

            // number of hardlinks
            uintmax_t nlink = fs::hard_link_count(itr->path());

            // owner of file
            struct stat info;
            stat(cur_file.c_str(), &info);
            struct passwd *pw = getpwuid(info.st_uid);
            struct group *gp = getgrgid(info.st_gid);
            string owner = string(pw->pw_name);
            string group = string(gp->gr_name);


            cout << cur_file << " " << nlink << endl;
        } else if (is_directory(itr->path())) {
            //cout << endl;
            //print_dir(itr->path().string());
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
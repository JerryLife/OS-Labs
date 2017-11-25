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

size_t get_dir_size(fs::path path) {

    size_t size = 0;

    if (fs::is_regular_file(path))
        return fs::file_size(path);
    else {
        fs::directory_iterator dir_itr, dir_itr_end;
        for (dir_itr = fs::directory_iterator(path);
                dir_itr != dir_itr_end; ++ dir_itr) {
            if (fs::is_directory(dir_itr->path())) {
                size += get_dir_size(dir_itr->path());
            } else {
                size += fs::file_size(dir_itr->path());
            }
        }
        return size;
    }
}

void print_info(fs::path path) {
    // file name
    string cur_file = path.string();
    cur_file = cur_file.substr(2, cur_file.size());

    // permission
    fs::file_status s = status(path);
    unsigned perm = s.permissions();

    // number of hardlinks
    unsigned nlink = (unsigned) fs::hard_link_count(path);

    // owner and group of file
    struct stat info;
    stat(cur_file.c_str(), &info);
    struct passwd *pw = getpwuid(info.st_uid);
    struct group *gp = getgrgid(info.st_gid);
    string owner = string(pw->pw_name);
    string group = string(gp->gr_name);

    // size of file
    size_t size_of_file = 0;
    if (fs::is_regular_file(path))
        size_of_file = fs::file_size(path);
    else if (fs::is_directory(path))
        size_of_file = get_dir_size(path);

    // date of last modification
    time_t last_modified_time = fs::last_write_time(path);

    /////////////////// Format ////////////////////////////////
    // print permissions
    printf(fs::is_directory(path) ? "d" : "-");
    printf((perm & fs::owner_read) ? "r" : "-");
    printf((perm & fs::owner_write) ? "w" : "-");
    printf((perm & fs::owner_exe) ? "x" : "-");
    printf((perm & fs::group_read) ? "r" : "-");
    printf((perm & fs::group_write) ? "w" : "-");
    printf((perm & fs::group_exe) ? "x" : "-");
    printf((perm & fs::others_read) ? "r" : "-");
    printf((perm & fs::others_write) ? "w" : "-");
    printf((perm & fs::others_exe) ? "x" : "-");

    // print hard links
    printf("%2u", nlink);

    // print owner and group
    printf(" %s %s", owner.c_str(), group.c_str());

    // print size of file
    printf("%7lu", size_of_file);

    // print current time
    string str_time = ctime(&last_modified_time);
    printf(" %s", str_time.substr(0, str_time.size() - 1).c_str()); // delete '\n'

    // print file name
    string file_name;
    if (cur_file.rfind('/') != -1)
        file_name = cur_file.substr(cur_file.rfind('/') + 1, cur_file.size());
    else file_name = cur_file;
    printf(" %s", file_name.c_str());

    printf("\n");
    //////////////////////////////////////////////////////////

}

void print_dir(string dir) {

    static vector<string> dir_paths;
    
    if (fs::is_regular_file(dir)) {
        fs::path p{dir};
        print_info(p);
        return;
    }

    fs::directory_iterator end_itr;
    fs::path p(dir);
    for (fs::directory_iterator itr(p); itr != end_itr; ++itr) {
        if (itr->path().string() == "." || itr->path().string() == "..")
            continue;

        // get information of all files
        print_info(itr->path());
        
        // if directory, save it
        if (fs::is_directory(itr->path())) {
            dir_paths.push_back(itr->path().string());
        }
    }

    // recursively print directories
    while (!dir_paths.empty()) {
        string path = dir_paths[dir_paths.size() - 1];
        dir_paths.pop_back();
        printf("\n%s:\n", path.c_str());
        print_dir(path);
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
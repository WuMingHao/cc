#ifndef SA_SYS_UTIL_H
#define SA_SYS_UTIL_H

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>


static int make_socket_non_blocking(int sfd)
{
	int flags = 0, s = 0;
	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl()");
		return -1;
	}
	flags |= O_NONBLOCK;
	s = fcntl(sfd, F_SETFL, flags);
	if (s == -1) {
		perror("fcntl()");
		return -1;
	}
	return 0;
}

static void display_suc(char* msg)
{
	printf("\033[0;32m%s\033[0m", msg);
}

static void display_err(char* msg)
{
	printf("\033[0;31m%s\033[0m", msg);
}

static bool is_dir_exist(const char* path)
{
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("stat");
        return false;
    }
    
    return S_ISDIR(st.st_mode);
}

static bool is_file_exist(const char* path)
{
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("stat");
        return false;
    }
    
    return S_ISREG(st.st_mode);
}

#endif

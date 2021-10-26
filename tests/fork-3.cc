// design_patterns_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/26.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// see also:
// https://indradhanush.github.io/blog/writing-a-unix-shell-part-1/

int main() {
    pid_t child_pid;
    pid_t wait_result;
    int stat_loc;

    child_pid = fork();

    // The child process
    if (child_pid == 0) {
        printf("### Child ###\nCurrent PID: %d and Child PID: %d\n",
               getpid(), child_pid);
        sleep(1); // Sleep for one second
    } else {
        wait_result = waitpid(child_pid, &stat_loc, WUNTRACED);
        printf("### Parent ###\nCurrent PID: %d and Child PID: %d\nWait Result: %d\n",
               getpid(), child_pid, wait_result);
    }

    return 0;
}
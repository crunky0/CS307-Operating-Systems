#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    int pipefd[2];
    pid_t pid_man, pid_grep;

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_man = fork();
    if (pid_man < 0) {
        perror("fork failed for man");
        exit(EXIT_FAILURE);
    }

    if (pid_man == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        execlp("man", "man", "diff", NULL);
        perror("exec failed for man");
        exit(EXIT_FAILURE);
    }

    pid_grep = fork();
    if (pid_grep < 0) {
        perror("fork failed for grep");
        exit(EXIT_FAILURE);
    }

    if (pid_grep == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        close(pipefd[0]);

        int output_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 
0644);
        if (output_fd < 0) {
            perror("open failed for output.txt");
            exit(EXIT_FAILURE);
        }
        dup2(output_fd, STDOUT_FILENO);
        close(output_fd);

        execlp("grep", "grep", "-A", "1", "--", "-y", NULL);
        perror("exec failed for grep");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(pid_man, NULL, 0);
    waitpid(pid_grep, NULL, 0);

    printf("Piped command execution finished.\n");
    return 0;
}

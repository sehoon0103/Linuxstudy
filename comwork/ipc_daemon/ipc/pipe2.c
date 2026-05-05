#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* cat /etc/passwd | wc -l */

extern char **environ;

char *const front_argv[] = {
    "/bin/cat",
    "/etc/passwd",
    NULL
};

char *const back_argv[] = {
    "/bin/wc",
    "-l",
    NULL
};

int main() {
    pid_t pid;
    int pipefd[2];

    pipe(pipefd);

    pid = fork();
    if (pid == 0) {
        close(pipefd[0]); // 읽기 fd는 필요없으므로 닫는다.
        dup2(pipefd[1], STDOUT_FILENO); // 쓰기 fd를 표준출력으로
        // 모든 표준 출력은 쓰기 fd로 전송된다.
        close(pipefd[1]); // 표준출력으로 복제되었으므로 닫는다.
        execve(front_argv[0], front_argv, environ);
    }

    pid = fork();
    if (pid == 0) {
        close(pipefd[1]); // 읽기 fd는 필요없으므로 닫는다.
        dup2(pipefd[0], STDIN_FILENO); // 읽기 fd를 표준입력으로
        // 읽기 fd에서 읽은 값을 표준 입력으로 사용한다.
        close(pipefd[0]); // 표준입력으로 복제되었으므로 닫는다.
        execve(back_argv[0], back_argv, environ);
    }

    wait(NULL);
    // 부모프로세스에서의 fd는 별개로 닫아야 함
    close(pipefd[0]); // 읽기 fd가 닫히면 2번째 프로세스의 표준입력도 닫힘
    close(pipefd[1]); // 닫히면서 2번째 프로세스도 종료됨

    wait(NULL);

    return 0;
}

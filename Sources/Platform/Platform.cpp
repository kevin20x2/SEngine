//
// Created by vinkzheng on 2025/8/28.
//

#include "Platform.h"

#include "Core/Log.h"
#ifdef _WIN32
#include "Windows.h"
#endif


bool FPlatform::ExecuteCommand(const FString &command,FString & Result)
{

#ifdef _WIN32
    // Windows 实现
    HANDLE readPipe, writePipe;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&readPipe, &writePipe, &sa, 0)) {
        SLogE("CreatePipe() failed");
        return false;
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = writePipe;
    si.hStdError = writePipe;

    std::string cmd = "cmd /c " + command;  // 使用 cmd 解释命令

    if (!CreateProcessA(NULL, cmd.data(), NULL, NULL, TRUE,
                        CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(readPipe);
        CloseHandle(writePipe);
        return "创建进程失败";
    }

    CloseHandle(writePipe);

    char buffer[4096];
    DWORD bytesRead;
    while (ReadFile(readPipe, buffer, sizeof(buffer)-1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        Result += buffer;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(readPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (exitCode != 0) {
        SLogE("{}", Result );
        Result +=  "Fail To Execute Command " + std::to_string(exitCode);
        return false;
    }

#else
    // Linux/macOS 实现
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        return "创建管道失败";
    }

    pid_t pid;
    char* argv[] = {const_cast<char*>("/bin/sh"),
                   const_cast<char*>("-c"),
                   const_cast<char*>(command.c_str()),
                   NULL};

    extern char** environ;  // 环境变量

    // 创建子进程
    if (posix_spawnp(&pid, "/bin/sh", NULL, NULL, argv, environ) != 0) {
        close(pipefd[0]);
        close(pipefd[1]);
        return "创建进程失败";
    }

    close(pipefd[1]);  // 关闭写端

    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer)-1)) > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }

    close(pipefd[0]);  // 关闭读端

    int status;
    waitpid(pid, &status, 0);  // 等待子进程结束

    if (WIFEXITED(status)) {
        int exitStatus = WEXITSTATUS(status);
        if (exitStatus != 0) {
            return "命令执行失败: " + std::to_string(exitStatus);
        }
    }
#endif

    return true;
}

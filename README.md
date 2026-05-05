# Linuxstudy

# comwork

Linux system programming and kernel interface learning workspace.

이 저장소는 Ubuntu/Linux 환경에서 리눅스 시스템 프로그래밍, 커널 모듈, 문자 디바이스 드라이버, ioctl, IPC, systemd daemon, Buildroot/QEMU 실행환경을 단계적으로 학습한 내용을 정리한 작업 디렉토리입니다.

단순한 C 예제만 모아둔 구조가 아니라, 사용자 공간 프로그램에서 커널 인터페이스를 호출하고, daemon과 Unix domain socket, shared library를 거쳐 커널 드라이버와 연동되는 흐름까지 실습한 내용을 포함합니다.

## 전체 구조

```text
comwork/
├── add_syscall/          # 사용자 정의 시스템콜 추가 실습
├── driver/               # 커널 문자 디바이스 드라이버 및 ioctl 실습
├── gdb/                  # gdb 디버깅 기초 실습
├── ioctl/                # 사용자 공간 ioctl 호출 실습
├── ipc_daemon/           # IPC, systemd, 최종 daemon 프로젝트
├── makeKmodule/          # 커널 모듈/Kbuild 기초 실습
├── makefile/             # Makefile, Buildroot, Kernel config 실습
├── run.sh                # QEMU aarch64 실행 스크립트
└── shscript/             # shell script 실습
```

## 학습 흐름

```text
1. 기본 개발 도구 및 빌드 환경
   - Makefile
   - Shell script
   - GDB

2. 커널 빌드 및 커널 모듈 기초
   - Kbuild
   - Kconfig
   - module_init / module_exit
   - .ko 모듈 생성

3. 사용자 공간과 커널 공간의 경계 학습
   - 사용자 정의 system call
   - 문자 디바이스 드라이버
   - ioctl 인터페이스

4. 사용자 공간 IPC와 서비스 구조 학습
   - fork
   - mmap
   - pipe
   - semaphore
   - signal
   - socket
   - systemd socket/service

5. 최종 통합 과제
   - sample application
   - shared library
   - Unix domain socket
   - systemd daemon
   - kernel driver ioctl 연동
```

## 디렉토리별 설명

## 1. add_syscall

사용자 정의 시스템콜을 커널에 추가하고, 사용자 공간 프로그램에서 `syscall()`을 통해 직접 호출하는 실습입니다.

```text
add_syscall/
├── hello/
│   ├── hello.c
│   ├── new_syscall.c
│   ├── hello
│   └── new_syscall.o
└── stack_syscall/
    ├── stack_syscall.c
    ├── user_program.c
    ├── stack
    └── stack_syscall.o
```

### hello

`new_syscall.c`는 `SYSCALL_DEFINE1(new_syscall, int, code)` 형태로 커널 시스템콜을 정의합니다. 내부에서 `printk()`를 사용해 커널 로그를 출력하고, 전달받은 값에 1을 더해 반환합니다.

`hello.c`는 사용자 공간 프로그램입니다. 시스템콜 번호를 직접 정의한 뒤 `syscall()`을 호출하여 커널에 추가한 시스템콜을 테스트합니다.

### stack_syscall

`stack_syscall.c`는 커널 내부에 static stack을 두고, push/pop 동작을 시스템콜로 제공하는 예제입니다.

* `push_data(long data)`
* `pop_data()`

`user_program.c`는 명령행 인자를 받아 `push [data]` 또는 `pop` 명령으로 커널 내부 stack을 제어합니다.

### 학습 포인트

* `SYSCALL_DEFINE0`, `SYSCALL_DEFINE1`
* 사용자 정의 시스템콜 추가 흐름
* 사용자 공간의 `syscall()` 호출
* 커널 내부 static 데이터 구조
* 사용자 공간과 커널 공간의 경계

## 2. makeKmodule

가장 기본적인 리눅스 커널 모듈과 Kbuild 구조를 학습한 디렉토리입니다.

```text
makeKmodule/
└── comento/
    ├── main.c
    ├── Kconfig
    ├── Makefile
    ├── comento.mod.c
    ├── comento.mod
    ├── modules.order
    ├── comento.ko
    ├── built-in.a
    ├── comento.o
    ├── comento.mod.o
    └── main.o
```

`main.c`는 `module_init()`과 `module_exit()`을 사용하는 최소 커널 모듈 예제입니다. 모듈 로드 시 init 함수가 실행되고, 언로드 시 exit 함수가 실행됩니다.

`Kconfig`와 `Makefile`은 커널 빌드 시스템에 모듈을 연결하기 위한 설정 파일입니다.

### 학습 포인트

* 커널 모듈 기본 구조
* `module_init()` / `module_exit()`
* `__init` / `__exit`
* `MODULE_AUTHOR`, `MODULE_DESCRIPTION`, `MODULE_LICENSE`
* Kconfig
* Kbuild Makefile
* `.ko`, `.mod.c`, `.mod.o`, `modules.order` 등 빌드 산출물 구조

## 3. driver

커널 문자 디바이스 드라이버를 작성하고, 사용자 공간에서 ioctl로 제어하는 실습입니다.

```text
driver/
├── comento/
│   ├── main.c
│   ├── Kconfig
│   ├── Makefile
│   ├── comento.mod.c
│   ├── comento.mod
│   ├── modules.order
│   ├── comento.ko
│   ├── built-in.a
│   ├── comento.o
│   ├── comento.mod.o
│   └── main.o
├── user_program.c
└── comento_ioctl
```

`driver/comento/main.c`는 문자 디바이스 드라이버 구현 파일입니다. 디바이스 파일을 통해 사용자 공간과 커널 공간이 통신할 수 있도록 구성되어 있습니다.

`driver/user_program.c`는 사용자 공간에서 디바이스 파일을 열고 ioctl 명령을 보내는 테스트 프로그램입니다.

### 학습 포인트

* 리눅스 문자 디바이스 드라이버 구조
* 디바이스 파일 기반 사용자-커널 통신
* ioctl 명령 정의 및 처리
* 사용자 공간 테스트 프로그램 작성
* 커널 모듈 빌드 결과 확인

## 4. ioctl

사용자 공간에서 `/dev/comento-device`를 열고 ioctl 명령을 보내는 기본 예제입니다.

```text
ioctl/
├── ioctl.c
└── ioctl
```

`ioctl.c`는 `_IO()` 매크로를 사용해 ioctl 명령을 정의하고, `open()`, `ioctl()`, `close()` 흐름으로 디바이스에 명령을 전달합니다.

### 학습 포인트

* `open()`
* `ioctl()`
* `close()`
* `_IO` 매크로
* magic number 기반 ioctl command 정의
* 사용자 공간에서 커널 디바이스 제어

## 5. makefile

일반 C 프로그램 빌드, Makefile 작성, Buildroot 설정, Kernel config, QEMU 실행환경 구성을 학습한 디렉토리입니다.

```text
makefile/
├── Makefile
├── buildroot_config
├── kernel_config
└── basics/
    ├── main.c
    ├── distance.c
    ├── distance.h
    ├── executable
    ├── main.o
    └── distance.o
```

`basics/distance.c`는 두 점 사이의 유클리드 거리를 계산하는 `get_distance()` 함수를 구현합니다.

`basics/distance.h`는 함수 원형을 선언하고, `basics/main.c`는 해당 함수를 호출하는 예제입니다.

상위 `Makefile`은 Buildroot 다운로드/설정/빌드, Linux kernel 다운로드/설정/빌드, toolchain 다운로드 등의 흐름을 포함합니다.

### 학습 포인트

* C 파일 분리 컴파일
* header/source 구조
* Makefile 작성
* Buildroot 설정
* Kernel config 적용
* QEMU 기반 실행환경 준비

## 6. shscript

shell script 기본 구조와 `find` 명령을 이용한 자동화 실습입니다.

```text
shscript/
├── shell_script_studied.sh
└── one/
    ├── one
    ├── two
    └── thr
```

`shell_script_studied.sh`는 인자로 받은 디렉토리 아래에서 빈 디렉토리를 찾아 `.gitkeep` 파일을 생성하는 스크립트입니다.

### 학습 포인트

* `#!/bin/sh`
* positional parameter `$0`, `$1`, `$#`
* shell 조건문
* `find`
* `-empty`
* `-type d`
* `-exec`
* `.gitkeep`을 이용한 빈 디렉토리 Git 추적

## 7. gdb

간단한 C 프로그램을 대상으로 gdb 디버깅을 학습한 디렉토리입니다.

```text
gdb/
├── factorial.c
└── factorial
```

`factorial.c`는 재귀 방식의 factorial 함수를 구현하고, 명령행 인자를 받아 결과를 출력합니다.

### 학습 포인트

* 재귀 함수
* `argc` / `argv`
* `atoi()`
* gdb 디버깅 대상 프로그램 구성
* breakpoint, step, print 등을 이용한 실행 흐름 확인

## 8. ipc_daemon/ipc

사용자 공간 IPC 기초를 실습한 디렉토리입니다.

```text
ipc_daemon/ipc/
├── fork.c
├── mmap.c
├── pipe.c
├── pipe2.c
├── semaphore.c
├── signal.c
├── socket-client.c
├── socket-server.c
├── fork
├── mmap
├── pipe
├── pipe2
├── semaphore
└── signal
```

파일명 기준으로 다음 IPC 요소를 학습한 묶음입니다.

* process 생성: `fork`
* memory mapping: `mmap`
* pipe 기반 통신: `pipe`, `pipe2`
* semaphore 동기화
* signal 처리
* socket client/server 통신

현재 이 디렉토리의 각 소스 파일은 파일명 기준으로 학습 항목을 정리했습니다.

## 9. ipc_daemon/systemd

systemd socket/service 기초 구조를 학습한 디렉토리입니다.

```text
ipc_daemon/systemd/
├── comento-client.c
├── comento-daemon.c
├── comento.service
├── comento.socket
├── comento-client
└── comento-daemon
```

systemd socket unit과 service unit을 이용해 client/daemon 구조를 실습한 묶음입니다.

현재 이 디렉토리의 각 소스 파일은 파일명 기준으로 학습 항목을 정리했습니다.

## 10. ipc_daemon/project

최종 통합 과제물 성격의 프로젝트입니다.

앞에서 학습한 커널 드라이버, ioctl, IPC, socket, systemd, shared library 개념을 하나의 흐름으로 연결한 구조입니다.

```text
ipc_daemon/project/
├── include/
│   └── comento.h
├── src/
│   ├── library.c
│   ├── sample.c
│   └── service.c
├── systemd/
│   ├── comento.service
│   └── comento.socket
├── lib/
│   └── libcomento.so
├── comento-daemon
└── sample
```

### 전체 동작 흐름

```text
sample application
    ↓
libcomento.so
    ↓
/run/comento.sock
    ↓
comento-daemon
    ↓
/dev/keyringctl ioctl
    ↓
kernel driver
```

또는 암복호화 요청의 경우 다음 흐름으로 동작합니다.

```text
sample application
    ↓
libcomento.so
    ↓
Unix domain socket
    ↓
comento-daemon
    ↓
openssl enc -aes-256-cbc -kfile /dev/keyring<uid>
    ↓
result response
```

### include/comento.h

사용자 프로그램에서 사용할 API를 선언한 헤더입니다.

```c
int encrypt(uid_t uid, const char *in, size_t in_len, char *out, size_t *out_len);
int decrypt(uid_t uid, const char *in, size_t in_len, char *out, size_t *out_len);
int create_keyring(uid_t uid);
int destroy_keyring(uid_t uid);
```

### src/library.c

`libcomento.so`의 구현 파일입니다.

사용자 프로그램이 직접 socket이나 ioctl을 다루지 않도록, 내부 `request()` 함수에서 Unix domain socket `/run/comento.sock`에 접속해 daemon에게 요청을 전달합니다.

제공 API는 다음과 같습니다.

* `encrypt()`
* `decrypt()`
* `create_keyring()`
* `destroy_keyring()`

### src/service.c

`comento-daemon`의 소스 파일입니다.

systemd가 넘겨준 listen fd를 `sd_listen_fds(0)`로 받고, `accept()`를 통해 클라이언트 요청을 처리합니다.

요청 타입은 다음과 같습니다.

```c
#define MSG_TYPE_ENCRYPT 0
#define MSG_TYPE_DECRYPT 1
#define MSG_TYPE_CREATE 2
#define MSG_TYPE_DESTROY 3
```

`CREATE`와 `DESTROY` 요청은 `/dev/keyringctl`에 ioctl을 보내 keyring을 생성하거나 삭제합니다.

`ENCRYPT`와 `DECRYPT` 요청은 `/usr/bin/openssl`을 `fork()` 후 `execve()`로 실행하여 암복호화를 처리합니다.

### src/sample.c

`libcomento.so`와 `comento.h`를 사용하는 테스트 프로그램입니다.

지원 명령은 다음과 같습니다.

```text
sample create <id>
sample destroy <id>
sample test <id>
```

`test` 명령은 문자열을 암호화한 뒤 다시 복호화하여 결과를 출력합니다.

### systemd/comento.socket

Unix domain socket `/run/comento.sock`을 열기 위한 systemd socket unit입니다.

### systemd/comento.service

`/usr/bin/comento-daemon`을 실행하기 위한 systemd service unit입니다.

### 학습 포인트

* systemd socket activation
* daemon process
* Unix domain socket server/client
* shared library API 설계
* 사용자 공간 daemon과 커널 드라이버 ioctl 연동
* 외부 프로그램 실행을 위한 `fork()` / `execve()`
* `/dev/keyringctl`, `/dev/keyring<uid>` 기반 흐름
* sample application → library → daemon → kernel driver 구조

## 11. run.sh

QEMU aarch64 가상 머신 실행 스크립트입니다.

```text
run.sh
```

실행 흐름은 다음과 같습니다.

```text
qemu-system-aarch64
  -kernel linux/arch/arm64/boot/Image
  -drive format=raw,file=buildroot/output/images/rootfs.ext4,if=virtio
  -append "root=/dev/vda console=ttyAMA0 nokaslr"
  -nographic
  -M virt
  -cpu cortex-a72
  -m 2G
  -smp 2
```

### 학습 포인트

* QEMU aarch64 실행
* 직접 빌드한 Linux kernel Image 사용
* Buildroot rootfs.ext4 사용
* virtio block device 연결
* console 기반 부팅

## 빌드 산출물 구분

이 저장소에는 학습 과정에서 생성된 실행파일, 오브젝트 파일, 커널 모듈, 공유 라이브러리 산출물이 함께 포함되어 있습니다.

예시는 다음과 같습니다.

```text
*.o
*.ko
*.a
실행파일
*.so
```

이 파일들은 소스 분석 대상이라기보다 빌드 결과물입니다. Git에 올릴 때는 학습 기록 보존 목적이라면 포함할 수 있지만, 일반적인 소스 저장소 기준으로는 `.gitignore` 처리하는 것이 더 적절할 수 있습니다.

## 주요 학습 내용 요약

이 작업 디렉토리를 통해 다음 내용을 실습했습니다.

* Linux user space program 작성
* Makefile 기반 C 빌드
* Buildroot 기반 root filesystem 구성
* Linux kernel config 적용
* QEMU aarch64 실행환경 구성
* Kernel module 작성
* Kbuild/Kconfig 구조 이해
* 사용자 정의 system call 추가
* Character device driver 작성
* ioctl 기반 사용자-커널 통신
* shell script 자동화
* gdb 디버깅
* IPC 기초: fork, pipe, mmap, semaphore, signal, socket
* systemd socket/service 구성
* daemon process 작성
* Unix domain socket 기반 IPC
* shared library API 작성
* sample application, library, daemon, kernel driver 연동 구조 구현

## 최종 과제 구조 요약

최종적으로 `ipc_daemon/project/`에서는 다음과 같은 구조를 구현했습니다.

```text
사용자 애플리케이션
    ↓
공유 라이브러리
    ↓
Unix domain socket
    ↓
systemd daemon
    ↓
ioctl 또는 openssl 처리
    ↓
커널 문자 디바이스 / keyring device
```

이 구조를 통해 사용자 애플리케이션이 커널 드라이버를 직접 제어하지 않고, 라이브러리와 daemon을 거쳐 필요한 기능을 요청하는 흐름을 실습했습니다.

## 주의사항

* 일부 디렉토리에는 빌드 산출물이 포함되어 있습니다.
* `ipc_daemon/ipc/`와 `ipc_daemon/systemd/`의 일부 파일은 파일명 기준으로 학습 항목을 정리했습니다.
* 실제 실행을 위해서는 커널 이미지, Buildroot rootfs, 관련 커널 모듈, systemd unit 배치, 디바이스 파일 구성이 필요합니다.
* 이 저장소는 학습 목적의 코드와 산출물을 포함합니다.

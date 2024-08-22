#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "sysinfo.h"
#include "proc.h"

uint64 free_mem(void);   // 声明 free_mem 函数
int num_procs(void);     // 声明 num_procs 函数

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int mask;
  
  // 获取系统调用的参数，即跟踪掩码
  if(argint(0, &mask) < 0)
    return -1;

  // 设置当前进程的 trace_mask
  struct proc *p = myproc();
  p->trace_mask = mask;

  return 0;
}

uint64
sys_sysinfo(void)
{
    struct sysinfo info;
    struct sysinfo *uinfo;

    if(argaddr(0, (uint64 *)&uinfo) < 0)
        return -1;

    // 获取空闲内存数
    info.freemem = free_mem();
    
    // 获取正在使用的进程数
    info.nproc = num_procs();

    // 将信息复制到用户空间
    if(copyout(myproc()->pagetable, (uint64)uinfo, (char *)&info, sizeof(info)) < 0)
        return -1;

    return 0;
}

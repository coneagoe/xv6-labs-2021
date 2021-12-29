#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

extern uint64 free_byte_count(void);
extern uint64 not_unused_proc_count(void);

#define offsetof(TYPE, MEMBER)  ((uint64) &((TYPE *)0)->MEMBER)


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
  int trace_mask;

  if (argint(0, &trace_mask) < 0)
    return -1;

  myproc()->trace_mask = trace_mask;
  return 0;
}


uint64
sys_sysinfo(void)
{
  uint64 info, freemem, nproc;

  if (argaddr(0, &info) < 0)
    return -1;

  freemem = free_byte_count();
  nproc = not_unused_proc_count();

  if (copyout(myproc()->pagetable, info, (char *)&freemem, sizeof(freemem)) < 0 ||
      copyout(myproc()->pagetable, info + offsetof(struct sysinfo, nproc), (char *)&nproc, sizeof(nproc)) < 0) {
    return -1;
  }

  return 0;
}



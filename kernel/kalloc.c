// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

void
kinit()
{
  // 初始化每个 CPU 的 kmem 锁
  for(int i = 0; i < NCPU; i++) {
    initlock(&kmem[i].lock, "kmem");
    kmem[i].freelist = 0;
  }
  
  // 将可用的内存范围分配到各个 CPU 的空闲列表中
  freerange(end, (void*)PHYSTOP);
}


void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    // 获取当前CPU编号
    int cpu = cpuid();
    
    // 关闭中断，防止并发问题
    push_off();

    // 获取当前CPU的kmem锁
    acquire(&kmem[cpu].lock);

    // 将页添加到当前CPU的空闲列表中
    struct run *r = (struct run *)p;
    r->next = kmem[cpu].freelist;
    kmem[cpu].freelist = r;

    // 释放锁
    release(&kmem[cpu].lock);

    // 恢复中断
    pop_off();
  }
}


// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  // 检查释放的页面是否对齐到页边界，并确保地址在合法的物理内存范围内
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // 用垃圾数据填充内存以捕获悬空引用
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  int cpu = cpuid();  // 获取当前 CPU 编号
  push_off();  // 关闭中断，防止并发问题
  acquire(&kmem[cpu].lock);  // 获取当前 CPU 的 kmem 锁

  // 将页面添加到当前 CPU 的空闲列表中
  r->next = kmem[cpu].freelist;
  kmem[cpu].freelist = r;

  release(&kmem[cpu].lock);  // 释放锁
  pop_off();  // 恢复中断
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  int cpu = cpuid();

  push_off();  // 关闭中断，防止并发问题
  acquire(&kmem[cpu].lock);  // 获取当前 CPU 的 kmem 锁

  r = kmem[cpu].freelist;  // 尝试从当前 CPU 的空闲列表中获取页面
  if(r)
    kmem[cpu].freelist = r->next;  // 如果成功获取页面，将空闲列表指针移到下一个页面

  release(&kmem[cpu].lock);  // 释放当前 CPU 的锁

  if(!r) {  // 如果当前 CPU 的空闲列表为空，尝试从其他 CPU 的空闲列表中获取页面
    for(int i = 0; i < NCPU; i++) {
      if(i == cpu)  // 跳过当前 CPU
        continue;
      
      acquire(&kmem[i].lock);  // 获取其他 CPU 的 kmem 锁
      r = kmem[i].freelist;  // 尝试从其他 CPU 的空闲列表中获取页面
      if(r) {
        kmem[i].freelist = r->next;  // 如果成功获取页面，将空闲列表指针移到下一个页面
        release(&kmem[i].lock);  // 释放其他 CPU 的锁
        break;
      }
      release(&kmem[i].lock);  // 如果没有获取到页面，继续尝试下一个 CPU
    }
  }

  pop_off();  // 恢复中断

  if(r)
    memset((char*)r, 5, PGSIZE);  // 填充页面内容以帮助调试
  return (void*)r;
}


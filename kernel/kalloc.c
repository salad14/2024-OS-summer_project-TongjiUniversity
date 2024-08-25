// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"


int useReference[PHYSTOP/PGSIZE];  // 引用计数数组
struct spinlock ref_count_lock;     // 自旋锁用于保护引用计数数组

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

// Reference count array
static uint refcount[PHYSTOP / PGSIZE];  // Array to store reference counts

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&ref_count_lock, "ref_count_lock"); // 初始化自旋锁
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Increment the reference count of a page
void incref(uint64 pa) {
    acquire(&kmem.lock);
    refcount[pa / PGSIZE]++;
    release(&kmem.lock);
}

// Decrement the reference count of a page and free it if count reaches 0
void decref(uint64 pa) {
    acquire(&kmem.lock);
    if (--refcount[pa / PGSIZE] == 0) {
        kfree((void*)pa);
    }
    release(&kmem.lock);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  acquire(&ref_count_lock);
  useReference[(uint64)pa/PGSIZE] -= 1;
  int temp = useReference[(uint64)pa/PGSIZE];
  release(&ref_count_lock);

  if (temp > 0) // 如果引用计数不为 0，不释放页面
    return;

  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r) {
    memset((char*)r, 5, PGSIZE); // 填充垃圾数据
    acquire(&ref_count_lock);
    useReference[(uint64)r / PGSIZE] = 1; // 初始化引用计数为 1
    release(&ref_count_lock);
  }
  return (void*)r;
}


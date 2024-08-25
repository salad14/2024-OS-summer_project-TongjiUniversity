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
  // ��ʼ��ÿ�� CPU �� kmem ��
  for(int i = 0; i < NCPU; i++) {
    initlock(&kmem[i].lock, "kmem");
    kmem[i].freelist = 0;
  }
  
  // �����õ��ڴ淶Χ���䵽���� CPU �Ŀ����б���
  freerange(end, (void*)PHYSTOP);
}


void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    // ��ȡ��ǰCPU���
    int cpu = cpuid();
    
    // �ر��жϣ���ֹ��������
    push_off();

    // ��ȡ��ǰCPU��kmem��
    acquire(&kmem[cpu].lock);

    // ��ҳ��ӵ���ǰCPU�Ŀ����б���
    struct run *r = (struct run *)p;
    r->next = kmem[cpu].freelist;
    kmem[cpu].freelist = r;

    // �ͷ���
    release(&kmem[cpu].lock);

    // �ָ��ж�
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

  // ����ͷŵ�ҳ���Ƿ���뵽ҳ�߽磬��ȷ����ַ�ںϷ��������ڴ淶Χ��
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // ��������������ڴ��Բ�����������
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  int cpu = cpuid();  // ��ȡ��ǰ CPU ���
  push_off();  // �ر��жϣ���ֹ��������
  acquire(&kmem[cpu].lock);  // ��ȡ��ǰ CPU �� kmem ��

  // ��ҳ����ӵ���ǰ CPU �Ŀ����б���
  r->next = kmem[cpu].freelist;
  kmem[cpu].freelist = r;

  release(&kmem[cpu].lock);  // �ͷ���
  pop_off();  // �ָ��ж�
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  int cpu = cpuid();

  push_off();  // �ر��жϣ���ֹ��������
  acquire(&kmem[cpu].lock);  // ��ȡ��ǰ CPU �� kmem ��

  r = kmem[cpu].freelist;  // ���Դӵ�ǰ CPU �Ŀ����б��л�ȡҳ��
  if(r)
    kmem[cpu].freelist = r->next;  // ����ɹ���ȡҳ�棬�������б�ָ���Ƶ���һ��ҳ��

  release(&kmem[cpu].lock);  // �ͷŵ�ǰ CPU ����

  if(!r) {  // �����ǰ CPU �Ŀ����б�Ϊ�գ����Դ����� CPU �Ŀ����б��л�ȡҳ��
    for(int i = 0; i < NCPU; i++) {
      if(i == cpu)  // ������ǰ CPU
        continue;
      
      acquire(&kmem[i].lock);  // ��ȡ���� CPU �� kmem ��
      r = kmem[i].freelist;  // ���Դ����� CPU �Ŀ����б��л�ȡҳ��
      if(r) {
        kmem[i].freelist = r->next;  // ����ɹ���ȡҳ�棬�������б�ָ���Ƶ���һ��ҳ��
        release(&kmem[i].lock);  // �ͷ����� CPU ����
        break;
      }
      release(&kmem[i].lock);  // ���û�л�ȡ��ҳ�棬����������һ�� CPU
    }
  }

  pop_off();  // �ָ��ж�

  if(r)
    memset((char*)r, 5, PGSIZE);  // ���ҳ�������԰�������
  return (void*)r;
}


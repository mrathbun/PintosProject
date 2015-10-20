#include <stdio.h>
#include <string.h>
#include "threads/thread.h"

#define f 16384
#define LA59 16110
#define LA1 273

/*Implementations*/

static int ntof(int n) {
  return n*f;
}

static int ftonR0(int a) {
  return a/f;
}

static int ftonRN(int a) {
  if(a>=0) {
    a = a+(f/2);
    return a/f;
  }
  else {
    a = a-(f/2);
    return a/f;
  }
}

static int faddf(int a, int b) {
  return a+b;
}

static int fsubf(int a, int b) {
  return a-b;
}

static int faddn(int a, int n) {
  return a+(n*f);
}

static int fsubn(int a, int n) {
  return a-(n*f);
}

static int nsubf(int a, int n) {
  return (n*f)-a;
}

static int fmulf(int a, int b) {
  return (((int64_t)a)*(b/f));
}

static int fmuln(int a, int n) {
  return a*n;
}

static int fdivf(int a, int b) {
  return (((int64_t)a)*(f/b));
}

static int fdivn(int a, int n) {
  return a/n;
}

static int cPriority(struct thread *t) {
  int t1,t2,t3,r;
  t1 = ntof(PRI_MAX);
  t2 = fdivn(t->recent_cpu,4);
  t3 = (t->nice)*2;
  t3 = ntof(t3);
  r = fsubf(t1,t2);
  r = fsubf(r,t3);
  r = ftonR0(r);
  if(r < 0) return 0;
  else if(r > 63) return 63;
  else return r; 
}

static int cRecent_Cpu_Final(struct thread *t) {
  int r;
  r = t->recent_cpu; 
  r = fmuln(r,100);
  return ftonRN(r);
}

static int cRecent_Cpu_Fixed(struct thread *t) {
  int t1,t2,r;
  t1 = fmuln(thread_get_load_avg_fixed(),2);
  t2 = faddn(t1,1);
  r = fdivf(t1,t2);
  r = fmulf(r,t->recent_cpu);
  return faddn(r,t->nice);
}

static int cLoad_Avg_Final() {
  int t2;
  t2 = thread_get_load_avg_fixed(); 
  t2 = fmuln(t2,100);
  return ftonRN(t2);

}

static int cLoad_Avg_Fixed() {
  int t1,t2;
  t1 = fmulf(thread_get_load_avg_fixed(),LA59);
  t2 = fmuln(LA1,thread_get_ready_threads);
  return faddf(t1,t2);
}

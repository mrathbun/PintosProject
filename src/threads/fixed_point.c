#include <stdio.h>
#include <string.h>
#include "threads/thread.h"

#define f 16384
#define LA59 16111
#define LA1 273

/* Set of functions to handle fixed-point computations.
   The format is 17.14. This is needed to represent recent_cpu
   and load_avg. There are also the functions to calculate
   recent_cpu, load_avg, and priority. */

/* converts normal number to fixed-point number*/
static int ntof(int n) {
  return n*f;
}

/* converts fixed-point number to normal number and rounds down */
static int ftonR0(int a) {
  return a/f;
}

/* converts fixed-point number to normal number and rounds to nearest */
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

/* adds to fixed-point numbers */
static int faddf(int a, int b) {
  return a+b;
}

/* subtracts a fixed-point number from a fixed-point number*/
static int fsubf(int a, int b) {
  return a-b;
}

/* adds a fixed-point number and a normal number */
static int faddn(int a, int n) {
  return a+(n*f);
}

/* subtracts a normal number from a fixed-point number */
static int fsubn(int a, int n) {
  return a-(n*f);
}

/* subtracts a fixed-point number from a normal number */
static int nsubf(int a, int n) {
  return (n*f)-a;
}

/* multiplies 2 fixed-point numbers */
static int fmulf(int a, int b) {
  return (((int64_t)a)* b/f);
}

/* multiplies a fixed-point number and normal number */
static int fmuln(int a, int n) {
  return a*n;
}

/* divides a fixed-point number by a fixed-point number */
static int fdivf(int a, int b) {
  return (((int64_t)a)*f/b);
}

/* divides a fixed-point number by a normal number */
static int fdivn(int a, int n) {
  return a/n;
}

/* calculates priority and returns it in a normal number */
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

/* converts recent_cpu from fixed-point to normal number*100 */
static int cRecent_Cpu_Final(struct thread *t) {
  int r;
  r = t->recent_cpu; 
  r = fmuln(r,100);
  return ftonRN(r);
}

/* calculates recent_cpu and returns it in fixed-point form */
static int cRecent_Cpu_Fixed(struct thread *t) {
  int t1,t2,r;
  t1 = fmuln(thread_get_load_avg_fixed(),2);
  t2 = faddn(t1,1);
  r = fdivf(t1,t2);
  r = fmulf(r,t->recent_cpu);
  return faddn(r,t->nice);
}

/* converts load_avg from fixed-point to normal number*100 */
static int cLoad_Avg_Final() {
  int t2;
  t2 = thread_get_load_avg_fixed(); 
  t2 = fmuln(t2,100);
  return ftonRN(t2);
}

/* calculates load_avg and returns it in fixed-point form */
static int cLoad_Avg_Fixed() {
  int t1,t2;
  t1 = fmulf(thread_get_load_avg_fixed(),LA59);
  t2 = fmuln(LA1,thread_get_ready_threads());
  return faddf(t1,t2);
}

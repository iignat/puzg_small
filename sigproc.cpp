#include "sigproc.h"


void smoothingfunc(unsigned short *arr, const unsigned short len) {
  for (unsigned short i=1;i<len-1;i++) {
      arr[i-1]=arr[i-1]/4 + arr[i]/2+arr[i+1]/4;
  }
}

int comp (const unsigned short *i, const unsigned short *k) {
  return (int)(*i-*k);
}

unsigned short findAfunc(unsigned short * arr,const unsigned short len){
  qsort (arr,len,sizeof(unsigned short),(int (*)(const void *, const void *))comp );
  return arr[0];
}
unsigned short findWfunc(unsigned short * arr,const unsigned short len){
  unsigned short n=0;
  for (unsigned short i=1;i<len-1;i++) {
    if(arr[i-1]<arr[i] && arr[i]>=arr[i+1])n++;
  }
  return n;
}

float det(float a11,float a12,float a13,float a21,float a22,float a23,float a31,float a32,float a33){
  return a11*a22*a33+a21*a32*a13+a12*a23*a31-a13*a22*a31-a21*a12*a33-a11*a32*a23;
}

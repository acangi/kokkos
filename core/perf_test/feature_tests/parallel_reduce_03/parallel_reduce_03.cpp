#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/time.h>
#include <Kokkos_Core.hpp>

// Define test function to be called in Kokkos::parallel_reduce. It returns 0.
struct testfunc {
  typedef int value_type;

  KOKKOS_INLINE_FUNCTION
  void operator() (const int i, int &tempresult) const {
    tempresult += 0;
  }
};

int main (int argc, char* argv[]) {

  int n = 10;        // size of problem, default value.
  int nrepeat = 100; // number of repeats, default value.
  
  // Read arguments from command line
  for(int i=0; i<argc; i++) {
    if(strcmp(argv[i], "-n") == 0) {
      n = atoi(argv[++i]);
      printf("  User n is %d\n", n);
     }
    else if(strcmp(argv[i], "--nrepeat") == 0) {
      nrepeat = atoi(argv[++i]);
      printf("  User nrepeat is %d\n", nrepeat);
     }
    else if( (strcmp(argv[i], "-h") == 0) ||  (strcmp(argv[i], "--help") == 0) ) {
      printf("  Options:\n");
      printf("  -n <int>:              problem size (default: 100)\n");
      printf("  --nrepeat <int>:       number of repeats (default: 100)\n");
      printf("  --help (-h):           print this message\n");
      exit(1); 
    }
  }

  //checkinput(n,nrepeat);

  Kokkos::initialize(argc,argv);

  //----------------------------------------
  // Choose execution space and memory space.
  // Choose execution space.
  typedef Kokkos::Serial    ExecSpace;
  //typedef Kokkos::OpenMP    ExecSpace;
  //typedef Kokkos::Cuda      ExecSpace;
  
  // Choose memory space.
  typedef Kokkos::HostSpace MemSpace;
  //typedef Kokkos::OpenMP    MemSpace;
  //typedef Kokkos::CudaSpace MemSpace;

  // Choose memory layout.
  typedef Kokkos::LayoutRight Layout;

  // Choose range policy.
  typedef Kokkos::RangePolicy<ExecSpace> range_policy;
  //----------------------------------------

  // Timer.
  struct timeval begin,end;

  Kokkos::fence();

  gettimeofday(&begin,NULL);

  int result = 0;
  for(int i=0; i<nrepeat; i++) {
    // Kokkos:parallel_reduce, size n.
    Kokkos::parallel_reduce(range_policy(0,n), testfunc(), result);
  }

  Kokkos::fence();

  Kokkos::finalize();

  gettimeofday(&end,NULL);

  // Calculate time.
  double time = 1.0*(end.tv_sec-begin.tv_sec) +
                1.0e-6*(end.tv_usec-begin.tv_usec);
    
  // Print problem size and time.
  printf("  n( %d ) nrepeat( %d ) time( %g s ) rate( %g 1/s )\n",
         n, nrepeat, time, 1.0*nrepeat/time);

  // Print result.
  printf("  Result is: %d\n", result);
  if( result != 0 ) {
    printf("  Error: result is incorrect!\n");
  }

  return 0;
}

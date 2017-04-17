#include <Kokkos_Core.hpp>
#include <cstdio>

// Declare checkinput.
void checkinput(int &n, int &nrepeat);

// Define test function to be called in Kokkos::parallel_reduce. 
// Simple sum_{i=0}^{n-1}=0.5*n*(n-1).
struct testfunc {
  typedef int value_type;

  KOKKOS_INLINE_FUNCTION
  void operator() (const int i, int &tsum) const {
    tsum += i;
  }
};

int main (int argc, char* argv[]) {
  
  int n = 10;        // Default value.  
  int nrepeat = 100; // Default value.

  for(int i=0; i<argc; i++) {
    if( strcmp(argv[i], "-n") == 0) {
      n = atoi(argv[++i]);
      printf("  User n is %d\n",n);
    } 
    else if( strcmp(argv[i], "--nrepeat") == 0) {
      nrepeat = atoi(argv[++i]);
      printf("  User nrepeat is %d\n",nrepeat);
    } 
    else if( (strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "-help") == 0) ) {
      printf("   Options:\n");
      printf("  -n <int>:              problem size (default: 10)\n");
      printf("  --nrepeat <int>:       number of repeats (default: 100)\n");
      printf("  --help (-h):           print this message\n");
      exit(1); 
    }
  }
 
  checkinput(n,nrepeat);

  Kokkos::initialize (argc, argv);

  // Timer.
  struct timeval begin,end;

  //Kokkos::fence();


  gettimeofday(&begin,NULL);

  int result = 0;
  for(int i=0; i<nrepeat; i++) {
    // Kokkos:parallel_reduce, size n.
    Kokkos::parallel_reduce(n, testfunc(), result);
    //printf("Kokkos result for n=%i is: %d\n", n-1, result);
  }

  //Kokkos::fence();

  gettimeofday(&end,NULL);

  // Calculate time.
  double time = 1.0*(end.tv_sec-begin.tv_sec) +
                1.0e-6*(end.tv_usec-begin.tv_usec);
  
  // Print result. 
  int seqsum = 0;
  for(int i=0; i<n; i++) {
    seqsum += i;
  }
  printf("Sum of integers from 0 to %d\n" 
         "computed with parallel_reduce: %d\n"
         "computed sequentially        : %d\n"
         "computed analytically        : %g\n", n-1, result, seqsum, 0.5*n*(n-1));

  // Print problem size and timing.
  printf("n( %d ) nrepeat( %d ) time( %g s ) rate( %g 1/s )\n",
         n, nrepeat, time, 1.0*nrepeat/time);

  Kokkos::finalize();

  return 0;
}

void checkinput(int &n, int &nrepeat) {
  if ( n == -1 ) {
    n = 10;  // Set n to default value.
  }
}

# Lightning Grep (gg)
Lightning grep is yet another implementation of grep. The main differentiator of lightning grep is the search is implemented using a very native algorithm, but accelerated using AVX or AVX2 instruction. The resulting implementation performs better than grep and ag for most cases.

To benchmark between grep, ag and gg, refer to the benchmark script. Current benchmark result :
```sh
richard@richard-Aspire-S7-392:~/dev/lightning-grep$ ./benchmark.sh 
Single file profile result:
grep	0.316483950	1.000
ag	0.878796627	1.001
gg	0.180695135	1.000
Multifile file profile result:
grep	Time: 0.748276342	CPUs Used: 0.995	Normalized:  0.74453496029
ag	Time: 0.624317881	CPUs Used: 2.710	Normalized:  1.69190145751
gg	Time: 0.152102082	CPUs Used: 3.785	Normalized:  0.57570638037
```
## Limitation
- Linux only for now. 
- No regex support.

## Key Observations
- mmap() doesn't scale very well in multithread application. Linux kernel needs to perform internal tracking when application calls mmap, and that seems to be bottleneck the application. 
- The hardware prefetcher slows down dramatically for non-serialized memory read. My initial attempt of parallelized Rabin-Karp failed because of this. 
- Fetching from SSD is really slow, the grep algorithm doesn't matter at this point. The choice of application only matters once the disk cache is warm. 

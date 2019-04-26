This minimal boost install was put together using bcp and copying over built libs.

General steps:

1. Download and extract boost
2. Run bootstrap
3. Build bcp: bjam tools/bcp
4. Use bcp to copy only required files: dist/bin/bcp stacktrace.hpp C:\boost_minimal\boost
5. Copy LICENSE_1_0.txt and README.md to C:\boost_minimal


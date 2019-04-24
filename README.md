# DFD_on_FPGA
The hardware acceleration design of DFD algorithm developed by Vivado HLS 2017.4

# How to use the code
1. Create a working space in HLS
2. Add DFD.cpp/DFD.h to Source and Test.cpp to test bench.
3. Run C simulation and High Level Synthesis in HLS
4. The result will be saved in result.txt
5. Open Matlab to load the .txt file and use these two command lines
  ```
  $ result_img = reshape(result, 640,480);
  $ mesh(result_img);
  ```
# About the author
Rulin Chen, a undergraduate student in EE, conducted by Alex Noel Josephraj, professor in Shantou University, Guangdong Province, China. 

# Hardware-accelerator-for-genome-sequence-alignment
This is test file for WGA
Author : Yi-Chun Hsu
Latest edition : 2024/6/14

for cpp test files
    $ make
    $ ./bin/wga_final <input file> <output file> <bandwidth of BSW> <parameter of GACT-X>

    For the visualized DP matrix, please refer to the file:
    ./cpp/src/genome_alignment_visualize.cpp

for verilog test files
  RTL simulation:
  	
  	GACT:
  	vcs tb.v GACT.v -full64 -R -v2005 -debug_access+all +v2k +define+FSDB+GACT
  
  	GACT_X:
  	vcs tb.v GACT_X.v -full64 -R -v2005 -debug_access+all +v2k +define+FSDB+GACT_X
  
  Gate level simulation:
  	
  	GACT:
  	vcs tb.v GACT_syn.v tsmc13.v -full64 -R +neg_tchk -debug_access+all +v2k +define+FSDB+GACT+SYN
  
  	GACT_X:
  	vcs tb.v GACT_X_syn.v tsmc13.v -full64 -R +neg_tchk -debug_access+all +v2k +define+FSDB+GACT_X+SYN

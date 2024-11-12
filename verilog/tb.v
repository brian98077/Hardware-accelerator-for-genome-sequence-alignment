`timescale 1ns / 1ps
`define CYCLE 10.0
`define END_CYCLE 50000
`define LENGTH 128
`define NUMBER 100
`define SCORE_BITS 12

`define R "pattern/R.dat"
`define Q "pattern/Q.dat"

`ifdef GACT
    `define GOLDEN "pattern/golden_GACT.dat"
    `define SDFFILE "GACT_syn.sdf"
`endif

`ifdef GACT_X
    `define GOLDEN "pattern/golden_GACT_X.dat"
    `define SDFFILE "GACT_X_syn.sdf"
`endif

module tb;

    reg clk = 1;
    always #(`CYCLE/2) clk = ~clk;

    reg reset;
    reg [`LENGTH*2-1:0] input_R [0:`NUMBER-1];
    reg [`LENGTH*2-1:0] input_Q [0:`NUMBER-1];
    wire [`SCORE_BITS-1:0] score;
    reg input_valid;
    wire done;
    reg [`SCORE_BITS-1:0] golden [0:`NUMBER-1];

    reg [`LENGTH*2-1:0] data_R;
    reg [`LENGTH*2-1:0] data_Q;

    initial begin
        $readmemb(`R, input_R);
        $readmemb(`Q, input_Q);
        $readmemh(`GOLDEN, golden);
    end

    GACT gact(.clk(clk), .reset(reset), .input_valid(input_valid), .input_R(data_R), .input_Q(data_Q),
        .done(done), .score(score) ); 


    initial begin
        `ifdef SYN
            $sdf_annotate(`SDFFILE, gact);
        `endif
        `ifdef FSDB
            `ifdef SYN
                $fsdbDumpfile("GACT_syn.fsdb");
            `elsif GACT
                $fsdbDumpfile("GACT.fsdb");
            `elsif GACT_X
                $fsdbDumpfile("GACT_X.fsdb");
            `endif
            $fsdbDumpvars(0, tb, "+mda");
        `endif
    end

    integer j;
    initial begin
        reset = 0;
        clk = 0;
        input_valid = 0;
        j = 0;

        while(j < `NUMBER) begin
            #(`CYCLE)   begin
                reset = 1;
                data_R = 0;
                data_Q = 0;
            end

            #(`CYCLE*2) reset = 0;

            #(`CYCLE*3);
            @(negedge clk) begin
                input_valid = 1;
                data_R = input_R[j];
                data_Q = input_Q[j];
            end

            #(`CYCLE*3);
            @(negedge clk) begin
                input_valid = 0;
            end
            
            #(`CYCLE*`LENGTH*2);
            j = j + 1;
        end
    end
    
    initial begin 
        #(`CYCLE*`END_CYCLE);
        $display("\n\033[0;31m================================================================\n");
        $display("====================  Time limit exceeded  =====================\n");
        $display("================================================================\033[0m\n");
        $finish;
    end

    integer i, error, pass_num;
    initial begin
        i = 0;
        error = 0;
        pass_num = 0;

        while(i < `NUMBER) begin
            repeat(9 + `LENGTH*2) @(negedge clk);
            if(done) begin
                if(golden[i] == score) pass_num = pass_num + 1;
                else begin
                    $display("pattern %3d error, golden: %3d, output: %3d", i+1, golden[i], score);
                    error = error + 1;
                end
            end

            i = i + 1;
        end
    end

    always@ (negedge clk) begin
        if(error + pass_num == `NUMBER && done) begin
            if (pass_num == 100 && error == 0) begin    
                $display("\n\033[0;32m================================================================\n");
                $display("===================== ALL TEST CASES PASS  =====================\n");
                $display("================================================================\033[0m\n");
                $finish;
            end 
            else begin
                $display("\n\033[0;31m================================================================\n");
                $display("============================  FAIL  ============================\n");
                $display("                   total error :%d\n", error);
                $display("================================================================\033[0m\n");
                $finish;
            end
        end
    end

endmodule
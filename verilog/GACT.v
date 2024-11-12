`define R_LEN 128
`define Q_LEN 128
`define SCORE_BITS 12
`define MATCH 2
`define MISMATCH 1
`define GAP 1

module GACT (
    input                         clk,
    input                         reset,
    input [2*`R_LEN-1:0]          input_R,
    input [2*`Q_LEN-1:0]          input_Q,
    input                         input_valid,
    output reg                    done,
    output reg [`SCORE_BITS-1:0]  score
);
    
    // state
    parameter STATE_IDLE  = 0;
    parameter STATE_INPUT = 1;
    parameter STATE_OP    = 2;
    parameter STATE_DONE  = 3;

    reg [1:0] state, next_state;
    reg [2*`R_LEN-1:0] R;
    reg [2*`Q_LEN-1:0] Q;
    reg [$clog2(`Q_LEN)+1:0] counter;
    reg [`SCORE_BITS-1:0] leftup_buffer [`Q_LEN-1:0];
    wire [`SCORE_BITS-1:0] score_buffer [`Q_LEN-1:0];
    wire [1:0] array_R [`R_LEN-1:0];

    // FSM
    always @(*) begin
        case (state)
            STATE_IDLE  : next_state = (input_valid) ? STATE_INPUT : STATE_IDLE;
            STATE_INPUT : next_state = STATE_OP;
            STATE_OP    : next_state = (counter == 2*`Q_LEN) ? STATE_DONE : STATE_OP;
            STATE_DONE  : next_state = STATE_IDLE;
            default: state = STATE_IDLE;
        endcase
    end

    always @(posedge reset or posedge clk) begin
        if(reset) state <= STATE_IDLE;
        else state <= next_state;
    end

    // input
    always @(posedge clk) begin
        if(state == STATE_IDLE && next_state == STATE_INPUT) begin
            Q <= input_Q;
            R <= input_R;
        end
        else if(state == STATE_OP && counter <= `R_LEN - 1) begin
            Q <= Q;
            R <= R >> 2;
        end
        else begin
            Q <= Q;
            R <= R;
        end
    end
    
    // counter
    always @(posedge reset or posedge clk) begin
        if(reset) counter <= 0;
        else if(state == STATE_OP) counter <= counter + 1;
        else counter <= counter;
    end

    // score
    always @(posedge reset or posedge clk) begin
        if(reset) score <= 0;
        else score <= score_buffer[`Q_LEN-1];
    end

    // done
    always @(posedge reset or posedge clk) begin
        if(reset) done <= 0;
        else if(state == STATE_OP && next_state == STATE_DONE) done <= 1;
        else done <= 0;
    end

    // PE array
    genvar i;
    generate
        for(i=0;i<`Q_LEN;i=i+1) begin : PE_array
            if(i==0) begin
                PE pe(.clk(clk), .reset(reset), .input_valid(state == STATE_OP && counter < `Q_LEN), .input_R(R[1:0]), .input_Q(Q[i*2+1:i*2]), 
                      .input_leftup(0), .input_left(0), .input_up(0), .output_R(array_R[0]), .output_score(score_buffer[0]));
            end
            else begin
                PE pe(.clk(clk), .reset(reset), .input_valid(counter >= i && state == STATE_OP && counter < `Q_LEN + i), .input_R(array_R[i-1]), .input_Q(Q[i*2+1:i*2]), 
                      .input_leftup(leftup_buffer[i-1]), .input_left(score_buffer[i]), .input_up(score_buffer[i-1]), .output_R(array_R[i]), .output_score(score_buffer[i]));
            end
        end
    endgenerate

    for (i=0;i<`Q_LEN;i=i+1) begin
        always @(posedge reset or posedge clk) begin
            if(reset) leftup_buffer[i] <= 0;
            else leftup_buffer[i] <= score_buffer[i];
        end   
    end
    

endmodule

module PE (
    input clk,
    input reset,
    input input_valid,
    input  [1:0] input_R,
    input  [1:0] input_Q,
    input  [`SCORE_BITS-1:0] input_leftup,
    input  [`SCORE_BITS-1:0] input_left,
    input  [`SCORE_BITS-1:0] input_up,
    output reg [1:0] output_R,
    output reg [`SCORE_BITS-1:0] output_score
);
    wire [`SCORE_BITS:0] output_temp_leftup = input_leftup - `MISMATCH;
    wire [`SCORE_BITS:0] output_temp_left   = input_left - `GAP;
    wire [`SCORE_BITS:0] output_temp_up     = input_up - `GAP;
    wire [`SCORE_BITS:0] output_leftup = (output_temp_leftup[`SCORE_BITS]) ? 0 : output_temp_leftup;
    wire [`SCORE_BITS:0] output_left   = (output_temp_left[`SCORE_BITS])   ? 0 : output_temp_left;
    wire [`SCORE_BITS:0] output_up     = (output_temp_up[`SCORE_BITS])     ? 0 : output_temp_up;

    always @(posedge reset or posedge clk) begin
        if(reset) begin
            output_score <= 0;
            output_R <= 0;
        end
        else if(input_valid) begin
            if(input_R == input_Q) begin
                output_score <= input_leftup  + `MATCH;
                output_R <= input_R;
            end
            else begin
                if(output_leftup > output_left && output_leftup > output_up) begin
                    output_score <= output_leftup;
                    output_R <= input_R;
                end
                else if(output_left > output_up) begin
                    output_score <= output_left;
                    output_R <= input_R;
                end
                else begin
                    output_score <= output_up;
                    output_R <= input_R;
                end
            end
        end
        else begin
            output_score <= output_score;
            output_R <= input_R;
        end
    end
endmodule
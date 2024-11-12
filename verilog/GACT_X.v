`define R_LEN 128
`define Q_LEN 128
`define SCORE_BITS 12
`define MATCH 2
`define MISMATCH 1
`define GAP 1
`define PARAMETER_Y 5

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
    reg  signed [`SCORE_BITS+1:0] leftup_buffer [`Q_LEN-1:0];
    wire signed [`SCORE_BITS+1:0] score_buffer  [`Q_LEN-1:0];
    wire signed [`SCORE_BITS+1:0] current_max   [`Q_LEN-1:0];
    wire max_change   [`Q_LEN-1:0];
    wire stop_flag  [`Q_LEN-1:0];
    wire valid_flag [`Q_LEN-1:0];
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
                      .stop_flag(0), .valid_flag(0), .input_leftup(0), .input_left(0), .input_up(0), .max(0) , .max_change(0) ,.output_R(array_R[0]), .output_score(score_buffer[0]),
                      .output_stop_flag(stop_flag[0]), .output_valid_flag(valid_flag[0]), .output_max(current_max[0]), .output_max_change(max_change[0]));
            end
            else begin
                PE pe(.clk(clk), .reset(reset), .input_valid(counter >= i && state == STATE_OP && counter < `Q_LEN + i), .input_R(array_R[i-1]), .input_Q(Q[i*2+1:i*2]),
                      .stop_flag(stop_flag[i]), .valid_flag(valid_flag[i]), .input_leftup(leftup_buffer[i-1]), .input_left(score_buffer[i]), .input_up(score_buffer[i-1]),
                      .max((current_max[i] > current_max[i-1]) ? current_max[i] : current_max[i-1]), .max_change((max_change[i] || (current_max[i-1] > current_max[i])) ? 1'b1 : 1'b0),
                      .output_R(array_R[i]), .output_score(score_buffer[i]), .output_stop_flag(stop_flag[i]), .output_valid_flag(valid_flag[i]), .output_max(current_max[i]), .output_max_change(max_change[i]));
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
    input  stop_flag,
    input  valid_flag,
    input  signed [`SCORE_BITS+1:0] input_leftup,
    input  signed [`SCORE_BITS+1:0] input_left,
    input  signed [`SCORE_BITS+1:0] input_up,
    input  signed [`SCORE_BITS+1:0] max,
    input  max_change,
    output reg [1:0] output_R,
    output reg signed [`SCORE_BITS+1:0] output_score,
    output reg output_stop_flag,
    output reg output_valid_flag,
    output reg signed [`SCORE_BITS+1:0] output_max,
    output reg output_max_change
);
    wire signed [`SCORE_BITS+1:0] output_leftup = input_leftup - `MISMATCH;
    wire signed [`SCORE_BITS+1:0] output_left   = input_left - `GAP;
    wire signed [`SCORE_BITS+1:0] output_up     = input_up - `GAP;

    always @(posedge reset or posedge clk) begin
        if(reset) begin
            output_score <= 0;
            output_R <= 0;
            output_stop_flag <= 0;
            output_valid_flag <= 0;
            output_max <= 0;
        end
        else if(input_valid) begin
            if(stop_flag && valid_flag) begin
                output_score <= 0;
                output_R <= input_R;
                output_stop_flag <= stop_flag;
                output_valid_flag <= valid_flag;
                output_max <= max;
                output_max_change <= 0;
            end
            else if(valid_flag) begin
                if(input_R == input_Q) begin
                    output_score <= (input_leftup  + `MATCH >= max - `PARAMETER_Y || max_change) ? input_leftup  + `MATCH : 0;
                    output_R <= input_R;
                    output_stop_flag <= (input_leftup  + `MATCH >= max - `PARAMETER_Y || max_change) ? 0 : 1;
                    output_valid_flag <= (max_change) ? 0 : valid_flag;
                    output_max <= (input_leftup  + `MATCH > max) ? input_leftup  + `MATCH : max;
                    output_max_change <= (input_leftup  + `MATCH > max) ? 1 : 0;
                end
                else begin
                    if(output_leftup > output_left && output_leftup > output_up) begin
                        output_score <= (output_leftup >= max - `PARAMETER_Y || max_change) ? output_leftup : 0;
                        output_R <= input_R;
                        output_stop_flag <= (output_leftup >= max - `PARAMETER_Y || max_change) ? 0 : 1;
                        output_valid_flag <= (max_change) ? 0 : valid_flag;
                        output_max <= (output_leftup > max) ? output_leftup : max;
                        output_max_change <= (output_leftup > max) ? 1 : 0;
                    end
                    else if(output_left > output_up) begin
                        output_score <= (output_left >= max - `PARAMETER_Y || max_change) ? output_left : 0;
                        output_R <= input_R;
                        output_stop_flag <= (output_left >= max - `PARAMETER_Y || max_change) ? 0 : 1;
                        output_valid_flag <= (max_change) ? 0 : valid_flag;
                        output_max <= (output_left > max) ? output_left : max;
                        output_max_change <= (output_left > max) ? 1 : 0;
                    end
                    else begin
                        output_score <= (output_up >= max - `PARAMETER_Y || max_change) ? output_up : 0;
                        output_R <= input_R;
                        output_stop_flag <= (output_up >= max - `PARAMETER_Y || max_change) ? 0 : 1;
                        output_valid_flag <= (max_change) ? 0 : valid_flag;
                        output_max <= (output_up > max) ? output_up : max;
                        output_max_change <= (output_up > max) ? 1 : 0;
                    end
                end
            end
            else begin
                if(input_R == input_Q) begin
                    output_score <= input_leftup  + `MATCH;
                    output_R <= input_R;
                    output_stop_flag <= stop_flag;
                    output_valid_flag <= (input_leftup  + `MATCH >= max - `PARAMETER_Y && !(input_leftup  + `MATCH > max)) ? 1 : 0;
                    output_max <= (input_leftup  + `MATCH > max) ? input_leftup  + `MATCH : max;
                    output_max_change <= (output_leftup > max) ? 1 : 0;
                end
                else begin
                    if(output_leftup > output_left && output_leftup > output_up) begin
                        output_score <= (output_leftup >= max - `PARAMETER_Y) ? output_leftup : 0;
                        output_R <= input_R;
                        output_stop_flag <= stop_flag;
                        output_valid_flag <= (output_leftup >= max - `PARAMETER_Y && !(output_leftup > max)) ? 1 : 0;
                        output_max <= (output_leftup > max) ? output_leftup : max;
                        output_max_change <= (output_leftup > max) ? 1 : 0;
                    end
                    else if(output_left > output_up) begin
                        output_score <= (output_left >= max - `PARAMETER_Y) ? output_left : 0;
                        output_R <= input_R;
                        output_stop_flag <= stop_flag;
                        output_valid_flag <= (output_left >= max - `PARAMETER_Y && !(output_left > max)) ? 1 : 0;
                        output_max <= (output_left > max) ? output_left : max;
                        output_max_change <= (output_left > max) ? 1 : 0;
                    end
                    else begin
                        output_score <= (output_up >= max - `PARAMETER_Y) ? output_up : 0;
                        output_R <= input_R;
                        output_stop_flag <= stop_flag;
                        output_valid_flag <= (output_up >= max - `PARAMETER_Y && !(output_up> max)) ? 1 : 0;
                        output_max <= (output_up > max) ? output_up : max;
                        output_max_change <= (output_up > max) ? 1 : 0;
                    end
                end
            end   
        end
        else begin
            output_score <= output_score;
            output_R <= input_R;
            output_stop_flag <= stop_flag;
            output_valid_flag <= valid_flag;
            output_max <= max;
            output_max_change <= max_change;
        end
    end
endmodule
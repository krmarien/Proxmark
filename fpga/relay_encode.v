module relay_encode(
    clk,
    reset,
    data_in,
    data_out
);
    input clk, reset, data_in;
    output data_out;

    reg [0:0] data_out = 1'b0;

    reg [3:0] buffer_in = 4'b0;
    reg [6:0] data_out_counter = 7'b0;

    reg [3:0] counter = 4'b0;

    always @(posedge clk)
    begin
        // encode
        counter = counter + 1;

        if (counter[3:0] == 4'b0)
            buffer_in = {buffer_in[2:0], data_in};

        if (|data_out_counter[6:0] == 1'b1)
            data_out_counter = data_out_counter - 1;

        if (buffer_in == 4'hc || buffer_in == 4'hf)
        begin
            data_out = 1'b1;
            buffer_in = 4'b0;
            data_out_counter = 7'b1000000;
        end

        if (data_out_counter[6:0] == 7'b0)
            data_out = 1'b0;

        // reset
        if (reset == 1'b1)
        begin
            buffer_in = 4'b0;
            data_out = 1'b0;
        end
    end

endmodule
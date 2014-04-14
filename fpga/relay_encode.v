module relay_encode(
    clk,
    reset,
    data_in,
    data_out
);
    input clk, reset, data_in;
    output data_out;

    reg [0:0] data_out = 1'b0;

    reg [31:0] buffer_in = 32'b0;
    reg [5:0] data_out_counter = 6'b0;

    always @(posedge clk)
    begin
        // encode
        buffer_in = {buffer_in[30:0], data_in};

        if (|data_out_counter[5:0] == 1'b1)
            data_out_counter = data_out_counter - 1;

        if (buffer_in == 32'hffff0000 || buffer_in == 32'hffffffff)
        begin
            data_out = 1'b1;
            buffer_in = 32'b0;
            data_out_counter = 6'b100000;
        end

        if (data_out_counter[5:0] == 5'b0)
            data_out = 1'b0;

        // reset
        if (reset == 1'b1)
        begin
            buffer_in = 32'b0;
            data_out = 1'b0;
        end
    end

endmodule
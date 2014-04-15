module relay_decode(
    clk,
    reset,
    mode,
    data_in,
    data_out
);
    input clk, reset, mode, data_in;
    output data_out;

    reg [0:0] data_out = 1'b0;

    reg [6:0] one_counter = 7'b0;
    reg [6:0] zero_counter = 7'b0;

    reg [0:0] receiving = 1'b0;

    reg [6:0] data_out_counter = 7'b0;

    always @(posedge clk)
    begin
        // remove wrong bits
        receiving = receiving | data_in == 1'b1;

        one_counter = one_counter + (data_in == 1'b1 & receiving);
        zero_counter = zero_counter + (data_in == 1'b0 & receiving);

        if (|data_out_counter[6:0] == 1'b1)
            data_out_counter = data_out_counter - 1;

        if (one_counter + zero_counter == 7'd64)
        begin
            data_out = (one_counter > zero_counter);

            data_out_counter = (mode ? 7'b0100000 : 7'b1000000);

            one_counter = 7'b0;
            zero_counter = 7'b0;
        end

        if (data_out_counter[6:0] == 7'b0)
            data_out = 1'b0;

        // reset
        if (reset == 1'b1)
        begin
            one_counter = 7'b0;
            zero_counter = 7'b0;
            receiving = 1'b0;
        end
    end

endmodule
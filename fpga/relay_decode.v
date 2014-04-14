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

    reg [5:0] one_counter = 6'b0;
    reg [5:0] zero_counter = 6'b0;

    reg [0:0] receiving = 1'b0;
    reg [0:0] encoded = 1'b0;

    reg [5:0] data_out_counter = 6'b0;
    reg [5:0] wait_counter = 6'b0;

    always @(posedge clk)
    begin
        // remove wrong bits
        receiving = receiving | data_in == 1'b1;

        one_counter = one_counter + (data_in == 1'b1 & receiving);
        zero_counter = zero_counter + (data_in == 1'b0 & receiving);

        if (one_counter + zero_counter == 6'd32)
        begin
            encoded = (one_counter > zero_counter);

            one_counter = 6'b0;
            zero_counter = 6'b0;
        end

        // decode
        if (|wait_counter[5:0] == 1'b1)
            wait_counter = wait_counter - 1;

        if (|data_out_counter[5:0] == 1'b1)
            data_out_counter = data_out_counter - 1;

        if (encoded == 1'b1 && wait_counter[5:0] == 5'b0)
        begin
            wait_counter = 6'b100000;
            data_out_counter = (mode ? 6'b010000 : 6'b100000);
            data_out = 1'b1;
        end

        if (data_out_counter[5:0] == 5'b0)
            data_out = 1'b0;

        // reset
        if (reset == 1'b1)
        begin
            one_counter = 6'b0;
            zero_counter = 6'b0;
            encoded = 1'b0;
            receiving = 1'b0;
        end
    end

endmodule
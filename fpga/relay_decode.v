module relay_decode(
    clk,
    reset,
    mode,
    data_in,
    data_out,
    data_available
);
    input clk, reset, mode, data_in;
    output[3:0] data_out;
    output[0:0] data_available;

    reg [3:0] data_out = 4'b0;
    reg [0:0] data_available = 1'b0;

    reg [6:0] one_counter = 7'b0;
    reg [6:0] zero_counter = 7'b0;

    reg [0:0] receiving = 1'b0;

    always @(posedge clk)
    begin
        // remove wrong bits
        receiving = receiving | data_in == 1'b1;

        one_counter = one_counter + (data_in == 1'b1 & receiving);
        zero_counter = zero_counter + (data_in == 1'b0 & receiving);

        if (one_counter + zero_counter == 7'd64)
        begin
            if (one_counter > zero_counter)
                data_out = (mode == 1'b1 ? 4'hc : 4'hf);
            else
                data_out = 4'b0;

            data_available = 1'b1;

            one_counter = 7'b0;
            zero_counter = 7'b0;
        end
        else
        begin
            data_out = 4'b0;
            data_available = 1'b0;
        end

        // reset
        if (reset == 1'b1)
        begin
            one_counter = 7'b0;
            zero_counter = 7'b0;
            receiving = 1'b0;
            data_out = 4'ha;
            data_available = 1'b0;
        end
    end

endmodule
module relay_receive(
    clk,
    reset,
    data_in,
    data_out
);
    input clk, reset, data_in;
    output [3:0] data_out;

    reg [3:0] data_out = 4'b0;

    reg [5:0] one_counter = 6'b0;
    reg [5:0] zero_counter = 6'b0;

    reg [0:0] receiving = 1'b0;

    always @(posedge clk)
    begin
        receiving = receiving | data_in == 1'b1;

        one_counter = one_counter + (data_in == 1'b1 & receiving);
        zero_counter = zero_counter + (data_in == 1'b0 & receiving);

        if (one_counter + zero_counter == 6'd32)
        begin
            if (one_counter > zero_counter)
                data_out = 4'hff;
            else
                data_out = 4'b0;

            one_counter = 6'b0;
            zero_counter = 6'b0;
        end

        if (reset == 1'b1)
        begin
            one_counter = 6'b0;
            zero_counter = 6'b0;
            data_out = 4'b0;
            receiving = 1'b0;
        end
    end

endmodule
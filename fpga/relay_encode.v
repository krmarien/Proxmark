module relay_encode(
    clk,
    reset,
    mode,
    data_in,
    data_out
);
    input clk, reset, mode, data_in;
    output data_out;

    reg [0:0] data_out = 1'b0;

    reg [7:0] buffer_in = 4'b0;
    reg [7:0] data_out_counter = 8'b0;
    reg [6:0] data_out_delay_counter = 7'b0;

    reg [2:0] bit_counter = 3'b0;
    reg [0:0] comm_active = 1'b0;
    reg [0:0] received_zero = 1'b0;

    reg [3:0] counter = 4'b0;

    always @(posedge clk)
    begin
        // encode
        counter = counter + 1;

        if (counter[3:0] == 4'b0)
        begin
            bit_counter = bit_counter + 1;
            buffer_in = {buffer_in[6:0], data_in};
        end

        if (|data_out_counter == 1'b1)
            data_out_counter = data_out_counter - 1;

        if (|data_out_delay_counter == 1'b1)
            data_out_delay_counter = data_out_delay_counter - 1;

        if (mode == 1'b0 && counter[3:0] == 4'b0) // encode (and demodulate) reader data
        begin
            // start of comm
            if (buffer_in[7:6] == 2'b0 && buffer_in[3:0] == 4'hf && comm_active == 1'b0)
            begin
                bit_counter = 3'b0;
                comm_active = 1'b1;
                received_zero = 1'b0;
            end

            if (comm_active == 1'b1 && bit_counter == 3'b0)
            begin
                // end of comm
                if (buffer_in == 8'hff && received_zero == 1'b1)
                begin
                    comm_active = 1'b0;
                    received_zero = 1'b0;
                end
                // 0xff => 0x00
                if (buffer_in == 8'hff)
                begin
                    data_out = 1'b0;
                    received_zero = 1'b1;
                end
                // 0xXf => 0xc0
                else if (buffer_in[3:0] == 4'hf)
                begin
                    data_out_counter = 8'b1000000;
                    data_out = 1'b1;
                    received_zero = 1'b1;
                end
                // 0xfX => 0x0c
                else if (buffer_in[7:4] == 4'hf)
                begin
                    data_out_delay_counter = 7'b1000000;
                    data_out_counter = 8'b10000000;
                    data_out = 1'b0;
                    received_zero = 1'b0;
                end
            end
        end
        else if ((buffer_in == 8'h0e || buffer_in == 8'h0f) && mode == 1'b1) // encode tag data
        begin
            data_out = 1'b1;
            buffer_in = 8'b0;
            data_out_counter = 8'b1000000;
        end

        if (data_out_counter == 8'b0)
            data_out = 1'b0;

        if (data_out_delay_counter == 7'b0 && |data_out_counter == 1'b1)
            data_out = 1'b1;

        // reset
        if (reset == 1'b1)
        begin
            buffer_in = 8'b0;
            data_out = 1'b0;
        end
    end

endmodule
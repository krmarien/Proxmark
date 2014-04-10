//-----------------------------------------------------------------------------
// ISO14443-A support for the Proxmark III
// Gerhard de Koning Gans, April 2008
//-----------------------------------------------------------------------------

// constants for the different modes:
`define MASTER          3'b000
`define SLAVE           3'b001
`define DELAY           3'b010

module relay(
    pck0, ck_1356meg, ck_1356megb,
    ssp_frame, ssp_din, ssp_dout, ssp_clk,
    data_in, data_out,
    mod_type
);
    input pck0, ck_1356meg, ck_1356megb;
    input ssp_dout;
    output ssp_frame, ssp_din, ssp_clk;
    input data_in;
    output data_out;
    input [2:0] mod_type;

reg ssp_clk = 1'b0;
reg ssp_frame = 1'b0;
reg data_out = 1'b0;
reg ssp_din = 1'b0;


reg [6:0] div_counter = 7'b0;

reg [7:0] buf_data_in = 8'b0;
reg [3:0] buf_data_in_cntr = 4'b0;

reg [0:0] receive_counter = 1'b0;
reg [31:0] delay_counter = 32'h0;
reg [3:0] counter = 4'b0;

reg [7:0] receive_buffer = 8'b0;

reg sending_started = 1'b0;
reg received_complete = 1'b0;
reg [7:0] received = 8'b0;
reg [3:0] send_buf = 4'b0;

reg [16:0] to_arm_delay = 17'b0;

always @(posedge ck_1356meg)
begin
    div_counter <= div_counter + 1;
    buf_data_in = {buf_data_in[6:0], data_in};
    buf_data_in_cntr = buf_data_in[7] + buf_data_in[6] + buf_data_in[5] + buf_data_in[4] + buf_data_in[3] + buf_data_in[2] + buf_data_in[1] + buf_data_in[0];

    if (div_counter[3:0] == 4'b1000) ssp_clk <= 1'b0;
    if (div_counter[3:0] == 4'b0000) ssp_clk <= 1'b1;

    if (sending_started == 1'b1 && received_complete == 1'b0) begin
        delay_counter = delay_counter + 1;
    end

    if (div_counter[2:0] == 3'b100) // 1.695MHz
    begin
        if (mod_type == `MASTER) // Sending from ARM to other Proxmark
        begin
            receive_counter <= receive_counter + 1;

            if (div_counter[6:4] == 3'b000) ssp_frame = 1'b1;
            else ssp_frame = 1'b0;

            if (receive_counter[0] == 1'b0) begin
                data_out = ssp_dout;

                send_buf = {send_buf[2:0], ssp_dout};
                receive_buffer = {receive_buffer[6:0], |buf_data_in_cntr[3:2]};

                if (send_buf == 4'ha && sending_started == 1'b0) begin
                    delay_counter = 32'b0;
                    sending_started = 1'b1;
                end

                if (receive_buffer[3:0] == 4'ha && sending_started == 1'b1) begin
                    receive_buffer = 8'b0;
                    received_complete = 1'b1;
                end
            end

            counter <= 4'b0;
        end
        else if (mod_type == `SLAVE) // Sending from other Proxmark to ARM
        begin
            counter <= counter + 1;

            if (counter[0] == 1'b0) begin
                receive_buffer = {receive_buffer[6:0], |buf_data_in_cntr[3:2]};
                data_out = |buf_data_in_cntr[3:2];

                ssp_frame = (receive_buffer[7:4] == 4'b1111);
                if (receive_buffer[7:4] == 4'b1111) begin
                    received = receive_buffer;
                    receive_buffer = 8'b0;
                end

                ssp_din <= received[7];
                received = {received[6:0], 1'b0};
            end

            receive_counter <= 4'b0;
        end
        else if (mod_type == `DELAY) // Sending delay to ARM
        begin
            if (to_arm_delay[16] == 1'b1) begin
                sending_started = 1'b0;
                received_complete = 1'b0;
                counter <= counter + 1;

                if (counter[0] == 1'b0) begin
                    ssp_frame = (counter[3:0] == 4'b0000);
                    ssp_din <= delay_counter[31];
                    delay_counter = {delay_counter[30:0], 1'b0};
                end

                if (counter[3:0] == 4'b1111) begin
                    to_arm_delay <= 17'b0;
                end
            end
            else begin
                to_arm_delay <= to_arm_delay + 1;
            end
        end
    end
end

endmodule
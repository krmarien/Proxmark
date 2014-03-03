//-----------------------------------------------------------------------------
// ISO14443-A support for the Proxmark III
// Gerhard de Koning Gans, April 2008
//-----------------------------------------------------------------------------

// constants for the different modes:
`define MASTER			3'b000
`define SLAVE       	3'b001
`define DELAY   		3'b010

module relay(
    pck0, ck_1356meg, ck_1356megb,
    adc_d, adc_clk,
    ssp_frame, ssp_din, ssp_dout, ssp_clk, hisn_ssp_clk, hisn_ssp_frame,
    cross_hi, cross_lo,
    data_in, data_out,
    mod_type
);
    input pck0, ck_1356meg, ck_1356megb;
    input [7:0] adc_d;
    output adc_clk;
    input ssp_dout, hisn_ssp_clk, hisn_ssp_frame;
    output ssp_frame, ssp_din, ssp_clk;
    input cross_hi, cross_lo;
    input data_in;
    output data_out;
    input [2:0] mod_type;

reg ssp_clk;
reg ssp_frame;
reg data_out;
reg ssp_din;


reg [2:0] div_counter;
reg clk;

reg buf_data_in;

reg [0:0] receive_counter;
reg [31:0] delay_counter = 32'hDEADBEEF;
reg [3:0] counter;

reg [7:0] receive_buffer;

reg sending_started;
reg received_complete;
reg [7:0] received;

reg [16:0] to_arm_delay;

always @(posedge ck_1356meg)
begin
    div_counter <= div_counter + 1;
    clk = div_counter[2]; // 1,695MHz
    buf_data_in = data_in;
end

always @(posedge clk)
begin
    if (mod_type == `MASTER) begin // Sending from ARM to other Proxmark
        receive_counter <= receive_counter + 1;
        ssp_clk <= hisn_ssp_clk;
        ssp_frame = hisn_ssp_frame;

        if (sending_started == 1'b1 && received_complete == 1'b0) begin
            delay_counter = delay_counter + 1;
        end

        if (receive_counter[0] == 1'b0) begin
            data_out = ssp_dout;

            receive_buffer = {receive_buffer[6:0], buf_data_in};

            if (ssp_dout == 1'b1 && sending_started == 1'b0) begin
                //delay_counter = 32'b0;
                sending_started = 1'b1;
            end

            if (receive_buffer[0] == 1'b1 && sending_started == 1'b1) begin
                receive_buffer = 8'b0;
                received_complete = 1'b1;
            end
        end

        counter <= 4'b0;
    end
    else if (mod_type == `SLAVE) begin // Sending from other Proxmark to ARM
        counter <= counter + 1;
        ssp_clk <= ~ssp_clk;

        if (counter[0] == 1'b0) begin
            receive_buffer = {receive_buffer[6:0], buf_data_in};
            data_out = buf_data_in;

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
    else if (mod_type == `DELAY) begin // Sending delay to ARM
        if (to_arm_delay[16] == 1'b1) begin
            sending_started = 1'b0;
            received_complete = 1'b0;
            counter <= counter + 1;
            ssp_clk <= ~ssp_clk;

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

endmodule
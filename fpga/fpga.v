//-----------------------------------------------------------------------------
// The FPGA is responsible for interfacing between the A/D, the coil drivers,
// and the ARM. In the low-frequency modes it passes the data straight
// through, so that the ARM gets raw A/D samples over the SSP. In the high-
// frequency modes, the FPGA might perform some demodulation first, to
// reduce the amount of data that we must send to the ARM.
//
// I am not really an FPGA/ASIC designer, so I am sure that a lot of this
// could be improved.
//
// Jonathan Westhues, March 2006
// Added ISO14443-A support by Gerhard de Koning Gans, April 2008
//-----------------------------------------------------------------------------

`include "hi_iso14443a.v"

module fpga(
	spck, miso, mosi, ncs,
	pck0, ck_1356meg, ck_1356megb,
	pwr_lo, pwr_hi, pwr_oe1, pwr_oe2, pwr_oe3, pwr_oe4,
	adc_d, adc_clk, adc_noe,
	ssp_frame, ssp_din, ssp_dout, ssp_clk,
	cross_hi, cross_lo,
	dbg
);
	input spck, mosi, ncs;
	output miso;
	input pck0, ck_1356meg, ck_1356megb;
	output pwr_lo, pwr_hi, pwr_oe1, pwr_oe2, pwr_oe3, pwr_oe4;
	input [7:0] adc_d;
	output adc_clk, adc_noe;
	input ssp_dout;
	output ssp_frame, ssp_din, ssp_clk;
	input cross_hi, cross_lo;
	output dbg;

reg ssp_clk;
reg ssp_frame;

//-----------------------------------------------------------------------------
// The SPI receiver. This sets up the configuration word, which the rest of
// the logic looks at to determine how to connect the A/D and the coil
// drivers (i.e., which section gets it). Also assign some symbolic names
// to the configuration bits, for use below.
//-----------------------------------------------------------------------------

reg [15:0] shift_reg;
reg [7:0] divisor;
reg [7:0] conf_word;

reg [2:0] div_counter;
reg clk;

reg bit_to_arm;
reg [17:0] counter;
reg [0:0] receive_counter;
reg [31:0] deadbeaf = 32'hDEADBEEF;
reg [31:0] test_cmd;

// We switch modes between transmitting to the 13.56 MHz tag and receiving
// from it, which means that we must make sure that we can do so without
// glitching, or else we will glitch the transmitted carrier.
always @(posedge ncs)
begin
	case(shift_reg[15:12])
		4'b0001: conf_word <= shift_reg[7:0];		// FPGA_CMD_SET_CONFREG
		4'b0010: divisor <= shift_reg[7:0];			// FPGA_CMD_SET_DIVISOR
	endcase
end

always @(posedge spck)
begin
	if(~ncs)
	begin
		shift_reg[15:1] <= shift_reg[14:0];
		shift_reg[0] <= mosi;
	end
end

// For the high-frequency simulated tag: what kind of modulation to use.
wire [2:0] hi_simulate_mod_type;
assign hi_simulate_mod_type = conf_word[2:0];

//-----------------------------------------------------------------------------
// And then we instantiate the modules corresponding to each of the FPGA's
// major modes, and use muxes to connect the outputs of the active mode to
// the output pins.
//-----------------------------------------------------------------------------

hi_iso14443a hisn(
	pck0, ck_1356meg, ck_1356megb,
	pwr_lo, pwr_hi, pwr_oe1, pwr_oe2, pwr_oe3, pwr_oe4,
	adc_d, adc_clk,
	hi_ssp_frame, hi_ssp_din, ssp_dout, hi_ssp_clk,
	cross_hi, cross_lo,
	dbg,
	hi_simulate_mod_type
);

always @(posedge ck_1356meg)
begin
	div_counter <= div_counter + 1;
	clk = div_counter[2];
end

always @(posedge clk)
begin
	if (hi_simulate_mod_type == 3'b101) begin
		receive_counter <= receive_counter + 1;
		ssp_clk <= hi_ssp_clk;
		ssp_frame = hi_ssp_frame;

		if (receive_counter[0] == 1'b0) begin
			test_cmd = {test_cmd[30:0], ssp_dout};
		end

		/*if (test_cmd[7:0] == 8'hEF) begin
			deadbeaf = 32'hFFFFFFFF;
		end*/

		counter <= 4'b0;
	end
	else if (hi_simulate_mod_type == 3'b110) begin
		counter <= counter + 1;
		if (counter[17:4] == 7'b0) begin
			ssp_clk <= ~ssp_clk;
			ssp_frame = (counter[3:2] == 2'b00);
			if (counter[0] == 1'b0) begin
				bit_to_arm <= deadbeaf[31];
				deadbeaf = {deadbeaf[30:0], deadbeaf[31]};
			end
		end

		receive_counter <= 4'b0;
	end
	else begin
		deadbeaf = 32'hDEADBEEF;
		ssp_frame = hi_ssp_frame;
		ssp_clk <= hi_ssp_clk;
		bit_to_arm <= hi_ssp_din;
	end
end

assign ssp_din = bit_to_arm;

// In all modes, let the ADC's outputs be enabled.
assign adc_noe = 1'b0;

endmodule
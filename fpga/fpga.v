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
`include "relay.v"
`include "relay_test.v"
`include "util.v"

`define SNIFFER			3'b000
`define TAGSIM_LISTEN	3'b001
`define TAGSIM_MOD		3'b010
`define READER_LISTEN	3'b011
`define READER_MOD		3'b100
`define FAKE_READER		3'b101
`define FAKE_TAG		3'b110

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
	input dbg;


//-----------------------------------------------------------------------------
// The SPI receiver. This sets up the configuration word, which the rest of
// the logic looks at to determine how to connect the A/D and the coil
// drivers (i.e., which section gets it). Also assign some symbolic names
// to the configuration bits, for use below.
//-----------------------------------------------------------------------------

reg [15:0] shift_reg = 16'b0;
reg [7:0] divisor = 8'b0;
reg [7:0] conf_word = 8'b0;

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

wire major_mode;
assign major_mode = conf_word[5];

// For the high-frequency simulated tag: what kind of modulation to use.
wire [2:0] hi_simulate_mod_type;
assign hi_simulate_mod_type = conf_word[2:0];
wire [2:0] relay_mod_type;
wire [2:0] mod_type;

wire hisn_ssp_dout;

wire relay_out;

wire relay_active = (hi_simulate_mod_type == `FAKE_READER || hi_simulate_mod_type == `FAKE_TAG || hi_simulate_mod_type == 3'b111);

//-----------------------------------------------------------------------------
// And then we instantiate the modules corresponding to each of the FPGA's
// major modes, and use muxes to connect the outputs of the active mode to
// the output pins.
//-----------------------------------------------------------------------------

hi_iso14443a hisn(
	pck0, ck_1356meg, ck_1356megb,
	, hisn_pwr_hi, hisn_pwr_oe1, hisn_pwr_oe2, hisn_pwr_oe3, hisn_pwr_oe4,
	adc_d, hisn_adc_clk,
	hisn_ssp_frame, /*hisn_ssp_din*/, hisn_ssp_dout, hisn_ssp_clk,
	cross_hi, cross_lo,
	,
	mod_type
);

relay r(
	ck_1356meg,
	~relay_active,
	dbg,
	hi_simulate_mod_type,
	relay_mod_type,
	relay_out,
	,//hisn_ssp_din,
	relay_relay,
	hisn_ssp_din
);

relay_test rt(
	pck0, ck_1356meg, ck_1356megb,
	rt_ssp_frame, rt_ssp_din, ssp_dout, rt_ssp_clk,
	dbg, rt_data_out,
	hi_simulate_mod_type
);

mux2 mux_ssp_clk		(major_mode, ssp_clk,   hisn_ssp_clk,   		rt_ssp_clk);
mux2 mux_ssp_din		(major_mode, ssp_din,   hisn_ssp_din,           rt_ssp_din);
mux2 mux_ssp_frame		(major_mode, ssp_frame, hisn_ssp_frame, 		rt_ssp_frame);
mux2 mux_pwr_oe1		(major_mode, pwr_oe1,   hisn_pwr_oe1,   		1'b0);
mux2 mux_pwr_oe2		(major_mode, pwr_oe2,   hisn_pwr_oe2,   		1'b0);
mux2 mux_pwr_oe3		(major_mode, pwr_oe3,   hisn_pwr_oe3,   		1'b0);
mux2 mux_pwr_oe4		(major_mode, pwr_oe4,   hisn_pwr_oe4,   		1'b0);
mux2 mux_pwr_lo			(major_mode, pwr_lo,    relay_relay,  			rt_data_out);
mux2 mux_pwr_hi			(major_mode, pwr_hi,    hisn_pwr_hi,    		1'b0);
mux2 mux_adc_clk		(major_mode, adc_clk,   hisn_adc_clk,   		1'b0);

assign mod_type = relay_active ? relay_mod_type : hi_simulate_mod_type;

assign hisn_ssp_dout = relay_active ? relay_out : ssp_dout;

// In all modes, let the ADC's outputs be enabled.
assign adc_noe = 1'b0;

endmodule

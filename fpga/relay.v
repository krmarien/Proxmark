`define SNIFFER			3'b000
`define TAGSIM_LISTEN	3'b001
`define TAGSIM_MOD		3'b010
`define READER_LISTEN	3'b011
`define READER_MOD		3'b100
`define FAKE_READER		3'b101
`define FAKE_TAG		3'b110

`include "relay_mode.v"
`include "relay_decode.v"
`include "relay_encode.v"

module relay (
	clk,
	reset,
	data_in,
	hi_simulate_mod_type,
	mod_type,
	data_out,
	relay_raw,
	relay_encoded
);
	input clk, reset, data_in;
	input [2:0] hi_simulate_mod_type;
	output [2:0] mod_type;
	output data_out;
	input relay_raw;
	output relay_encoded;

	wire [3:0] data_in_decoded;
	reg [2:0] previous_mod_type;
	wire mod_switched;

	assign mod_switched = (previous_mod_type == `READER_MOD && mod_type == `READER_LISTEN) || (previous_mod_type == `TAGSIM_MOD && mod_type == `TAGSIM_LISTEN);

	always @(posedge clk)
	begin
		previous_mod_type = mod_type;
	end

	relay_mode rm(
		clk,
		data_in_decoded,
		data_in_available,
		hi_simulate_mod_type,
		mod_type,
		data_out
	);

	relay_encode re(
		clk,
		reset || mod_switched,
		(hi_simulate_mod_type == `FAKE_READER),
		(mod_type != `TAGSIM_MOD && mod_type != `READER_MOD) & relay_raw,
		relay_encoded
	);

	relay_decode rd(
		clk,
		reset || mod_switched,
		(hi_simulate_mod_type == `FAKE_READER),
		data_in,
		data_in_decoded,
		data_in_available
	);
endmodule
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
		reset,
		(hi_simulate_mod_type == `FAKE_READER),
		(mod_type != `TAGSIM_MOD && mod_type != `READER_MOD) & relay_raw,
		relay_encoded
	);

	relay_decode rd(
		clk,
		reset,
		(hi_simulate_mod_type == `FAKE_READER),
		data_in,
		data_in_decoded,
		data_in_available
	);
endmodule
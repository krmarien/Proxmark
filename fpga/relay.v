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

	reg [399:0] tmp_signal = 400'hf00f0f0ff00f0f0ff0f00f0ff00f0f0f0f0f0ff00ff00f0ff0f00ff00ff00f0f0f0f0ff0f0f0f00ff00ff0f00f0f;
	reg [3:0] div_counter = 4'b0;

	always @(posedge clk)
	begin
		div_counter <= div_counter + 1;

		if (div_counter[3:0] == 4'b1000 && (hi_simulate_mod_type == `FAKE_READER || hi_simulate_mod_type == `FAKE_TAG))
		begin
			tmp_signal = {tmp_signal[398:0], tmp_signal[399]};
		end
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
		reset,
		1'b1,//(hi_simulate_mod_type == `FAKE_READER),
		tmp_signal[399],//(mod_type != `TAGSIM_MOD && mod_type != `READER_MOD) & relay_raw,
		relay_encoded
	);

	relay_decode rd(
		clk,
		reset,
		(hi_simulate_mod_type == `FAKE_READER),
		relay_encoded,//data_in,
		data_in_decoded,
		data_in_available
	);
endmodule
`include "relay_decode.v"
`include "relay_encode.v"

`define SNIFFER			3'b000
`define TAGSIM_LISTEN	3'b001
`define TAGSIM_MOD		3'b010
`define READER_LISTEN	3'b011
`define READER_MOD		3'b100
`define FAKE_READER		3'b101
`define FAKE_TAG		3'b110

`define READER_START_COMM				8'hc0
`define READER_START_COMM_FIRST_CHAR	8'hc
`define READER_END_COMM_1 				16'h0000
`define READER_END_COMM_2 				16'hc000
`define TAG_START_COMM_FIRST_CHAR		8'hf
`define TAG_END_COMM 					8'h00

module relay (
	clk,
	reset,
	data_in,
	hi_simulate_mod_type,
	mod_type,
	data_out,
	relay_raw,
	relay_encoded,
	ssp_din
);
	input clk, reset, data_in;
	input [2:0] hi_simulate_mod_type;
	output [2:0] mod_type;
	output data_out;
	input relay_raw;
	output relay_encoded, ssp_din;

	reg [2:0] mod_type = 3'b0;
	wire [0:0] data_out;
	wire [0:0] relay_encoded;
	reg [0:0] ssp_din;

	wire [3:0] data_in_decoded;
	wire data_in_available;
	reg [3:0] div_counter = 4'b0;

	reg [23:0] receive_buffer = 24'b0;
	reg [0:0] half_byte_counter = 1'b0;

	reg [79:0] in_buf = 80'h00f0f00f00f00f000f;
	reg send_to_arm = 1'b0;
	reg [19:0] to_arm_delay = 20'b0;

	assign data_out = hi_simulate_mod_type == `FAKE_READER ? receive_buffer[7] : receive_buffer[3];

	always @(posedge clk)
	begin
		div_counter <= div_counter + 1;

		// Debug signal
		/*if (hi_simulate_mod_type == 3'b111 && div_counter[3:0] == 4'b0000)
		begin
			mod_type = `READER_LISTEN;
			if (to_arm_delay[19] == 1'b0)
			begin
				to_arm_delay = to_arm_delay + 1;
			end
			else begin
				ssp_din = in_buf[79];
				in_buf = {in_buf[78:0], 1'b0};
			end
		end*/

		// When there will be transmitted something in the near future, stop sending carrier
		if (in_buf[79]/*data_in*/ == 1'b1 && mod_type == `READER_LISTEN && hi_simulate_mod_type != 3'b111)
		begin
			mod_type = 3'b0;
		end

		// Buffer decoded signals
		if (data_in_available == 1'b1 && (hi_simulate_mod_type == `FAKE_READER || hi_simulate_mod_type == `FAKE_TAG)) begin
			receive_buffer = {receive_buffer[19:0], data_in_decoded};
			half_byte_counter = half_byte_counter + 1;

			// Debug signal
			/*if (|in_buf[79:76] == 1'b0 && send_to_arm == 1'b0)
			begin
				in_buf = {in_buf[75:0], data_in_decoded};
			end
			else
			begin
				send_to_arm = 1'b1;
			end*/
		end

		if (div_counter[3:0] == 4'b1000 && (hi_simulate_mod_type == `FAKE_READER || hi_simulate_mod_type == `FAKE_TAG))
		begin
			in_buf = {in_buf[78:0], 1'b0};
		end

		if (div_counter[3:0] == 4'b1000 && (hi_simulate_mod_type == `FAKE_READER || hi_simulate_mod_type == `FAKE_TAG))
		begin
			if (hi_simulate_mod_type == `FAKE_READER) // Fake Reader
			begin
				if (receive_buffer[23:4] == {16'b0, `READER_START_COMM_FIRST_CHAR})
				begin
					mod_type = `READER_MOD;
					half_byte_counter = 3'b0;
				end
				else if ((receive_buffer[23:4] == {`READER_END_COMM_1, 4'b0} || receive_buffer[23:4] == {`READER_END_COMM_2, 4'b0}) && half_byte_counter == 3'd0)
				begin
					mod_type = 3'b0;
				end
			end
			else if (hi_simulate_mod_type == `FAKE_TAG) // Fake Tag
			begin
				if (receive_buffer[19:0] == {16'b0, `TAG_START_COMM_FIRST_CHAR})
				begin
					mod_type = `TAGSIM_MOD;
					half_byte_counter = 3'b0;
				end
				else if (receive_buffer[11:0] == {`TAG_END_COMM, 4'b0}  && half_byte_counter == 3'd0)
				begin
					mod_type = `TAGSIM_LISTEN;
				end
			end
		end
	end

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
		in_buf[79],//data_in,
		data_in_decoded,
		data_in_available
	);
endmodule
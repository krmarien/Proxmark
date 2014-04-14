

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
`define TAG_START_COMM_FIRST_CHAR		8'hf0
`define TAG_END_COMM 					8'h00

module relay (
	clk,
	data_in,
	hi_simulate_mod_type,
	mod_type,
	data_out
);
	input clk, data_in;
	input [2:0] hi_simulate_mod_type;
	output [2:0] mod_type;
	output data_out;

	reg [2:0] mod_type;
	wire [0:0] data_out;


	reg [3:0] div_counter = 4'b0;

	reg [0:0] buf_data_in = 1'b0;

	reg [19:0] receive_buffer = 20'b0;
	reg [2:0] bit_counter = 3'b0;

	reg [79:0] tmp_signal = 80'hc0c00c00c00c000c0000;

	assign data_out = receive_buffer[3];

	always @(posedge clk)
	begin
		div_counter <= div_counter + 1;
	    buf_data_in = data_in;

	    if (!(hi_simulate_mod_type == `FAKE_READER || hi_simulate_mod_type == `FAKE_TAG))
		begin
			tmp_signal = 80'hc0c00c00c00c000c0000;
		end

		// div_counter[3:0] == 4'b1000 => 0.8475MHz
		if (div_counter[3:0] == 4'b1000 && (hi_simulate_mod_type == `FAKE_READER || hi_simulate_mod_type == `FAKE_TAG))
		begin
			//receive_buffer = {receive_buffer[18:0], buf_data_in};
			receive_buffer = {receive_buffer[18:0], tmp_signal[79]};
  			tmp_signal = {tmp_signal[78:0], 1'b0};
			bit_counter = bit_counter + 1;

			if (hi_simulate_mod_type == `FAKE_READER) // Fake Reader
			begin
				if (receive_buffer[19:0] == {16'b0, `READER_START_COMM_FIRST_CHAR})
				begin
					mod_type = `READER_MOD;
					bit_counter = 3'b0;
				end
				else if ((receive_buffer[19:0] == {`READER_END_COMM_1, 4'b0} || receive_buffer[19:0] == {`READER_END_COMM_2, 4'b0}) && bit_counter == 3'd0)
				begin
					mod_type = `READER_LISTEN;
				end
			end
			else if (hi_simulate_mod_type == `FAKE_TAG) // Fake Tag
			begin
				if (receive_buffer[19:0] == {16'b0, `TAG_START_COMM_FIRST_CHAR})
				begin
					mod_type = `TAGSIM_MOD;
					bit_counter = 3'b0;
				end
				else if (receive_buffer[11:0] == {`TAG_END_COMM, 4'b0}  && bit_counter == 3'd0)
				begin
					mod_type = `TAGSIM_LISTEN;
				end
			end
		end
	end
endmodule
module mux2(sel, y, x0, x1);
    input sel;
    input x0, x1;
    output y;
    reg y;

always @(x0 or x1 or sel)
begin
    case (sel)
        1'b1: y = x0;
        1'b0: y = x1;
    endcase
end

endmodule

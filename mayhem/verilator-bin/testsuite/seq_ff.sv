module seq_ff #(parameter int W = 8) (
    input  logic         clk,
    input  logic         rst_n,
    input  logic [W-1:0] d,
    output logic [W-1:0] q
);
   always_ff @(posedge clk or negedge rst_n) begin
      if (!rst_n) q <= '0;
      else        q <= d;
   end
endmodule

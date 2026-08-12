`define WIDTH 4
module gen_case #(parameter int N = 3) (
    input  logic [`WIDTH-1:0] in [N],
    output logic [`WIDTH-1:0] out
);
   logic [`WIDTH-1:0] acc [N+1];
   assign acc[0] = '0;
   generate
      for (genvar g = 0; g < N; g++) begin : loop
         assign acc[g+1] = acc[g] ^ in[g];
      end
   endgenerate
   assign out = acc[N];

   /* verilator lint_off WIDTH */
   wire [2:0] narrow = out;
   /* verilator lint_on WIDTH */
endmodule

module case_func (input logic [2:0] sel, input logic [7:0] a, output logic [7:0] y);
   function automatic logic [7:0] rot(input logic [7:0] v, input int n);
      logic [7:0] r;
      r = (v << n) | (v >> (8 - n));
      return r;
   endfunction

   always_comb begin
      unique case (sel)
        3'd0: y = a;
        3'd1: y = ~a;
        3'd2: y = rot(a, 1);
        3'd3: y = rot(a, 3);
        default: y = 8'h5A;
      endcase
   end
endmodule

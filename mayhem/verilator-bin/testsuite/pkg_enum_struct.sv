package p;
   typedef enum logic [1:0] { IDLE, RUN, DONE } state_e;
   typedef struct packed { logic [3:0] hi; logic [3:0] lo; } pair_t;
   function automatic logic [7:0] swap(input pair_t v);
      return {v.lo, v.hi};
   endfunction
endpackage

module pkg_enum_struct (input logic [7:0] i, output logic [7:0] o);
   import p::*;
   state_e st;
   pair_t  pr;
   always_comb begin
      pr = pair_t'(i);
      st = (i == 0) ? IDLE : RUN;
      o  = (st == IDLE) ? 8'h00 : swap(pr);
   end
endmodule

interface simple_if;
   logic       valid;
   logic [7:0] data;
   modport src (output valid, output data);
   modport dst (input  valid, input  data);
endinterface

module consumer (simple_if.dst b, output logic [7:0] o);
   assign o = b.valid ? b.data : 8'hFF;
endmodule

module iface_mod (input logic v, input logic [7:0] d, output logic [7:0] o);
   simple_if bus();
   assign bus.valid = v;
   assign bus.data  = d;
   consumer u (.b(bus), .o(o));
endmodule

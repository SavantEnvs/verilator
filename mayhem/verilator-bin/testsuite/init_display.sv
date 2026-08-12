module init_display;
   localparam string NAME = "vl";
   integer i;
   reg [31:0] mem [0:7];
   initial begin
      for (i = 0; i < 8; i = i + 1)
        mem[i] = i * 32'h1111_1111;
      $display("%s %0d %x", NAME, i, mem[3]);
      if (mem[1] !== 32'h1111_1111) $stop;
      $finish;
   end
endmodule

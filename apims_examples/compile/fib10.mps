define $fibres =
  2=>1<Int>;
  Gend
in
define $fib =
  1=>2<Int>;
  $fibres
in
(nu fib: $fib)
( def Fib() =
    link(2,fib,s,2);
    ( Fib()
    | s[1]>>x;
      if x <=1
      then s[1]<<1;
           end
      else link(2,fib,s1,1);
           s1[2]<<x-1;
           link(2,fib,s2,1);
           s2[2]<<x-2;
           s1[2]>>f1;
           s2[2]>>f2;
           s[1]<<f1+f2;
           end
    )
  in Fib()
| link(2,fib,f,1);
  f[2]<<10;
  f[2]>>f10;
  end
)

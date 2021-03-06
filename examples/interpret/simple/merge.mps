define $sstream = rec $x. 1=>2<String>;$x in
define $finstream = rec $y. 1=>2{^Write: 1=>2<String>; $y,
                                 ^Stop: Gend} in
(nu reallog: $sstream)
(nu log: $finstream)
( link (2,reallog,theLog,2);
  // STOP WITHOUT HANDELING LOG! {{{
  (nu x: 1=>2<$sstream@(2 of 2)>;Gend)
  link(2,x,t,1);
  t[2]<<theLog;
  end // }}}
| link(2,reallog,theLog,1); // {{{
  def Log(theLog: $sstream@(1 of 2)) =
    link(2,log,s,2);
    def LogObject(this: $finstream@(2 of 2), theLog: $sstream@(1 of 2)) =
      this[1]>>
      {^Stop: Log(theLog),
       ^Write: this[1]>>msg; theLog[2]<<msg; LogObject(this,theLog)
      }
    in LogObject(s,theLog)
  in Log(theLog) // }}}
| link(2,log,s,1); s[2]<<^Write;s[2]<<"Log1.1"; s[2]<<^Write;s[2]<<"Log1.2";s[2]<<^Stop;end
| link(2,log,s,1); s[2]<<^Write;s[2]<<"Log2.1"; s[2]<<^Write;s[2]<<"Log2.2";s[2]<<^Stop;end
| link(2,log,s,1); s[2]<<^Write;s[2]<<"Log3.1"; s[2]<<^Write;s[2]<<"Log3.2";s[2]<<^Stop;end
| link(2,log,s,1); s[2]<<^Write;s[2]<<"Log4.1"; s[2]<<^Write;s[2]<<"Log4.2";s[2]<<^Stop;end
| link(2,log,s,1); s[2]<<^Write;s[2]<<"Log5.1"; s[2]<<^Write;s[2]<<"Log5.2";s[2]<<^Stop;end
| link(2,log,s,1); s[2]<<^Write;s[2]<<"Log6.1"; s[2]<<^Write;s[2]<<"Log6.2";s[2]<<^Stop;end
)

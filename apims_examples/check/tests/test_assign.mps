// Test assign
(nu a : 1=>2<Int>; Gend)
( link(2,a,s,2);
  s[1]>>x;
  x : Int = 3*x;
  link(2,a,t,1);
  t[2]<<x;
  end
| link(2,a,s,1);
  s[2]<<5;
  link(2,a,t,2);
  t[1]>>x;
  end
)
